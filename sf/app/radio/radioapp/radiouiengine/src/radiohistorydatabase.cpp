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
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QFile>
#include <QDateTime>
#include <QVariant>
//#include <QSqlRecord>

// User includes
#include "radiohistorydatabase.h"
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

#ifdef LOGGING_ENABLED
#   define GET_ERR( param ) GETSTRING( param.lastError().text() )
#   define GET_ERR_PTR( param ) GETSTRING( param->lastError().text() )
#endif // LOGGING_ENABLED

/*!
 *
 */
RadioHistoryDatabase::RadioHistoryDatabase()
{
    connectToDatabase();
}

/*!
 *
 */
RadioHistoryDatabase::~RadioHistoryDatabase()
{
    if ( mDatabase && mDatabase->isOpen() ) {
        mDatabase->close();
    }
}

/*!
 *
 */
void RadioHistoryDatabase::addItem( const QString& artist,
                                    const QString& title,
                                    const QString& stationName,
                                    uint frequency,
                                    bool fromRds )
{
    LOG_FORMAT( "RadioHistoryModelPrivate::addItem. Artist: %s, Title: %s", GETSTRING( artist ), GETSTRING( title ) );

    QSqlQuery query( *mDatabase );
    mDatabase->transaction();

    query.prepare( SQL_ADD_ITEM );
    query.addBindValue( artist );
    query.addBindValue( title );
    query.addBindValue( stationName );
    query.addBindValue( static_cast<int>( frequency / 1000 ) );
    query.addBindValue( fromRds );
    query.addBindValue( QDateTime::currentDateTime().toTime_t() );

    commitTransaction( query );
}

/*!
 *
 */
void RadioHistoryDatabase::clearRecent()
{
    QSqlQuery query( *mDatabase );
    mDatabase->transaction();

    query.prepare( SQL_CLEAR_TAGS );

    commitTransaction( query );
}

/*!
 *
 */
void RadioHistoryDatabase::clearTagged()
{
    QSqlQuery query( *mDatabase );
    mDatabase->transaction();

    query.prepare( SQL_DELETE_ALL );

    commitTransaction( query );
}

/*!
 *
 */
bool RadioHistoryDatabase::connectToDatabase()
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

    return true;
}

/*!
 *
 */
void RadioHistoryDatabase::commitTransaction( QSqlQuery& query )
{
    LOG_METHOD;

    bool success = false;
    Q_UNUSED( success );
    if ( query.exec() ) {

        success = mDatabase->commit();
        LOG_ASSERT( success, LOG_FORMAT( "Commit failed! err: %s", GET_ERR_PTR( mDatabase ) ) );

    } else {
        LOG_FORMAT( "RadioHistoryModelPrivate::commitTransaction FAILED, rolling back: error = %s", GET_ERR( query ) );
        success = mDatabase->rollback();
        LOG_ASSERT( success, LOG_FORMAT( "Rollback failed! err: %s", GET_ERR_PTR( mDatabase ) ) );
    }
}
