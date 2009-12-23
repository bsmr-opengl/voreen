/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2009 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
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

#include "voreen/core/vis/voreenpainter.h"

#include "voreen/core/vis/network/networkevaluator.h"
#include "voreen/core/vis/network/processornetwork.h"
#include "voreen/core/vis/processors/image/canvasrenderer.h"

namespace voreen {

VoreenPainterOverlay::VoreenPainterOverlay(tgt::GLCanvas* canvas)
    : canvas_(canvas),
      activated_(false),
      activateable_(false),
      name_("Unnamed")
{}

//------------------------------------------------------------------------------

const std::string VoreenPainter::loggerCat_ = "voreen.core.VoreenPainer";

VoreenPainter::VoreenPainter(tgt::GLCanvas* canvas, NetworkEvaluator* evaluator, CanvasRenderer* canvasRenderer)
    : tgt::Painter(canvas)
    , evaluator_(evaluator)
    , canvasRenderer_(canvasRenderer)
{
    tgtAssert(canvas, "No canvas");
    tgtAssert(evaluator_, "No network evaluator");
    tgtAssert(canvasRenderer_, "No canvas renderer");

    overlayMgr_ = new OverlayManager();

    stereoMode_ = VRN_MONOSCOPIC;
}

VoreenPainter::~VoreenPainter() {
    delete overlayMgr_;
}

void VoreenPainter::initialize() {

    if (getCanvas()) {
        getCanvas()->getGLFocus();
    }
    else {
        LWARNING("initialize(): No canvas");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}

void VoreenPainter::sizeChanged(const tgt::ivec2& size) {

    getCanvas()->getGLFocus();

    tgt::ivec2 validSize = size;
    validSize.y = validSize.y ? validSize.y : 1;

    // setup viewport, projection etc.:
    glViewport(0, 0, static_cast<GLint>(validSize.x), static_cast<GLint>(validSize.y));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    tgtAssert(canvasRenderer_, "No canvas renderer");
    canvasRenderer_->portResized(0, size);

    if (overlayMgr_)
        overlayMgr_->resize(validSize.x, validSize.y);
}

void VoreenPainter::paint() {
    if (stereoMode_ == VRN_STEREOSCOPIC) { // TODO: resupport stereo
//         glDrawBuffer(GL_BACK_LEFT);
// //         camera->setStereo(true); <- TODO doesn't exist anymore, FL
//         camera->setEye(tgt::Camera::EYE_LEFT);
//         glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//         if (evaluator_)
//             evaluator_->render();
//         if (overlayMgr_)
//             overlayMgr_->paint();
//
//         // we must invalidate here or nothing will happen when we render the other eye-perspective
//         evaluator_->invalidate();
//
//         glDrawBuffer(GL_BACK_RIGHT);
//         camera->setEye(tgt::Camera::EYE_RIGHT);
//         glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//         if (evaluator_)
//             evaluator_->render();
//         if (overlayMgr_)
//             overlayMgr_->paint();
//
//         glDrawBuffer(GL_BACK);
// //         camera->setStereo(false); // doesn't exist anymore, FL
//         camera->setEye(tgt::Camera::EYE_MIDDLE);
    }
    else {
        if (getCanvas())
            getCanvas()->getGLFocus();
        if (evaluator_)
            evaluator_->process();
        if (overlayMgr_)
            overlayMgr_->paint();
    }
}

void VoreenPainter::repaint() {
    getCanvas()->repaint();
}

void VoreenPainter::renderToSnapshot(const std::string& fileName, const tgt::ivec2& size)
    throw (std::bad_alloc, tgt::FileException) {

    tgtAssert(canvasRenderer_, "No canvas renderer");
    canvasRenderer_->renderToImage(fileName, size);

}

void VoreenPainter::addCanvasOverlay(VoreenPainterOverlay* overlay) {
    overlayMgr_->removeOverlay(overlay);
    overlayMgr_->addOverlay(overlay);
    getCanvas()->getGLFocus();
    if (getCanvas())
        overlay->resize(getCanvas()->getSize().x, getCanvas()->getSize().y);
}

void VoreenPainter::delCanvasOverlay(VoreenPainterOverlay* overlay) {
    overlayMgr_->removeOverlay(overlay);
}

void VoreenPainter::setStereoMode(int stereoMode) {
    stereoMode_ = stereoMode;
}

NetworkEvaluator* VoreenPainter::getEvaluator() const {
    return evaluator_;
}

CanvasRenderer* VoreenPainter::getCanvasRenderer() const {
    return canvasRenderer_;
}

//------------------------------------------------------------------------

OverlayManager::OverlayManager() {
    overlays_.clear();
}

void OverlayManager::addOverlay(VoreenPainterOverlay* overlay) {
    overlays_.push_back(overlay);
}

void OverlayManager::removeOverlay(VoreenPainterOverlay* overlay) {
    std::vector<VoreenPainterOverlay*> temp;
    for (size_t i = 0; i < overlays_.size(); ++i) {
        if (overlays_.at(i) != overlay)
            temp.push_back(overlays_.at(i));
    }
    overlays_.clear();
    overlays_ = temp;
}

std::vector<VoreenPainterOverlay*> OverlayManager::getOverlays() {
    return overlays_;
}

void OverlayManager::paint() {
    for (size_t i = 0; i < overlays_.size(); ++i) {
        if (overlays_.at(i)->getIsActivated())
            overlays_.at(i)->paint();
    }
}

void OverlayManager::resize(int width, int height) {
    for (size_t i=0; i < overlays_.size(); ++i)
        overlays_.at(i)->resize(width,height);
}

} // namespace
