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

#include "voreen/qt/widgets/processor/canvasrendererwidget.h"
#include "voreen/core/vis/network/networkevaluator.h"
#include "voreen/core/vis/voreenpainter.h"
#include "voreen/qt/widgets/snapshotplugin.h"
#include "voreen/qt/voreenapplicationqt.h"

#include <QDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QMainWindow>

namespace voreen {

CanvasRendererWidget::CanvasRendererWidget(QWidget* parent, CanvasRenderer* canvasRenderer, NetworkEvaluator* evaluator) :
    QProcessorWidget(canvasRenderer, parent),
    evaluator_(evaluator),
    snapshotTool_(0)
{
    tgtAssert(canvasRenderer, "No CanvasRenderer");
    tgtAssert(evaluator_, "No evaluator");

    setWindowTitle(QString::fromStdString(canvasRenderer->getName()));

}

CanvasRendererWidget::~CanvasRendererWidget() {

    delete snapshotTool_;

    // deregister canvas at owning CanvasRenderer
    CanvasRenderer* canvasRenderer = dynamic_cast<CanvasRenderer*>(processor_);
    if (canvasRenderer)
        canvasRenderer->setCanvas(0);

    if (canvasWidget_)
        delete canvasWidget_->getPainter();
    delete canvasWidget_;

}

void CanvasRendererWidget::initialize() {
    QProcessorWidget::initialize();

    CanvasRenderer* canvasRenderer = dynamic_cast<CanvasRenderer*>(processor_);
    tgtAssert(canvasRenderer, "CanvasRenderer expected");

    canvasWidget_ = new tgt::QtCanvas("", tgt::ivec2(getSize().x, getSize().y), tgt::GLCanvas::RGBADD, this, true, 0);
    canvasWidget_->setMinimumSize(256, 256);
    // necessary for receiving keyboard events
    setFocusPolicy(Qt::StrongFocus);

    QGridLayout* layout = new QGridLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(canvasWidget_, 0, 0);
    setLayout(layout);

    show();

    VoreenPainter* painter = new VoreenPainter(canvasWidget_, evaluator_, canvasRenderer);
    canvasWidget_->setPainter(painter);
    painter->initialize();

    canvasRenderer->setCanvas(canvasWidget_);
    initialized_ = true;
}

void CanvasRendererWidget::keyPressEvent(QKeyEvent* event) {
    if ((event->modifiers() == Qt::AltModifier) && (event->key() == Qt::Key_P) ) {
#ifdef VRN_WITH_DEVIL
        if (!snapshotTool_)
            createSnapshotTool();
        snapshotTool_->show();
        snapshotTool_->setFocus();
#endif
    }
}

void CanvasRendererWidget::resizeEvent(QResizeEvent* event) {
    QProcessorWidget::resizeEvent(event);
    if (snapshotTool_)
        snapshotTool_->updateFromProcessor();
}

void CanvasRendererWidget::createSnapshotTool() {

    if (snapshotTool_)
        return;

    snapshotTool_ = new SnapshotPlugin(VoreenApplicationQt::qtApp()->getMainWindow(),
        static_cast<CanvasRenderer*>(processor_));
    snapshotTool_->createWidgets();
    snapshotTool_->createConnections();
    snapshotTool_->adjustSize();
    snapshotTool_->setFixedSize(snapshotTool_->sizeHint());
}

void CanvasRendererWidget::showSnapshotTool() {
    if (!snapshotTool_)
        createSnapshotTool();
    snapshotTool_->show();
    snapshotTool_->raise();
}

} //namespace voreen
