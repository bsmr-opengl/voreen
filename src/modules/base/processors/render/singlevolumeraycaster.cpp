/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2010 The Voreen Team. <http://www.voreen.org>   *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#include "voreen/modules/base/processors/render/singlevolumeraycaster.h"

#include "tgt/textureunit.h"

#include <sstream>

using tgt::vec3;
using tgt::TextureUnit;

namespace voreen {

SingleVolumeRaycaster::SingleVolumeRaycaster()
    : VolumeRaycaster()
    , transferFunc_("transferFunction", "Transfer function")
    , camera_("camera", "Camera", new tgt::Camera(vec3(0.f, 0.f, 3.5f), vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f)))
    , compositingMode1_("compositing1", "Compositing (OP2)", Processor::INVALID_PROGRAM)
    , compositingMode2_("compositing2", "Compositing (OP3)", Processor::INVALID_PROGRAM)
    , volumeInport_(Port::INPORT, "volumehandle.volumehandle", false, Processor::INVALID_PROGRAM)
    , entryPort_(Port::INPORT, "image.entrypoints")
    , exitPort_(Port::INPORT, "image.exitpoints")
    , outport_(Port::OUTPORT, "image.output", true, Processor::INVALID_PROGRAM)
    , outport1_(Port::OUTPORT, "image.output1", true, Processor::INVALID_PROGRAM)
    , outport2_(Port::OUTPORT, "image.output2", true, Processor::INVALID_PROGRAM)
{

    // VolumeRaycaster Props
    addProperty(useSegmentation_);
    addProperty(segment_);
    addProperty(isoValue_);

    addProperty(transferFunc_);
    addProperty(camera_);

    addProperty(maskingMode_);
    addProperty(gradientMode_);
    addProperty(classificationMode_);
    addProperty(shadeMode_);
    addProperty(compositingMode_);

    compositingMode1_.addOption("dvr", "DVR");
    compositingMode1_.addOption("mip", "MIP");
    compositingMode1_.addOption("iso", "ISO");
    compositingMode1_.addOption("fhp", "FHP");
    compositingMode1_.addOption("fhn", "FHN");
    addProperty(compositingMode1_);

    compositingMode2_.addOption("dvr", "DVR");
    compositingMode2_.addOption("mip", "MIP");
    compositingMode2_.addOption("iso", "ISO");
    compositingMode2_.addOption("fhp", "FHP");
    compositingMode2_.addOption("fhn", "FHN");
    addProperty(compositingMode2_);

    addProperty(lightPosition_);
    addProperty(lightAmbient_);
    addProperty(lightDiffuse_);
    addProperty(lightSpecular_);
    addProperty(materialShininess_);
    addProperty(applyLightAttenuation_);
    addProperty(lightAttenuation_);

    addPort(volumeInport_);
    addPort(entryPort_);
    addPort(exitPort_);
    addPort(outport_);
    addPort(outport1_);
    addPort(outport2_);
}

SingleVolumeRaycaster::~SingleVolumeRaycaster() {
}

std::string SingleVolumeRaycaster::getProcessorInfo() const {
    return "This is the standard volume renderer in Voreen. It allows to generate three output "
           "renderings, where the first one also provides depth values. Several "
           "shading and compositing modes are supported.<br/>"
           "See CubeMeshProxyGeometry, MeshEntryExitPoints.";
}

Processor* SingleVolumeRaycaster::create() const {
    return new SingleVolumeRaycaster();
}

void SingleVolumeRaycaster::initialize() throw (VoreenException) {
    VolumeRaycaster::initialize();

    loadShader();

    if (!raycastPrg_) {
        LERROR("Failed to load shaders!");
        initialized_ = false;
        throw VoreenException(getClassName() + ": Failed to load shaders!");
    }
    initialized_ = true;

    portGroup_.initialize();
    portGroup_.addPort(outport_);
    portGroup_.addPort(outport1_);
    portGroup_.addPort(outport2_);
}

void SingleVolumeRaycaster::deinitialize() throw (VoreenException) {
    portGroup_.deinitialize();

    VolumeRaycaster::deinitialize();
}

void SingleVolumeRaycaster::loadShader() {
    raycastPrg_ = ShdrMgr.loadSeparate("passthrough.vert", "rc_singlevolume.frag",
                                       generateHeader(), false, false);
}

void SingleVolumeRaycaster::compile(VolumeHandle* volumeHandle) {
    raycastPrg_->setHeaders(generateHeader(volumeHandle), false);
    raycastPrg_->rebuild();
}

bool SingleVolumeRaycaster::isReady() const {
    //check if all inports are connected:
    if(!entryPort_.isReady() || !exitPort_.isReady() || !volumeInport_.isReady())
        return false;

    //check if at least one outport is connected:
    if (!outport_.isReady() && !outport1_.isReady() && !outport2_.isReady())
        return false;

    return true;
}

void SingleVolumeRaycaster::process() {

    // compile program if needed
    if (getInvalidationLevel() >= Processor::INVALID_PROGRAM) {
        PROFILING_BLOCK("compile");
        compile(volumeInport_.getData());
    }
    LGL_ERROR;

    // bind transfer function
    tgt::TextureUnit transferUnit;
    transferUnit.activate();
    LGL_ERROR;
    if (transferFunc_.get())
        transferFunc_.get()->bind();

    portGroup_.activateTargets();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LGL_ERROR;

    transferFunc_.setVolumeHandle(volumeInport_.getData());

    // bind entry params
    tgt::TextureUnit entryUnit, entryDepthUnit, exitUnit, exitDepthUnit;
    entryPort_.bindTextures(entryUnit.getEnum(), entryDepthUnit.getEnum());
    LGL_ERROR;

    // bind exit params
    exitPort_.bindTextures(exitUnit.getEnum(), exitDepthUnit.getEnum());
    LGL_ERROR;

    // vector containing the volumes to bind; is passed to bindVolumes()
    std::vector<VolumeStruct> volumeTextures;

    // add main volume
    TextureUnit volUnit;
    volumeTextures.push_back(VolumeStruct(
        volumeInport_.getData()->getVolumeGL(),
        &volUnit,
        "volume_",
        "volumeParameters_")
    );

    updateBrickingParameters(volumeInport_.getData());
    TextureUnit brickUnit1, brickUnit2;
    addBrickedVolumeModalities(volumeInport_.getData(), volumeTextures, &brickUnit1, &brickUnit2);

    // segmentation volume
    //VolumeHandle* volumeSeg = volumeInport_.getData()->getRelatedVolumeHandle(Modality::MODALITY_SEGMENTATION);
    VolumeHandle* volumeSeg = 0;

    bool usingSegmentation = (maskingMode_.get() == "Segmentation") && volumeSeg;
    TextureUnit segUnit;
    if (usingSegmentation) {
        // Important to set the correct texture unit before getVolumeGL() is called or
        // glTexParameter() might influence the wrong texture.
        segUnit.activate();
        volumeTextures.push_back(VolumeStruct(volumeSeg->getVolumeGL(),
                                              &segUnit,
                                              "segmentation_",
                                              "segmentationParameters_"));

        // set texture filtering for this texture unit
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    // initialize shader
    raycastPrg_->activate();

    // set common uniforms used by all shaders
    setGlobalShaderParameters(raycastPrg_, camera_.get());
    // bind the volumes and pass the necessary information to the shader
    bindVolumes(raycastPrg_, volumeTextures);

    // pass the remaining uniforms to the shader
    raycastPrg_->setUniform("entryPoints_", entryUnit.getUnitNumber());
    raycastPrg_->setUniform("entryPointsDepth_", entryDepthUnit.getUnitNumber());
    entryPort_.setTextureParameters(raycastPrg_, "entryParameters_");
    raycastPrg_->setUniform("exitPoints_", exitUnit.getUnitNumber());
    raycastPrg_->setUniform("exitPointsDepth_", exitDepthUnit.getUnitNumber());
    exitPort_.setTextureParameters(raycastPrg_, "exitParameters_");

    if (compositingMode_.get() ==  "iso" ||
        compositingMode1_.get() == "iso" ||
        compositingMode2_.get() == "iso")
        raycastPrg_->setUniform("isoValue_", isoValue_.get());

    if (classificationMode_.get() == "transfer-function")
        raycastPrg_->setUniform("transferFunc_", transferUnit.getUnitNumber());

    if (usingSegmentation) {
        GLfloat seg = segment_.get() / 255.f;
        raycastPrg_->setUniform("segment_", seg);
    }

    setBrickedVolumeUniforms(volumeInport_.getData());
    LGL_ERROR;

    glPushAttrib(GL_LIGHTING_BIT);
    setLightingParameters();

    {
        PROFILING_BLOCK("raycasting");
        renderQuad();
    }

    glPopAttrib();
    raycastPrg_->deactivate();

    if (usingSegmentation) {
        // restore default texture filtering mode
        segUnit.activate();
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    TextureUnit::setZeroUnit();
    LGL_ERROR;
}

std::string SingleVolumeRaycaster::generateHeader(VolumeHandle* volumeHandle) {
    std::string headerSource = VolumeRaycaster::generateHeader(volumeHandle);

    headerSource += transferFunc_.get()->getShaderDefines();

    // configure compositing mode for port 2
    headerSource += "#define RC_APPLY_COMPOSITING_1(result, color, samplePos, gradient, t, tDepth) ";
    if (compositingMode1_.get() == "dvr")
        headerSource += "compositeDVR(result, color, t, tDepth);\n";
    else if (compositingMode1_.get() == "mip")
        headerSource += "compositeMIP(result, color, t, tDepth);\n";
    else if (compositingMode1_.get() == "iso")
        headerSource += "compositeISO(result, color, t, tDepth, isoValue_);\n";
    else if (compositingMode1_.get() == "fhp")
        headerSource += "compositeFHP(samplePos, result, t, tDepth);\n";
    else if (compositingMode1_.get() == "fhn")
        headerSource += "compositeFHN(gradient, result, t, tDepth);\n";

    // configure compositing mode for port 3
    headerSource += "#define RC_APPLY_COMPOSITING_2(result, color, samplePos, gradient, t, tDepth) ";
    if (compositingMode2_.get() == "dvr")
        headerSource += "compositeDVR(result, color, t, tDepth);\n";
    else if (compositingMode2_.get() == "mip")
        headerSource += "compositeMIP(result, color, t, tDepth);\n";
    else if (compositingMode2_.get() == "iso")
        headerSource += "compositeISO(result, color, t, tDepth, isoValue_);\n";
    else if (compositingMode2_.get() == "fhp")
        headerSource += "compositeFHP(samplePos, result, t, tDepth);\n";
    else if (compositingMode2_.get() == "fhn")
        headerSource += "compositeFHN(gradient, result, t, tDepth);\n";

    portGroup_.reattachTargets();
    headerSource += portGroup_.generateHeader();
    return headerSource;
}

} // namespace
