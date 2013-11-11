/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

// System includes
#include <QSqlDatabase>
#include <QFile>
#include <QDateTime>
#include <QVariant>
#include <QStringList>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>

// User includes
#include "radiohistorymodel_p.h"
#include "radiohistorymodel.h"
#include "radiohistoryitem.h"
#include "radiohistoryitem_p.h"
#include "radiostation.h"
#include "radiologger.h"

static const QLatin1String DATABASE_NAME    ( "c:\\radioplayhistory.db" );
static const QLatin1String DATABASE_DRIVER  ( "QSQLITE" );
static const QLatin1String HISTORY_TABLE    ( "history" );
static const QLatin1String SQL_CREATE_TABLE ( "CREATE TABLE history ("
                                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                "artist TEXT NOT NULL, "
                                                "title TEXT NOT NULL, "
                                                "station TEXT NOT NULL, "
                                                "frequency INTEGER NOT NULL, "
                                                "tagged INTEGER NOT NULL DEFAULT 0, "
                                                "fromRds INTEGER NOT NULL DEFAULT 1, "
                                                "time INTEGER NOT NULL)" );

static const QLatin1String SQL_ADD_ITEM     ( "INSERT INTO history (artist,title,station,frequency,fromRds,time) "
                                                "VALUES ( ?,?,?,?,?,? )" );

static const QLatin1String SQL_SELECT_ALL   ( "SELECT * FROM history ORDER BY id DESC" );
static const QLatin1String SQL_SELECT_TAGGED( "SELECT * FROM history WHERE tagged=1" );// ORDER BY id DESC";

static const QLatin1String SQL_DELETE_ALL   ( "DELETE FROM history" );
static const QLatin1String SQL_DELETE_RECENT( "DELETE FROM history WHERE tagged=0" );
//static const QLatin1String SQL_DELETE_TAGGED    = "DELETE FROM history WHERE tagged=1";
static const QLatin1String SQL_CLEAR_TAGS   ( "UPDATE history SET tagged = 0 WHERE tagged = 1" );

//static static const QLatin1String SQL_FIND_ITEM_BY_ID( "SELECT * FROM history WHERE id = ?" );
static const QLatin1String SQL_TOGGLE_TAG   ( "UPDATE history SET tagged = ? WHERE id = ?" );

static const QLatin1String SQL_DELETE_ITEM_FORMAT_STR ( "DELETE FROM history WHERE id = %1" );
static const QLatin1String SQL_REMOVE_TAG_FORMAT_STR  ( "UPDATE history SET tagged = 0 WHERE id = %1" );
static const QLatin1String OR_ID_IS_FORMAT_STR        (" OR id = %1");

static const int MAX_ID_COUNT_IN_QUERY = 5;

#ifdef LOGGING_ENABLED
#   define GET_ERR( param ) GETSTRING( param.lastError().text() )
#   define GET_ERR_PTR( param ) GETSTRING( param->lastError().text() )
#endif // LOGGING_ENABLED

/*!
 * Static utility function to parse a frequency
 */
static  QString parseFrequency( const uint frequency )
{
    QString loc = qtTrId( "txt_rad_dblist_val_l1_mhz" );
    return loc.arg( RadioStation::parseFrequency( frequency ) );
}

/*!
 *
 */
RadioHistoryModelPrivate::RadioHistoryModelPrivate( RadioHistoryModel* model,
                                                    RadioUiEngine& uiEngine ) :
    q_ptr( model ),
    mUiEngine( uiEngine ),
    mRtItemClass( -1 ),
    mTopItemIsPlaying( false ),
    mShowDetails( true ),
    mViewMode( ShowAll )
{
}

/*!
 *
 */
RadioHistoryModelPrivate::~RadioHistoryModelPrivate()
{
    if ( mDatabase && mDatabase->isOpen() ) {
        mDatabase->close();
    }
}

/*!
 *
 */
bool RadioHistoryModelPrivate::connectToDatabase()
{
    LOG_METHOD;
    QSqlDatabase db = QSqlDatabase::addDatabase( DATABASE_DRIVER );
    if ( db.isValid() ) {
        mDatabase.reset( new QSqlDatabase( db ) );
        mDatabase->setDatabaseName( DATABASE_NAME );

        if ( !mDatabase->open() ) {
            LOG_FORMAT( "Failed to open database! error = %s", GET_ERR_PTR( mDatabase ) );
            mDatabase.reset();
            return false;
        }

        // Create the table if it does not exist
        if ( !mDatabase->tables().contains( HISTORY_TABLE ) ) {
            LOG( "RadioHistoryModelPrivate::connectToDatabase: Creating database tables." );
            QSqlQuery query;
            if ( !query.exec( SQL_CREATE_TABLE ) ) {
                LOG_FORMAT( "Database creation failed! error = %s", GET_ERR( query ) );
                mDatabase->close();
                mDatabase.reset();
                return false;
            }
        }
    } else {
        LOG_FORMAT( "Invalid database! error = %s", GET_ERR( db ) );
        return false;
    }

    mQueryModel.reset( new QSqlQueryModel() );
    setViewMode( ShowAll );

    return mQueryModel->lastError().type() == QSqlError::NoError;
}

/*!
 *
 */
void RadioHistoryModelPrivate::addItem( const QString& artist,
                                        const QString& title,
                                        const RadioStation& station,
                                        bool fromRds )
{
    LOG_FORMAT( "RadioHistoryModelPrivate::addItem. Artist: %s, Title: %s", GETSTRING( artist ), GETSTRING( title ) );

    if ( !mQueryModel ) {
        return;
    }

    mTopItemIsPlaying = true;

    QSqlQuery query = beginTransaction();

    query.prepare( SQL_ADD_ITEM );
    query.addBindValue( artist );
    query.addBindValue( title );
    query.addBindValue( station.name() );
    query.addBindValue( static_cast<int>( station.frequency() / 1000 ) );
    query.addBindValue( fromRds );
    query.addBindValue( QDateTime::currentDateTime().toTime_t() );

    commitTransaction( query, InsertRows, 0 );
}

/*!
 *
 */
int RadioHistoryModelPrivate::rowCount() const
{
    if ( !mQueryModel ) {
        return 0;
    }
    return mQueryModel->rowCount();
}

/*!
 *
 */
QVariant RadioHistoryModelPrivate::data( const int row, const int role ) const
{
    if ( mQueryModel->lastError().type() == QSqlError::NoError ) {

        QSqlRecord record = mQueryModel->record( row );
        if ( role == Qt::DisplayRole ) {

            QString artist = record.value( RadioHistoryValue::Artist ).toString();
            if ( artist.isEmpty() ) {
                artist = qtTrId( "txt_rad_dblist_unknown" );
            }

            const QString title = record.value( RadioHistoryValue::Title ).toString();
            const QString station = record.value( RadioHistoryValue::Station ).toString();
            const uint frequency = record.value( RadioHistoryValue::Frequency ).toUInt() * 1000;

            QStringList list;
            if ( mShowDetails ) {
                QString formatter = qtTrId( "txt_rad_dblist_1_2" );
                LOG_FORMAT( "---formatter--- %s", GETSTRING( formatter ) );

                const QString firstRow = QString( formatter ).arg( artist ).arg( title );
                LOG_FORMAT( "---firstRow--- %s", GETSTRING( firstRow ) );
                list.append( firstRow );

                const uint timeInSecs = record.value( RadioHistoryValue::Time ).toUInt();
                QDateTime dateTime;
                dateTime.setTime_t( timeInSecs );

                QString time = dateTime.toString( Qt::SystemLocaleShortDate );
                LOG_FORMAT( "---time--- %s", GETSTRING( time ) );

                QString name = !station.isEmpty() ? station : parseFrequency( frequency );
                LOG_FORMAT( "---name--- %s", GETSTRING( name ) );
                const QString secondRow = QString( formatter ).arg( time ).arg( name );
                LOG_FORMAT( "---secondRow--- %s", GETSTRING( secondRow ) );

                list.append( secondRow );
            } else {
                list.append( artist );
                list.append( title );
            }

            return list;
        } else if ( role == Qt::DecorationRole ) {
            QVariantList list;
            const bool tagged = record.value( RadioHistoryValue::Tagged ).toBool();
            if ( tagged ) {
                list.append( mTaggedIcon );
            } else {
                list.append( mNonTaggedIcon );
            }
            return list;
        }
    }

    return QVariant();
}

/*!
 *
 */
void RadioHistoryModelPrivate::removeAll( bool removeTagged )
{
    if ( !mQueryModel ) {
        return;
    }

    QSqlQuery query = beginTransaction();

    query.prepare( removeTagged ? SQL_CLEAR_TAGS : SQL_DELETE_ALL );

    // Commented out because rowsRemoved() seems to crash HbListView
//    commitTransaction( query, RemoveRows, 0, rowCount() - 1 );

    commitTransaction( query, NoOp, 0 );
    q_ptr->reset();
}

/*!
 *
 */
void RadioHistoryModelPrivate::removeByModelIndices( QModelIndexList& indices,  bool removeTags )
{
    if ( !mQueryModel ) {
           return;
    }       
    QString sqlStr = "";
    int rowIndex = -1;    
    
    QSqlQuery query( *mDatabase );
    mDatabase->transaction();
    // List needs to be sorted and indices needs to go throught from largest to smallest.
    // This is for keeping QmodelIndexing in sync after begin- and endremoverows
    // calls when content is not yet actually removed.
    // Real removal happens in QSqlQuery::exec
    qSort(indices);
    QModelIndexList::const_iterator iter = indices.constEnd();
    QModelIndexList::const_iterator begin = indices.constBegin();
    for ( int counter = 1; iter != begin; ) {
        iter--;
        rowIndex = (*iter).row();
        if( rowIndex > -1 ) {
            QSqlRecord record = mQueryModel->record(rowIndex);
            
            if( counter > 1 ) {
                sqlStr += QString( OR_ID_IS_FORMAT_STR ).arg(record.value("id").toInt());  
            } else {
                sqlStr = QString( removeTags ? SQL_REMOVE_TAG_FORMAT_STR 
                                 : SQL_DELETE_ITEM_FORMAT_STR ).arg(record.value("id").toInt());  
            }
            // adding max MAX_ID_COUNT_IN_QUERY ids to Query
            if( counter == MAX_ID_COUNT_IN_QUERY ) {
                if( !prepareAndExec( query, sqlStr ) ) {
                    // error, do not proceed
                    break;
                }
                counter = 1;
                sqlStr = "";
            } else {
                counter++;
            }   
            q_ptr->beginRemoveRows( QModelIndex(), rowIndex, rowIndex );
            q_ptr->endRemoveRows();
        }
    }
    if( !query.lastError().isValid() && sqlStr.length() ) {
        prepareAndExec( query, sqlStr );
    }
    if( query.lastError().isValid() ) {
        // in case of error, rollback everyhing and reset model
        mDatabase->rollback();
        q_ptr->reset(); 
    } else {
        mDatabase->commit();
        refreshModel();
    }
}

/*!
 *
 */
void RadioHistoryModelPrivate::setViewMode( ViewMode mode )
{
    if ( !mQueryModel ) {
        return;
    }

    mViewMode = mode;
    mQueryModel->setQuery( mode == ShowTagged ? SQL_SELECT_TAGGED : SQL_SELECT_ALL, *mDatabase );
}

/*!
 *
 */
void RadioHistoryModelPrivate::toggleTagging( const RadioHistoryItem& item, const int row )
{
    QSqlQuery updateQuery = beginTransaction();

    updateQuery.prepare( SQL_TOGGLE_TAG );
    updateQuery.addBindValue( item.isTagged() ? 0 : 1 );
    updateQuery.addBindValue( item.id() );
    Operation operation = ChangeData;
    if ( mViewMode == ShowTagged && item.isTagged() ) {
        operation = RemoveRows;
    }
    commitTransaction( updateQuery, operation, row );
}

/*!
 *
 */
RadioHistoryItem RadioHistoryModelPrivate::itemAtIndex( const QModelIndex& index ) const
{
    LOG_METHOD;
    RadioHistoryItem item;
    QSqlRecord record = mQueryModel->record( index.row() );
    item.data_ptr()->initFromRecord( record );
    return item;
}

/*!
 *
 */
void RadioHistoryModelPrivate::refreshModel()
{
    setViewMode( mViewMode );
}

/*!
 *
 */
QSqlQuery RadioHistoryModelPrivate::beginTransaction()
{
    LOG_METHOD;
    QSqlQuery newQuery( *mDatabase );
    mDatabase->transaction();
    return newQuery;
}

/*!
 *
 */
void RadioHistoryModelPrivate::commitTransaction( QSqlQuery& query, Operation operation, int start, int end )
{
    LOG_METHOD;
    if ( end == -1 ) {
        end = start;
    }

    bool success = false;
    Q_UNUSED( success );
    if ( query.exec() ) {
        if ( operation == InsertRows ) {
            q_ptr->beginInsertRows( QModelIndex(), start, end );
        } else if ( operation == RemoveRows ) {
            q_ptr->beginRemoveRows( QModelIndex(), start, end );
        }

        success = mDatabase->commit();
        LOG_ASSERT( success, LOG_FORMAT( "Commit failed! err: %s", GET_ERR_PTR( mDatabase ) ) );

        refreshModel();

        if ( operation == InsertRows ) {
            q_ptr->endInsertRows();
            q_ptr->emitItemAdded();
        } else if ( operation == RemoveRows ) {
            q_ptr->endRemoveRows();
        } else if ( operation == ChangeData ) {
            q_ptr->reportChangedData( start );
        }
    } else {
        LOG_FORMAT( "RadioHistoryModelPrivate::commitTransaction FAILED, rolling back: error = %s", GET_ERR( query ) );
        success = mDatabase->rollback();
        LOG_ASSERT( success, LOG_FORMAT( "Rollback failed! err: %s", GET_ERR_PTR( mDatabase ) ) );
    }
}

bool RadioHistoryModelPrivate::prepareAndExec( QSqlQuery& query, const QString& sqlStr )
{
    bool isOk = true;
    isOk = query.prepare(sqlStr);

    if(isOk) {
        isOk = query.exec();
    }
    return isOk;
}
