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

#include "modules/mod_sampler2d.frag"

uniform SAMPLER2D_TYPE shadeTex_;
uniform SAMPLER2D_TYPE depthTex_;
uniform TEXTURE_PARAMETERS textureParameters_;

uniform float saturation_;

vec4 toGrayScale(in vec4 color, in float saturation) {
    float brightness = (0.30 * color.r) + (0.59 * color.g) + (0.11 * color.b);
    vec4 grayscale = vec4(brightness, brightness, brightness, color.a);

    return mix(grayscale, color, saturation);
}

void main() {
    vec2 p = gl_FragCoord.xy * screenDimRCP_;
    gl_FragColor = toGrayScale(textureLookup2Dnormalized(shadeTex_, textureParameters_, p), saturation_);
    gl_FragDepth = textureLookup2Dnormalized(depthTex_, textureParameters_, p).z;
}
