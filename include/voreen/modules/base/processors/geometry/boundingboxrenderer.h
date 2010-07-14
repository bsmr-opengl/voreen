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

#ifndef VRN_BOUNDINGBOXRENDERER_H
#define VRN_BOUNDINGBOXRENDERER_H

#include "voreen/core/processors/geometryrendererbase.h"

namespace voreen {

///Draws bounding box around the data set
class BoundingBoxRenderer : public GeometryRendererBase {
public:
    BoundingBoxRenderer();

    void setLineWidth(float width);
    ///Set the stipplePattern to be used. @see OpenGL docs
    void setStipplePattern(int stippleFactor, int stipplePattern);
    virtual std::string getCategory() const { return "Geometry"; }
    virtual std::string getClassName() const { return "BoundingBox"; }
    virtual Processor::CodeState getCodeState() const { return CODE_STATE_STABLE; }
    virtual std::string getProcessorInfo() const;
    virtual Processor* create() const {return new BoundingBoxRenderer();}

    virtual void render();
private:
    ColorProperty bboxColor_;
    FloatProperty width_;
    IntProperty stippleFactor_;
    IntProperty stipplePattern_;
    BoolProperty showGrid_;
    IntVec3Property tilesProp_;
    BoolProperty applyDatasetTransformationMatrix_;

    VolumePort inport_;
};

}

#endif
