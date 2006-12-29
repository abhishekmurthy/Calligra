/* This file is part of the KDE project
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>
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

#include "KoPathCommand.h"
#include "KoParameterShape.h"
#include "KoShapeControllerBase.h"
#include "KoShapeContainer.h"
#include <klocale.h>
#include <kdebug.h>
#include <math.h>

KoPathBaseCommand::KoPathBaseCommand( QUndoCommand *parent )
: QUndoCommand( parent )
{
}

KoPathBaseCommand::KoPathBaseCommand( KoPathShape *shape, QUndoCommand *parent )
: QUndoCommand( parent )
{
    m_shapes.insert( shape );
}

void KoPathBaseCommand::repaint( bool normalizeShapes )
{
    foreach( KoPathShape *shape, m_shapes )
    {
        if( normalizeShapes )
            shape->normalize();
        // TODO use the proper adjustment if the actual point size could be retrieved
        shape->repaint( shape->outline().controlPointRect().adjusted( -5.0, -5.0, 5.0, 5.0 ) );
    }
}

KoPointMoveCommand::KoPointMoveCommand( const KoPathShapePointMap &pointMap, const QPointF &offset, QUndoCommand *parent )
: QUndoCommand( parent )
, m_pointMap( pointMap )
, m_offset( offset )
{
    setText( i18n( "Move points" ) );
}

void KoPointMoveCommand::redo()
{
    KoPathShapePointMap::iterator it( m_pointMap.begin() );
    for ( ; it != m_pointMap.end(); ++it )
    {
        QPointF offset = it.key()->documentToShape( m_offset ) - it.key()->documentToShape( QPointF( 0, 0 ) );
        QMatrix matrix;
        matrix.translate( offset.x(), offset.y() );

        // repaint old bounding rect
        it.key()->repaint();
        foreach( KoPathPoint *p, it.value() )
        {
            p->map( matrix, true );
        }
        it.key()->normalize();
        // repaint new bounding rect
        it.key()->repaint();
    }
}

void KoPointMoveCommand::undo()
{
    m_offset *= -1.0;
    redo();
    m_offset *= -1.0;
}

KoControlPointMoveCommand::KoControlPointMoveCommand( KoPathPoint *point, const QPointF &offset, KoPathPoint::KoPointType pointType, QUndoCommand *parent )
: QUndoCommand( parent )
, m_point( point )
, m_offset( point->parent()->documentToShape( offset ) - point->parent()->documentToShape( QPointF( 0, 0 ) ) )
, m_pointType( pointType )
{
    setText( i18n( "Move control point" ) );
}

void KoControlPointMoveCommand::redo()
{
    KoPathShape * pathShape = m_point->parent();
    pathShape->repaint();

    if ( m_pointType == KoPathPoint::ControlPoint1 )
    {
        m_point->setControlPoint1( m_point->controlPoint1() + m_offset );
        if( m_point->properties() & KoPathPoint::IsSymmetric )
        {
            // set the other control point so that it lies on the line between the moved
            // control point and the point, with the same distance to the point as the moved point
            m_point->setControlPoint2( 2.0 * m_point->point() - m_point->controlPoint1() );
        }
        else if( m_point->properties() & KoPathPoint::IsSmooth )
        {
            // move the other control point so that it lies on the line through point and control point
            // keeping its distance to the point
            QPointF direction = m_point->point() - m_point->controlPoint1();
            direction /= sqrt( direction.x()*direction.x() + direction.y()*direction.y() );
            QPointF distance = m_point->point() - m_point->controlPoint2();
            qreal length = sqrt( distance.x()*distance.x() + distance.y()*distance.y() );
            m_point->setControlPoint2( m_point->point() + length * direction );
        }
    }
    else if( m_pointType == KoPathPoint::ControlPoint2 )
    {
        m_point->setControlPoint2( m_point->controlPoint2() + m_offset );
        if( m_point->properties() & KoPathPoint::IsSymmetric )
        {
            // set the other control point so that it lies on the line between the moved
            // control point and the point, with the same distance to the point as the moved point
            m_point->setControlPoint1( 2.0 * m_point->point() - m_point->controlPoint2() );
        }
        else if( m_point->properties() & KoPathPoint::IsSmooth )
        {
            // move the other control point so that it lies on the line through point and control point
            // keeping its distance to the point
            QPointF direction = m_point->point() - m_point->controlPoint2();
            direction /= sqrt( direction.x()*direction.x() + direction.y()*direction.y() );
            QPointF distance = m_point->point() - m_point->controlPoint1();
            qreal length = sqrt( distance.x()*distance.x() + distance.y()*distance.y() );
            m_point->setControlPoint1( m_point->point() + length * direction );
        }
    }

    pathShape->normalize();
    pathShape->repaint();
}

void KoControlPointMoveCommand::undo()
{
    m_offset *= -1.0;
    redo();
    m_offset *= -1.0;
}

KoPointPropertyCommand::KoPointPropertyCommand( KoPathPoint *point, KoPathPoint::KoPointProperties property, QUndoCommand *parent )
: KoPathBaseCommand( parent )
{
    m_shapes.insert( point->parent() );
    PointPropertyChangeset changeset;
    changeset.point = point;
    changeset.newProperty = property;
    changeset.oldProperty = point->properties();
    changeset.firstControlPoint = point->controlPoint1();
    changeset.secondControlPoint = point->controlPoint2();
    m_changesets.append( changeset );

    setText( i18n( "Set point properties" ) );
}

KoPointPropertyCommand::KoPointPropertyCommand( const QList<KoPathPoint*> &points, const QList<KoPathPoint::KoPointProperties> &properties, QUndoCommand *parent )
: KoPathBaseCommand( parent )
{
    Q_ASSERT(points.size() == properties.size());
    uint pointCount = points.size();
    for( uint i = 0; i < pointCount; ++i )
    {
        PointPropertyChangeset changeset;
        changeset.point = points[i];
        changeset.newProperty = properties[i];
        changeset.oldProperty = changeset.point->properties();
        changeset.firstControlPoint = changeset.point->controlPoint1();
        changeset.secondControlPoint = changeset.point->controlPoint2();
        m_changesets.append( changeset );
        m_shapes.insert( changeset.point->parent() );
    }

    setText( i18n( "Set point properties" ) );
}

void KoPointPropertyCommand::redo()
{
    repaint( false );

    uint pointCount = m_changesets.count();
    for( uint i = 0; i < pointCount; ++ i)
    {
        PointPropertyChangeset &changeset = m_changesets[i];
        KoPathPoint::KoPointProperties newProperties = changeset.newProperty;
        KoPathPoint *point = changeset.point;

        if( newProperties & KoPathPoint::IsSymmetric )
        {
            newProperties &= ~KoPathPoint::IsSmooth;
            point->setProperties( newProperties );

            // calculate vector from node point to first control point and normalize it
            QPointF directionC1 = point->controlPoint1() - point->point();
            qreal dirLengthC1 = sqrt( directionC1.x()*directionC1.x() + directionC1.y()*directionC1.y() );
            directionC1 /= dirLengthC1;
            // calculate vector from node point to second control point and normalize it
            QPointF directionC2 = point->controlPoint2() - point->point();
            qreal dirLengthC2 = sqrt( directionC2.x()*directionC2.x() + directionC2.y()*directionC2.y() );
            directionC2 /= dirLengthC2;
            // calculate the average distance of the control points to the node point
            qreal averageLength = 0.5 * (dirLengthC1 + dirLengthC2);
            // compute position of the control points so that they lie on a line going through the node point
            // the new distance of the control points is the average distance to the node point
            point->setControlPoint1( point->point() + 0.5 * averageLength * (directionC1 - directionC2) );
            point->setControlPoint2( point->point() + 0.5 * averageLength * (directionC2 - directionC1) );
        }
        else if( newProperties & KoPathPoint::IsSmooth )
        {
            newProperties &= ~KoPathPoint::IsSymmetric;
            point->setProperties( newProperties );

            // calculate vector from node point to first control point and normalize it
            QPointF directionC1 = point->controlPoint1() - point->point();
            qreal dirLengthC1 = sqrt( directionC1.x()*directionC1.x() + directionC1.y()*directionC1.y() );
            directionC1 /= dirLengthC1;
            // calculate vector from node point to second control point and normalize it
            QPointF directionC2 = point->controlPoint2() - point->point();
            qreal dirLengthC2 = sqrt( directionC2.x()*directionC2.x() + directionC2.y()*directionC2.y() );
            directionC2 /= dirLengthC2;
            // compute position of the control points so that they lie on a line going through the node point
            // the new distance of the control points is the average distance to the node point
            point->setControlPoint1( point->point() + 0.5 * dirLengthC1 * (directionC1 - directionC2) );
            point->setControlPoint2( point->point() + 0.5 * dirLengthC2 * (directionC2 - directionC1) );
        }
        else
        {
            newProperties &= ~KoPathPoint::IsSymmetric;
            newProperties &= ~KoPathPoint::IsSmooth;
            point->setProperties( newProperties );
        }
    }
    repaint( true );
}

void KoPointPropertyCommand::undo()
{
    repaint( false );

    uint pointCount = m_changesets.count();
    for( uint i = 0; i < pointCount; ++ i)
    {
        KoPathPoint *point = m_changesets[i].point;
        point->setProperties( m_changesets[i].oldProperty );
        point->setControlPoint1( m_changesets[i].firstControlPoint );
        point->setControlPoint2( m_changesets[i].secondControlPoint );
    }

    repaint( true );
}

KoPointRemoveCommand::KoPointRemoveCommand( const KoPathShapePointMap &pointMap, QUndoCommand *parent )
: QUndoCommand( parent )
, m_pointMap( pointMap )
{
    setText( i18n( "Remove point" ) );
}

void KoPointRemoveCommand::redo()
{
    KoPathShapePointMap::iterator it( m_pointMap.begin() );
    m_data.clear();
    for ( ; it != m_pointMap.end(); ++it )
    {
        it.key()->repaint();

        foreach( KoPathPoint *p, it.value() )
        {
            QPair<KoSubpath*, int> pointdata = it.key()->removePoint( p );
            m_data.push_back( KoPointRemoveData( p, pointdata.first, pointdata.second ) );
        }

        QPointF offset = it.key()->normalize();

        QMatrix matrix;
        matrix.translate( -offset.x(), -offset.y() );
        foreach( KoPathPoint *p, it.value() )
        {
            p->map( matrix );
        }
        // repaint new bounding rect
        // TODO is this really needed
        it.key()->repaint();
    }
}

void KoPointRemoveCommand::undo()
{
    // insert the points in inverse order
    KoPathShape * pathShape = 0;
    for( int i = m_data.size()-1; i >= 0; --i )
    {
        KoPointRemoveData &data = m_data[i];
        if ( pathShape && pathShape != data.m_point->parent() )
        {
            pathShape->normalize();
            pathShape->repaint();
        }
        pathShape = data.m_point->parent();
        pathShape->insertPoint( data.m_point, data.m_subpath, data.m_position );
    }
    if ( pathShape )
    {
        pathShape->normalize();
        pathShape->repaint();
    }
}

KoSegmentSplitCommand::KoSegmentSplitCommand( KoPathShape *shape, const KoPathSegment &segment, double splitPosition, QUndoCommand *parent )
: KoPathBaseCommand( shape, parent )
, m_deletePoint( false )
{
    if( segment.first && segment.second )
    {
        m_segments << segment;
        m_oldNeighbors << qMakePair( *segment.first, *segment.second );
        m_newNeighbors << qMakePair( *segment.first, *segment.second );
        m_splitPoints << 0;
        m_splitPointPos << qMakePair( (KoSubpath*)0, 0 );
        m_splitPos << splitPosition;
    }

    setText( i18n( "Split segment" ) );
}

KoSegmentSplitCommand::KoSegmentSplitCommand( KoPathShape *shape, const QList<KoPathSegment> &segments, const QList<double> &splitPositions, QUndoCommand *parent )
: KoPathBaseCommand( shape, parent )
, m_deletePoint( false )
{
    Q_ASSERT(segments.size() == splitPositions.size());
    // check all the segments
    for( int i = 0; i < segments.size(); ++i )
    {
        const KoPathSegment &segment = segments[i];
        if( segment.first && segment.second )
        {
            m_segments << segment;
            m_oldNeighbors << qMakePair( *segment.first, *segment.second );
            m_newNeighbors << qMakePair( *segment.first, *segment.second );
            m_splitPoints << 0;
            m_splitPointPos << qMakePair( (KoSubpath*)0, 0 );
            m_splitPos << splitPositions[i];
        }
    }

    setText( i18n( "Split segment" ) );
}

KoSegmentSplitCommand::KoSegmentSplitCommand( KoPathShape *shape, const QList<KoPathSegment> &segments, double splitPosition, QUndoCommand *parent )
: KoPathBaseCommand( shape, parent )
, m_deletePoint( false )
{
    // check all the segments
    for( int i = 0; i < segments.size(); ++i )
    {
        const KoPathSegment &segment = segments[i];
        if( segment.first && segment.second )
        {
            m_segments << segment;
            m_oldNeighbors << qMakePair( *segment.first, *segment.second );
            m_newNeighbors << qMakePair( *segment.first, *segment.second );
            m_splitPoints << 0;
            m_splitPointPos << qMakePair( (KoSubpath*)0, 0 );
            m_splitPos << splitPosition;
        }
    }

    setText( i18n( "Split segment" ) );
}

KoSegmentSplitCommand::~KoSegmentSplitCommand()
{
    if( m_deletePoint )
    {
        foreach( KoPathPoint* p, m_splitPoints )
            delete p;
    }
}

void KoSegmentSplitCommand::redo()
{
    repaint( false );

    m_deletePoint = false;

    KoPathShape *shape = *m_shapes.begin();

    for( int i = 0; i < m_segments.size(); ++i )
    {
        KoPathSegment &segment = m_segments[i];
        KoPathPoint *splitPoint = m_splitPoints[i];
        if( ! splitPoint )
        {
            m_splitPoints[i] = shape->splitAt( segment, m_splitPos[i] );
            m_newNeighbors[i] = qMakePair( *segment.first, *segment.second );
        }
        else
        {
            shape->insertPoint( splitPoint, m_splitPointPos[i].first, m_splitPointPos[i].second );
            *segment.first = m_newNeighbors[i].first;
            *segment.second = m_newNeighbors[i].second;
        }
    }
    repaint( true );
}

void KoSegmentSplitCommand::undo()
{
    repaint( false );

    m_deletePoint = true;

    KoPathShape *shape = *m_shapes.begin();

    for( int i = m_segments.size()-1; i >= 0; --i )
    {
        KoPathSegment &segment = m_segments[i];
        m_splitPointPos[i] = shape->removePoint( m_splitPoints[i] );
        *segment.first = m_oldNeighbors[i].first;
        *segment.second = m_oldNeighbors[i].second;
    }
    repaint( true );
}

KoPointJoinCommand::KoPointJoinCommand( KoPathShape *shape, KoPathPoint *point1, KoPathPoint *point2, QUndoCommand *parent )
: KoPathBaseCommand( shape, parent )
, m_point1( point1 )
, m_point2( point2 )
, m_joined( false )
{
    setText( i18n( "Join points" ) );
}

void KoPointJoinCommand::redo()
{
    KoPathShape *shape = *m_shapes.begin();
    m_joined = shape->joinBetween( m_point1, m_point2 );
    shape->repaint();
}

void KoPointJoinCommand::undo()
{
    KoPathShape *shape = *m_shapes.begin();
    if( m_joined )
    {
        shape->breakAt( KoPathSegment( m_point1, m_point2 ) );
        shape->repaint();
    }
}

KoSubpathBreakCommand::KoSubpathBreakCommand( KoPathShape *shape, KoPathPoint *breakPoint, QUndoCommand *parent )
: KoPathBaseCommand( shape, parent )
, m_broken( false )
, m_breakSegment( 0, 0 )
, m_breakPoint( breakPoint )
, m_newPoint( 0 )
{
    Q_ASSERT( breakPoint );

    KoSubpath *subpath = shape->subpathOfPoint( breakPoint );
    // copy the whole subpath as it can be reordered during breaking
    foreach( KoPathPoint* point, *subpath )
    {
        KoPathPoint data = *point;
        m_pointData.append( PointData( point, data ) );
    }

    setText( i18n( "Break subpath" ) );
}

KoSubpathBreakCommand::KoSubpathBreakCommand( KoPathShape *shape, const KoPathSegment &segment, QUndoCommand *parent )
: KoPathBaseCommand( shape, parent )
, m_broken( false )
, m_breakSegment( segment )
, m_breakPoint( 0 )
, m_newPoint( 0 )
{
    KoSubpath *subpath = shape->subpathOfPoint( segment.first );
    // copy the whole subpath as it can be reordered during breaking
    foreach( KoPathPoint* point, *subpath )
    {
        KoPathPoint data = *point;
        m_pointData.append( PointData( point, data ) );
    }

    setText( i18n( "Break subpath" ) );
}

KoSubpathBreakCommand::~KoSubpathBreakCommand()
{
}

void KoSubpathBreakCommand::redo()
{
    KoPathShape *shape = *m_shapes.begin();

    repaint(false);

    if( m_breakPoint )
        m_broken = shape->breakAt( m_breakPoint, m_newPoint );
    else if( m_breakSegment.first && m_breakSegment.second )
        m_broken = shape->breakAt( m_breakSegment );

    repaint(false);
}

void KoSubpathBreakCommand::undo()
{
    if( ! m_broken )
        return;

    KoPathShape *shape = *m_shapes.begin();

    repaint(false);

    KoSubpath *subpath = 0;
    if( m_breakPoint )
    {
        subpath = shape->subpathOfPoint( m_breakPoint );
        shape->joinBetween( m_breakPoint, m_newPoint );
    }
    else
    {
        shape->joinBetween( m_breakSegment.first, m_breakSegment.second );
        subpath = shape->subpathOfPoint( m_breakSegment.first );
    }

    subpath->clear();

    delete m_newPoint;
    m_newPoint = 0;

    foreach( PointData data, m_pointData )
    {
        *data.first = data.second;
        subpath->append( data.first );
    }

    repaint(false);
}

KoSegmentTypeCommand::KoSegmentTypeCommand( KoPathShape *shape, const KoPathSegment &segment, bool changeToLine, QUndoCommand *parent )
: KoPathBaseCommand( shape, parent )
, m_changeToLine( changeToLine )
{
    if( segment.first && segment.second )
        m_segments.append( segment );

    setText( i18n( "Change segment type" ) );
}

KoSegmentTypeCommand::KoSegmentTypeCommand( KoPathShape *shape, const QList<KoPathSegment> &segments, bool changeToLine, QUndoCommand *parent )
: KoPathBaseCommand( shape, parent )
, m_changeToLine( changeToLine )
{
    foreach( KoPathSegment segment, segments )
    {
        if( segment.first && segment.second )
            m_segments.append( segment );
    }

    setText( i18n( "Change segment type" ) );
}

void KoSegmentTypeCommand::redo()
{
    repaint( false );

    m_oldPointData.clear();
    foreach( KoPathSegment s, m_segments )
    {
        m_oldPointData.insert( s.first, *s.first );
        m_oldPointData.insert( s.second, *s.second );
    }

    foreach( KoPathSegment s, m_segments )
    {
        if( m_changeToLine )
        {
            s.first->unsetProperty( KoPathPoint::HasControlPoint2 );
            s.second->unsetProperty( KoPathPoint::HasControlPoint1 );
        }
        else
        {
            // check if segment is already a curve
            if( s.first->properties() & KoPathPoint::HasControlPoint2 || s.second->properties() & KoPathPoint::HasControlPoint1 )
                continue;

            QPointF pointDiff = s.second->point() - s.first->point();
            s.first->setControlPoint2( s.first->point() + 0.3 * pointDiff );
            s.second->setControlPoint1( s.first->point() + 0.7 * pointDiff );
        }
    }

    QPointF offset = (*m_shapes.begin())->normalize();
    QMatrix matrix;
    matrix.translate( -offset.x(), -offset.y() );
    QMap<KoPathPoint*, KoPathPoint>::iterator it = m_oldPointData.begin();
    for(; it != m_oldPointData.end(); ++it )
        it.value().map( matrix );

    repaint( false );
}

void KoSegmentTypeCommand::undo()
{
    repaint( false );

    KoPathPoint defaultPoint( 0, QPointF(0,0) );
    foreach( KoPathSegment s, m_segments )
    {
        *s.first = m_oldPointData.value( s.first, defaultPoint );
        *s.second = m_oldPointData.value( s.second, defaultPoint );
    }

    repaint( true );
}

KoPathCombineCommand::KoPathCombineCommand( KoShapeControllerBase *controller, const QList<KoPathShape*> &paths,
                                            QUndoCommand *parent )
: QUndoCommand( parent )
, m_controller( controller )
, m_paths( paths )
, m_combinedPath( 0 )
, m_isCombined( false )
{
    setText( i18n( "Combine paths" ) );
}

KoPathCombineCommand::~KoPathCombineCommand()
{
    if( m_isCombined && m_controller )
    {
        foreach( KoPathShape* path, m_paths )
            delete path;
    }
    else
        delete m_combinedPath;
}

void KoPathCombineCommand::redo()
{
    if( ! m_paths.size() )
        return;

    if( ! m_combinedPath )
    {
        m_combinedPath = new KoPathShape();
        KoShapeContainer * parent = m_paths.first()->parent();
        if(parent)
            parent->addChild(m_combinedPath);
        m_combinedPath->setBorder( m_paths.first()->border() );
        m_combinedPath->setShapeId( m_paths.first()->shapeId() );
        // combine the paths
        foreach( KoPathShape* path, m_paths )
            m_combinedPath->combine( path );
    }

    m_isCombined = true;

    if( m_controller )
    {
        foreach( KoPathShape* p, m_paths )
            m_controller->removeShape( p );

        m_controller->addShape( m_combinedPath );
    }
}

void KoPathCombineCommand::undo()
{
    if( ! m_paths.size() )
        return;

    m_isCombined = false;

    if( m_controller )
    {
        m_controller->removeShape( m_combinedPath );
        foreach( KoPathShape* p, m_paths )
        {
            m_controller->addShape( p );
        }
    }
}

KoParameterChangeCommand::KoParameterChangeCommand( KoParameterShape *shape, int handleId, const QPointF &startPoint, const QPointF &endPoint, QUndoCommand *parent )
: QUndoCommand( parent )
, m_shape( shape )
, m_handleId( handleId )    
, m_startPoint( startPoint )    
, m_endPoint( endPoint )    
{
    setText( i18n( "Change parameter" ) );
}

KoParameterChangeCommand::~KoParameterChangeCommand()
{
}

/// redo the command
void KoParameterChangeCommand::redo()
{
    m_shape->repaint();
    m_shape->moveHandle( m_handleId, m_endPoint );
    m_shape->repaint();
}

/// revert the actions done in redo
void KoParameterChangeCommand::undo()
{
    m_shape->repaint();
    m_shape->moveHandle( m_handleId, m_startPoint );
    m_shape->repaint();
}

KoParameterToPathCommand::KoParameterToPathCommand( KoParameterShape *shape, QUndoCommand *parent )
: QUndoCommand( parent )
{
    m_shapes.append( shape );

    setText( i18n( "Convert to Path" ) );
}

KoParameterToPathCommand::KoParameterToPathCommand( const QList<KoParameterShape*> &shapes, QUndoCommand *parent )
: QUndoCommand( parent )
, m_shapes( shapes )
{
    setText( i18n( "Convert to Path" ) );
}

KoParameterToPathCommand::~KoParameterToPathCommand()
{
}

void KoParameterToPathCommand::redo()
{
    foreach( KoParameterShape *shape, m_shapes )
    {
        shape->setModified( true );
        // TODO use global handle sizes here when we have them
        shape->repaint( shape->outline().controlPointRect().adjusted( -5.0, -5.0, 5.0, 5.0 ) );
    }
}

void KoParameterToPathCommand::undo()
{
    foreach( KoParameterShape *shape, m_shapes )
    {
        shape->setModified( false );
        // TODO use global handle sizes here when we have them
        shape->repaint( shape->outline().controlPointRect().adjusted( -5.0, -5.0, 5.0, 5.0 ) );
    }
}

KoPathSeparateCommand::KoPathSeparateCommand( KoShapeControllerBase *controller, const QList<KoPathShape*> &paths,
                                              QUndoCommand *parent )
: QUndoCommand( parent )
, m_controller( controller )
, m_paths( paths )
, m_isSeparated( false )
{
    setText( i18n( "Separate paths" ) );
}

KoPathSeparateCommand::~KoPathSeparateCommand()
{
    if( m_isSeparated && m_controller )
    {
        foreach( KoPathShape* p, m_paths )
            delete p;
    }
    else
    {
        foreach( KoPathShape* p, m_separatedPaths )
            delete p;
    }
}

void KoPathSeparateCommand::redo()
{
    if( m_separatedPaths.isEmpty() )
    {
        foreach( KoPathShape* p, m_paths )
        {
            QList<KoPathShape*> separatedPaths;
            if( p->separate( separatedPaths ) )
                m_separatedPaths << separatedPaths;
        }
    }

    m_isSeparated = true;

    if( m_controller )
    {
        foreach( KoPathShape* p, m_paths )
            m_controller->removeShape( p );
        foreach( KoPathShape *p, m_separatedPaths )
            m_controller->addShape( p );
    }
    foreach( KoPathShape* p, m_paths )
        p->repaint();
}

void KoPathSeparateCommand::undo()
{
    if( m_controller )
    {
        foreach( KoPathShape *p, m_separatedPaths )
            m_controller->removeShape( p );
        foreach( KoPathShape* p, m_paths )
            m_controller->addShape( p );
    }

    m_isSeparated = false;

    foreach( KoPathShape* p, m_paths )
        p->repaint();
}
