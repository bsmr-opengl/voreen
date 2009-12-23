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

#include "modules/vrn_shaderincludes.frag"

// variables for storing compositing results
vec4 result = vec4(0.0);
vec4 firstHit = vec4(0.0);

// declare entry and exit parameters
uniform SAMPLER2D_TYPE entryPoints_;            // ray entry points
uniform SAMPLER2D_TYPE entryPointsDepth_;       // ray entry points depth
uniform SAMPLER2D_TYPE exitPoints_;                // ray exit points
uniform SAMPLER2D_TYPE exitPointsDepth_;        // ray exit points depth
// declare volume
uniform sampler3D volume_;                      // volume data set
uniform VOLUME_PARAMETERS volumeParameters_;    // texture lookup parameters for volume_
uniform vec3 p1_;
uniform vec3 p2_;
uniform vec3 p3_;


/***
 * Performs the ray traversal
 * returns the final fragment color.
 ***/
void rayTraversal(in vec3 first, in vec3 last) {

    // calculate the required ray parameters
    float t     = 0.0;
    float tIncr = 0.0;
    float tEnd  = 1.0;
    vec3 rayDirection;
    raySetup(first, last, rayDirection, tIncr, tEnd);

    RC_BEGIN_LOOP {
        vec3 samplePos = first + t * rayDirection;
        vec4 voxel = getVoxel(volume_, volumeParameters_, samplePos);

        // apply masking
        if (RC_NOT_MASKED(samplePos, voxel.a)) {
            vec4 color;
            if(distance(samplePos, p1_) < 0.01) {
                color = vec4(1.0,0.0,0.0,1.0);
            }
            else if(distance(samplePos, p2_) < 0.01) {
                color = vec4(0.0,1.0,0.0,1.0);
            }
            else if(distance(samplePos, p3_) < 0.01) {
                color = vec4(0.0,0.0,1.0,1.0);
            }
            else {
                // calculate gradients
                voxel.xyz = RC_CALC_GRADIENTS(voxel.xyz, samplePos, volume_, volumeParameters_, t, rayDirection, entryPoints_);

                // apply classification
                color = RC_APPLY_CLASSIFICATION(transferFunc_, voxel);

                // apply shading
                color.rgb = RC_APPLY_SHADING(voxel.xyz, samplePos, volumeParameters_, color.rgb, color.rgb, color.rgb);
            }

            // if opacity greater zero, apply compositing
            if (color.a > 0.0) {
                result = RC_APPLY_COMPOSITING(result, color, samplePos, voxel.xyz, t, tDepth);
                firstHit = compositeFHP(samplePos, firstHit, t, tDepth);
            }
        }
    } RC_END_LOOP(result);
}

/***
 * The main method.
 ***/
void main() {

    vec3 frontPos = textureLookup2D(entryPoints_, gl_FragCoord.xy).rgb;
    vec3 backPos = textureLookup2D(exitPoints_, gl_FragCoord.xy).rgb;

    // initialize light and material parameters
    matParams = gl_FrontMaterial;

    // determine whether the ray has to be casted
    if (frontPos == backPos)
        // background needs no raycasting
        discard;
    else
        // fragCoords are lying inside the bounding box
        rayTraversal(frontPos, backPos);

    /*
    #ifdef TONE_MAPPING_ENABLED
        result.r = 1.0 - exp(-result.r * TONE_MAPPING_VALUE);
        result.g = 1.0 - exp(-result.g * TONE_MAPPING_VALUE);
        result.b = 1.0 - exp(-result.b * TONE_MAPPING_VALUE);
    #endif
    */

    gl_FragData[0] = result;
    gl_FragData[1] = firstHit;
}
