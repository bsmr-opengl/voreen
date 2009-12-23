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

#include "voreen/qt/widgets/property/qpropertywidgetfactory.h"

#include "voreen/qt/widgets/property/boolpropertywidget.h"
#include "voreen/qt/widgets/property/buttonpropertywidget.h"
#include "voreen/qt/widgets/property/camerapropertywidget.h"
#include "voreen/qt/widgets/property/colorpropertywidget.h"
#include "voreen/qt/widgets/property/filedialogpropertywidget.h"
#include "voreen/qt/widgets/property/floatpropertywidget.h"
#include "voreen/qt/widgets/property/floatvec2propertywidget.h"
#include "voreen/qt/widgets/property/floatvec3propertywidget.h"
#include "voreen/qt/widgets/property/floatvec4propertywidget.h"
#include "voreen/qt/widgets/property/intpropertywidget.h"
#include "voreen/qt/widgets/property/intvec2propertywidget.h"
#include "voreen/qt/widgets/property/intvec3propertywidget.h"
#include "voreen/qt/widgets/property/intvec4propertywidget.h"
#include "voreen/qt/widgets/property/optionpropertywidget.h"
#include "voreen/qt/widgets/property/propertyvectorwidget.h"
#include "voreen/qt/widgets/property/shaderpropertywidget.h"
#include "voreen/qt/widgets/property/stringpropertywidget.h"
#include "voreen/qt/widgets/property/transfuncpropertywidget.h"
#include "voreen/qt/widgets/property/volumecollectionpropertywidget.h"
#include "voreen/qt/widgets/property/volumehandlepropertywidget.h"


namespace voreen {

QPropertyWidget* QPropertyWidgetFactory::createWidget(BoolProperty* p) {
    return new BoolPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(ButtonProperty* p) {
    return new ButtonPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(CameraProperty* p) {
    return new CameraPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(ColorProperty* p) {
    return new ColorPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(FileDialogProperty* p) {
    return new FileDialogPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(FloatProperty* p) {
    return new FloatPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(FloatVec2Property* p) {
    return new FloatVec2PropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(FloatVec3Property* p) {
    return new FloatVec3PropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(FloatVec4Property* p) {
    return new FloatVec4PropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(IntProperty* p) {
    return new IntPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(IntVec2Property* p) {
    return new IntVec2PropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(IntVec3Property* p) {
    return new IntVec3PropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(IntVec4Property* p) {
    return new IntVec4PropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(OptionPropertyBase* p) {
    return new OptionPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(PropertyVector* p) {
    return new PropertyVectorWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(ShaderProperty* p) {
    return new ShaderPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(StringProperty* p) {
    return new StringPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(TransFuncProperty* p) {
    return new TransFuncPropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(VolumeHandleProperty* p) {
    return new VolumeHandlePropertyWidget(p, 0);
}

QPropertyWidget* QPropertyWidgetFactory::createWidget(VolumeCollectionProperty* p) {
    return new VolumeCollectionPropertyWidget(p, 0);
}

} // namespace voreen
