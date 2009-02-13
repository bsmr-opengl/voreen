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

#ifndef VRN_SLICEENTRYPOINTS_H
#define VRN_SLICEENTRYPOINTS_H


#include "voreen/core/vis/processors/render/entryexitpoints.h"
#include "voreen/core/vis/processors/portmapping.h"

#include "tgt/glmath.h"


namespace voreen {


/**
 * Calculates entry points for multiple slices arranged in 3D.
 */
class SliceEntryPoints : public EntryExitPoints {
public:
	SliceEntryPoints();
    virtual ~SliceEntryPoints();

	virtual const Identifier getClassName() const {return "EntryExitPoints.SliceEntryPoints";}
	virtual const std::string getProcessorInfo() const;
    virtual Processor* create() {return new SliceEntryPoints();}

    virtual std::string generateHeader();

    /**
    * Process voreen message, accepted identifiers:
    * - switch.virtualClipplane
    */
    virtual void processMessage(Message* msg, const Identifier& dest=Message::all_);
	virtual void process(LocalPortMapping* portMapping);
    
    void setNeedToReRender(bool needed);
protected:
    bool needToReRender_;
};

} // namespace voreen

#endif //VRN_SLICEENTRYPOINTS_H
