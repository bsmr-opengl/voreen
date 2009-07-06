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

#ifndef VRN_NUMERICPROPERTY_H
#define VRN_NUMERICPROPERTY_H

#include "voreen/core/vis/properties/condition.h"
#include "voreen/core/vis/properties/templateproperty.h"

namespace voreen {

template<typename T>
class NumericProperty : public TemplateProperty<T> {

template<typename> friend class NumericPropertyValidation;

public:
    typedef T ElemType;

    NumericProperty(const std::string& id, const std::string& guiText, const T& value,
                    const T& minValue, const T& maxValue, const T& stepping,
                    bool invalidate = true, bool invalidateShader = false);
    virtual ~NumericProperty() {}

    const T& getMinValue() const { return minValue_; }
    void setMinValue(const T& minValue) { minValue_ = minValue; }

    const T& getMaxValue() const { return maxValue_; }
    void setMaxValue(const T& maxValue) { maxValue_ = maxValue; }

    T getStepping() const { return stepping_; }
    void setStepping(const T stepping) { stepping_ = stepping; }

    bool getInstantValueChange() const { return instantValueChange_; }
    void setInstantValueChange(const bool instantChange) { instantValueChange_ = instantChange; }

    size_t getNumDecimals() const { return numDecimals_; }
    void setNumDecimals(const size_t numDecimals) { numDecimals_ = numDecimals; }

    void updateFromXml(TiXmlElement* propElem);
    TiXmlElement* serializeToXml() const;

    virtual std::string toString() const { 
        return Property::valueToString(value_);
    }

protected:
    using TemplateProperty<T>::errors_;
    using TemplateProperty<T>::value_;

    T minValue_;
    T maxValue_;
    T stepping_;
    bool instantValueChange_; /** see QSlider.setTracking - emits valueChanged while slider dragged */
    size_t numDecimals_;
};


// ----------------------------------------------------------------------------

template<typename T>
NumericProperty<T>::NumericProperty(const std::string& id, const std::string& guiText, const T& value,
                                    const T& minValue, const T& maxValue, const T& stepping,
                                    bool invalidate, bool invalidateShader)
    : TemplateProperty<T>(id, guiText, value, invalidate, invalidateShader),
    minValue_(minValue),
    maxValue_(maxValue),
    stepping_(stepping),
    instantValueChange_(true),
    numDecimals_(2)
{
    addValidation(NumericPropertyValidation<T>(this));
    validate(value);
}

} // namespace voreen

#endif // VRN_NUMERICPROPERTY_H
