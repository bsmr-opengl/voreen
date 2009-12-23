#include "voreen/modules/flowreen/simpletexture.h"
#include <memory.h>

namespace voreen {

template<typename T>
SimpleTexture<T>::SimpleTexture(const tgt::ivec2& dimensions, const bool autoFree)
    : dimensions_(dimensions.x, dimensions.y, 1),
    numElements_(dimensions_.x * dimensions_.y),
    data_(new T[numElements_]),
    autoFree_(autoFree)
{
    memset(data_, 0, sizeof(T) * numElements_);
}

template<typename T>
SimpleTexture<T>::SimpleTexture(const tgt::ivec3& dimensions, const bool autoFree)
    : dimensions_((dimensions.z >= 1) ? dimensions : tgt::ivec3(dimensions.x, dimensions.y, 1)),
    numElements_(dimensions_.x * dimensions_.y * dimensions_.z),
    data_(new T[numElements_]),
    autoFree_(autoFree)
{
    memset(data_, 0, sizeof(T) * numElements_);
}

template<typename T>
SimpleTexture<T>::~SimpleTexture() {
    if (autoFree_ == true)
        free();
}

template<typename T>
void SimpleTexture<T>::free() {
    delete [] data_;
}

template<typename T>
const T& SimpleTexture<T>::operator[](size_t index) const {
    if (index >= numElements_)
        index = numElements_ - 1;
    return data_[index];
}

template<typename T>
const T& SimpleTexture<T>::operator[](const tgt::ivec2& position) const {
    size_t index = positionToIndex(position);
    return operator[](index);
}

template<typename T>
const T& SimpleTexture<T>::operator[](const tgt::ivec3& position) const {
    size_t index = positionToIndex(position);
    return operator[](index);
}

template<typename T>
T& SimpleTexture<T>::operator[](size_t index) {
    if (index >= numElements_)
        index = numElements_ - 1;
    return data_[index];
}

template<typename T>
T& SimpleTexture<T>::operator[](const tgt::ivec2& position) {
    size_t index = positionToIndex(position);
    return operator[](index);
}

template<typename T>
T& SimpleTexture<T>::operator[](const tgt::ivec3& position) {
    size_t index = positionToIndex(position);
    return operator[](index);
}

template<typename T>
void SimpleTexture<T>::createWhiteNoise() {
    for (size_t i = 0; i < numElements_; ++i)
        data_[i] = T(rand());
}

template<>
void SimpleTexture<float>::createWhiteNoise() {
    for (size_t i = 0; i < numElements_; ++i)
        data_[i] = rand() / static_cast<float>(RAND_MAX);
}

template<>
void SimpleTexture<unsigned char>::createWhiteNoise() {
    for (size_t i = 0; i < numElements_; ++i)
        data_[i] = static_cast<unsigned char>(rand() & 255);
}

// protected methods
//
template<typename T>
size_t SimpleTexture<T>::positionToIndex(const tgt::ivec2& position) const {
    return static_cast<size_t>(position.y * dimensions_.x + position.x);
}

template<typename T>
size_t SimpleTexture<T>::positionToIndex(const tgt::ivec3& position) const {
    if (dimensions_.z <= 1)
        return static_cast<size_t>((position.y * dimensions_.x) + position.x);

    return static_cast<size_t>((position.z * dimensions_.y * dimensions_.x)
        + (position.y * dimensions_.x) + position.x);
}

// ----------------------------------------------------------------------------

template class SimpleTexture<float>;
template class SimpleTexture<unsigned char>;

}   // namespace
