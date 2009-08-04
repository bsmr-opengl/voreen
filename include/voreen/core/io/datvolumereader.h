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

#ifndef VRN_DATVOLUMEREADER_H
#define VRN_DATVOLUMEREADER_H

#include "voreen/core/io/volumereader.h"

namespace voreen {

/**
 * Reader for <tt>.dat</tt> files. These files can either be volume description files (the actual
 * volume data is contained in a corresponding <tt>.raw</tt>) or they may be volumes of their own.
 * These two types can be distinguished by their size and the contents of the first 6 bytes. The
 * format used for the .dat-volumes says that the first six bytes contain the information about
 * the dimension of the volume. If the file size is different from the multiplicated value of these
 * dimensions, it must be a meta description file.
 *
 * The following tags are understood by the reader:
 * - <tt>ObjectFileName</tt>: Name of the corresponding <tt>.raw</tt> file. The name may not
 *    contain whitespaces.
 * - <tt>TaggedFileName</tt>: ignored
 * - <tt>Resolution</tt>: Number of voxels in x-, y- and z-direction
 * - <tt>SliceThickness</tt>: Thickness for x-, y- and z-direction
 * - <tt>Format</tt>: Data format like \c UCHAR, \c USHORT, \c FLOAT. Must be supported by the
 *   RawVolumeReader, see RawVolumeReader::readHints() for details.
 * - <tt>NbrTags</tt>: ignored
 * - <tt>ObjectType</tt>: ignored
 * - <tt>ObjectModel</tt>: \c I (intensity) or \c RGBA or \c RGB
 * - <tt>GridType</tt>: only \c EQUIDISTANT is supported
 * - <tt>BitsStored</tt>: specifies logical data type, e.g. 12 for 12 bit CT data where each
 *   voxel is stored in a 16 bit \c short.
 * - <tt>ZeroPoint</tt>: offset for the voxel data for supporting signed data
 * - <tt>MetaString</tt>: a string containing arbitrary meta-data
 *
 * Example file <tt>nucleon.raw</tt>:
 \verbatim
ObjectFileName: nucleon.raw
TaggedFileName: ---
Resolution:     41 41 41
SliceThickness: 1.0 1.0 1.0
Format:         UCHAR
NbrTags:        0
ObjectType:     TEXTURE_VOLUME_OBJECT
ObjectModel:    I
GridType:       EQUIDISTANT
 \endverbatim
 */
class DatVolumeReader : public VolumeReader {
public:
    DatVolumeReader(IOProgress* progress = 0);

    virtual VolumeSet* read(const std::string& fileName)
        throw (tgt::FileException, std::bad_alloc);

    virtual VolumeSet* readSlices(const std::string& fileName, size_t firstSlice=0, size_t lastSlice=0)
        throw (tgt::FileException, std::bad_alloc);

    virtual VolumeSet* readBrick(const std::string& fileName, tgt::ivec3 brickStartPos, int brickSize)
        throw(tgt::FileException, std::bad_alloc);

private:
    static const std::string loggerCat_;

    VolumeSet* readVolumeFile(const std::string& fileName, const tgt::ivec3& dims,size_t firstSlice, size_t lastSlice)
        throw (tgt::FileException, std::bad_alloc);

    VolumeSet* readVolumeFileBrick(const std::string& fileName,const tgt::ivec3& dims, tgt::ivec3 brickStartPos,
        int brickSize) throw (tgt::FileException, std::bad_alloc);

    VolumeSet* readMetaFile(const std::string& fileName,size_t firstSlice, size_t lastSlice)
        throw (tgt::FileException, std::bad_alloc);

    VolumeSet* readMetaFileBrick(const std::string& fileName, tgt::ivec3 brickStartPos, int brickSize)
        throw (tgt::FileException, std::bad_alloc);
};

} // namespace voreen

#endif // VRN_DATVOLUMEREADER_H
