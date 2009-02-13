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

#ifndef SEGMENTATIONPLUGIN_H
#define SEGMENTATIONPLUGIN_H

#include "widgetplugin.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class QCheckBox;
class QSpinBox;

namespace voreen {

class SegmentationPlugin : public WidgetPlugin {
    Q_OBJECT
public:
    SegmentationPlugin(QWidget* parent = 0, MessageReceiver* msgreceiver = 0);

protected:
    virtual void createWidgets();
    virtual void createConnections();

public slots:
    void toggleUseSegmentation(bool);
    void setSegmentation(int);

private:
    QCheckBox* segmentationSwitch_;
    QSpinBox* segmentationSelect_;
};

} // namespace voreen

#endif
