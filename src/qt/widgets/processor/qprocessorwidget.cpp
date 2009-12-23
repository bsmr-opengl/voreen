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

#include "tgt/vector.h"
#include "voreen/qt/widgets/processor/qprocessorwidget.h"
#include "voreen/qt/voreenapplicationqt.h"
#include "voreen/core/vis/processors/processor.h"

#include <QDialog>
#include <QMainWindow>
#include <QMoveEvent>

namespace voreen {

QProcessorWidget::QProcessorWidget(Processor* processor, QWidget* parent) :
    QWidget(parent, Qt::Tool),
    ProcessorWidget(processor)
{
    tgtAssert(processor, "No processor");
    setWindowTitle(QString::fromStdString(processor->getName()));
}

void QProcessorWidget::setVisible(bool visible) {
    if (isInitialized()) {
        if (visible) {
            initialized_ = false;
            restoreGeometryFromMeta();
            initialized_ = true;
        }
        else {
            saveGeometryToMeta();
        }
    }

    QWidget::setVisible(visible);
}

bool QProcessorWidget::isVisible() const {
    return QWidget::isVisible();
}

void QProcessorWidget::setSize(int height, int width) {
    QWidget::resize(height, width);
}

tgt::ivec2 QProcessorWidget::getSize() const {
    return tgt::ivec2(QWidget::width(), QWidget::height());
}

void QProcessorWidget::setPosition(int x, int y) {
    // set position relative to mainwindow
    if (VoreenApplicationQt::qtApp() && VoreenApplicationQt::qtApp()->getMainWindow()) {
        QPoint mainPindowPos = VoreenApplicationQt::qtApp()->getMainWindow()->pos();
        QWidget::move(mainPindowPos.x() + x, mainPindowPos.y() + y);
    }
    else
        QWidget::move(x, y);
}

tgt::ivec2 QProcessorWidget::getPosition() const {
    // calculate position relative to mainwindow
    if (VoreenApplicationQt::qtApp() && VoreenApplicationQt::qtApp()->getMainWindow()) {
       QPoint mainPindowPos = VoreenApplicationQt::qtApp()->getMainWindow()->pos();
       return tgt::ivec2(QWidget::x() - mainPindowPos.x(), QWidget::y() - mainPindowPos.y());
    }
    else
        return tgt::ivec2(QWidget::x(), QWidget::y());
}

void QProcessorWidget::processorNameChanged() {
    setWindowTitle(QString::fromStdString(processor_->getName()));
}

void QProcessorWidget::moveEvent(QMoveEvent* eve) {
    QWidget::moveEvent(eve);
    ProcessorWidget::onMove();
}

void QProcessorWidget::resizeEvent(QResizeEvent* eve) {
    QWidget::resizeEvent(eve);
    ProcessorWidget::onResize();
}

void QProcessorWidget::showEvent(QShowEvent* eve) {
    QWidget::showEvent(eve);
    ProcessorWidget::onShow();
}

void QProcessorWidget::hideEvent(QHideEvent* eve) {
    QWidget::hideEvent(eve);
    ProcessorWidget::onHide();
}

} // namespace voreen
