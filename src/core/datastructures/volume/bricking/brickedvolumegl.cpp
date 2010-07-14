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

#include "voreen/core/datastructures/volume/bricking/brickedvolumegl.h"

namespace voreen {

BrickedVolumeGL::BrickedVolumeGL(BrickedVolume* brickedVolume)
    : VolumeGL(brickedVolume->getEepVolume())
    , packedVolumeGL_(0)
    , indexVolumeGL_(0)
{
    packedVolumeGL_ = new VolumeGL(brickedVolume->getPackedVolume());
    indexVolumeGL_ = new VolumeGL(brickedVolume->getIndexVolume());
}

BrickedVolumeGL::~BrickedVolumeGL() {
    delete packedVolumeGL_;
    delete indexVolumeGL_;
}

VolumeGL* BrickedVolumeGL::getPackedVolumeGL() {
    return packedVolumeGL_;
}

VolumeGL* BrickedVolumeGL::getIndexVolumeGL() {
    return indexVolumeGL_;
}

void BrickedVolumeGL::setPackedVolumeGL(VolumeGL* packedVolumeGL) {
    packedVolumeGL_ = packedVolumeGL;
}

void BrickedVolumeGL::setIndexVolumeGL(VolumeGL* indexVolumeGL) {
    indexVolumeGL_ = indexVolumeGL;
}

} //namespace
