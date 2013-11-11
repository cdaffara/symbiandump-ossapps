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
* Description:  The implementation for presentation elements.
*
*/


#include <bautils.h>

#include "irdebug.h" 
#include "irsessiondb.h"
#include "irsessionloggerutility.h"

_LIT( KSessionTable, "Session" ); 
_LIT( KID, "id" );
_LIT( KSessionLogCol,  "SessionData" );
_LIT( KSessionIndex, "SessionIndex" );

const TInt KMaxNoSession = 15;


// ---------------------------------------------------------------------------
// Function : NewL()
// two phased construction
// ---------------------------------------------------------------------------
//
  CIRSessionDb* CIRSessionDb::NewL()
    {
    IRLOG_DEBUG( "CIRSessionDb::NewL" );
    CIRSessionDb* self;
    self = CIRSessionDb::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRSessionDb::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// Function : NewLC()
// Two phased construction
// ---------------------------------------------------------------------------
//
  CIRSessionDb* CIRSessionDb::NewLC()
    {
    IRLOG_DEBUG( "CIRSessionDb::NewLC" );
    CIRSessionDb *self;
    self = new( ELeave )CIRSessionDb;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRSessionDb::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRSessionDb::~CIRSessionDb()
// default destructor
// ---------------------------------------------------------------------------
//
CIRSessionDb::~CIRSessionDb()
    {
    IRLOG_DEBUG( "CIRSessionDb::~CIRSessionDb" );
    CloseDb();
    iFsSession.Close();
    IRLOG_DEBUG( "CIRSessionDb::~CIRSessionDb - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRSessionDb::CreateDbConditional()
// Creates both the dbms files conditionally( only if not yet created )
// calls CreateDbL()
// ---------------------------------------------------------------------------
//
 TInt CIRSessionDb::CreateDbConditional( TFileName &aSession )
    {
    IRLOG_DEBUG( "CIRSessionDb::CreateDbConditional" );
    iDbFile.Copy( aSession );
    if ( !BaflUtils::FileExists( iFsSession, iDbFile ) )
        {
        TRAPD( error, CreateDbL( iDbFile ) );
        if ( error )
            {
            IRLOG_DEBUG( "CIRSessionDb::CreateDbConditional - Exiting ( 1 )." );
            return error;
            }    
        }
    IRLOG_DEBUG( "CIRSessionDb::CreateDbConditional - Exiting ( 2 )." );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIRSessionDb:AddSessionStartL()
// adds the session log entry into data base
// ---------------------------------------------------------------------------
//
void CIRSessionDb::AddSessionStartL( CIRSessionLogger& aSession )
    {
    IRLOG_DEBUG( "CIRSessionDb::AddSessionStartL" );
    OpenDbL();
    RDbTable sessionlogtable;
    TInt error=sessionlogtable.Open( iSessionDb, KSessionTable, sessionlogtable.
        EUpdatable );
    CleanupClosePushL( sessionlogtable );
    if ( error )
        {
        CloseDb();
        User::LeaveIfError( error );    
        }
    
    //! arrange the presets in incresing order of index
    sessionlogtable.SetIndex( KSessionIndex );
    sessionlogtable.Reset();

    //if session log is greater or equal to than 5
    if ( sessionlogtable.CountL() >= KMaxNoSession )
        {
        //first row is selected
        sessionlogtable.FirstL();
        //the current row is selected
        sessionlogtable.GetL();
        //delete that entry
        sessionlogtable.DeleteL();
        }    
    CleanupStack::PopAndDestroy( &sessionlogtable );
    //Algorithm : else condition need not handle seperatly
    //Algorithm : add sessionid and informations like
    //starttime,connectedfrom,sessionid,connectiontype,channelid
    //currentnetwork,homenetwork,sessiontable 
    //Algorithm: if no. of session is greater than 5

    _LIT( query, "SELECT * FROM %S" );    
    HBufC* sqlStr = HBufC::NewLC( query().Length() + KSessionTable().Length() );
    sqlStr->Des().Format( query, &KSessionTable );
    
    // Create a view on the database
    RDbView view;     
    error = view.Prepare( iSessionDb, *sqlStr );
    if ( error )
        {
        CloseDb();
        User::LeaveIfError( error );    
        }
    CleanupStack::PopAndDestroy( sqlStr );     
    CleanupClosePushL( view );
    error = view.EvaluateAll();
    if ( error )
        {
        CloseDb();
        User::LeaveIfError( error );    
        }
    CDbColSet* columns = view.ColSetL();
    CleanupStack::PushL( columns );
    
    RDbColWriteStream writeStream;
    TRAP( error, //trap start
       // Insert a row. Column order matches sql select statement
        view.InsertL();
        //get index
        view.SetColL( columns->ColNo( KID ), aSession.SessionId() );    
        //!open stream
        writeStream.OpenLC( view, columns->ColNo( KSessionLogCol ) );
        aSession.ExternalizeL( writeStream );
        writeStream.CommitL();
        CleanupStack::PopAndDestroy( &writeStream );
         );
         
    CleanupStack::PopAndDestroy( columns );
    if ( error != KErrNone )
        {
        CloseDb();
        User::LeaveIfError( error );
        }
    view.PutL();
    CleanupStack::PopAndDestroy( &view );
    CloseDb();
    IRLOG_DEBUG( "CIRSessionDb::AddSessionStartL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRSessionDb::GetAllPresetL()
// gets all the preset into an array 
// ---------------------------------------------------------------------------
//
void CIRSessionDb::GetAllSessionL(
    CArrayPtrFlat<CIRSessionLogger>& aSessionDataList )
    {
    IRLOG_DEBUG( "CIRSessionDb::GetAllSessionL" );
    OpenDbL();
    //not sure about this resetanddestroy
     //! Open for preset master
    aSessionDataList.ResetAndDestroy();        
    //! temp item for holding the retrived data
    CIRSessionLogger *item;
    RDbColReadStream instream;    
    RDbTable table;
    TInt error = table.Open( iSessionDb, KSessionTable, table.EReadOnly );
    CleanupClosePushL( table );
    if ( error != KErrNone )
         {
         //if open fails function leaves
         CloseDb();
         User::LeaveIfError( error );
         }

    CDbColSet* colSet = table.ColSetL();
    CleanupStack::PushL( colSet );

    //! arrange the presets in incresing order of index
    table.SetIndex( KSessionIndex );
    table.Reset();
    //! recursively retrive the preset data from the master table
    for ( table.FirstL(); table.AtRow(); table.NextL() )
        {
        item = CIRSessionLogger::NewL();
        CleanupStack::PushL( item );
        table.GetL();
        instream.OpenLC( table, colSet->ColNo( KSessionLogCol ) );
        item->InternalizeL( instream );
        //update sessionid
        aSessionDataList.AppendL( item );
        CleanupStack::PopAndDestroy( &instream );
        CleanupStack::Pop( item );
        }
        
    CleanupStack::PopAndDestroy( colSet );
    //!close the master table
    CleanupStack::PopAndDestroy( &table );
    CloseDb();
    IRLOG_DEBUG( "CIRSessionDb::GetAllSessionL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : DeleteAllSession
// delete all the session from session log entry
// ---------------------------------------------------------------------------
//
void CIRSessionDb::DeleteAllSessionL()
    { 
    IRLOG_DEBUG( "CIRSessionDb::DeleteAllSessionL" ); 
    //opening a data base 
    OpenDbL();
    RDbTable sessionlogtable;
    //data base table opened
    TInt error = sessionlogtable.Open( iSessionDb, KSessionTable, sessionlogtable.
        EUpdatable );
    CleanupClosePushL( sessionlogtable );
    if ( error != KErrNone )
         {
         //if error we leave
         CloseDb();
         User::LeaveIfError( error );
         }
    //data base begin     
    error = iSessionDb.Begin();
    if ( error != KErrNone )
         {
         //if open fails function leaves
         CloseDb();
         User::LeaveIfError( error );
         }

    //! arrange the presets in incresing order of index
    sessionlogtable.SetIndex( KSessionIndex );
    sessionlogtable.Reset();
    sessionlogtable.FirstL();
    while( sessionlogtable.AtRow() )
        {
        //deleting all the rows in the table
        sessionlogtable.GetL();
        sessionlogtable.DeleteL();
        sessionlogtable.FirstL();
        }
    //saving the change
    CleanupStack::PopAndDestroy( &sessionlogtable );
    iSessionDb.Commit();
    CloseDb();    
    IRLOG_DEBUG( "CIRSessionDb::DeleteAllSessionL - Exiting." );
    }   

// ---------------------------------------------------------------------------
// CIRSessionDb::ConstructL()
// Standard 2nd phase construction
// ---------------------------------------------------------------------------
//
void CIRSessionDb::ConstructL()
    {
    IRLOG_DEBUG( "CIRSessionDb::ConstructL" );
    User::LeaveIfError( iFsSession.Connect() );
    IRLOG_DEBUG( "CIRSessionDb::ConstructL- Exiting." );    
    }

// ---------------------------------------------------------------------------
// CIRSessionDb::CloseDb()
// Closes the database 
// ---------------------------------------------------------------------------
//
void CIRSessionDb::CloseDb()
    {
    IRLOG_DEBUG( "CIRSessionDb::CloseDb" );
    iSessionDb.Close();
    IRLOG_DEBUG( "CIRSessionDb::CloseDb - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : CreateSessionTableL
// creates sessionlogtable with two column one is sessionid and rest of session
// log data
// ---------------------------------------------------------------------------
//SessionTable
//---------------------------
//| KID   |     KSessionLogCol |
//---------------------------
//|TInt32 | EDbColLongText8 |
//---------------------------
//
void CIRSessionDb::CreateSessionTableL()
    {
    IRLOG_DEBUG( "CIRSessionDb::CreateSessionTableL" );
    //start time of session
    TDbCol sessionid( KID, EDbColInt32 );
    sessionid.iAttributes = TDbCol::ENotNull; 
    
    //!this column is used to store preset data
    //!The column stores a potentially large amount of non-Unicode text data.
    TDbCol sessionlogcol( KSessionLogCol, EDbColLongText8 );
       sessionlogcol.iAttributes = TDbCol::ENotNull; 
       
    CDbColSet* sessionlogcolset = CDbColSet::NewLC();
    sessionlogcolset->AddL( sessionid );
    sessionlogcolset->AddL( sessionlogcol );

    // Create the sessionlog table with two columns
    
    User::LeaveIfError( iSessionDb.CreateTable( KSessionTable,
         *sessionlogcolset ) );
    
    //Create the KeyIndex for the table
    CreateSessionIndexL();
    CleanupStack::PopAndDestroy( sessionlogcolset );
    IRLOG_DEBUG( "CIRSessionDb::CreateSessionTableL - Exiting." );    
    }

// ---------------------------------------------------------------------------
// Function : CreateSessionIndexL
// sets sessionid as the primary key
// ---------------------------------------------------------------------------
//
void CIRSessionDb::CreateSessionIndexL()
    {
    IRLOG_DEBUG( "CIRSessionDb::CreateSessionIndexL" );
    TDbKeyCol sessionid( KID );
    CDbKey* index = CDbKey::NewLC();
    index->AddL( sessionid );
    User::LeaveIfError( iSessionDb.CreateIndex( KSessionIndex, KSessionTable,
        *index ) );
    CleanupStack::PopAndDestroy( index );
    IRLOG_DEBUG( "CIRSessionDb::CreateSessionIndexL - Exiting." );
    }
    
// ---------------------------------------------------------------------------
// CIRSessionDb::OpenDbL()
// opening the data base 
// ---------------------------------------------------------------------------
//
 void CIRSessionDb::OpenDbL()
    {
    IRLOG_DEBUG( "CIRSessionDb::OpenDbL" );
    CloseDb();
    TInt error = KErrNone;
    if ( !BaflUtils::FileExists( iFsSession, iDbFile ) )
        {
        //if file doesn't exist function leaves with error code
        //KErrNotFound
        error = KErrNotFound;
        User::LeaveIfError( error );
        }

    error = iSessionDb.Open( iFsSession, iDbFile );    
    if ( error != KErrNone )
        {
        //if database is failed to open then 
        //function leaves
        IRLOG_ERROR2( "CIRSessionDb::OpenDbL - Opening session database failed ( %d )", error );
        User::LeaveIfError( error );
        }
    if ( iSessionDb.IsDamaged() || !iSessionDb.InTransaction() )
        {
        //if data base is damaged then 
        //it tried to recover
        //if recovery is not possible function leaves
        error = iSessionDb.Recover();
        if ( KErrNone == error )
            {
            //if recovered data base is compacted
            error = iSessionDb.Compact();    
            }
        User::LeaveIfError( error );    
        }
    IRLOG_DEBUG( "CIRSessionDb::OpenDbL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRSessionDb::CreateDbL()
// Creates both the dbms files 
// calls CreateFavMasterTableL(),CreateFavUrlTableL
// database filename
// ---------------------------------------------------------------------------
//
 void CIRSessionDb::CreateDbL( TFileName& aSession )
    {
    IRLOG_DEBUG( "CIRSessionDb::CreateDbL" );
    CloseDb();
    TInt error = iSessionDb.Replace( iFsSession, aSession );
    if ( error != KErrNone )
        {
        IRLOG_ERROR2( "CIRSessionDb::CreateDbL - Creating session database failed ( %d )", error );
        }
    User::LeaveIfError( error );
    CreateSessionTableL();
    IRLOG_DEBUG( "CIRSessionDb::CreateDbL - Exiting." );
    }
