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
#include "irnmslogdb.h"
#include "irnmslogger.h"

_LIT( KNmsLogTable, "NmsLog" ); 
_LIT( KID, "id" );
_LIT( KNmsLogCol,  "NmsLogData" );
_LIT( KNmsLogIndex, "NmsLogIndex" );

const TInt KMaxNoNmsLog = 15;

// ---------------------------------------------------------------------------
// Function : NewL()
// two phased construction
// ---------------------------------------------------------------------------
//
  CIRNmsLogDb* CIRNmsLogDb::NewL()
    {
    IRLOG_DEBUG( "CIRNmsLogDb::NewL" );
    CIRNmsLogDb* self;
    self = CIRNmsLogDb::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRNmsLogDb::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// Function : NewLC()
// Two phased construction
// ---------------------------------------------------------------------------
//
  CIRNmsLogDb* CIRNmsLogDb::NewLC()
    {
    IRLOG_DEBUG( "CIRNmsLogDb::NewLC" );
    CIRNmsLogDb *self;
    self = new( ELeave )CIRNmsLogDb;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRNmsLogDb::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRNmsLogDb::~CIRNmsLogDb()
// default destructor
// ---------------------------------------------------------------------------
//
CIRNmsLogDb::~CIRNmsLogDb()
    {
    IRLOG_DEBUG( "CIRNmsLogDb::~CIRNmsLogDb" );
    CloseDb();
    iFsSession.Close();
    IRLOG_DEBUG( "CIRNmsLogDb::~CIRNmsLogDb - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNmsLogDb::CreateDbConditional()
// Creates both the dbms files conditionally( only if not yet created )
// calls CreateDbL()
// ---------------------------------------------------------------------------
//
 TInt CIRNmsLogDb::CreateDbConditional( TFileName &aNmsLog )
    {
    IRLOG_DEBUG( "CIRNmsLogDb::CreateDbConditional" );
    iDbFile.Copy( aNmsLog );
    if ( !BaflUtils::FileExists( iFsSession, iDbFile ) )
        {
        TRAPD( error, CreateDbL( iDbFile ) );
        if ( error )
            {
            IRLOG_DEBUG( "CIRNmsLogDb::CreateDbConditional - Exiting ( 1 )." );
            return error;
            }    
        }
    IRLOG_DEBUG( "CIRNmsLogDb::CreateDbConditional - Exiting ( 2 )." );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIRNmsLogDb:AddNmsLogStartL()
// adds the NmsLog log entry into data base
// ---------------------------------------------------------------------------
//
void CIRNmsLogDb::AddNmsLogStartL( CIRNmsLogger& aNmsLog )
    {
    IRLOG_DEBUG( "CIRNmsLogDb::AddNmsLogStartL" );
    OpenDbL();
    RDbTable nmsLogtable;
    TInt error=nmsLogtable.Open( iNmsLogDb, KNmsLogTable, nmsLogtable.
        EUpdatable );
    CleanupClosePushL( nmsLogtable );
    if ( error )
        {
        CloseDb();
        User::LeaveIfError( error );    
        }
    
    //! arrange the presets in incresing order of index
    nmsLogtable.SetIndex( KNmsLogIndex );
    nmsLogtable.Reset();

    //if NmsLog log is greater or equal to than 5
    if ( nmsLogtable.CountL() >= KMaxNoNmsLog )
        {
        //first row is selected
        nmsLogtable.FirstL();
        //the current row is selected
        nmsLogtable.GetL();
        //delete that entry
        nmsLogtable.DeleteL();
        }    
    CleanupStack::PopAndDestroy( &nmsLogtable );
    //Algorithm : else condition need not handle seperatly
    //Algorithm : add NmsLogid and informations like
    //starttime,connectedfrom,NmsLogid,connectiontype,channelid
    //currentnetwork,homenetwork,NmsLogtable 
    //Algorithm: if no. of NmsLog is greater than 5

    _LIT( query, "SELECT * FROM %S" );    
    HBufC* sqlStr=HBufC::NewLC( query().Length() + KNmsLogTable().Length() );
    sqlStr->Des().Format( query, &KNmsLogTable );
    
    // Create a view on the database
    RDbView view;     
    error = view.Prepare( iNmsLogDb, *sqlStr );
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
        view.SetColL( columns->ColNo( KID ), aNmsLog.NmsLogId() );    
        //!open stream
        writeStream.OpenLC( view, columns->ColNo( KNmsLogCol ) );
        aNmsLog.ExternalizeL( writeStream );
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
     IRLOG_DEBUG( "CIRNmsLogDb::AddNmsLogStartL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNmsLogDb::GetAllNmsLogL()
// gets all the preset into an array 
// ---------------------------------------------------------------------------
//
void CIRNmsLogDb::GetAllNmsLogL(
    CArrayPtrFlat<CIRNmsLogger>& aNmsLogDataList )
    {
    IRLOG_DEBUG( "CIRNmsLogDb::GetAllNmsLogL" );
    OpenDbL();
    //not sure about this resetanddestroy
     //! Open for preset master
    aNmsLogDataList.ResetAndDestroy();        
    //! temp item for holding the retrived data
    CIRNmsLogger *item;
    RDbColReadStream instream;    
    RDbTable table;
    TInt error = table.Open( iNmsLogDb, KNmsLogTable, table.EReadOnly );
    CleanupClosePushL( table );
    if ( error!=KErrNone )
         {
         //if open fails function leaves
         CloseDb();
         User::LeaveIfError( error );
         }

    CDbColSet* colSet = table.ColSetL();
    CleanupStack::PushL( colSet );

    //! arrange the presets in incresing order of index
    table.SetIndex( KNmsLogIndex );
    table.Reset();
    //! recursively retrive the preset data from the master table
    for ( table.FirstL(); table.AtRow(); table.NextL() )
        {
        item = CIRNmsLogger::NewL();
        CleanupStack::PushL( item );
        table.GetL();
        instream.OpenLC( table, colSet->ColNo( KNmsLogCol ) );
        item->InternalizeL( instream );
        //update NmsLogid
        aNmsLogDataList.AppendL( item );
        CleanupStack::PopAndDestroy( &instream );
        CleanupStack::Pop( item );
        }
        
    CleanupStack::PopAndDestroy( colSet );
    //!close the master table
    CleanupStack::PopAndDestroy( &table );
    CloseDb();
    IRLOG_DEBUG( "CIRNmsLogDb::GetAllNmsLogL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : DeleteAllNmsLog
// delete all the NmsLog from NmsLog log entry
// ---------------------------------------------------------------------------
//
void CIRNmsLogDb::DeleteAllNmsLogL()
    { 
    IRLOG_DEBUG( "CIRNmsLogDb::DeleteAllNmsLogL" ); 
    //opening a data base 
    OpenDbL();
    RDbTable nmsLogtable;
    //data base table opened
    TInt error = nmsLogtable.Open( iNmsLogDb, KNmsLogTable, nmsLogtable.
        EUpdatable );
    CleanupClosePushL( nmsLogtable );
    if ( error != KErrNone )
         {
         //if error we leave
         CloseDb();
         User::LeaveIfError( error );
         }
    //data base begin     
    error = iNmsLogDb.Begin();
    if ( error != KErrNone )
         {
         //if open fails function leaves
         CloseDb();
         User::LeaveIfError( error );
         }

    //! arrange the presets in incresing order of index
    nmsLogtable.SetIndex( KNmsLogIndex );
    nmsLogtable.Reset();
    nmsLogtable.FirstL();
    while( nmsLogtable.AtRow() )
        {
        //deleting all the rows in the table
        nmsLogtable.GetL();
        nmsLogtable.DeleteL();
        nmsLogtable.FirstL();
        }
    //saving the change
    CleanupStack::PopAndDestroy( &nmsLogtable );
    iNmsLogDb.Commit();
    CloseDb();    
    IRLOG_DEBUG( "CIRNmsLogDb::DeleteAllNmsLogL - Exiting." );
    }   

// ---------------------------------------------------------------------------
// CIRNmsLogDb::ConstructL()
// Standard 2nd phase construction
// ---------------------------------------------------------------------------
//
void CIRNmsLogDb::ConstructL()
    {
    IRLOG_DEBUG( "CIRNmsLogDb::ConstructL" );
    User::LeaveIfError( iFsSession.Connect() );
    IRLOG_DEBUG( "CIRNmsLogDb::ConstructL- Exiting." );    
    }

// ---------------------------------------------------------------------------
// CIRNmsLogDb::CloseDb()
// Closes the database 
// ---------------------------------------------------------------------------
//
void CIRNmsLogDb::CloseDb()
    {
    IRLOG_DEBUG( "CIRNmsLogDb::CloseDb" );
    iNmsLogDb.Close();
    IRLOG_DEBUG( "CIRNmsLogDb::CloseDb - Exiting." );
    }


// ---------------------------------------------------------------------------
// Function : CreateNmsLogTableL
// creates NmsLogtable with two column one is NmsLogid and rest of NmsLog
// log data
// ---------------------------------------------------------------------------
//NmsLogTable
//---------------------------
//| KID   |     KNmsLogCol |
//---------------------------
//|TInt32 | EDbColLongText8 |
//---------------------------
//
void CIRNmsLogDb::CreateNmsLogTableL()
    {
    IRLOG_DEBUG( "CIRNmsLogDb::CreateNmsLogTableL" );
    //start time of NmsLog
    TDbCol nmsLogid( KID, EDbColInt32 );
    nmsLogid.iAttributes = TDbCol::ENotNull; 
    
    //!this column is used to store preset data
    //!The column stores a potentially large amount of non-Unicode text data.
    TDbCol nmsLogcol( KNmsLogCol, EDbColLongText8 );
    nmsLogcol.iAttributes = TDbCol::ENotNull; 
       
    CDbColSet* nmsLogcolset = CDbColSet::NewLC();
    nmsLogcolset->AddL( nmsLogid );
    nmsLogcolset->AddL( nmsLogcol );

    // Create the NmsLoglog table with two columns
    
    User::LeaveIfError( iNmsLogDb.CreateTable( KNmsLogTable,
         *nmsLogcolset ) );
    
    //Create the KeyIndex for the table
    CreateNmsLogIndexL();
    CleanupStack::PopAndDestroy( nmsLogcolset );
    IRLOG_DEBUG( "CIRNmsLogDb::CreateNmsLogTableL - Exiting." );    
    }

// ---------------------------------------------------------------------------
// Function : CreateNmsLogIndexL
// sets NmsLogid as the primary key
// ---------------------------------------------------------------------------
//
void CIRNmsLogDb::CreateNmsLogIndexL()
    {
    IRLOG_DEBUG( "CIRNmsLogDb::CreateNmsLogIndexL" );
    TDbKeyCol nmsLogid( KID );
    CDbKey* index = CDbKey::NewLC();
    index->AddL( nmsLogid );
    User::LeaveIfError( iNmsLogDb.CreateIndex( KNmsLogIndex, KNmsLogTable,
        *index ) );
    CleanupStack::PopAndDestroy( index );
    IRLOG_DEBUG( "CIRNmsLogDb::CreateNmsLogIndexL - Exiting." );
    }
    
// ---------------------------------------------------------------------------
// CIRNmsLogDb::OpenDbL()
// opening the data base 
// ---------------------------------------------------------------------------
//
 void CIRNmsLogDb::OpenDbL()
    {
    IRLOG_DEBUG( "CIRNmsLogDb::OpenDbL" );
    CloseDb();
    TInt error = KErrNone;
    if ( !BaflUtils::FileExists( iFsSession, iDbFile ) )
        {
        //if file doesn't exist function leaves with error code
        //KErrNotFound
        error = KErrNotFound;
        User::LeaveIfError( error );
        }

    error = iNmsLogDb.Open( iFsSession, iDbFile );    
    if ( error!=KErrNone )
        {
        //if database is failed to open then 
        //function leaves
        IRLOG_ERROR2( "CIRNmsLogDb::OpenDbL - \
        		Opening NmsLog database failed ( %d )", error );
        User::LeaveIfError( error );
        }
    if ( iNmsLogDb.IsDamaged() || !iNmsLogDb.InTransaction() )
        {
        //if data base is damaged then 
        //it tried to recover
        //if recovery is not possible function leaves
        error = iNmsLogDb.Recover();
        if ( KErrNone == error )
            {
            //if recovered data base is compacted
            error = iNmsLogDb.Compact();    
            }
        User::LeaveIfError( error );    
        }
    IRLOG_DEBUG( "CIRNmsLogDb::OpenDbL - Exiting." );
    }
    

// ---------------------------------------------------------------------------
// CIRNmsLogDb::CreateDbL()
// Creates both the dbms files 
// calls CreateFavMasterTableL(),CreateFavUrlTableL
// database filename
// ---------------------------------------------------------------------------
//
 void CIRNmsLogDb::CreateDbL( TFileName& aNmsLog )
    {
    IRLOG_DEBUG( "CIRNmsLogDb::CreateDbL" );
    CloseDb();
    TInt error = iNmsLogDb.Replace( iFsSession, aNmsLog );
    if ( error != KErrNone )
        {
        IRLOG_ERROR2( "CIRNmsLogDb::CreateDbL - \
        		Creating NmsLog database failed ( %d )", error );
        }
    User::LeaveIfError( error );
    CreateNmsLogTableL();
    IRLOG_DEBUG( "CIRNmsLogDb::CreateDbL - Exiting." );
    }
