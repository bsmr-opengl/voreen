/**********************************************************************
 *                                                                    *
 * tgt - Tiny Graphics Toolbox                                        *
 *                                                                    *
 * Copyright (C) 2006-2008 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the tgt library. This library is free         *
 * software; you can redistribute it and/or modify it under the terms *
 * of the GNU Lesser General Public License version 2.1 as published  *
 * by the Free Software Foundation.                                   *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU Lesser General Public License for more details.                *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License in the file "LICENSE.txt" along with this library.         *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 **********************************************************************/

#ifdef TGT_HAS_FFMPEG

#include "videoencoder.h"

#include "tgt/logmanager.h"

#include <sstream>
#include <string>
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

namespace tgt {

namespace {

/**
 * seperated by whitespace
 * @see VidepEncoder#getSupportedFormatsByFileEnding()
 */
const char* supportedFormatsFileEndings = "mpeg mpg wmv avi flv";

enum PixelOrderManipulation {
    NONE, FLIP_VERT, REVERSE
};

/**
 * float to uint8_t conversion
 * TODO this is misplaced here -> centralize
 * TODO extend TextureContainer to return just GL_RGB, atm we get GL_RGBA, so we just drop the alpha channel
 * TODO consider moving pixelOrderManip there too
 * TODO we have to put down an assumption about the pixel-order expected or let it be specified for the input data
 * @param pixels as floats
 * @param size number of pixels
 * @param lineSize (ignored but for FLIP_VERT)
 * @param manip (defaults: NONE)
 * @return pixels of type uint8_t
 */
uint8_t* floatToUint8_tRGB(float* pixels, const int size,
                           const int lineSize, enum PixelOrderManipulation manip = NONE)
{
    tgt::vec4* pixels_f = reinterpret_cast<tgt::vec4*> (pixels);
    tgt::col3* pixels_b = new tgt::col3[size];
    int j = 0;
    for (int i = 0; i < size; i++) {
        /*
         * 0 ... lineSize-1
         * lineSize ... 2*lineSize-1
         * ...
         * size-lineSize ... size-1
         */
        switch (manip) {
        case NONE:
            j = i; // up to compiler optimisation
            break;
        case REVERSE:
            j = size - i - 1;
            break;
        case FLIP_VERT:
            int curLine = (size / lineSize) - (i / lineSize) - 1;
            j = curLine * lineSize + (i % lineSize); // assume int div truncates
            break;
        }

        // dropping alpha channel and clamp to [0.0; 1.0]
        pixels_b[i] = tgt::clamp(pixels_f[j].xyz(), 0.f, 1.f) * 255.f;
    }
    return reinterpret_cast<uint8_t*> (pixels_b);
}

/**
 * default encoder-input-pixel-type
 */
const enum PixelFormat defaultPixelFormatCodec = PIX_FMT_YUV420P;

/**
 * size of encoder's output-buffer and half of IO-buffer's size
 */
unsigned int videoOutBufferSize;

/*
 * some default codec parameters commonly used
 */
const int defaultGopSize = 10;
const int defaultMaxBFrame = 1;
const int minimalFps = 25;
const int minimalBitrate = 400000;

struct VEAVContext {

    /**
     * log category for tgt-logging
     */
    std::string loggerCat_;

    // Encoder format-configuration and stream-state
    AVFormatContext* formatContext_;
    AVStream* videoStream;

    SwsContext* swsContext; // converts inputFrame to videoFrame, which is ready for encoding
    AVFrame* videoFrame; // encoded with videoFormat pix_fmt

    int frameCount;

    /**
     * encoder's output buffer
     * @see #videoOutBufferSize
     */
    uint8_t* videoBuffer;

    GLenum pixelFormat, pixelType; // input specification

    /**
     * init picture- & video-buffer
     * @see #initCodec(int,int,int,int) should be run before
     */
    bool initBuffers() {
        AVCodecContext* codecContext = videoStream->codec;
        PixelFormat pixelFormat = codecContext->pix_fmt;
        const int width = codecContext->width;
        const int height = codecContext->height;

        // alloc frame/picture buffer
        videoFrame = avcodec_alloc_frame();
        videoOutBufferSize = avpicture_get_size(pixelFormat, width, height);
        uint8_t* pictureBuffer = static_cast<uint8_t*> (av_malloc(
                videoOutBufferSize));
        if (!pictureBuffer) {
            LERRORC(loggerCat_, "unable to alloc picture buffer");
            return false;
        }
        avpicture_fill(reinterpret_cast<AVPicture*> (videoFrame),
                pictureBuffer, pixelFormat, width, height);

        // alloc video buffer
        videoBuffer = static_cast<uint8_t*> (av_malloc(videoOutBufferSize));

        return true;
    }

    /**
     * init video codec
     */
    bool initCodec(const int bitRate, const int width, const int height,
            const int fps) {
        AVCodecContext* codecContext = videoStream->codec;
        codecContext->codec_id = formatContext_->video_codec_id;
        codecContext->codec_type = CODEC_TYPE_VIDEO;

        codecContext->bit_rate = bitRate;
        codecContext->width = width;
        codecContext->height = height;

        codecContext->time_base.num = 1;
        codecContext->time_base.den = fps;

        codecContext->gop_size = defaultGopSize;
        codecContext->pix_fmt = defaultPixelFormatCodec;

        if (codecContext->codec_id == CODEC_ID_MPEG2VIDEO) {
            codecContext->max_b_frames = defaultMaxBFrame;
        }

        if (codecContext->codec_id == CODEC_ID_MPEG1VIDEO) {
            codecContext->mb_decision = FF_MB_DECISION_RD;
        }

        // use fourcc "DX50" instead of "FMP4", as more players support it
        if (codecContext->codec_id == CODEC_ID_MPEG4){
            const char xvid[] = "DX50";
            codecContext->codec_tag = (xvid[3]<<24) + (xvid[2]<<16) + (xvid[1]<<8) + xvid[0];
        }


        codecContext->strict_std_compliance = FF_COMPLIANCE_VERY_STRICT; // we are the good ones

        AVCodec* codec = avcodec_find_encoder(codecContext->codec_id);

        if (!codec) {
            LERRORC(loggerCat_, "Could not find Codec " << codecContext->codec_id);
            return false;
        }

        if (avcodec_open(codecContext, codec) < 0) {
            LERRORC(loggerCat_, "Could not load Codec " << codec->id);
            return false;
        }

        return true;
        /* codecContext == videoStream->codec */
    }

    bool checkParams(const int fps, const int width, const int height,
            GLenum pixelFormat, GLenum pixelType, const int bitRate) {

        // TODO check limits of swscale conversion
        if (GL_RGB != pixelFormat && GL_FLOAT != pixelType) {
            LERRORC(loggerCat_,"only GL_RGB + GL_FLOAT as input supported");
            return false;
        }

        if (width <= 0 || height <= 0) {
            LERRORC(loggerCat_, "illegal dimensions: (" << width << "x" << height << ")");
            return false;
        }

        if ((width % 2) + (height % 2) != 0) {
            LERRORC(loggerCat_, "width and height must be multiple of 2 - " << width << ":" << height);
            return false;
        }

        if (fps < minimalFps) {
            LERRORC(loggerCat_, "low fps: " << fps << "; minimum: " << minimalFps);
            return false;
        }

        if (bitRate < minimalBitrate) {
            LERRORC(loggerCat_, "low bitrate: " << bitRate << "; minimum: " << minimalBitrate);
            return false;
        }

        return true;
    }

    /**
     * TODO split to more seperate init methods
     */
    VEAVContext(std::string filePath, const int fps, const int width,
            const int height, GLenum pixelFormat, GLenum pixelType,
            const int bitRate) {

        // loggerCat_
        // std::string-s concat with "+", but iostream-string-s concat with "<<"
        loggerCat_ = "";
        loggerCat_ += "tgt.VideoEncoder.AVContext::";
        loggerCat_ += filePath;

        if (!checkParams(fps, width, height, pixelFormat, pixelType, bitRate)) {
            return;
        }

        //
        swsContext = 0;
        frameCount = 0;

        pixelFormat = pixelFormat;
        pixelType = pixelType;

        // avcodec init
        av_register_all();

        // let ffmpeg guess what stream-format matches to container-format
        AVOutputFormat* outputFormat = guess_format(0, filePath.c_str(), 0);

        if (!outputFormat) {
            LWARNINGC(loggerCat_, "Could not guess output format, using MPEG");
            outputFormat = guess_format("mpeg", 0, 0);
            filePath.append(".mpeg");
        }

        if (!outputFormat) {
            LERRORC(loggerCat_, "Could not find suitable output format");
            return;
        }
        // init format-context and video-stream
        formatContext_ = avformat_alloc_context();
        formatContext_->oformat = outputFormat;
        formatContext_->audio_codec_id = CODEC_ID_NONE;
        formatContext_->video_codec_id = outputFormat->video_codec;

        if (formatContext_->video_codec_id != CODEC_ID_NONE) {
            videoStream = av_new_stream(formatContext_, 1);

            if (!videoStream) {
                LERRORC(loggerCat_, "Could not allocate stream");
                return;
            }
            if (!(initCodec(bitRate, width, height, fps) && initBuffers())) {
                return;
            }
        } else {
            LERRORC(loggerCat_, "Failed to allocate format-context");
            return;
        }

        // must set parameters
        if (av_set_parameters(formatContext_, 0) < 0) {
            LERRORC(loggerCat_, "invalid format parameters");
            return;
        }

#ifdef TGT_DEBUG
        dump_format(formatContext_, 0, filePath.c_str(), 1);//outputs format specs
#endif

        // open video's file (will overwrite)
        if (int err = url_fopen(&formatContext_->pb, filePath.c_str(), URL_WRONLY) < 0) {
            LERRORC(loggerCat_, filePath << " is not accessible (with write-access):  "<< err);
            return;
        }

        /*
         * must be called before any I/O
         * double videoOutBufferSize for slow fs
         */
        url_setbufsize(formatContext_->pb, 2 * videoOutBufferSize);
        url_resetbuf(formatContext_->pb, URL_WRONLY);

        // write format's header (if any)
        av_write_header(formatContext_);
    }

    /**
     * assume frames are passed sequential
     */
    void nextFrame(uint8_t* pixels) {
        AVCodecContext* codecContext = videoStream->codec;

        // since pixels are filled inside, this is rather a lightweight allocation
        AVFrame* inputFrame = avcodec_alloc_frame();

        /*
         * fill input into inputFrame
         * inputFrame->data ^= pixels is freed outside this scope
         */
        avpicture_fill((AVPicture*) inputFrame, pixels, PIX_FMT_RGB24,
                codecContext->width, codecContext->height);

        // convert RGB24 -> YUV420P
        if (swsContext == 0) {
            swsContext = sws_getContext(codecContext->width,
                    codecContext->height, PIX_FMT_RGB24, codecContext->width,
                    codecContext->height, codecContext->pix_fmt, SWS_BICUBIC,
                    0, 0, 0);
        }
        sws_scale(swsContext, inputFrame->data, inputFrame->linesize, 0,
                codecContext->height, videoFrame->data, videoFrame->linesize);

        av_free(inputFrame);

        int outputSize = avcodec_encode_video(codecContext, videoBuffer,
                videoOutBufferSize, videoFrame);
        if (outputSize > 0) {
            AVPacket packet;
            av_init_packet(&packet);

            packet.pts = av_rescale_q(codecContext->coded_frame->pts,
                    codecContext->time_base, videoStream->time_base);
            if (codecContext->coded_frame->key_frame) {
                packet.flags |= PKT_FLAG_KEY;
            }
            packet.stream_index = videoStream->index;
            packet.data = videoBuffer;
            packet.size = outputSize;

            if (av_write_frame(formatContext_, &packet) != 0) {
                LWARNINGC(loggerCat_, "error while writing frame to file");
            }

            // this would free videoBuffer, we want to reuse
            //av_destruct_packet(&packet);
        }
        frameCount++;
    }

    /**
     * write file trailer and finish encoding
     */
    virtual ~VEAVContext() {
        avcodec_close(videoStream->codec);
        av_free(videoFrame->data[0]);
        av_free(videoFrame);

        av_free(videoBuffer);

        av_write_trailer(formatContext_);

        url_fclose(formatContext_->pb);

        av_free(formatContext_);
    }
};

VEAVContext* encoderContext;

} /* anonymous namespace */

std::string VideoEncoder::loggerCat_("tgt.VideoEncoder");

VideoEncoder::VideoEncoder() {
    encoderContext = 0;
}

VideoEncoder::~VideoEncoder() {
}

void VideoEncoder::startVideoEncoding(std::string filePath, const int fps,
        const int width, const int height) {
    if (encoderContext != 0) {
        LWARNING("already encoding a video");
        return;
    }

    // we must not reuse this context because it still stores state of finished encoding
    // TODO consider reusing everything but videoStream
    encoderContext = new VEAVContext(filePath, fps, width, height, GL_RGB,
            GL_FLOAT, 2000000/*2Mbps*/);

    if (!encoderContext) {
        LWARNING("Encoding NOT started due to previous error");
    }
}

void VideoEncoder::stopVideoEncoding() {
    if (encoderContext == 0) {
        LERROR("Encoding has already aborted");
        return;
    }

    AVCodecContext* codecContext = encoderContext->videoStream->codec;

    if (encoderContext->frameCount % codecContext->time_base.den != 0) {
        LWARNING("passed frames doesn't match fps: " << encoderContext->frameCount << "/" << codecContext->time_base.den);
    }

    delete encoderContext;
    encoderContext = 0;
}

/**
 * TODO assumption of float not applied here cause we could specify pixelComponents' type when starting animation
 */
void VideoEncoder::nextFrame(GLvoid* pixels) {
    AVCodecContext* codecContext = encoderContext->videoStream->codec;

    float* pixelsFloat = reinterpret_cast<float*> (pixels);

    // TODO move this conversion into VEAVContext and let it rely it on input specs
    uint8_t* pixels8 = tgt::floatToUint8_tRGB(pixelsFloat, codecContext->width
            * codecContext->height, codecContext->width, FLIP_VERT);
    delete[] pixelsFloat;

    encoderContext->nextFrame(pixels8);
    delete[] pixels8;
}

/**
 * static
 */
std::vector<std::string> VideoEncoder::getSupportedFormatsByFileEnding() {
    std::stringstream ss(supportedFormatsFileEndings);
    std::vector<std::string> formats;
    std::string format;

    while (ss >> format)
        formats.push_back(format);

    return formats;
}

} // namespace tgt

#endif // TGT_HAS_FFMPEG
