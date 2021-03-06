/* This file is part of the KDE project
 * Copyright (C) 2006-2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KWPAGESTYLE_P_H
#define KWPAGESTYLE_P_H

#include <QSharedData>
#include <KoText.h>
#include <KoShapeBackground.h>

class KWPageStylePrivate : public QSharedData
{
public:
    KWPageStylePrivate() : fullPageBackground(0) { clear(); }
    ~KWPageStylePrivate();
    void clear();

    KWPageStyle::PageUsageType pageUsage;
    KoColumns columns;
    KoPageLayout pageLayout;
    QString name;
    QString displayName;
    qreal headerDistance, footerDistance, footNoteDistance, endNoteDistance;
    qreal headerMinimumHeight,footerMinimumHeight;
    Words::HeaderFooterType headers, footers;
    bool headerDynamicSpacing;
    bool footerDynamicSpacing;
    qreal footNoteSeparatorLineWidth; ///< width of line; so more like 'thickness'
    int footNoteSeparatorLineLength; ///< It's a percentage of page.
    Qt::PenStyle footNoteSeparatorLineType; ///< foot note separate type
    Words::FootNoteSeparatorLinePos footNoteSeparatorLinePos; ///< alignment in page

    // See parag 16.2 for all the ODF features.
    KoText::Direction direction;
    KoShapeBackground *fullPageBackground;
    QString nextStyleName;

    // called from the command
    void copyProperties(KWPageStylePrivate *other) {
        pageUsage = other->pageUsage;
        columns = other->columns;
        pageLayout = other->pageLayout;
        //name = other->name;
        headerDistance = other->headerDistance;
        footerDistance = other->footerDistance;
        footNoteDistance = other->footNoteDistance;
        endNoteDistance = other->endNoteDistance;
        headerMinimumHeight = other->headerMinimumHeight;
        footerMinimumHeight = other->footerMinimumHeight;
        headers = other->headers;
        footers = other->footers;
        footNoteSeparatorLineWidth = other->footNoteSeparatorLineWidth;
        footNoteSeparatorLineLength = other->footNoteSeparatorLineLength;
        footNoteSeparatorLineType = other->footNoteSeparatorLineType;
        footNoteSeparatorLinePos = other->footNoteSeparatorLinePos;
        direction = other->direction;
        fullPageBackground = other->fullPageBackground;
        nextStyleName = other->nextStyleName;
    }
};

#endif
