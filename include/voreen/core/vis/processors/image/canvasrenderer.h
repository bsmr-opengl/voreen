/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2008 Visualization and Computer Graphics Group, *
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

#ifndef VRN_CANVASRENDERER_H
#define VRN_CANVASRENDERER_H

#include "tgt/glmath.h"

#include "voreen/core/vis/processors/processor.h"
#include "voreen/core/vis/processors/image/copytoscreenrenderer.h"

namespace voreen {

/**
 * A CanvasRenderer is the last processor in a network. Its only purpose is to copy
 * its input to the finaltarget of texture container. It inherits from CopyToScreenRenderer
 * because of the coarseness properties they both share. 
 */
//TODO: Since the CopyToScreenRenderer is more or less deprecated now, that functionality
//should be shifted to the CanvasRenderer and CoarsenessRenderer in my opinion (Stephan)
class CanvasRenderer : public CopyToScreenRenderer {
public:
	CanvasRenderer();
    ~CanvasRenderer();
    virtual void process(LocalPortMapping* portMapping);

	virtual const Identifier getClassName() const { return "Miscellaneous.Canvas"; }
	virtual const std::string getProcessorInfo() const;
    virtual Processor* create() { return new CanvasRenderer(); }
};

/**
 * A CacheRenderer is the last processor in a network. Its only purpose is to copy
 * its input to the finaltarget of texture container. It inherits from CopyToScreenRenderer
 * because of the coarseness properties they both share. But since the CopyToScreenRenderer
 * is more or less deprecated now, that functionality should be shifted to the CanvasRenderer
 * and CoarsenessRenderer in my opinion (Stephan)
 */
//TODO: include this functionality into CanvasRenderer. joerg
class CacheRenderer : public CopyToScreenRenderer {
public:
	CacheRenderer();
    ~CacheRenderer();

	virtual void process(LocalPortMapping* portMapping);

	virtual const Identifier getClassName() const { return "Miscellaneous.CacheRenderer"; }
	virtual const std::string getProcessorInfo() const;
    virtual Processor* create() { return new CacheRenderer(); }
};

// ---------------------------------------------------------------------------

/**
 * A NullRenderer is the last processor in a network. Its only purpose is to terminate a
 * network and to keep the id of the rendered image from the texture container.
 * This processor does neither create any output nor does it render to the frame buffer!
 *
 * @author  Dirk Feldmann, October 2008
 */
//TODO: simply allow multiple CanvasRenderers
class NullRenderer : public CopyToScreenRenderer {
public:
	NullRenderer();
    ~NullRenderer();
    virtual void process(LocalPortMapping* portMapping);

	virtual const Identifier getClassName() const { return "Miscellaneous.NullCanvas"; }
	virtual const std::string getProcessorInfo() const;
    virtual Processor* create() { return new NullRenderer(); }

    /**
     * Returns the ID of the image which will be copied to the frame buffer in order
     * to enable others to access the render target from the TextureContainer where
     * the final image is held. (added by Dirk for prosem SS 2008)
     */
    int getImageID() { return imageID_; }

protected:
    int imageID_;   /** render target holding the final image within TextureContainer. */
};

} // namespace voreen

#endif //VRN_CANVASRENDERER_H
