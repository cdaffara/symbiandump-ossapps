/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     Retrieving maptile path from lookup db implementation
*
*/

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <locationservicedefines.h>
#include <maptileservice.h>
#include "mylocationsdefines.h"
#include "maptiledblookuptable.h"

// select all from
const QString KSelectAllFrom( "SELECT * FROM " );

// string 'where'
const QString KWhere( " WHERE " );

// string ' = '
const QString KEqual( " = " );

// string 'AND'
const QString KAnd( " AND " );
// string 'OR'
const QString KOr( " OR " );

// string '( ' 
const QString KOpenBrace( "( " );
// string ' )'
const QString KCloseBrace( " )" );

// Maptile table name
const QString KMaptileLookupTable( "maptilelookup " );
// column source id
const QString KSourceId( "sourceid" );
// column source type
const QString KSourceType( "sourcetype" );
// column maptile path
const QString KFilePath( "filepath" );
// column fetching status
const QString KStatus( "fetchingstatus" );

// column user setting status
const QString KUserSetting( "usersetting" );

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::LookupMapTileDatabase()
// Default constructor.
// -----------------------------------------------------------------------------
//
LookupMapTileDatabase::LookupMapTileDatabase(  QObject *parent ) :
        QObject( parent ),
        mDb( NULL ),
        mDbOpen( false )
{
	mDb = new QSqlDatabase();
    *mDb = QSqlDatabase::addDatabase( "QSQLITE" );
    mDb->setDatabaseName( KLocationDataLookupDbName );
    if (!mDb->open())
    {
        return;
    }

    // create lookup table if doesnot exist
    QSqlQuery query( *mDb );
    QString queryString;
    QTextStream ( &queryString ) << "create table if not exists " << KMaptileLookupTable << KOpenBrace << 
                       KSourceId << " int," <<
                       KSourceType << " int," <<
                       KFilePath << " varchar(255)," <<
                       KStatus << " int ," << 
                       KUserSetting << " bool " << "default 0" << KCloseBrace;
                       
                       
    query.exec( queryString );
    
    mDb->close();
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::~LookupMapTileDatabase()
// Destructor.
// -----------------------------------------------------------------------------
//
LookupMapTileDatabase::~LookupMapTileDatabase()
{
    close();
    delete mDb;
}

// ---------------------------------------------------------
// LookupMapTileDatabase::open()
// ---------------------------------------------------------
bool LookupMapTileDatabase::open()
{
    if( !mDbOpen )
    {
        mDbOpen = mDb->open();
    }
    return mDbOpen;
}

// ---------------------------------------------------------
// LookupMapTileDatabase::close()
// ---------------------------------------------------------
void LookupMapTileDatabase::close()
{
    if( mDbOpen )
        mDb->close();
    mDbOpen = false;
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::createEntry()
// Creates an entry in the lookup table.
// -----------------------------------------------------------------------------
//
void LookupMapTileDatabase::createEntry( const MaptileLookupItem& aLookupItem )
{
    if( mDbOpen )
    {
        QString queryString;
        QTextStream ( &queryString ) <<  
                "INSERT INTO " << KMaptileLookupTable << 
                KOpenBrace << KSourceId << ", " << KSourceType << ", " << KFilePath << ", " << KStatus << KCloseBrace <<
                " VALUES " << 
                KOpenBrace << ":sid" << ", " << ":stype"  << ", " << ":path" << ", " << ":status" << KCloseBrace;
                

        QSqlQuery query(*mDb);
        query.prepare( queryString );

        query.bindValue(":sid", aLookupItem.iUid);
        query.bindValue(":stype", aLookupItem.iSource);
        query.bindValue(":path", aLookupItem.iFilePath);
        query.bindValue(":status", aLookupItem.iFetchingStatus);
        query.exec();
    }
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::updateEntry()
// Updates an entry in the lookup table.
// -----------------------------------------------------------------------------
//
void LookupMapTileDatabase::updateEntry( const MaptileLookupItem& aLookupItem )
{
    if( mDbOpen )
    {
        QString queryString; //UPDATE maptilelookup SET filepath = ?, status = ? WHERE sourceid = ? AND sourcetype = ?"
        QTextStream ( &queryString ) <<  
                "UPDATE " << KMaptileLookupTable << " SET " <<
                KFilePath << " = ?, " << KStatus << " = ? ," << KUserSetting << " = ? " <<
                KWhere << 
                KSourceId << " = ? " << KAnd  << KSourceType << " = ? ";
                
        QSqlQuery query(*mDb);
        query.prepare( queryString );
    
        query.addBindValue( aLookupItem.iFilePath );
        query.addBindValue( aLookupItem.iFetchingStatus );   
        query.addBindValue( false ); 
        query.addBindValue( aLookupItem.iUid );
        query.addBindValue( aLookupItem.iSource );
        query.exec();
    }
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::deleteEntry()
// Deletes an entry from the lookup table.
// -----------------------------------------------------------------------------
//
void LookupMapTileDatabase::deleteEntry(MaptileLookupItem& aLookupItem)
{
    if( mDbOpen )
    {
        QString queryString; // DELETE FROM lplookup WHERE sourceid = ? AND sourcetype = ?"
        QTextStream ( &queryString ) <<  
                "DELETE FROM  " << KMaptileLookupTable <<
                KWhere << 
                KSourceId << " = ? " << KAnd  << KSourceType << " = ? ";
        
        QSqlQuery query(*mDb);
        query.prepare( queryString );
        
        query.addBindValue( aLookupItem.iUid );
        query.addBindValue( aLookupItem.iSource );
        
        query.exec();
    }
}


// -----------------------------------------------------------------------------
// LookupMapTileDatabase::deleteMapTile()
// Deletes an maptile if there's no reference to maptile in lookupdb
// -----------------------------------------------------------------------------
//
void LookupMapTileDatabase::deleteMapTile( const MaptileLookupItem& aLookupItem)
{
    if( mDbOpen )
    {
        QString queryString; //  "SELECT filepath FROM maptilelookuptable WHERE filepath = ?"
        QTextStream ( &queryString ) <<  
                KSelectAllFrom << KMaptileLookupTable << 
                KWhere <<
                KFilePath << KEqual << " ? " ;
        QSqlQuery query(*mDb);
        query.prepare( queryString );
        query.addBindValue( aLookupItem.iFilePath );
        
        query.exec();
        
        // Delete if no reference to maptile
        if ( !query.first() ) 
        {
            //delete all releted  maptile 
            QString temp = aLookupItem.iFilePath;
            temp.append(MAPTILE_IMAGE_PORTRAIT);       
            QFile file;
            file.remove(temp);
            
            temp = aLookupItem.iFilePath;
            temp.append(MAPTILE_IMAGE_CONTACT);
            temp.append(MAPTILE_IMAGE_LANDSCAPE);
            file.remove(temp);
            
            temp = aLookupItem.iFilePath;
            temp.append(MAPTILE_IMAGE_CALENDAR);
            temp.append(MAPTILE_IMAGE_LANDSCAPE);
            file.remove(temp);
            
            temp = aLookupItem.iFilePath;
            temp.append(MAPTILE_IMAGE_HURRIGANES);         
            file.remove(temp);
            
        }
    }
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::findEntriesByMapTileFetchingStatus()
// Finds a list of lookup items given a fetching status.
// -----------------------------------------------------------------------------
//
void LookupMapTileDatabase::findEntriesByMapTileFetchingState(const quint32 aFetchingState,
        QList<MaptileLookupItem>& aLookupItemArray)
{
    if( mDbOpen )
    {
        QString queryString; //  "SELECT * FROM maptilelookuptable WHERE fetchingstatus = %d"
        QTextStream ( &queryString ) <<  
                KSelectAllFrom << KMaptileLookupTable << 
                KWhere <<
                KStatus << KEqual << " ? " ;
        QSqlQuery query(*mDb);
        query.prepare( queryString );
        query.addBindValue( aFetchingState );
        
        query.exec();
        
        while( query.next() )
        {    
            QSqlRecord rec = query.record();
            MaptileLookupItem lookupItem;
            lookupItem.iUid = query.value( rec.indexOf( KSourceId ) ).toUInt();
            lookupItem.iSource = query.value( rec.indexOf( KSourceType ) ).toUInt();
            lookupItem.iFilePath = query.value( rec.indexOf( KFilePath ) ).toString();
            lookupItem.iFetchingStatus = query.value( rec.indexOf( KStatus ) ).toUInt();
            lookupItem.iUserSetting = query.value( rec.indexOf( KUserSetting ) ).toBool();
            aLookupItemArray.append( lookupItem );
        }
    }
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::getAllCalendarIds()
// Gets the list of calendar ids .
// -----------------------------------------------------------------------------
//
void LookupMapTileDatabase::getAllCalendarIds( QList<quint32>& aIdArray )
{
    if( mDbOpen )
    {
        QString queryString; //  "SELECT cntuid FROM maptilelookuptable WHERE sourcetype = %d");
        QTextStream ( &queryString ) <<  
                KSelectAllFrom << KMaptileLookupTable << 
                KWhere <<
                KSourceType << KEqual << " ? " ;
        QSqlQuery query(*mDb);
        query.prepare( queryString );
        query.addBindValue( ESourceCalendar );
        
        query.exec();
        
        while( query.next() )
        {    
            QSqlRecord rec = query.record();
            quint32 id = query.value( rec.indexOf( KSourceId ) ).toUInt();
            aIdArray.append( id );
        }
    }
}



// -----------------------------------------------------------------------------
// LookupMapTileDatabase::resetEntry()
// Reset the entry with null value and get the used maptile path as part of aLookupItem.
// -----------------------------------------------------------------------------
//
void LookupMapTileDatabase::resetEntry(MaptileLookupItem &aLookupItem)
{
    if( mDbOpen )
    {
        // getEntry will replace the fetching status. so copy fetching status to temparory variable. 
	      quint32 tempStatus = aLookupItem.iFetchingStatus;
	      
	      if( getEntry( aLookupItem ) )
	      {
	      	  // set file path to nullstring
	          aLookupItem.iFilePath = "";
	          aLookupItem.iFetchingStatus = tempStatus;
	          // update entry in db
	          updateEntry( aLookupItem );
	      }
	  }
}
 
// -----------------------------------------------------------------------------
// LookupMapTileDatabase::findNumberOfAddress()
// find the number of address associated with the aId.
// -----------------------------------------------------------------------------
//
int LookupMapTileDatabase::findNumberOfAddress( int& aId )
{
    int count = 0;

    if( mDbOpen )
    {
        QString queryString; //  "SELECT * FROM maptilelookup WHERE sourceid = aId AND ( fetchingstatus = MapTileFetchingInProgress OR fetchingstatus = MapTileFetchingNetworkError )"
        QTextStream ( &queryString ) <<  
                KSelectAllFrom << KMaptileLookupTable << 
                KWhere <<
                KSourceId << KEqual << aId << 
                KAnd << 
                KOpenBrace << KStatus << KEqual << MapTileService::MapTileFetchingInProgress << 
                              KOr << KStatus << KEqual << MapTileService::MapTileFetchingNetworkError << KCloseBrace ;
        QSqlQuery query(*mDb);
        query.exec( queryString );
        while( query.next() )  count++;
    }
    
    return count;
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::findEntry()
// Finds an entry in the lookup table.
// -----------------------------------------------------------------------------
//
bool LookupMapTileDatabase::findEntry( const MaptileLookupItem& aLookupItem )
{ 
    if( mDbOpen )
    {
        QString queryString; //  "SELECT * FROM maptilelookup WHERE sourceid = aLookupItem.iUid AND sourcetype = aLookupItem.iSource"
        QTextStream ( &queryString ) <<  
                KSelectAllFrom << KMaptileLookupTable << 
                KWhere <<
                KSourceId << KEqual << aLookupItem.iUid << 
                KAnd << 
                KSourceType << KEqual << aLookupItem.iSource ;
        QSqlQuery query(*mDb);
        query.exec( queryString );
        
        if ( query.first() ) 
        {
            return true;
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::getEntry()
// Gets a lookup item from the lookup table using source id and type.
// -----------------------------------------------------------------------------
//
bool LookupMapTileDatabase::getEntry( MaptileLookupItem& aLookupItem )
{ 
    if( mDbOpen )
    {
        QString queryString; //  "SELECT * FROM maptilelookup WHERE sourceid = aLookupItem.iUid AND sourcetype = aLookupItem.iSource"
        QTextStream ( &queryString ) <<  
                KSelectAllFrom << KMaptileLookupTable << 
                KWhere <<
                KSourceId << KEqual << aLookupItem.iUid << 
                KAnd << 
                KSourceType << KEqual << aLookupItem.iSource ;
        QSqlQuery query(*mDb);
        query.exec( queryString );
        
        if ( query.first() ) 
        {
            QSqlRecord rec = query.record();
            aLookupItem.iFilePath = query.value( rec.indexOf( KFilePath ) ).toString();
            aLookupItem.iFetchingStatus = query.value( rec.indexOf( KStatus ) ).toUInt();
            aLookupItem.iUserSetting = query.value( rec.indexOf( KUserSetting ) ).toBool();
            return true;
            		
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::findEntryByFilePath()
// Finds an entry in the lookup table for maptile image file
// -----------------------------------------------------------------------------
//
bool LookupMapTileDatabase::findEntryByFilePath( const QString& aFilePath )
{
    if( mDbOpen )
    {
        QString queryString; //  "SELECT filepath FROM maptilelookuptable WHERE filepath = ?"
        QTextStream ( &queryString ) <<  
                KSelectAllFrom << KMaptileLookupTable << 
                KWhere <<
                KFilePath << KEqual << " ? " ;
        QSqlQuery query(*mDb);
        query.prepare( queryString );
        query.addBindValue( aFilePath );
        
        query.exec();
        
        if( query.first() )
        {
            return true;
        }
    }
    
    return false;
}

// -----------------------------------------------------------------------------
// LookupMapTileDatabase::updateUserSetting()
// Updates an entry in the lookup table.
// -----------------------------------------------------------------------------
//
void LookupMapTileDatabase::updateUserSetting( const MaptileLookupItem& aLookupItem )
{
    if( mDbOpen )
    {
      
        QString queryString; 
        QTextStream ( &queryString ) <<  
                "UPDATE " << KMaptileLookupTable << " SET " <<
                    KStatus << " = ? ," << KUserSetting << " = ? " << 
                KWhere << 
                KSourceId << " = ? " << KAnd  << KSourceType << " = ? ";
                
        QSqlQuery query(*mDb);
        query.prepare( queryString );
        query.addBindValue( aLookupItem.iFetchingStatus );
        query.addBindValue( aLookupItem.iUserSetting );
        query.addBindValue( aLookupItem.iUid );
        query.addBindValue( aLookupItem.iSource );
        query.exec();
    }
}

// End of file

