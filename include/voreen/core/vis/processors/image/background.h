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

#ifndef VRN_BACKGROUND_H
#define VRN_BACKGROUND_H

#include "voreen/core/vis/processors/image/imageprocessor.h"

namespace voreen {

/**
 * Adds a background to the image.
 */
class Background : public ImageProcessor {
public:
    Background();
    ~Background();

    virtual std::string getCategory() const { return "Image Processing"; }
    virtual std::string getClassName() const { return "Background"; }
    virtual std::string getModuleName() const { return "core"; }
    virtual Processor::CodeState getCodeState() const { return CODE_STATE_STABLE; }
    virtual const std::string getProcessorInfo() const;
    virtual Processor* create() const;

    virtual void initialize() throw (VoreenException);

    /**
     * draws the existing content over a background
     */
    void process();
    virtual bool isReady() const;

protected:

    /**
     * Render the background
     */
    void renderBackground();

    void onBackgroundModeChanged();

    /**
     * load (and create) needed textures
     */
    void loadTexture();

    /**
     * create an alpha-circle
     */
    void createRadialTexture();

    /**
     * create a cloud texture
     */
    void createCloudTexture();

    ColorProperty firstcolor_;
    ColorProperty secondcolor_;
    IntProperty angle_;
    tgt::Texture* tex_;
    bool textureLoaded_;
    FileDialogProperty filename_;
    FloatProperty tile_;
    StringOptionProperty modeProp_;

    RenderPort inport_;
    RenderPort outport_;
    RenderPort privatePort_;
};

} // namespace

#endif
