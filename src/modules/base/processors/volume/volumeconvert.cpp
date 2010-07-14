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

#include "voreen/modules/base/processors/volume/volumeconvert.h"
#include "voreen/core/datastructures/volume/gradient.h"

namespace voreen {

const std::string VolumeConvert::loggerCat_("voreen.VolumeConvert");

VolumeConvert::VolumeConvert()
    : VolumeProcessor(),
    inport_(Port::INPORT, "volumehandle.input"),
    outport_(Port::OUTPORT, "volumehandle.output", 0),
    conversion_("conversion", "Conversion")
{
    addPort(inport_);
    addPort(outport_);

    conversion_.addOption("8", "8 bit");
    conversion_.addOption("12", "12 bit");
    conversion_.addOption("16", "16 bit");
    addProperty(conversion_);
}

VolumeConvert::~VolumeConvert() {}

Processor* VolumeConvert::create() const {
    return new VolumeConvert();
}

std::string VolumeConvert::getProcessorInfo() const {
    return std::string("Converts a volume to [8,12,16] bit.");
}

void VolumeConvert::process() {
    Volume* inputVolume = inport_.getData()->getVolume();
    Volume* outputVolume = 0;

    if (conversion_.get() == "8") {
        outputVolume = new VolumeUInt8(inputVolume->getDimensions());
        outputVolume->convert(inputVolume);
    }
    else if (conversion_.get() == "12") {
        outputVolume = new VolumeUInt16(inputVolume->getDimensions(), inputVolume->getSpacing(), 12);
        outputVolume->convert(inputVolume);
    }
    else if (conversion_.get() == "16") {
        outputVolume = new VolumeUInt16(inputVolume->getDimensions());
        outputVolume->convert(inputVolume);
    }
    else {
        LERROR("Unknown conversion");
    }

    // assign computed volume to outport
    if (outputVolume)
        outport_.setData(new VolumeHandle(outputVolume), true);
    else
        outport_.deleteVolume();
}

void VolumeConvert::deinitialize() throw (VoreenException) {
    outport_.deleteVolume();

    VolumeProcessor::deinitialize();
}

}   // namespace
