/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KOPATHSHAPE_H
#define KOPATHSHAPE_H

#include <koffice_export.h>

#include <QFlags>
#include <QPainterPath>
#include <QSet>
#include <QList>

#include "KoShape.h"

#define KoPathShapeId "KoPathShape"

class KoPathShape;
class KoPointGroup;

/**
 * @brief A KoPathPoint represents a point in a path.
 *
 * A KoPathPoint stores a point in a path. Additional to this point 
 * 2 control points are stored. 
 * controlPoint1 is used to describe the second point of a cubic 
 * bezier ending at the point. controlPoint2 is used to describe the 
 * first point of a cubic bezier curve starting at the point.
 */
class KoPathPoint
{
public:    
    enum KoPointProperty
    {
        Normal = 0, /// it has no control point
        CanHaveControlPoint1 = 1, /// it can have a control point 1
        CanHaveControlPoint2 = 2, /// it can have a control point 2
        HasControlPoint1 = 4, /// it has a control point 1
        HasControlPoint2 = 8, /// it has a control point 2
        StartSubpath = 16, /// it starts a new subpath by a moveTo command
        CloseSubpath = 32 /// it closes a subpath 
    };
    Q_DECLARE_FLAGS( KoPointProperties, KoPointProperty )

    /**
     * @brief Constructor
     *
     * @param path a pointer to the path shape this point is used in
     * @param point the point
     * @param properties describing the point
     */
    KoPathPoint( KoPathShape * path, const QPointF & point, KoPointProperties properties = Normal )
    : m_shape( path )
    , m_point( point )
    , m_properties( properties )    
    , m_pointGroup( 0 )
    {}

    /**
     * @brief Copy Constructor
     */
    KoPathPoint( KoPathPoint & pathPoint );
        
    /**
     * @brief Destructor
     */
    ~KoPathPoint() {}

    /**
     * @brief get the point 
     *
     * @return point
     */
    QPointF point() { return m_point; }

    /**
     * @brief get the control point 1
     *
     * This points is used for controling a curve ending at this point
     *
     * @return control point 1 of this point
     */
    QPointF controlPoint1() { return m_controlPoint1; }

    /**
     * @brief get the control point 2
     *
     * This points is used for controling a curve starting at this point
     *
     * @return control point 2 of this point
     */
    QPointF controlPoint2() { return m_controlPoint2; }

    /**
     * @brief Set the point
     * 
     * @param point to set 
     */
    void setPoint( const QPointF & point );

    /**
     * @brief Set the control point 1
     * 
     * @param point to set 
     */
    void setControlPoint1( const QPointF & point );

    /**
     * @brief Set the control point 2
     * 
     * @param point to set 
     */
    void setControlPoint2( const QPointF & point );

    void removeControlPoint1() { /*TODO*/ }
    void removeControlPoint2() { /*TODO*/ }

    /**
     * @brief Get the properties of a point
     *
     * @return properties of the point
     */
    KoPointProperties properties() { return m_properties; }

    /**
     * @brief Set the properties of a point
     *
     * @return the properties of the point
     */
    void setProperties( KoPointProperties properties );

    /**
     * @brief apply matrix on the point
     *
     * This does a matrix multiplication on all points of the point
     */
    void map( const QMatrix &matrix ) { map( matrix, true ); }

protected:    
    friend class KoPointGroup;
    friend class KoPathShape;
    void removeFromGroup();
    void addToGroup( KoPointGroup *pointGroup );
    void map( const QMatrix &matrix, bool mapGroup );
    KoPointGroup * group() { return m_pointGroup; }

    KoPathShape * m_shape;
    QPointF m_point;
    QPointF m_controlPoint1;
    QPointF m_controlPoint2;
    KoPointProperties m_properties;
    KoPointGroup * m_pointGroup;
};

/**
 * @brief A KoPointGroup represents points in a path that should be treated as one
 *
 * In svg it is possible when you use a close and the create a new subpath not using 
 * a moveTo that the new subpath starts at the same point as the last subpath. As 
 * every point can only have 2 control points we have this class to group points 
 * together which should be handled as one in e.g. a move. 
 */
class KoPointGroup
{
public:    
    KoPointGroup() {}
    ~KoPointGroup() {}

    /**
     * @brief Add a point to the group
     */
    void add( KoPathPoint * point );
    /**
     * @brief Remove a point from the group
     * 
     * This also remove the pointer to the group in the point.
     * When the second last point is removed from the group, the 
     * group removes also the last point and deletes itself.
     */
    void remove( KoPathPoint * point );

    void map( const QMatrix &matrix );

    /**
     * @brief get The point belonging to the group
     *
     * @return all points of the group
     */
    const QSet<KoPathPoint *> & points() const { return m_points; }

private:
    QSet<KoPathPoint *> m_points;
};

/**
 * @brief This is the base for all graphical objects.
 *
 * All graphical objects are based on this object e.g. lines, rectangulars, pies 
 * and so on.
 *
 * The KoPathShape uses KoPathPoint's to describe the path of the shape. 
 *
 * Here a short example:
 * 3 points connected by a curveTo's described by the following svg:
 * M 100,200 C 100,100 250,100 250,200 C 250,200 400,300 400,200.
 * 
 * This will be stored in 3 KoPathPoint's as 
 * The first point contains in 
 *       point 100,200 
 *       controlPoint2 100,100
 * The second point contains in
 *       point 250,200
 *       controlPoint1 250,100
 *       controlPoint2 250,300
 * The third point contains in
 *       point 400,300
 *       controlPoint1 400,200
 *       
 * Not the segments are stored but the points. Out of the points the segments are 
 * generated. See the outline method. The reason for storing it like that is that 
 * it is the points that are modified by the user and not the segments.
 */
class FLAKE_EXPORT KoPathShape : public KoShape
{
public:
    /**
     * @brief
     */
    KoPathShape();

    /**
     * @brief
     */
    virtual ~KoPathShape();

    virtual void paint(QPainter &painter, const KoViewConverter &converter);
    virtual const QPainterPath outline() const;
    virtual QRectF boundingRect() const;
    virtual QSizeF size() const;
    virtual QPointF position() const;
    virtual void resize( const QSizeF &newSize );

    /**
     * @brief Creats a new Subpath
     *
     * Moves the pen to p and starts a new subpath.
     *
     * @return The newly created point 
     */
    KoPathPoint * moveTo( const QPointF &p );

    /**
     * @brief add a line
     *
     * Adds a straight line between the last point and the given p.
     *
     * @return The newly created point 
     */
    KoPathPoint * lineTo( const QPointF &p );

    /**
     * @brief add a cubic Bezier curve.
     *
     * Adds a cubic Bezier curve between the last point and the given p,
     * using the control points specified by c1, and c2.
     *
     * @return The newly created point 
     */
    KoPathPoint * curveTo( const QPointF &c1, const QPointF &c2, const QPointF &p );

    /**
     * @brief close the current subpath
     */
    void close();

    /**
     * @brief The path is updated
     *
     * This is called when a point of the path is updated. It will be used 
     * to make it possible to cache things.
     */
    void update() {}

    /**
     * @brief Normalizes the path data.
     *
     * The path points are transformed so that the top-left corner
     * of the bounding rect is (0,0).
     * This should be called after adding points to the path.
     */
    void normalize();

protected:    
    void map( const QMatrix &matrix );

    void updateLast( KoPathPoint * lastPoint );

    void paintDebug( QPainter &painter );

    /// a KoSubpath contains a path from a moveTo until a close or a new moveTo
    typedef QList<KoPathPoint *> KoSubpath;
    QList<KoSubpath> m_points;
};
        
Q_DECLARE_OPERATORS_FOR_FLAGS( KoPathPoint::KoPointProperties )

#endif /* KOPATHSHAPE_H */
