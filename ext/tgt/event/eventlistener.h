/**********************************************************************
 *                                                                    *
 * tgt - Tiny Graphics Toolbox                                        *
 *                                                                    *
 * Copyright (C) 2006-2008 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the tgt library. This library is free         *
 * software; you can redistribute it and/or modify it under the terms *
 * of the GNU Lesser General Public License version 2.1 as published  *
 * by the Free Software Foundation.                                   *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU Lesser General Public License for more details.                *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License in the file "LICENSE.txt" along with this library.         *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 **********************************************************************/

#ifndef TGT_EVENTLISTENER_H
#define TGT_EVENTLISTENER_H

#include "tgt/config.h"
#include "tgt/event/event.h"
#include "tgt/event/mouseevent.h"
#include "tgt/event/timeevent.h"
#include "tgt/event/keyevent.h"

namespace tgt {

class EventListener {
public:
    EventListener() {}
    virtual ~EventListener() {}

    virtual void mousePressEvent(MouseEvent* e) { e->ignore(); }
    virtual void mouseReleaseEvent(MouseEvent* e) { e->ignore(); }
    virtual void mouseMoveEvent(MouseEvent* e) { e->ignore(); }
    virtual void mouseDoubleClickEvent(MouseEvent* e) { e->ignore(); }
    virtual void wheelEvent(MouseEvent* e) { e->ignore(); }
    virtual void timerEvent(TimeEvent* e) { e->ignore(); }
    virtual void keyEvent(KeyEvent* e) { e->ignore(); }

    virtual void onEvent(Event* e);
};

}

#endif //TGT_EVENTLISTENER_H
