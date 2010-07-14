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

#include "voreen/core/interaction/voreentrackball.h"

#include <cmath>

#include "tgt/matrix.h"
#include "tgt/vector.h"
#include "tgt/assert.h"
#include "tgt/quaternion.h"
#include "tgt/camera.h"

namespace voreen {

using tgt::Camera;
using tgt::Quaternion;
using tgt::vec3;
using tgt::vec2;
using tgt::mat4;
using tgt::quat;


VoreenTrackball::VoreenTrackball(Camera* camera)
    : camera_(camera),
      moveCenter_(true),
      size_(1.f)
{

    tgtAssert(camera, "No camera");

    center_ = camera_->getFocus();

    //saveCameraParameters();

}

VoreenTrackball::~VoreenTrackball() {
}

/*void VoreenTrackball::reset() {
    getCamera()->positionCamera(cameraPosition_, cameraFocus_, cameraUpVector_);
} */


/* Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 if we are away from the center of the sphere. */
vec3 VoreenTrackball::projectToSphere(const vec2& xy) const {

    static const float sqrt2 = sqrtf(2.f);
    vec3 result;

    float d = length(xy);
    if (d < size_ * sqrt2 / 2.f) {
        // Inside sphere
        // The factor "sqrt2/2.f" make a smooth changeover from sphere to hyperbola. If we leave
        // factor 1/sqrt(2) away, the trackball would bounce at the changeover.
        result.z = sqrtf(size_ * size_ - d*d);
    }
    else {
        // On hyperbola
        float t = size_ / sqrt2;
        result.z = t*t / d;
    }

    result.x = xy.x;
    result.y = xy.y;
    return normalize(result);
}

vec3 VoreenTrackball::coordTransform(const vec3& axis) const {
    mat4 rotation;
    camera_->getRotateMatrix().invert(rotation);
    return rotation * axis;
}

void VoreenTrackball::rotate(Quaternion<float> quat) {
    vec3 position = camera_->getPosition();
    position -= center_;
    position = quat::rotate(position, quat);
    position += center_;

    vec3 focus = camera_->getFocus();
    focus -= center_;
    // Usually focus - center == 0, so no need to rotate. But if we combine trackball
    // with some other navigations, this might be useful.
    focus = quat::rotate(focus, quat);
    focus += center_;

    vec3 upVector = camera_->getUpVector();
    upVector = quat::rotate(upVector, quat);

    camera_->positionCamera(position, focus, upVector);

    lastOrientationChange_ = quat;

}


void VoreenTrackball::rotate(vec3 axis, float phi) {
    // use coordTransform to get axis in world coordinates according to the axis given in camera coordinates
    rotate(quat::createQuat(phi, coordTransform(axis)));
}

void VoreenTrackball::rotate(const tgt::vec2& newMouse, const tgt::vec2& oldMouse) {

/* Project the points onto the virtual trackball,
 * then figure out the axis of rotation, which is the cross
 * product of P1-P2 and O-P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball -- it is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center (projectToSphere does that job).  This particular function was
 * chosen after trying out several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0) */

    if (newMouse == oldMouse) {
        // Zero rotation -> do nothing
        return;
    }

    // First, figure out z-coordinates for projection of P1 and P2 to deformed sphere
    vec3 p1 = projectToSphere(oldMouse);
    vec3 p2 = projectToSphere(newMouse);

    // Now, find the axis we are going to rotate about
    vec3 axis = cross(p2, p1);

    //... and calculate the angle phi between the two vectors which is the
    //  angle we need to rotate about
    vec3 d = p1 - p2;
    float t = length(d) / (2.0f * getSize());

    // avoid problems with out-of-control values...
    if (t > 1.0f) t = 1.0f;
    if (t < -1.0f) t = -1.0f;

    float phi = 2.f * asinf(t);

    rotate(axis, phi);
}

void VoreenTrackball::move(float length, vec3 axis) {

    if (length == 0.f || tgt::length(axis) == 0.f)
        return;

    float frustFactor = camera_->getFocalLength() / camera_->getFrustum().getNearDist();
    float frustWidth  = ( camera_->getFrustum().getRight()
                          - camera_->getFrustum().getLeft() );
    float frustHeight = ( camera_->getFrustum().getTop()
                          - camera_->getFrustum().getBottom() );
    axis = normalize(axis) * length * frustFactor;
    axis.x *= frustWidth;
    axis.y *= frustHeight;

    // find axis in world coordinates according to the axis given in camera coordinates
    axis = coordTransform(axis);

    moveCamera(-axis);
    if (moveCenter_) {
        center_ -= axis;
    };

}

void VoreenTrackball::move(const vec2& newMouse, const vec2& lastMouse) {
    vec2 mouseMotion = newMouse - lastMouse;
    vec3 axis;
    axis.x = mouseMotion.x;
    axis.y = mouseMotion.y;
    axis.z = 0;

    move(length(mouseMotion), axis);
}

void VoreenTrackball::moveCamera(const vec3& motionvector) {
    camera_->setPosition(camera_->getPosition() + motionvector);
    camera_->setFocus(camera_->getFocus() + motionvector);
}

void VoreenTrackball::zoom(float factor) {
    // zoom factor is inverse proportional to scaling of the look vector, so invert:
    factor = 1.f / factor;
    camera_->setPosition( (1.f-factor) * camera_->getFocus()
                              + factor * camera_->getPosition());
    //camera_->update();
}

void VoreenTrackball::zoom(const tgt::vec2& newMouse, const tgt::vec2& lastMouse, const vec2& mouseZoomInDirection) {
    zoom( 1 + dot( (lastMouse-newMouse), mouseZoomInDirection) );
}

void VoreenTrackball::zoomAbsolute(float focallength) {
   camera_->setPosition( camera_->getFocus() - focallength * camera_->getLook());
}

float VoreenTrackball::getCenterDistance() {
    return dot( center_ - camera_->getPosition(), camera_->getLook() );
}
/*
void VoreenTrackball::saveCameraParameters() {
    if (camera_) {
        cameraPosition_ = getCamera()->getPosition();
        cameraFocus_ = getCamera()->getFocus();
        cameraUpVector_ = getCamera()->getUpVector();
    }
    else {
        cameraPosition_ = vec3(0.f);
        cameraFocus_ = vec3(0.f, 0.f, -1.f);
        cameraUpVector_ = vec3(0.f, 1.f, 0.f);
    }
} */


float VoreenTrackball::getRotationAngle(float acuteness) const {
    return 1.f / acuteness;
}

float VoreenTrackball::getMovementLength(float acuteness) const {
    return getSize() / acuteness;
}

float VoreenTrackball::getZoomFactor(float acuteness, bool zoomIn) const {
    if (zoomIn) {
        return 1.f + 1.f/acuteness;
    }
    else {
        return 1.f - 1.f/acuteness;
    }
}

float VoreenTrackball::getRollAngle(float acuteness, bool left) const {
    if (left) {
        return - 1.f / acuteness;
    }
    else {
        return 1.f / acuteness;
    }
}

tgt::Camera* VoreenTrackball::getCamera() const {
    return camera_;
}

void VoreenTrackball::setCamera(tgt::Camera* camera) {
    tgtAssert(camera, "No camera");
    camera_ = camera;
    center_ = camera_->getFocus();
}

} // namespace