/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#ifndef KWFRAMESET_H
#define KWFRAMESET_H

#include "Words.h"
#include "KWFrame.h"
#include "words_export.h"

#include <QObject>


/**
 * A frameSet holds a number of frames (zero or more) and a frameSet holds the
 * content that is displayed on screen.
 * The FrameSet holds KWFrame objects that actually render the content this object
 * holds to the screen or to the printer.
 */
class WORDS_EXPORT KWFrameSet : public QObject
{
    Q_OBJECT
public:
    /// Constructor.
    explicit KWFrameSet(Words::FrameSetType type = Words::OtherFrameSet);
    virtual ~KWFrameSet();

    /**
     * Add a new Frame
     * @param frame the new frame to add.
     * @see frameAdded()
     */
    void addFrame(KWFrame *frame);

    /**
     * Remove a previously added Frame
     * @param frame the frame to remove
     * @param shape the shape of the frame
     * You shouldn't use this method in most cases but the convenience version with only a single
     * parameter
     */
    void removeFrame(KWFrame *frame, KoShape *shape);

    /**
     * Remove a previously added Frame
     * @param frame the frame to remove
     * @see frameRemoved()
     */
    void removeFrame(KWFrame *frame) {removeFrame(frame, frame->shape());}

    /**
     * Give this frameSet a name.
     * Since a frameSet holds content it is logical that the frameset can be given a name for
     * users to look for and use.
     * @param name the new name
     */
    void setName(const QString &name) {
        m_name = name;
    }
    /**
     * Return this framesets name.
     */
    const QString &name() const {
        return m_name;
    }

    /**
     * List all frames this frameset has.  In the order that any content will flow through them.
     */
    const QList<KWFrame*> frames() const {
        return m_frames;
    }

    /**
     * Return the amount of frames this frameset has.
     */
    int frameCount() const {
        return m_frames.count();
    }

    Words::FrameSetType type() const {
        return m_type;
    }

signals:
    /**
     * emitted whenever a frame is added
     * @param frame the frame that has just been added
     */
    void frameAdded(KWFrame *frame);
    /**
     * emitted whenever a frame that was formerly registerd is removed
     * @param frame the frame that has just been removed
     */
    void frameRemoved(KWFrame *frame);

protected:
    friend class KWFrame;

    /**
     * Called from addFrame.
     * Overwrite in inheriting classes to do something with the frame on add.
     * @param frame the frame that has just been added
     */
    virtual void setupFrame(KWFrame *frame) {
        Q_UNUSED(frame);
    }

    void cleanupFrames();

private:
    /// The list of frames that this frameset owns.
    QList<KWFrame*> m_frames;
    /// The type of the frameset.
    Words::FrameSetType m_type;
    /// The name of the frameset.
    QString m_name;
};

#endif
