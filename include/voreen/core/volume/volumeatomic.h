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

#ifndef VRN_VOLUMEATOMIC_H
#define VRN_VOLUMEATOMIC_H

// Note: please ensure that no OpenGL dependencies are added into this file

// This class is just a volume - just a plain volume.
// Would be great not to see some arcane dependencies here. (roland)

#include <algorithm>
#include <cstring>
#include <fstream>

#include "tgt/assert.h"
#include "tgt/logmanager.h"

#include "voreen/core/volume/volume.h"
#include "voreen/core/volume/volumeelement.h"

#include <typeinfo>

namespace voreen {

template<class T>
class VolumeAtomic : public Volume {
public:
    typedef T VoxelType;

    enum {
        BYTES_PER_VOXEL = sizeof(T),
        BITS_PER_VOXEL  = BYTES_PER_VOXEL * 8
    };

    /*
     * constructors and destructor
     */

    /**
     * While using this constructor the class will automatically allocate
     * an appropiate chunk of memory. This memory will be deleted by this class.
     */
    VolumeAtomic(const tgt::ivec3& dimensions,
                 const tgt::vec3& spacing = tgt::vec3(1.f),
                 int bitsStored = BITS_PER_VOXEL) throw (std::bad_alloc);

    /**
     * While using this constructor the class will use an preallocated chunk of
     * of memory given in \p data. This memory will be deleted by this class.
     */
    VolumeAtomic(T* data,
                 const tgt::ivec3& dimensions,
                 const tgt::vec3& spacing = tgt::vec3(1.f),
                 int bitsStored = BITS_PER_VOXEL);

    /// Deletes the \a data_ array
    ~VolumeAtomic();

    virtual VolumeAtomic<T>* clone() const throw (std::bad_alloc);
    virtual VolumeAtomic<T>* clone(void* data) const throw (std::bad_alloc);

    /*
     * getters and setters
     */

    virtual int getBitsAllocated() const {
        return BITS_PER_VOXEL;
    }
    virtual int getNumChannels() const {
        return VolumeElement<T>::getNumChannels();
    }
    virtual int getBytesPerVoxel() const {
        return BYTES_PER_VOXEL;
    }
    bool isSigned() {
        return isSigned_;
    }
    virtual size_t getNumBytes() const {
        return sizeof(T) * numVoxels_;
    }

    void setZeroPoint(T zeroPoint) {
        isSigned_ = true;
        zeroPoint_ = zeroPoint;
    }

    T getZeroPoint() {
        tgtAssert(isSigned_, "this is not a signed volume");
        return zeroPoint_;
    }

    /*
     * helpers for calculating the position in 3d - MUST BE INLINE for performance
     */
    inline static size_t calcPos(const tgt::ivec3& dimensions, size_t x, size_t y, size_t z) {
        return z*dimensions.x*dimensions.y + y*dimensions.x + x;
    }

    inline static size_t calcPos(const tgt::ivec3& dimensions, const tgt::ivec3& pos) {
        return pos.z*dimensions.x*dimensions.y + pos.y*dimensions.x + pos.x;
    }

    /*
      Methods for accessing the voxels - MUST BE INLINE for performance.
      tgtAssert will be away in the release version. Most compilers like gcc ignore
      inline functions in the debug version. So there will be no overhead in the
      debug version.
    */

    /// just get a proper pointer for read and write access
    inline T* voxel() {
        return data_;
    }
    /// just get a proper pointer for read access only
    inline const T* voxel() const {
        return data_;
    }
    /// get or set voxel
    inline T& voxel(size_t x, size_t y, size_t z) {
        tgtAssert( x < size_t(dimensions_.x), "x index out of bounds" );
        tgtAssert( y < size_t(dimensions_.y), "y index out of bounds" );
        tgtAssert( z < size_t(dimensions_.z), "z index out of bounds" );

        return data_[calcPos(dimensions_, x, y, z)];
    }
    /// get voxel
    inline const T& voxel(size_t x, size_t y, size_t z) const {
        tgtAssert( x < size_t(dimensions_.x), "x index out of bounds" );
        tgtAssert( y < size_t(dimensions_.y), "y index out of bounds" );
        tgtAssert( z < size_t(dimensions_.z), "z index out of bounds" );

        return data_[calcPos(dimensions_, x, y, z)];
    }
    /// get or set voxel
    inline T& voxel(const tgt::ivec3& pos) {
        tgtAssert( pos.x < dimensions_.x, "x index out of bounds" );
        tgtAssert( pos.y < dimensions_.y, "y index out of bounds" );
        tgtAssert( pos.z < dimensions_.z, "z index out of bounds" );
        tgtAssert( pos.x >= 0, "x index out of bounds" );
        tgtAssert( pos.y >= 0, "y index out of bounds" );
        tgtAssert( pos.z >= 0, "z index out of bounds" );

        return data_[calcPos(dimensions_, pos)];
    }
    /// get voxel
    inline const T& voxel(const tgt::ivec3& pos) const {
        tgtAssert( pos.x < dimensions_.x, "x index out of bounds" );
        tgtAssert( pos.y < dimensions_.y, "y index out of bounds" );
        tgtAssert( pos.z < dimensions_.z, "z index out of bounds" );
        tgtAssert( pos.x >= 0, "x index out of bounds" );
        tgtAssert( pos.y >= 0, "y index out of bounds" );
        tgtAssert( pos.z >= 0, "z index out of bounds" );

        return data_[calcPos(dimensions_, pos)];
    }
    /// get or set voxel
    inline T& voxel(size_t i) {
        tgtAssert( i < numVoxels_, "index out of bounds" );

        return data_[i];
    }
    /// get voxel
    inline const T& voxel(size_t i) const {
        tgtAssert( i < numVoxels_, "index out of bounds" );

        return data_[i];
    }

    /*
     * getVoxelFloat and setVoxelFloat
     */

    virtual float getVoxelFloat(const tgt::ivec3& pos, size_t channel = 0) const;
    virtual float getVoxelFloat(size_t x, size_t y, size_t z, size_t channel = 0) const;
    virtual void setVoxelFloat(float value, const tgt::ivec3& pos, size_t channel = 0);
    virtual void setVoxelFloat(float value, size_t x, size_t y, size_t z, size_t channel = 0);

    /*
     * math methods
     */

    /**
     * Calculates the voxel wise difference of two volumes.
     *
     * Calculates:
     *
     *     (v1-v2) + maxValue
     * r = ------------------    if v1-v2 not equal zero,
     *             2
     *
     * r = 0                     otherwise.
     */
    void calcDifferences(const VolumeAtomic<T>& v1, const VolumeAtomic<T>& v2);

    /**
     * Calculates the voxel wise first derivative of two volumes.
     *
     * Calculates:
     *
     *     (v2-v1) + maxValue
     * r = ------------------    if v2-v1 not equal zero,
     *             2k
     *
     * r = 0                     otherwise.
     */
    void calc1stDerivative(const VolumeAtomic<T>& v1, const VolumeAtomic<T>& v2, int k);

    /**
     * Calculates the voxel wise second derivative of three volumes.
     *
     * Calculates:
     *
     *     (v3 - 2 v2 + v1) + maxValue
     * r = ---------------------------    if v3-2v2+v1 not equal zero,
     *                2k*k
     *
     * r = 0                     otherwise.
     */
    void calc2ndDerivative( const VolumeAtomic<T>& v1,
                            const VolumeAtomic<T>& v2,
                            const VolumeAtomic<T>& v3,
                            int k);

    /*
     * further methods
     */
    T min() const;
    T max() const;

    virtual void clear();
    virtual void* getData();
    virtual VolumeAtomic<T>* scale(const tgt::ivec3& newDims, Filter filter) const throw (std::bad_alloc);
    virtual VolumeAtomic<T>* createSubset(const tgt::ivec3& pos, const tgt::ivec3& size) const throw (std::bad_alloc);
    virtual VolumeAtomic<T>* mirrorZ() const throw (std::bad_alloc);

protected:
    // protected default constructor
    VolumeAtomic() {}

    T* data_;

    bool isSigned_;
    T zeroPoint_;
    mutable T maxValue_;
    mutable T minValue_;
    mutable bool minMaxValid_;
};

/*
 * typedefs for easy usage
 */

typedef VolumeAtomic<uint8_t>   VolumeUInt8;
typedef VolumeAtomic<uint16_t>  VolumeUInt16;
typedef VolumeAtomic<uint32_t>  VolumeUInt32;

typedef VolumeAtomic<int8_t>    VolumeInt8;
typedef VolumeAtomic<int16_t>   VolumeInt16;
typedef VolumeAtomic<int32_t>   VolumeInt32;

typedef VolumeAtomic<float>     VolumeFloat;
typedef VolumeAtomic<double>    VolumeDouble;

typedef VolumeAtomic<tgt::col4>                 Volume4xUInt8;
typedef VolumeAtomic<tgt::Vector4<int8_t> >     Volume4xInt8;
typedef VolumeAtomic<tgt::Vector4<uint16_t> >   Volume4xUInt16;
typedef VolumeAtomic<tgt::Vector4<int16_t> >    Volume4xInt16;

typedef VolumeAtomic<tgt::col3>                 Volume3xUInt8;
typedef VolumeAtomic<tgt::Vector3<int8_t> >     Volume3xInt8;
typedef VolumeAtomic<tgt::Vector3<uint16_t> >   Volume3xUInt16;
typedef VolumeAtomic<tgt::Vector3<int16_t> >    Volume3xInt16;

typedef VolumeAtomic<tgt::Vector2<uint16_t> >   Volume2xUInt16;
typedef VolumeAtomic<tgt::Vector2< int16_t> >   Volume2xInt16;

typedef VolumeAtomic<tgt::vec3>  Volume3xFloat;
typedef VolumeAtomic<tgt::dvec3> Volume3xDouble;

typedef VolumeAtomic<tgt::vec4>  Volume4xFloat;
typedef VolumeAtomic<tgt::dvec4> Volume4xDouble;

//------------------------------------------------------------------------------
//  non inline implementation
//------------------------------------------------------------------------------

/*
 * constructors and destructor
 */

template<class T>
VolumeAtomic<T>::VolumeAtomic(  const tgt::ivec3& dimensions,
                                const tgt::vec3& spacing,
                                int bitsStored) throw (std::bad_alloc)
  : Volume(dimensions, bitsStored, spacing),
    data_(0),
    isSigned_(false),
    zeroPoint_(VolumeElement<T>::getZero()),
    minMaxValid_(false)
{
    try {
        data_ = new T[numVoxels_];
    }
    catch (std::bad_alloc) {
        throw; // throw it to the caller
    }
}

template<class T>
VolumeAtomic<T>::VolumeAtomic(  T* data,
                                const tgt::ivec3& dimensions,
                                const tgt::vec3& spacing,
                                int bitsStored)
  : Volume(dimensions, bitsStored, spacing),
    data_(data),
    isSigned_(false),
    zeroPoint_(VolumeElement<T>::getZero()),
    minMaxValid_(false)
{}

template<class T>
VolumeAtomic<T>* VolumeAtomic<T>::clone() const throw (std::bad_alloc) {
    VolumeAtomic<T>* newVolume;
    try {
        newVolume = (VolumeAtomic<T>*) clone(0);
    }
    catch (std::bad_alloc) {
        throw; // throw it to the caller
    }

    // copy over the data
    memcpy( newVolume->data_, data_, getNumBytes() );

    return newVolume;
}

template<class T>
VolumeAtomic<T>* VolumeAtomic<T>::clone(void* data) const throw (std::bad_alloc) {
    VolumeAtomic<T>* newVolume;

    if (data)
        newVolume = new VolumeAtomic<T>((T*) data, dimensions_, spacing_, bitsStored_); // use preallocated data
    else {
        try {
            newVolume = new VolumeAtomic<T>(dimensions_, spacing_, bitsStored_); // allocate a chunk of data
        }
        catch (std::bad_alloc) {
            throw; // throw it to the caller
        }
    }

    // copy over zero point and meta data
    newVolume->setZeroPoint(zeroPoint_);
    newVolume->meta() = meta();

    return newVolume;
}

template<class T>
VolumeAtomic<T>::~VolumeAtomic() {
    delete[] data_;
}

/*
 * getVoxelFloat and setVoxelFloat
 */

template<class T>
float VolumeAtomic<T>::getVoxelFloat(const tgt::ivec3& pos, size_t channel) const {
    typedef typename VolumeElement<T>::BaseType Base;
    Base value = VolumeElement<T>::getChannel( voxel(pos), channel );
    Base zero  = VolumeElement<T>::getChannel( zeroPoint_, channel );
    
    Base max;
    if ( typeid(*this) == typeid(VolumeFloat) || typeid(*this) == typeid(VolumeDouble) ) 
        max = static_cast<Base>(1);
    else
        max = static_cast<Base>( (1ll << static_cast<unsigned long long>(bitsStored_)) - 1ll );

    return float(value - zero)/float(max);
}

template<class T>
float VolumeAtomic<T>::getVoxelFloat(size_t x, size_t y, size_t z, size_t channel) const {
    return getVoxelFloat(tgt::ivec3(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z)), channel);
}

template<class T>
void VolumeAtomic<T>::setVoxelFloat(float value, const tgt::ivec3& pos, size_t channel) {
    typedef typename VolumeElement<T>::BaseType Base;
    Base zero  = VolumeElement<T>::getChannel( zeroPoint_, channel );
    
    Base max;
    if (typeid(*this) == typeid(VolumeFloat) || typeid(*this) == typeid(VolumeDouble)) 
        max = static_cast<Base>(1);
    else
        max = static_cast<Base>( (1ll << static_cast<unsigned long long>(bitsStored_)) - 1ll );

    VolumeElement<T>::setChannel(Base(value * float(max)) + zero, voxel(pos), channel);
}

template<class T>
void  VolumeAtomic<T>::setVoxelFloat(float value, size_t x, size_t y, size_t z, size_t channel) {
    setVoxelFloat( value, tgt::ivec3(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z)), channel );
}

/*
 * math methods
 */

template<class T>
void VolumeAtomic<T>::calcDifferences(const VolumeAtomic<T>& v1, const VolumeAtomic<T>& v2) {
    tgtAssert(numVoxels_ == v1.numVoxels_, "numVoxels_ must fit here");
    tgtAssert(numVoxels_ == v2.numVoxels_, "numVoxels_ must fit here");

    for (size_t i = 0; i < numVoxels_; ++i) {
        int res = v2.voxel(i) - v1.voxel(i);
        // TODO I don't understand why max is needed here
        voxel(i) = res ? (res + VolumeElement<T>::max())/2 : 0;
    }
}

template<class T>
void VolumeAtomic<T>::calc1stDerivative(const VolumeAtomic<T>& v1, const VolumeAtomic<T>& v2, int k) {
    tgtAssert(numVoxels_ == v1.numVoxels_, "numVoxels_ must fit here");
    tgtAssert(numVoxels_ == v2.numVoxels_, "numVoxels_ must fit here");

    for (size_t i = 0; i < numVoxels_; ++i) {
        int res = v2.voxel(i) - v1.voxel(i);
        // TODO I don't understand why max is needed here
        voxel(i) = res ? ( res + VolumeElement<T>::max() )/(2*k) : 0;
    }
}

template<class T>
void VolumeAtomic<T>::calc2ndDerivative(
    const VolumeAtomic<T>& v1,
    const VolumeAtomic<T>& v2,
    const VolumeAtomic<T>& v3,
    int k)
{
    tgtAssert(numVoxels_ == v1.numVoxels_, "numVoxels_ must fit here");
    tgtAssert(numVoxels_ == v2.numVoxels_, "numVoxels_ must fit here");
    tgtAssert(numVoxels_ == v3.numVoxels_, "numVoxels_ must fit here");

    for (int i = 0; i < numVoxels_; ++i) {
        int res = v3.voxel(i) - 2*v2.voxel(i) + v1.voxel(i);
        // TODO I don't understand why max is needed here
        voxel(i) = res ? ( res + VolumeElement<T>::max() )/(2*k*k) : 0;
    }
}

/*
 * further methods
 */

template<class T>
T VolumeAtomic<T>::min() const {
    
    if ( !minMaxValid_ ) {
        minValue_ = *std::min_element(data_, data_ + getNumVoxels());
        maxValue_ = *std::max_element(data_, data_ + getNumVoxels());
        minMaxValid_ = true;
    }

    return minValue_;

}

template<class T>
T VolumeAtomic<T>::max() const {

    if ( !minMaxValid_ ) {
        minValue_ = *std::min_element(data_, data_ + getNumVoxels());
        maxValue_ = *std::max_element(data_, data_ + getNumVoxels());
        minMaxValid_ = true;
    }

    return maxValue_;
}

template<class T>
void VolumeAtomic<T>::clear() {
    memset(data_, 0, getNumBytes());
}

template<class T>
void* VolumeAtomic<T>::getData() {
    return reinterpret_cast<void*>(data_);
}

template<class T>
VolumeAtomic<T>* VolumeAtomic<T>::scale(const tgt::ivec3& newDims, Filter filter) const throw (std::bad_alloc) {
    using tgt::vec3;
    using tgt::ivec3;

    LINFO("resampling with sampledimensions from " << dimensions_ << " to " << newDims);

    // build target volume
    VolumeAtomic<T>* v;
    try {
         v = new VolumeAtomic(newDims, spacing_, bitsStored_);
    }
    catch (std::bad_alloc) {
        throw; // throw it to the caller
    }

    v->setZeroPoint(zeroPoint_);
    v->meta() = meta();

    vec3 ratio = vec3(dimensions_) / vec3(newDims);
    vec3 invDims = 1.f / vec3(dimensions_);

    ivec3 pos = ivec3::zero; // iteration variable
    vec3 nearest; // knows the new position of the target volume

    /*
        Filter from the source volume to the target volume.
    */
    switch (filter) {
    case NEAREST:
        for (pos.z = 0; pos.z < newDims.z; ++pos.z) {
            nearest.z = static_cast<float>(pos.z) * ratio.z;

            for (pos.y = 0; pos.y < newDims.y; ++pos.y) {
                nearest.y = static_cast<float>(pos.y) * ratio.y;

                for (pos.x = 0; pos.x < newDims.x; ++pos.x) {
                    nearest.x = static_cast<float>(pos.x) * ratio.x;
                    v->voxel(pos) = voxel( ivec3(nearest + 0.5f) ); // round and do the lookup
                }
            }
        }
        break;

    case LINEAR:
        for (pos.z = 0; pos.z < newDims.z; ++pos.z) {
            nearest.z = static_cast<float>(pos.z) * ratio.z;

            for (pos.y = 0; pos.y < newDims.y; ++pos.y) {
                nearest.y = static_cast<float>(pos.y) * ratio.y;

                for (pos.x = 0; pos.x < newDims.x; ++pos.x) {
                    nearest.x = static_cast<float>(pos.x) * ratio.x;
                    vec3 p = nearest - floor(nearest); // get decimal part
                    ivec3 llb = ivec3( nearest );
                    ivec3 urf = ivec3( ceil(nearest) );
                    urf = tgt::min(urf, dimensions_ - 1); // clamp so the lookups do not exceed the dimensions

                    /*
                      interpolate linearly
                    */
                    typedef typename VolumeElement<T>::DoubleType Double;
                    v->voxel(pos) =
                        T(Double(voxel(llb.x, llb.y, llb.z)) * static_cast<double>((1.f-p.x)*(1.f-p.y)*(1.f-p.z))  // llB
                          + Double(voxel(urf.x, llb.y, llb.z)) * static_cast<double>((    p.x)*(1.f-p.y)*(1.f-p.z))  // lrB
                          + Double(voxel(urf.x, urf.y, llb.z)) * static_cast<double>((    p.x)*(    p.y)*(1.f-p.z))  // urB
                          + Double(voxel(llb.x, urf.y, llb.z)) * static_cast<double>((1.f-p.x)*(    p.y)*(1.f-p.z))  // ulB
                          + Double(voxel(llb.x, llb.y, urf.z)) * static_cast<double>((1.f-p.x)*(1.f-p.y)*(    p.z))  // llF
                          + Double(voxel(urf.x, llb.y, urf.z)) * static_cast<double>((    p.x)*(1.f-p.y)*(    p.z))  // lrF
                          + Double(voxel(urf.x, urf.y, urf.z)) * static_cast<double>((    p.x)*(    p.y)*(    p.z))  // urF
                          + Double(voxel(llb.x, urf.y, urf.z)) * static_cast<double>((1.f-p.x)*(    p.y)*(    p.z)));// ulF
                }
            }
        }
        break;
    }

    return v;
}

template<class T>
VolumeAtomic<T>* VolumeAtomic<T>::createSubset(const tgt::ivec3& pos, const tgt::ivec3& size) const
    throw (std::bad_alloc)
{
    VolumeAtomic<T>* subset;
    try {
        subset = new VolumeAtomic<T>(size, spacing_, bitsStored_);
    }
    catch (std::bad_alloc) {
        throw; // throw it to the caller
    }

    subset->setZeroPoint(zeroPoint_);
    subset->meta() = meta();

    // calculate new imageposition
    if (pos.z != 0.f)
        subset->meta().setImagePositionZ(meta().getImagePositionZ() - spacing_.z * pos.z);

    LINFO("Create subset: " << size << " from position: " << pos);

    // create values for ranges less than zero and greater equal dimensions_
    subset->clear(); // TODO: This can be optomized by avoiding to clear the values in range

    // now the rest
    tgt::ivec3 start = tgt::max(pos, tgt::ivec3::zero);// clamp values
    tgt::ivec3 end   = tgt::min(pos + size, dimensions_);    // clamp values

    VRN_FOR_EACH_VOXEL(i, tgt::ivec3(0), end-start)
        subset->voxel(i) = voxel(i+start);

    return subset;
}

template<class T>
VolumeAtomic<T>* VolumeAtomic<T>::mirrorZ() const throw (std::bad_alloc) {
    VolumeAtomic<T>* mirror;
    try {
        mirror = clone(0);
    }
    catch (std::bad_alloc) {
        throw; // throw it to the caller
    }

    VRN_FOR_EACH_VOXEL(i, tgt::ivec3(0), dimensions_)
        mirror->voxel(i) = voxel(i.x, i.y, dimensions_.z-i.z-1);

    return mirror;
}

} // namespace voreen

#endif // VRN_VOLUMEATOMIC_H
