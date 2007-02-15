/* This file is part of the KDE project

   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2006 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoShapeMoveStrategy.h"

#include "KoCanvasBase.h"
#include "KoShapeManager.h"
#include "KoSelection.h"
#include "KoPointerEvent.h"
#include "commands/KoShapeMoveCommand.h"
#include "kcommand.h"
#include "KoInteractionTool.h"

#include <kdebug.h>
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>

KoShapeMoveStrategy::KoShapeMoveStrategy( KoTool *tool, KoCanvasBase *canvas, const QPointF &clicked)
: KoInteractionStrategy(tool, canvas)
, m_initialTopLeft(99999, 99999)
, m_start(clicked)
{
    QList<KoShape*> selectedShapes = canvas->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection);
    QRectF boundingRect;
    foreach(KoShape *shape, selectedShapes) {
        if(shape->isLocked())
            continue;
        m_selectedShapes << shape;
        m_previousPositions << shape->position();
        m_newPositions << shape->position();
        boundingRect = boundingRect.unite( shape->boundingRect() );
    }
    m_initialTopLeft = boundingRect.topLeft();
    m_initialSelectionPosition = canvas->shapeManager()->selection()->position();
}

void KoShapeMoveStrategy::handleMouseMove(const QPointF &point, Qt::KeyboardModifiers modifiers) {
    QPointF diff = point - m_start;
    if(m_canvas->snapToGrid() && (modifiers & Qt::ShiftModifier) == 0) {
        QPointF newPos = m_initialTopLeft + diff;
        applyGrid(newPos);
        diff = newPos - m_initialTopLeft;
    }
    if(modifiers & (Qt::AltModifier | Qt::ControlModifier)) {
        // keep x or y position unchanged
        if(qAbs(diff.x()) < qAbs(diff.y()))
            diff.setX(0);
        else
            diff.setY(0);
    }

    Q_ASSERT(m_newPositions.count());
    QRectF canvasRect (QPointF(0.,0.) - m_canvas->documentOrigin(), QSizeF(m_canvas->canvasWidget()->size()));
    canvasRect = m_canvas->viewConverter()->viewToDocument(canvasRect);
    canvasRect.adjust(5, 5, -5, -5);
    int x=0;
    foreach(KoShape *shape, m_selectedShapes) {
        QRectF s = canvasRect; // the outline is not translated, so translate this rect.
        s.moveTopLeft(s.topLeft() - (m_previousPositions.at(x) + diff));
        if(! shape->outline().intersects(s))
            return;
            // TODO, instead of just returning here; we should allow the move to be converted into
            // a QDrag for DnD purposes.
        x++;
    }

    m_diff = diff;
    int i=0;
    foreach(KoShape *shape, m_selectedShapes) {
        QPointF newPos (m_previousPositions.at(i) + m_diff);
        m_newPositions[i] = newPos;
        shape->repaint();
        shape->setPosition(newPos);
        shape->repaint();
        i++;
    }
    m_canvas->shapeManager()->selection()->setPosition(m_initialSelectionPosition + m_diff);
}

QUndoCommand* KoShapeMoveStrategy::createCommand() {
    if(m_diff.x() == 0 && m_diff.y() == 0)
        return 0;
    return new KoShapeMoveCommand(m_selectedShapes, m_previousPositions, m_newPositions);
}

void KoShapeMoveStrategy::paint( QPainter &painter, KoViewConverter &converter) {
    SelectionDecorator decorator (KoFlake::NoHandle, false, false);
    decorator.setSelection(m_canvas->shapeManager()->selection());
    decorator.setHandleRadius( m_canvas->resourceProvider()->handleRadius() );
    decorator.paint(painter, converter);
}
