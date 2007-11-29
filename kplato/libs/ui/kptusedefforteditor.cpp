/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptusedefforteditor.h"

#include "kptitemmodelbase.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>

#include <kdebug.h>

#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"

namespace KPlato
{

UsedEffortItemModel::UsedEffortItemModel ( QWidget *parent )
    : QAbstractItemModel( parent ),
    m_completion( 0 )
{
    m_headers << i18n( "Resource" );
    for ( int i = 1; i <= 7; ++i ) {
        m_headers << KGlobal::locale()->calendar()->weekDayName( i, KCalendarSystem::ShortDayName );
    }
    m_headers << i18n( "This Week" );
}

Qt::ItemFlags UsedEffortItemModel::flags ( const QModelIndex &index ) const
{
    
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( ! index.isValid() || index.column() == 8 ) {
        return flags;
    }
    if ( index.column() == 0 ) {
        const Resource *r = resource( index );
        if ( r ) {
            if ( m_resourcelist.contains( r ) && ! m_completion->usedEffortMap().contains( r ) ) {
                return flags | Qt::ItemIsEditable;
            }
        }
        return flags;
    }
    return flags | Qt::ItemIsEditable;
}

QVariant UsedEffortItemModel::data ( const QModelIndex &index, int role ) const
{
    if ( ! index.isValid() ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole: {
            if ( index.column() == 0 ) {
                const Resource *r = resource( index );
                //kDebug()<<index.row()<<","<<index.column()<<""<<r;
                if ( r ) {
                    return r->name();
                }
                break;
            }
            Completion::UsedEffort *ue = usedEffort( index );
            if ( ue == 0 ) {
                return QVariant();
            }
            if ( index.column() == 8 ) {
                // Total
                //kDebug()<<index.row()<<","<<index.column()<<" total"<<endl;
                double res = 0.0;
                foreach ( QDate d, m_dates ) {
                    Completion::UsedEffort::ActualEffort *e = ue->effort( d );
                    if ( e ) {
                        res += e->normalEffort().toDouble( Duration::Unit_h );
                    }
                }
                return KGlobal::locale()->formatNumber( res, 1 );
            }
            Completion::UsedEffort::ActualEffort *e = ue->effort( m_dates.value( index.column() - 1 ) );
            double res = 0.0;
            if ( e ) {
                res = e->normalEffort().toDouble( Duration::Unit_h );
            }
            return KGlobal::locale()->formatNumber( res, 1 );
        }
        case Qt::EditRole: {
            if ( index.column() == 8 ) {
                return QVariant();
            }
            if ( index.column() == 0 ) {
                const Resource *r = resource( index );
                //kDebug()<<index.row()<<","<<index.column()<<" "<<r<<endl;
                if ( r ) {
                    return r->name();
                }
            } else {
                Completion::UsedEffort *ue = usedEffort( index );
                if ( ue == 0 ) {
                    return QVariant();
                }
                Completion::UsedEffort::ActualEffort *e = ue->effort( m_dates.value( index.column() - 1 ) );
                double res = 0.0;
                if ( e ) {
                    res = e->normalEffort().toDouble( Duration::Unit_h );
                }
                return res;
            }
            break;
        }
        case Role::EnumList: {
            if ( index.column() == 0 ) {
                QStringList lst = m_editlist.keys();
                return lst;
            }
            break;
        }
        case Role::EnumListValue: {
            if ( index.column() == 0 ) {
                return m_editlist.values().indexOf( resource( index ) );
            }
            break;
        }
        default: break;
    }
    return QVariant();
}

bool UsedEffortItemModel::setData ( const QModelIndex &idx, const QVariant &value, int role )
{
    kDebug()<<endl;
    switch ( role ) {
        case Qt::EditRole: {
            if ( idx.column() == 8 ) {
                return false;
            }
            if ( idx.column() == 0 ) {
                const Resource *er = resource( idx );
                Q_ASSERT( er != 0 );
                
                Q_ASSERT ( m_editlist.count() > value.toInt() );

                const Resource *v = m_editlist.values().value( value.toInt() );
                Q_ASSERT( v != 0 );
                
                int x = m_resourcelist.indexOf( er );
                Q_ASSERT( x != -1 );
                m_resourcelist.replace( x, v );
                m_completion->addUsedEffort( v );
                emit dataChanged( createIndex( idx.row(), 1 ), createIndex( idx.row(), columnCount() - 1 ) );
                emit rowInserted( createIndex( idx.row(), 0 ) );
                return true;
            }
            Completion::UsedEffort *ue = usedEffort( idx );
            if ( ue == 0 ) {
                return false;
            }
            QDate d = m_dates.value( idx.column() - 1 );
            Completion::UsedEffort::ActualEffort *e = ue->effort( d );
            if ( e == 0 ) {
                e = new Completion::UsedEffort::ActualEffort();
                ue->setEffort( d, e );
            }
            e->setNormalEffort( Duration( value.toDouble(), Duration::Unit_h ) );
            emit dataChanged( idx, idx );
            return true;
        }
        default: break;
    }
    return false;
}

bool UsedEffortItemModel::submit()
{
    kDebug()<<endl;
    return QAbstractItemModel::submit();
}

void UsedEffortItemModel::revert()
{
    kDebug()<<endl;
    QList<const Resource*> lst = m_resourcelist;
    foreach ( const Resource *r, lst ) {
        if ( ! m_completion->usedEffortMap().contains( r ) ) {
            int row = m_resourcelist.indexOf( r );
            if ( row != -1 ) {
                beginRemoveRows( QModelIndex(), row, row );
                m_resourcelist.removeAt( row );
                endRemoveRows();
            }
        }
    }
}

QVariant UsedEffortItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Vertical ) {
        return QVariant();
    }
    if ( section < 0 || section >= m_headers.count() ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
            return m_headers.at( section );
        case Qt::ToolTipRole: {
            if ( section >= 1 && section <= 7 ) {
                return KGlobal::locale()->formatDate( m_dates.at( section - 1 ) );
            }
            if ( section == 8 ) {
                return i18n( "Total effort this week" );
            }
            break;
        }
        default: break;
    }
    return QVariant();
}

int UsedEffortItemModel::columnCount(const QModelIndex & parent ) const
{
    int c = 0;
    if ( m_completion && ! parent.isValid() ) {
        c = 9;
    }
    return c;
}

int UsedEffortItemModel::rowCount(const QModelIndex & ) const
{
    int rows = 0;
    if ( m_completion ) {
        rows = m_resourcelist.count();
    }
    return rows;
}

bool UsedEffortItemModel::hasChildren ( const QModelIndex &parent ) const
{
    return ! parent.isValid();
}

QModelIndex UsedEffortItemModel::index ( int row, int column, const QModelIndex &parent ) const
{
    if ( parent.isValid() ) {
        return QModelIndex();
    }
    return createIndex( row, column );
}

void UsedEffortItemModel::setCompletion( Completion *completion )
{
    m_completion = completion;
    m_resourcelist.clear();
    QMap<QString, const Resource*> lst;
    foreach ( const Resource *r, completion->usedEffortMap().keys() ) {
        lst.insertMulti( r->name(), r );
    }
    m_resourcelist = lst.values();
    reset();
}

const Resource *UsedEffortItemModel::resource(const QModelIndex &index ) const
{
    int row = index.row();
    if ( m_completion == 0 || row < 0 || row >= m_resourcelist.count() ) {
        return 0;
    }
    return m_resourcelist.value( row );
}

Completion::UsedEffort *UsedEffortItemModel::usedEffort(const QModelIndex &index ) const
{
    const Resource *r = resource( index );
    if ( r == 0 ) {
        return 0;
    }
    return m_completion->usedEffort( r );
}

void UsedEffortItemModel::setCurrentMonday( const QDate &date )
{
    m_dates.clear();
    for ( int i = 0; i < 7; ++i ) {
        m_dates << date.addDays( i );
    }
    emit headerDataChanged ( Qt::Horizontal, 1, 7 );
    reset();
}

QModelIndex UsedEffortItemModel::addRow()
{
    if ( m_project == 0 ) {
        return QModelIndex();
    }
    m_editlist.clear();
    foreach ( Resource *r, m_project->resourceList() ) {
        if ( ! m_resourcelist.contains( r ) ) {
            m_editlist.insertMulti( r->name(), r );
        }
    }
    if ( m_editlist.isEmpty() ) {
        return QModelIndex();
    }
    int row = rowCount();
    beginInsertRows( QModelIndex(), row, row );
    m_resourcelist.append( m_editlist.values().first() );
    endInsertRows();
    return createIndex( row, 0, const_cast<Resource*>( m_editlist.values().first() ) );
}

//-----------
UsedEffortEditor::UsedEffortEditor( QWidget *parent )
    : QTableView( parent )
{
    UsedEffortItemModel *m = new UsedEffortItemModel(this );
    setModel( m );
    
    setItemDelegateForColumn ( 0, new EnumDelegate( this ) );
    
    setItemDelegateForColumn ( 1, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 2, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 3, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 4, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 5, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 6, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 7, new DoubleSpinBoxDelegate( this ) );
    
    connect ( model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), SIGNAL( changed() ) );
    
    connect ( m, SIGNAL( rowInserted( const QModelIndex& ) ), SIGNAL( resourceAdded() ) );
}

void UsedEffortEditor::setProject( Project *p )
{
    static_cast<UsedEffortItemModel*>( model() )->setProject( p );
}

void UsedEffortEditor::setCompletion( Completion *completion )
{
    static_cast<UsedEffortItemModel*>( model() )->setCompletion( completion );
}

void UsedEffortEditor::setCurrentMonday( const QDate &date )
{
    static_cast<UsedEffortItemModel*>( model() )->setCurrentMonday( date );
}

void UsedEffortEditor::addResource()
{
    UsedEffortItemModel *m = static_cast<UsedEffortItemModel*>( model() );
    QModelIndex i = m->addRow();
    if ( i.isValid() ) {
        setCurrentIndex( i );
        edit( i );
    }
}

//----------------------------------------
CompletionEntryItemModel::CompletionEntryItemModel ( QWidget *parent )
    : QAbstractItemModel( parent ),
    m_node( 0 ),
    m_manager( 0 ),
    m_completion( 0 )
{
    m_headers << i18n( "Date" )
            // xgettext: no-c-format
            << i18n( "% Completed" )
            << i18n( "Used Effort" )
            << i18n( "Remaining Effort" )
            << i18n( "Planned Effort" );

    m_flags[ 1 ] = Qt::ItemIsEditable;
    m_flags[ 3 ] = Qt::ItemIsEditable;

}

void CompletionEntryItemModel::slotDataChanged()
{
    refresh();
}

void CompletionEntryItemModel::setManager( ScheduleManager *sm )
{
    m_manager = sm;
    refresh();
}

Qt::ItemFlags CompletionEntryItemModel::flags ( const QModelIndex &index ) const
{
    if ( index.isValid() ) {
        return QAbstractItemModel::flags( index ) | m_flags[ index.column() ];
    }
    return QAbstractItemModel::flags( index );
}

QVariant CompletionEntryItemModel::date ( int row, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: 
        case Qt::EditRole: 
            return m_datelist.value( row );
        default: break;
    }
    return QVariant();
}

QVariant CompletionEntryItemModel::percentFinished ( int row, int role ) const
{
    Completion::Entry *e = m_completion->entry( date( row ).toDate() );
    if ( e == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole: 
        case Qt::EditRole: 
            return e->percentFinished;
        default: break;
    }
    return QVariant();
}

QVariant CompletionEntryItemModel::remainingEffort ( int row, int role ) const
{
    Completion::Entry *e = m_completion->entry( date( row ).toDate() );
    if ( e == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            //TODO
            QList<double> scales;
            Duration::Unit unit = Duration::Unit_h;
            if ( m_node && m_node->projectNode() ) {
                Project *p = static_cast<Project*>( m_node->projectNode() );
                unit = m_node->estimate()->unit();
                QList<double> scales; // TODO: week
                if ( m_node->estimate()->type() == Estimate::Type_Effort ) {
                    scales << p->standardWorktime()->day();
                    // rest is default
                }
            }
            //double v = Estimate::scale( e->remainingEffort, unit, scales );
            double v = e->remainingEffort.toDouble( unit );
            //kDebug()<<m_node->name()<<": "<<v<<" "<<unit<<" : "<<scales<<endl;
            return KGlobal::locale()->formatNumber( v, 1 ) +  Duration::unitToString( unit, true );
        }
        case Qt::EditRole:
            return e->remainingEffort.milliseconds();
        case Role::DurationScales: {
            QVariantList lst; // TODO: week
            if ( m_node && m_node->projectNode() ) {
                Project *p = static_cast<Project*>( m_node->projectNode() );
                if ( m_node->estimate()->type() == Estimate::Type_Effort ) {
                    lst.append( p->standardWorktime()->day() );
                }
            }
            if ( lst.isEmpty() ) {
                lst.append( 24.0 );
            }
            lst << 60.0 << 60.0 << 1000.0;
            return lst;
        }
        case Role::DurationUnit:
            if ( m_node ) {
                return static_cast<int>( m_node->estimate()->unit() );
            }
            return static_cast<int>( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CompletionEntryItemModel::actualEffort ( int row, int role ) const
{
    Completion::Entry *e = m_completion->entry( date( row ).toDate() );
    if ( e == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            //TODO
            QList<double> scales;
            Duration::Unit unit = Duration::Unit_h;
            if ( m_node && m_node->projectNode() ) {
                Project *p = static_cast<Project*>( m_node->projectNode() );
                unit = m_node->estimate()->unit();
                QList<double> scales; // TODO: week
                if ( m_node->estimate()->type() == Estimate::Type_Effort ) {
                    scales << p->standardWorktime()->day();
                    // rest is default
                }
            }
            double v = -1.0;
            if ( m_completion->entrymode() == Completion::EnterEffortPerResource ) {
                v = Estimate::scale( m_completion->actualEffortTo( date( row ).toDate() ), unit, scales );
            } else {
                v = Estimate::scale( e->totalPerformed, unit, scales );
            }
            //kDebug()<<m_node->name()<<": "<<v<<" "<<unit<<" : "<<scales<<endl;
            return KGlobal::locale()->formatNumber( v, 1 ) +  Duration::unitToString( unit, true );
        }
        case Qt::EditRole:
            return e->totalPerformed.milliseconds();
        case Role::DurationScales: {
            QVariantList lst; // TODO: week
            if ( m_node && m_node->projectNode() ) {
                Project *p = static_cast<Project*>( m_node->projectNode() );
                if ( m_node->estimate()->type() == Estimate::Type_Effort ) {
                    lst.append( p->standardWorktime()->day() );
                }
            }
            if ( lst.isEmpty() ) {
                lst.append( 24.0 );
            }
            lst << 60.0 << 60.0 << 1000.0;
            return lst;
        }
        case Role::DurationUnit:
            if ( m_node ) {
                return static_cast<int>( m_node->estimate()->unit() );
            }
            return static_cast<int>( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CompletionEntryItemModel::plannedEffort ( int row, int role ) const
{
    if ( m_node == 0 || m_manager == 0 ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            //TODO
            QList<double> scales;
            Duration::Unit unit = Duration::Unit_h;
            if ( m_node && m_node->projectNode() ) {
                Project *p = static_cast<Project*>( m_node->projectNode() );
                unit = m_node->estimate()->unit();
                QList<double> scales; // TODO: week
                if ( m_node->estimate()->type() == Estimate::Type_Effort ) {
                    scales << p->standardWorktime()->day();
                    // rest is default
                }
            }
            //double v = Estimate::scale( m_node->plannedEffort( m_manager->id() ), unit, scales );
            double v = m_node->plannedEffort( m_manager->id() ).toDouble( unit );
            //kDebug()<<m_node->name()<<": "<<v<<" "<<unit<<" : "<<scales<<endl;
            return KGlobal::locale()->formatNumber( v, 1 ) +  Duration::unitToString( unit, true );
        }
        case Qt::EditRole:
            return QVariant();
        case Role::DurationScales: {
            QVariantList lst; // TODO: week
            if ( m_node && m_node->projectNode() ) {
                Project *p = static_cast<Project*>( m_node->projectNode() );
                if ( m_node->estimate()->type() == Estimate::Type_Effort ) {
                    lst.append( p->standardWorktime()->day() );
                }
            }
            if ( lst.isEmpty() ) {
                lst.append( 24.0 );
            }
            lst << 60.0 << 60.0 << 1000.0;
            return lst;
        }
        case Role::DurationUnit:
            if ( m_node ) {
                return static_cast<int>( m_node->estimate()->unit() );
            }
            return static_cast<int>( Duration::Unit_h );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CompletionEntryItemModel::data ( const QModelIndex &index, int role ) const
{
    if ( ! index.isValid() ) {
        return QVariant();
    }
    switch ( index.column() ) {
        case 0: return date( index.row(), role );
        case 1: return percentFinished( index.row(), role );
        case 2: return actualEffort( index.row(), role );
        case 3: return remainingEffort( index.row(), role );
        case 4: return plannedEffort( index.row(), role );
        default: break;
    }
    return QVariant();
}

bool CompletionEntryItemModel::setData ( const QModelIndex &idx, const QVariant &value, int role )
{
    kDebug()<<endl;
    switch ( role ) {
        case Qt::EditRole: {
            if ( idx.column() == 0 ) {
                QDate od = date( idx.row() ).toDate();
                removeEntry( od );
                addEntry( value.toDate() );
                // emit dataChanged( idx, idx );
                return true;
            }
            if ( idx.column() == 1 ) {
                Completion::Entry *e = m_completion->entry( date( idx.row() ).toDate() );
                if ( e == 0 ) {
                    return false;
                }
                e->percentFinished = value.toInt();
                if ( m_completion->entrymode() == Completion::EnterCompleted && m_node ) {
                    // calculate used/remaining
                    Duration est = m_node->estimate()->pertExpected();
                    e->totalPerformed = est * e->percentFinished / 100;
                    e->remainingEffort = est - e->totalPerformed;
                }
                emit dataChanged( idx, createIndex( idx.row(), 3 ) );
                return true;
            }
            if ( idx.column() == 2 ) {
                Completion::Entry *e = m_completion->entry( date( idx.row() ).toDate() );
                if ( e == 0 ) {
                    return false;
                }
                Duration d( value.toList()[0].toLongLong() );
                if ( d == e->totalPerformed ) {
                    return false;
                }
                e->totalPerformed = d;
                emit dataChanged( idx, idx );
                return true;
            }
            if ( idx.column() == 3 ) {
                Completion::Entry *e = m_completion->entry( date( idx.row() ).toDate() );
                if ( e == 0 ) {
                    return false;
                }
                Duration d( value.toList()[0].toLongLong() );
                if ( d == e->remainingEffort ) {
                    return false;
                }
                e->remainingEffort = d;
                emit dataChanged( idx, idx );
                return true;
            }
        }
        default: break;
    }
    return false;
}

bool CompletionEntryItemModel::submit()
{
    kDebug()<<endl;
    return QAbstractItemModel::submit();
}

void CompletionEntryItemModel::revert()
{
    kDebug()<<endl;
    refresh();
}

QVariant CompletionEntryItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Vertical ) {
        return QVariant();
    }
    if ( section < 0 || section >= m_headers.count() ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
            return m_headers.at( section );
        default: break;
    }
    return QVariant();
}

int CompletionEntryItemModel::columnCount(const QModelIndex & parent ) const
{
    return 5;
}

int CompletionEntryItemModel::rowCount(const QModelIndex &idx ) const
{
    if ( m_completion == 0 || idx.isValid() ) {
        return 0;
    }
    return m_datelist.count();
}

bool CompletionEntryItemModel::hasChildren ( const QModelIndex &parent ) const
{
    return ! parent.isValid();
}

QModelIndex CompletionEntryItemModel::index ( int row, int column, const QModelIndex &parent ) const
{
    if ( parent.isValid() ) {
        return QModelIndex();
    }
    return createIndex( row, column );
}

void CompletionEntryItemModel::setCompletion( Completion *completion )
{
    m_completion = completion;
    refresh();
}

void CompletionEntryItemModel::refresh()
{
    m_datelist.clear();
    m_flags[ 2 ] = 0;
    if ( m_completion ) {
        m_datelist = m_completion->entries().keys();
        if ( m_completion->entrymode() == Completion::EnterEffortPerTask ) {
            m_flags[ 2 ] = Qt::ItemIsEditable;
        }
    }
    kDebug()<<m_datelist<<endl;
    reset();
}

QModelIndex CompletionEntryItemModel::addRow()
{
    if ( m_completion == 0 ) {
        return QModelIndex();
    }
    int row = rowCount();
    QDate d = QDate::currentDate();
    if ( row > 0 && d <= m_datelist.last() ) {
        d = m_datelist.last().addDays( 1 );
    }
    beginInsertRows( QModelIndex(), row, row );
    m_datelist.append( d );
    endInsertRows();
    return createIndex( row, 0 );
}

void CompletionEntryItemModel::removeEntry( const QDate date )
{
    int i = m_datelist.indexOf( date );
    if ( i != -1 ) {
        beginRemoveRows( QModelIndex(), i, i );
        m_datelist.removeAt( i );
        endRemoveRows();
        kDebug()<<date<<" removed"<<endl;
    }
    m_completion->takeEntry( date );
}

void CompletionEntryItemModel::addEntry( const QDate date )
{
    kDebug()<<date<<endl;
    Completion::Entry *e = new Completion::Entry();
    if ( m_completion->entries().isEmpty() ) {
        if ( m_node ) {
            e->remainingEffort = m_node->estimate()->pertExpected();
        }
    } else {
        e->percentFinished = m_completion->percentFinished();
        e->totalPerformed = m_completion->actualEffort();
        e->remainingEffort = m_completion->remainingEffort();
    }
    m_completion->addEntry( date, e );
    refresh();
    int i = m_datelist.indexOf( date );
    if ( i != -1 ) {
        emit rowInserted( date );
        emit dataChanged( createIndex( i, 1 ), createIndex( i, rowCount() - 1 ) );
    } else  kError()<<"Failed to find added entry: "<<date<<endl;
}


//-----------
CompletionEntryEditor::CompletionEntryEditor( QWidget *parent )
    : QTableView( parent )
{
    CompletionEntryItemModel *m = new CompletionEntryItemModel(this );
    setModel( m );
    
//    setItemDelegateForColumn ( 1, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 2, new DurationSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 3, new DurationSpinBoxDelegate( this ) );
    
    connect ( m, SIGNAL( rowInserted( const QDate ) ), SIGNAL( rowInserted( const QDate ) ) );
    connect ( model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), SIGNAL( changed() ) );
}

void CompletionEntryEditor::setCompletion( Completion *completion )
{
    kDebug()<<endl;
    model()->setCompletion( completion );
}

void CompletionEntryEditor::addEntry()
{
    kDebug()<<endl;
    QModelIndex i = model()->addRow();
    if ( i.isValid() ) {
        model()->setFlags( i.column(), model()->flags( i ) | Qt::ItemIsEditable );
        setCurrentIndex( i );
        edit( i );
        model()->setFlags( i.column(), model()->flags( i ) & ~Qt::ItemIsEditable );
    }
}

void CompletionEntryEditor::removeEntry()
{
    kDebug()<<endl;
    //static_cast<CompletionEntryItemModel*>( model() )->setCompletion( completion );
}


}  //KPlato namespace

#include "kptusedefforteditor.moc"
