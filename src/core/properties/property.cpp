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

#include "voreen/core/properties/property.h"
#include "voreen/core/properties/propertywidgetfactory.h"
#include "voreen/core/properties/propertywidget.h"
#include "voreen/core/properties/link/changeaction.h"
#include "voreen/core/properties/allproperties.h"

namespace voreen {

Property::Property(const std::string& id, const std::string& guiText, Processor::InvalidationLevel invalidationLevel)
    : id_(id)
    , guiName_(guiText)
    , owner_(0)
    , invalidationLevel_(invalidationLevel)
    , widgetsEnabled_(true)
    , visible_(true)
    , lod_(USER)
    , interactionModeVisited_(false)
    , linkCheckVisited_(false)
{
    tgtAssert(!id.empty(), "Property's id must not be empty");
}

Property::~Property() {
    disconnectWidgets();
}

Processor::InvalidationLevel Property::getInvalidationLevel() {
    return invalidationLevel_;
}

void Property::setWidgetsEnabled(bool enabled) {
    widgetsEnabled_ = enabled;
    for (std::set<PropertyWidget*>::iterator it = widgets_.begin(); it != widgets_.end(); ++it)
        (*it)->setEnabled(widgetsEnabled_);
}

bool Property::getWidgetsEnabled() const {
    return widgetsEnabled_;
}

void Property::initialize() throw (VoreenException) {
    // currently nothing to do
}

void Property::deinitialize() throw (VoreenException) {
    // currently nothing to do
}

void Property::setVisible(bool state) {
    visible_ = state;
    setWidgetsVisible(state);
}

bool Property::isVisible() const {
    return visible_;
}

void Property::setOwner(PropertyOwner* processor) {
    owner_ = processor;
}

PropertyOwner* Property::getOwner() const {
    return owner_;
}

void Property::addWidget(PropertyWidget* widget) {
    if (widget)
        widgets_.insert(widget);
}

void Property::removeWidget(PropertyWidget* widget) {
    if (widget)
        widgets_.erase(widget);
}

void Property::disconnectWidgets() {
    std::set<PropertyWidget*>::iterator it = widgets_.begin();
    for ( ; it != widgets_.end(); ++it)
        (*it)->disconnect();
}

void Property::updateWidgets() {
    std::set<PropertyWidget*>::iterator it = widgets_.begin();
    for ( ; it != widgets_.end(); ++it)
        (*it)->updateFromProperty();
}

void Property::setWidgetsVisible(bool state) {
    std::set<PropertyWidget*>::iterator it = widgets_.begin();
    for ( ; it != widgets_.end(); ++it)
        (*it)->setVisible(state);
}

std::string Property::getID() const {
    return id_;
}

std::string Property::getFullyQualifiedID() const {
    if (getOwner())
        return getOwner()->getName() + "." + getID();
    else
        return getID();
}

std::string Property::getGuiName() const {
    return guiName_;
}

std::string Property::getFullyQualifiedGuiName() const {
    if (getOwner())
        return getOwner()->getName() + "." + getGuiName();
    else
        return getGuiName();
}

PropertyWidget* Property::createWidget(PropertyWidgetFactory*) {
    return 0;
}

void Property::serialize(XmlSerializer& s) const {
    if (lod_ != USER)
        s.serialize("lod", lod_);

    for (std::set<PropertyWidget*>::const_iterator it = widgets_.begin(); it != widgets_.end(); ++it) {
        (*it)->updateMetaData();
        (*it)->getWidgetMetaData();
    }

    metaDataContainer_.serialize(s);
}

void Property::deserialize(XmlDeserializer& s) {
    try {
        int lod;
        s.deserialize("lod", lod);
        lod_ = static_cast<LODSetting>(lod);
    }
    catch (XmlSerializationNoSuchDataException&) {
        s.removeLastError();
        lod_ = USER;
    }

    metaDataContainer_.deserialize(s);
}

MetaDataContainer& Property::getMetaDataContainer() const {
    return metaDataContainer_;
}

PropertyWidget* Property::createAndAddWidget(PropertyWidgetFactory* f) {
    PropertyWidget* widget = createWidget(f);
    if (widget) {
        if (!visible_)
            widget->setVisible(visible_);
        if (!widgetsEnabled_)
            widget->setEnabled(false);
        addWidget(widget);
    }
    return widget;
}

void Property::registerLink(PropertyLink* link) {
    links_.push_back(link);
}

void Property::removeLink(PropertyLink* link) {
    for (std::vector<PropertyLink*>::iterator it = links_.begin(); it != links_.end(); ++it) {
        if (*it == link) {
            links_.erase(it);
            break;
        }
    }
}

const std::set<PropertyWidget*> Property::getPropertyWidgets() const {
    return widgets_;
}

Property::LODSetting Property::getLevelOfDetail() {
    return lod_;
}

void Property::setLevelOfDetail(LODSetting lod) {
    lod_ = lod;
}

void Property::invalidateOwner() {
    invalidateOwner(invalidationLevel_);
}

void Property::invalidateOwner(Processor::InvalidationLevel invalidationLevel) {
    if (getOwner())
        getOwner()->invalidate(invalidationLevel);
}

void Property::toggleInteractionMode(bool interactionmode, void* source) {
    if (!interactionModeVisited_) {
        interactionModeVisited_ = true;

        // propagate to owner
        if (getOwner() && (invalidationLevel_ != Processor::VALID)) {
            getOwner()->toggleInteractionMode(interactionmode, source);
        }

        // propagate over links
        for (size_t i=0; i<getLinks().size(); ++i) {
            Property* destProperty = getLinks()[i]->getDestinationProperty();
            tgtAssert(destProperty, "Link without destination property");
            destProperty->toggleInteractionMode(interactionmode, source);
        }
        interactionModeVisited_ = false;
    }
}

void Property::invalidate() {
    invalidateOwner();
    // notify widgets of updated values
    updateWidgets();
}

const std::vector<PropertyLink*>& Property::getLinks() const {
    return links_;
}

PropertyLink* Property::getLink(const Property* dest) const {
    for (size_t i=0; i<links_.size(); ++i) {
        if (links_[i]->getDestinationProperty() == dest)
            return links_[i];
    }
    return 0;
}

bool Property::isLinkedWith(const Property* dest, bool transitive) const {
    // true if dest is the object itself
    if (this == dest)
        return true;

    // check for direct links
    bool linkedDirectly = (getLink(dest) != 0);
    if (linkedDirectly)
        return true;

    // recursive search for indirect links
    if (transitive) {
        if (linkCheckVisited_)
            return false;
        else {
            linkCheckVisited_ = true;
            for (size_t i=0; i<links_.size(); ++i) {
                if (links_[i]->getDestinationProperty()->isLinkedWith(dest, true)) {
                    linkCheckVisited_ = false;
                    return true;
                }
            }
            linkCheckVisited_ = false;
        }
    }

    return false;
}

//TODO: this should be move to virtual methods of the individual property classes
std::string Property::getPropertyTypeText(const Property* prop) {
    if (typeid(BoolProperty) == typeid(*prop)) {
        return "bool";
    }
    else if (typeid(ColorProperty) == typeid(*prop)) {
        return "Color";
    }
    else if (typeid(FileDialogProperty) == typeid(*prop)) {
        return "FileDialog";
    }
    else if (typeid(FloatProperty) == typeid(*prop)) {
        return "float";
    }
    else if (typeid(IntProperty) == typeid(*prop)) {
        return "int";
    }
    else if (typeid(StringProperty) == typeid(*prop)) {
        return "string";
    }
    else if (typeid(FloatVec2Property) == typeid(*prop)) {
        return "FloatVector2";
    }
    else if (typeid(FloatVec3Property) == typeid(*prop)) {
        return "FloatVector3";
    }
    else if (typeid(FloatVec4Property) == typeid(*prop) || typeid(LightProperty) == typeid(*prop)) {
        return "FloatVector4";
    }
    else if (typeid(IntVec2Property) == typeid(*prop)) {
        return "IntVector2";
    }
    else if (typeid(IntVec3Property) == typeid(*prop)) {
        return "IntVector3";
    }
    else if (typeid(IntVec4Property) == typeid(*prop)) {
        return "IntVector4";
    }
    else if (typeid(FloatMat2Property) == typeid(*prop)) {
        return "FloatMatrix2x2";
    }
    else if (typeid(FloatMat3Property) == typeid(*prop)) {
        return "FloatMatrix3x3";
    }
    else if (typeid(FloatMat4Property) == typeid(*prop)) {
        return "FloatMatrix4x4";
    }
    else if (typeid(CameraProperty) == typeid(*prop)) {
        return "Camera";
    }
    else if (typeid(TransFuncProperty) == typeid(*prop)) {
        return "TransFunc";
    }
    else if (dynamic_cast<const OptionPropertyBase*>(prop)) {
        return "Option";
    }
    else if (typeid(ShaderProperty) == typeid(*prop)) {
        return "Shader";
    }
    else if (typeid(ButtonProperty) == typeid(*prop)) {
        return "Button";
    }
    else if (typeid(VolumeHandleProperty) == typeid(*prop)) {
        return "VolumeHandle";
    }
    else if (typeid(VolumeCollectionProperty) == typeid(*prop)) {
        return "VolumeCollection";
    }
    else {
        return "Unknown property type";
    }
}

} // namespace voreen
