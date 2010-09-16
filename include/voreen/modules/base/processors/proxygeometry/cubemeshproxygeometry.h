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

#ifndef VRN_CUBEMESHPROXYGEOMETRY_H
#define VRN_CUBEMESHPROXYGEOMETRY_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/allports.h"

#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/buttonproperty.h"

namespace voreen {

class MeshListGeometry;

/**
 * Provides a cube mesh proxy geometry with axis-aligned clipping.
 *
 * @see MeshEntryExitPoints
 * @see MeshClipping
 */
class CubeMeshProxyGeometry : public Processor {
public:
    CubeMeshProxyGeometry();
    virtual ~CubeMeshProxyGeometry();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "CubeMeshProxyGeometry"; }
    virtual std::string getCategory() const  { return "Volume Proxy Geometry"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE; }

    virtual std::string getProcessorInfo() const;

protected:
    virtual void process();

private:
    /// Ensure useful clipping planes on clip right property change.
    void onClipRightChange();

    /// Ensure useful clipping planes on clip left property change.
    void onClipLeftChange();

    /// Ensure useful clipping planes on clip front property change.
    void onClipFrontChange();

    /// Ensure useful clipping planes on clip back property change.
    void onClipBackChange();

    /// Ensure useful clipping planes on clip bottom property change.
    void onClipBottomChange();

    /// Ensure useful clipping planes on clip top property change.
    void onClipTopChange();

    /// Resets clipping plane parameters to extremal values.
    void resetClipPlanes();

    /// Adapt ranges of clip plane properties to the input volume's dimensions.
    void adjustClipPropertiesRanges();

    /// Adjust visibility of the clipping plane properties according to enableClipping_ property.
    void adjustClipPropertiesVisibility();

    VolumePort inport_;              ///< Inport for the dataset the proxy is generated for.
    GeometryPort outport_;           ///< Outport for the generated cube mesh proxy geometry.

    BoolProperty enableClipping_;    ///< Clipping enable / disable property.
    FloatProperty clipRight_;        ///< Right clipping plane position (x).
    FloatProperty clipLeft_;         ///< Left clipping plane position (-x).
    FloatProperty clipFront_;        ///< Front clipping plane position (y).
    FloatProperty clipBack_;         ///< Back clipping plane position property (-y).
    FloatProperty clipBottom_;       ///< Bottom clipping plane position property (z).
    FloatProperty clipTop_;          ///< Top clipping plane position property (-z).
    ButtonProperty resetClipPlanes_; ///< Reset clipping plane parameters to extremal values.

    tgt::ivec3 oldVolumeDimensions_;

    /// Cube mesh proxy geometry generated by this processor.
    MeshListGeometry* geometry_;

    /// Category used for logging.
    static const std::string loggerCat_;
};

} // namespace

#endif // VRN_CUBEMESHPROXYGEOMETRY_H
