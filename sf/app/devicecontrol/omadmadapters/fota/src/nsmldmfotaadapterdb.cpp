/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Fota adapter's DB implementation file
*
*/





// INCLUDE FILES
#include <sysutil.h>
#include <bautils.h>

#include "nsmldmfotaadapterdb.h"
#include "nsmlconstants.h"
#include "nsmldebug.h"

#ifndef __WINS__
                                    // This lowers the unnecessary compiler warning (armv5) to remark.
                                    // "Warning: #174-D: expression has no effect..." is caused by 
                                    // DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::NewL()
//  Creates a new instance of CNSmlDmFotaAdapterDb object.
// ---------------------------------------------------------------------------
// 
CNSmlDmFotaAdapterDb* CNSmlDmFotaAdapterDb::NewL()
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::NewL(): begin");
    
    CNSmlDmFotaAdapterDb* self = CNSmlDmFotaAdapterDb::NewLC(); 
    CleanupStack::Pop( self );

    _DBG_FILE("CNSmlDmFotaAdapterDb::NewL(): end");
    
    return self;
    }   

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::NewLC()
//  Creates a new instance of CNSmlDmFotaAdapterDb object. 
//  Pushes and leaves new instance onto CleanupStack.
// ---------------------------------------------------------------------------
// 
CNSmlDmFotaAdapterDb* CNSmlDmFotaAdapterDb::NewLC()
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::NewLC(): begin");
    
    CNSmlDmFotaAdapterDb* self = new( ELeave ) CNSmlDmFotaAdapterDb();
    CleanupStack::PushL( self );
    self->ConstructL();
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::NewLC(): end");
    
    return self;
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::ConstructL()
//  Second phase constructor.
// ---------------------------------------------------------------------------
// 
void CNSmlDmFotaAdapterDb::ConstructL()
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::ConstructL(): begin");
    
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( iRdbSession.Connect() );
    iFsSession.SetSessionToPrivate( KNSmlFotaAdapterDbDrive );

    TParse name;

#ifdef SYMBIAN_SECURE_DBMS
	name.Set( KNSmlFotaAdapterDbName(), NULL, NULL );
#else
    name.Set( KNSmlFotaAdapterDbName(), KNSmlDatabasesNonSecurePath, NULL );
#endif

	TInt err = iDatabase.Open( iRdbSession,
	                           name.FullName(),
	                           KNSmlDBMSSecureMDHostOneID );
    if ( err == KErrNotFound )
        {
        CreateDatabaseL( name.FullName() );
        err = iDatabase.Open( iRdbSession,
                              name.FullName(),
                              KNSmlDBMSSecureMDHostOneID );
        User::LeaveIfError( err );
        }
    else
        {
        
        if ( ( err == KErrEof ) || ( err == KErrCorrupt ) || 
             ( err == KErrArgument ) )
            {
            // something seriously wrong with the db, delete it and try 
            // to create new
            iRdbSession.DeleteDatabase( name.FullName(),
                                        KNSmlDMHostOnePolicyUID );
            CreateDatabaseL( name.FullName() );
            err = iDatabase.Open( iRdbSession,
                                  name.FullName(),
                                  KNSmlDBMSSecureMDHostOneID );
            }
        
        User::LeaveIfError( err );
        }

    User::LeaveIfError( iFwObjectTable.Open( iDatabase,
                                             KNSmlTableFwMgmtObject ) );
    iColSet = iFwObjectTable.ColSetL();
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::ConstructL(): end");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::CNSmlDmFotaAdapterDb()
//  Constructor.
// ---------------------------------------------------------------------------
// 
CNSmlDmFotaAdapterDb::CNSmlDmFotaAdapterDb()
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::CNSmlDmFotaAdapterDb(): begin");
    _DBG_FILE("CNSmlDmFotaAdapterDb::CNSmlDmFotaAdapterDb(): end");
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::~CNSmlDmFotaAdapterDb()
//  Destructor.
// ---------------------------------------------------------------------------
// 
CNSmlDmFotaAdapterDb::~CNSmlDmFotaAdapterDb()
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::~CNSmlDmFotaAdapterDb(): begin");

    iView.Close();
    
    delete iColSet;
    iFwObjectTable.Close();
        
    iDatabase.Close();
    iFsSession.Close();
    iRdbSession.Close();
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::~CNSmlDmFotaAdapterDb(): ends");
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::AddFwObjectL()
//  Inserts new uninitialized row to FwMgmtObject table and returns its id.
// ---------------------------------------------------------------------------
// 
TNSmlDmFwObjectId CNSmlDmFotaAdapterDb::AddFwObjectL()
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::AddFwObjectL(): begin");
    
    // Check OOD before inserting new row into FwMgmtObject table
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, 
                                               KNSmlMaxFwMgmtObjectRowSize ) )
        {
        User::Leave( KErrDiskFull );
        }
    
    iDatabase.Begin();
    
    // Insert new row, and set finalt result to its null value
    iFwObjectTable.InsertL();
    iFwObjectTable.SetColL( iColSet->ColNo( KNSmlFwMgmtObjectResult ), 
                            KNSmlFotaNullResult );
    iFwObjectTable.PutL();
    
    CommitAndCompact();
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::AddFwObjectL(): end");
    
    return iFwObjectTable.ColUint( iColSet->ColNo( KNSmlFwMgmtObjectId ) );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::DeleteFwObjectL()
//  Deletes a row from FwMgmtObject table identified by aId. If such row is 
//  not found, returns KErrNotFound, KErrNone otherwise.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::DeleteFwObjectL( const TNSmlDmFwObjectId aId )
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::DeleteFwObjectL(): begin");
    
    HBufC* sql = HBufC::NewL( KNSmlDeleteFwMgmtObject().Length() + 
                              KNSmlFwMgmtObjectIntegerLength );
    sql->Des().Format( KNSmlDeleteFwMgmtObject, aId );
    
    iDatabase.Begin();
    TInt result = iDatabase.Execute( *sql );
    CommitAndCompact();
    
    delete sql;
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::DeleteFwObjectL(): end");
    
    return ( result == 0 ) ? KErrNotFound : KErrNone;
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::GetEmptyFinalResultsL()
//  Finds all the rows in FwMgmtObject table that have null final result and
//  puts the PkgIds of those rows to aArray.
// ---------------------------------------------------------------------------
// 
void CNSmlDmFotaAdapterDb::GetEmptyFinalResultsL( RNSmlDmFwObjectArray& aArray )
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::GetEmptyFinalResultsL(): begin");
    
    aArray.Reset();
    
    HBufC* sql = HBufC::NewLC( KNSmlGetUnfinishedFwMgmtObjects().Length() + 
                               KNSmlFwMgmtObjectIntLength );
    sql->Des().Format( KNSmlGetUnfinishedFwMgmtObjects, KNSmlFotaNullResult );
    
    PrepareViewL( *sql, iView.EReadOnly );

    CleanupStack::PopAndDestroy( sql );
    
    while ( iView.NextL() )
        {
        // get final result and append it to aArray
        iView.GetL();
        aArray.AppendL( iView.ColUint( 
                        iColSet->ColNo( KNSmlFwMgmtObjectId ) ) );
        }
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::GetEmptyFinalResultsL(): end");
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::GetEmptyFinalResultsL()
//  Finds all the rows in FwMgmtObject table that have null final result, 
//  server id equals to aServerId and management uri is not an empty string.
//  Puts the PkgIds of those rows to aArray.
// ---------------------------------------------------------------------------
// 
void CNSmlDmFotaAdapterDb::GetEmptyFinalResultsL( RNSmlDmFwObjectArray& aArray, 
                                                  const TDesC8& aServerId )
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::GetEmptyFinalResultsL(serverid): begin");
    
    aArray.Reset();
    
    HBufC* serverId = ConvertTo16BitLC( aServerId );
    
    HBufC* sql = HBufC::NewLC( 
                        KNSmlGetUnfinishedFwMgmtObjectByServerId().Length() + 
                        KNSmlFwMgmtObjectIntLength + 
                        serverId->Length() );
    
    sql->Des().Format( KNSmlGetUnfinishedFwMgmtObjectByServerId, 
                       KNSmlFotaNullResult, 
                       serverId );
    
    PrepareViewL( *sql, iView.EReadOnly );

    CleanupStack::PopAndDestroy( sql );
    CleanupStack::PopAndDestroy( serverId );
    
    while ( iView.NextL() )
        {
        // get final result and append it to aArray
        iView.GetL();
        aArray.AppendL( iView.ColUint( 
                        iColSet->ColNo( KNSmlFwMgmtObjectId ) ) );
        }
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::GetEmptyFinalResultsL( serverid ): end");
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::PkgNameL()
//  Returns the PkgName field from FW object identified by aId. If no such 
//  object is found, returns NULL.
// ---------------------------------------------------------------------------
// 
HBufC8* CNSmlDmFotaAdapterDb::PkgNameL( const TNSmlDmFwObjectId aId )
    {
    return StrValueL( KNSmlFwMgmtObjectName, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::PkgVersionL()
//  Returns the PkgVersion field from FW object identified by aId. If no such 
//  object is found, returns NULL.
// ---------------------------------------------------------------------------
// 
HBufC8* CNSmlDmFotaAdapterDb::PkgVersionL( const TNSmlDmFwObjectId aId )
    {
    return StrValueL( KNSmlFwMgmtObjectVersion, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::PkgUrlL()
//  Returns the PkgUrl field from FW object identified by aId. If no such 
//  object is found, returns NULL.
// ---------------------------------------------------------------------------
// 
HBufC8* CNSmlDmFotaAdapterDb::PkgUrlL( const TNSmlDmFwObjectId aId )
    {
    return LongStrValueL( KNSmlFwMgmtObjectUrl, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::MgmtUriL()
//  Returns the MgmtUri field from FW object identified by aId. If no such 
//  object is found, returns NULL.
// ---------------------------------------------------------------------------
// 
HBufC8* CNSmlDmFotaAdapterDb::MgmtUriL( const TNSmlDmFwObjectId aId )
    {
    return StrValueL( KNSmlFwMgmtObjectMgmtUri, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::ProfileIdL()
//  Returns the ProfileId field from FW object identified by aId. If no such 
//  object is found, returns KErrNotFound.
// ---------------------------------------------------------------------------
// 
TSmlProfileId CNSmlDmFotaAdapterDb::ProfileIdL( const TNSmlDmFwObjectId aId )
    {
    return IntValueL( KNSmlFwMgmtObjectProfileId, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::ServerIdL()
//  Returns the ServerId field from FW object identified by aId. If no such 
//  object is found, returns NULL.
// ---------------------------------------------------------------------------
// 
HBufC8* CNSmlDmFotaAdapterDb::ServerIdL( const TNSmlDmFwObjectId aId )
    {
    return StrValueL( KNSmlFwMgmtObjectServerId, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::FinalResultL()
//  Returns the FinalResult field from FW object identified by aId. If no such 
//  object is found, returns KErrNotFound.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::FinalResultL( const TNSmlDmFwObjectId aId )
    {
    return IntValueL( KNSmlFwMgmtObjectResult, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::CorrelatorL()
//  Returns the Correlator field from FW object identified by aId. If no such 
//  object is found, returns NULL.
// ---------------------------------------------------------------------------
// 
HBufC8* CNSmlDmFotaAdapterDb::CorrelatorL( const TNSmlDmFwObjectId aId )
    {
    return StrValueL( KNSmlFwMgmtObjectCorrelator, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetPkgNameL()
//  Sets aName as the value of PkgName in FW object identified by aId. If no 
//  such object is found, returns KErrNotFound. If name is too large for the 
//  database, returns KErrOverflow.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetPkgNameL( const TDesC8& aName, 
                                        const TNSmlDmFwObjectId aId )
    {
    if ( aName.Length() > KFotaMaxPkgNameLength )
        {
        return KErrOverflow;
        }
    
    return SetStrValueL( KNSmlFwMgmtObjectName, aName, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetPkgVersionL()
//  Sets aVersion as the value of PkgVersion in FW object identified by aId.
//  If no such object is found, returns KErrNotFound. If version is too 
//  large for the database, returns KErrOverflow.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetPkgVersionL( const TDesC8& aVersion, 
                                           const TNSmlDmFwObjectId aId )
    {
    if ( aVersion.Length() > KFotaMaxPkgVersionLength )
        {
        return KErrOverflow;
        }
    
    return SetStrValueL( KNSmlFwMgmtObjectVersion, aVersion, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetPkgUrlL()
//  Sets aUrl as the value of PkgURL in FW object identified by aId. If no 
//  such object is found, returns KErrNotFound. If url is too large for the 
//  database, returns KErrOverflow.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetPkgUrlL( const TDesC8& aUrl, 
                                       const TNSmlDmFwObjectId aId )
    {
    if ( aUrl.Length() > KFotaMaxPkgURLLength )
        {
        return KErrOverflow;
        }
    
    return SetLongStrValueL( KNSmlFwMgmtObjectUrl, aUrl, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetMgmtUriL()
//  Sets aUri as the value of MgmtURI in FW object identified by aId. If no 
//  such object is found, returns KErrNotFound. If uri is too large for the 
//  database, returns KErrOverflow.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetMgmtUriL( const TDesC8& aUri, 
                                        const TNSmlDmFwObjectId aId )
    {
    if ( aUri.Length() > KNSmlMaxMgmtUriLength )
        {
        return KErrOverflow;
        }
    
    return SetStrValueL( KNSmlFwMgmtObjectMgmtUri, aUri, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetServerInfoL()
//  Sets aProfileId as the value of ProfileId and aServerId as the value of 
//  ServerId in FW object identified by aId. If no such object is found, 
//  returns KErrNotFound. If server id is too large for the database, 
//  returns KErrOverflow.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetServerInfoL( const TSmlProfileId aProfile, 
                                           const TDesC8& aServerId, 
                                           const TNSmlDmFwObjectId aId )
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::SetServerInfoL(): begin");
    
    if ( aServerId.Length() > KNSmlMaxServerIdLength )
        {
        return KErrOverflow;
        }
    
    HBufC* serverId = ConvertTo16BitLC( aServerId );
    TInt ret = UpdateRowL( aId );
    
    if ( ret == KErrNone )
        {
        // do update
        iView.SetColL( iColSet->ColNo( KNSmlFwMgmtObjectProfileId ), 
                       aProfile );
        iView.SetColL( iColSet->ColNo( KNSmlFwMgmtObjectServerId ), 
                       *serverId );
        iView.PutL();
        }
    
    CommitAndCompact();
    
    CleanupStack::PopAndDestroy( serverId );
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::SetServerInfoL(): end");
    
    return ret;
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetFinalResultL()
//  Sets aResult as the value of FinalResult in FW object identified by aId.
//  If no such object is found, returns KErrNotFound.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetFinalResultL( const TInt aResult, 
                                            const TNSmlDmFwObjectId aId )
    {
    return SetIntValueL( KNSmlFwMgmtObjectResult, aResult, aId );
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetCorrelatorL()
//  Sets aCorrealtor as the value of Correlator in FW object identified by aId.
//  If no such object is found, returns KErrNotFound. If given correlator is 
//  too large for the database, returns KErrOverflow.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetCorrelatorL( const TDesC8& aCorrelator, 
                                           const TNSmlDmFwObjectId aId )
    {
    if ( aCorrelator.Length() > KNSmlMaxCorrelatorLength )
        {
        return KErrOverflow;
        }
    
    return SetStrValueL( KNSmlFwMgmtObjectCorrelator, aCorrelator, aId );
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::CreateDatabaseL()
//  Creates Fota database.
// ---------------------------------------------------------------------------
// 
void CNSmlDmFotaAdapterDb::CreateDatabaseL( const TDesC& aFullName )
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::CreateDatabaseL(): begin");

    // Check OOD before creating new Fota DB
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, 
                                               KNSmlFotaAdapterEmptyDbSize ) )
        {
        User::Leave( KErrDiskFull );
        }

    // create sql query string, 5 = amount of length integers used in Format()
    HBufC* createFwMgmtTable = HBufC::NewLC( 
                                    KNSmlCreateFwMgmtObjectTable().Length() + 
                                    KNSmlFwMgmtObjectIntLength*5 );
    
    createFwMgmtTable->Des().Format( KNSmlCreateFwMgmtObjectTable, 
                                     KFotaMaxPkgNameLength,
                                     KFotaMaxPkgVersionLength,
                                     KNSmlMaxMgmtUriLength,
                                     KNSmlMaxServerIdLength,
                                     KNSmlMaxCorrelatorLength );

    User::LeaveIfError( iDatabase.Create( iRdbSession,
	                                      aFullName,
	                                      KNSmlDBMSSecureMDHostOneID ) );
    iDatabase.Begin();
    iDatabase.Execute( *createFwMgmtTable );

    CommitAndCompact();
    iDatabase.Close();
    
    CleanupStack::PopAndDestroy( createFwMgmtTable );
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::CreateDatabaseL(): end");
    }   


// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetStrValueL()
//  Sets aValue as the value of aColumn in FW object identified by aObject.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetStrValueL( const TDesC& aColumn, 
                                         const TDesC8& aValue,
                                         const TNSmlDmFwObjectId aObject )
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::SetStrValueL(): begin");
    
    HBufC* value = ConvertTo16BitLC( aValue );
    
    DBG_ARGS( _S16("CNSmlDmFotaAdapterDb::SetStrValueL(): ('%S', '%S', %d)"), 
              &aColumn, value, aObject );
    
    TInt ret = UpdateRowL( aObject );
    
    if ( ret == KErrNone )
        {
        // do update
        iView.SetColL( iColSet->ColNo( aColumn ), *value );
        iView.PutL();
        }
    
    CommitAndCompact();
    
    CleanupStack::PopAndDestroy( value );
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::SetStrValueL(): end");
    
    return ret;
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetLongStrValueL()
//  Streams aValue and its length as the value of aColumn in FW object 
//  identified by aObject.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetLongStrValueL( const TDesC& aColumn, 
                                             const TDesC8& aValue, 
                                             const TNSmlDmFwObjectId aObject )
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::SetLongStrValueL(): begin");
    
    HBufC* value = ConvertTo16BitLC( aValue );
    
    DBG_ARGS( _S16("CNSmlDmFotaAdapterDb::SetStrValueL(): ('%S', '%S', %d)"),
              &aColumn, value, aObject );
    
    TInt ret = UpdateRowL( aObject );
    
    if ( ret == KErrNone )
        {
        // do update
        
        RDbColWriteStream wStream;
        wStream.OpenL( iView, iColSet->ColNo( aColumn ) );
        CleanupClosePushL( wStream );
        
        wStream.WriteInt32L( value->Length() );
        wStream.WriteL( *value, value->Length() );
        
        CleanupStack::PopAndDestroy( &wStream );
        
        iView.PutL();
        }
    
    CommitAndCompact();
    
    CleanupStack::PopAndDestroy( value );
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::SetLongStrValueL(): end");
    
    return ret;
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::SetIntValueL()
//  Sets aValue as the value of aColumn in FW object identified by aObject.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::SetIntValueL( const TDesC& aColumn, 
                                         const TInt aValue, 
                                         const TNSmlDmFwObjectId aObject )
    {
    _DBG_FILE("CNSmlDmFotaAdapterDb::SetIntValueL(): begin");
    DBG_ARGS( _S16("CNSmlDmFotaAdapterDb::SetIntValueL(): ('%S', %d, %d)"), 
              &aColumn, aValue, aObject );
    
    TInt ret = UpdateRowL( aObject );
    
    if ( ret == KErrNone )
        {
        // do update
        iView.SetColL( iColSet->ColNo( aColumn ), aValue );
        iView.PutL();
        }
    
    CommitAndCompact();
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::SetIntValueL(): end");
    
    return ret;
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::UpdateRowL()
//  Prepares iView when setting a value to one of the rows in DB table. 
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::UpdateRowL( const TNSmlDmFwObjectId aObject )
    {
    TInt ret = KErrNone;
    
    HBufC* sql = FwMgmtObjectRowSqlLC( aObject );
    PrepareViewL( *sql, iView.EUpdatable );
    iDatabase.Begin();

    CleanupStack::PopAndDestroy( sql );
    
    if ( iView.FirstL() )
        {
        // fw object exists
        // Get current row for accessing and initiate update
        iView.GetL();
        iView.UpdateL();
        }
    else
        {
        // fw object does not exists, cannot update
        ret = KErrNotFound;
        }
    
    return ret;
    }
    
// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::StrValueL()
//  Returns the value in FW object identified by aObject in column aColumn.
// ---------------------------------------------------------------------------
// 
HBufC8* CNSmlDmFotaAdapterDb::StrValueL( const TDesC& aColumn, 
                                         const TNSmlDmFwObjectId aObject )
    {
    DBG_ARGS( _S16("CNSmlDmFotaAdapterDb::StrValueL('%S', %d): begin"), 
              &aColumn, aObject );
    
    HBufC8* value = NULL;
    
    FetchRowL( aObject );
    
    if ( iView.FirstL() )
        {
        // get value
        iView.GetL();
        TPtrC res = iView.ColDes( iColSet->ColNo( aColumn ) );
        
        // convert to 8-bit
        value = HBufC8::NewL( res.Length() );
        value->Des().Copy( res );
        }
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::StrValueL(): end");
    
    return value;
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::LongStrValueL()
//  Returns the value in FW object identified by aObject in column aColumn.
// ---------------------------------------------------------------------------
// 
HBufC8* CNSmlDmFotaAdapterDb::LongStrValueL( const TDesC& aColumn, 
                                             const TNSmlDmFwObjectId aObject )
    {
    DBG_ARGS( _S16("CNSmlDmFotaAdapterDb::LongStrValueL('%S', %d): begin"), 
              &aColumn, aObject );
    
    HBufC8* value = NULL;
    
    FetchRowL( aObject );
    
    if ( iView.FirstL() )
        {
        // get value
        iView.GetL();
        
        RDbColReadStream rStream;
        rStream.OpenL( iView, iColSet->ColNo( aColumn ) );
        CleanupClosePushL( rStream );
        
        TInt length = 0;
        TRAPD( err, length = rStream.ReadInt32L() );
        
        HBufC* buf = HBufC::NewLC( length );
        TPtr bufPtr = buf->Des();
        
        if ( err == KErrNone )
            {
            rStream.ReadL( bufPtr, length );
            }
        
        // convert to 8-bit
        value = HBufC8::NewL( bufPtr.Length() );
        value->Des().Copy( bufPtr );
        
        CleanupStack::PopAndDestroy( buf );
        CleanupStack::PopAndDestroy( &rStream );
        }
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::LongStrValueL(): end");
    
    return value;
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::IntValueL()
//  Returns the value in FW object identified by aObject in column aColumn.
// ---------------------------------------------------------------------------
// 
TInt CNSmlDmFotaAdapterDb::IntValueL( const TDesC& aColumn,
                                      const TNSmlDmFwObjectId aObject )
    {
    DBG_ARGS( _S16("CNSmlDmFotaAdapterDb::IntValueL('%S', %d): begin"), 
              &aColumn, aObject );
    
    TInt value = KErrNotFound;
    
    FetchRowL( aObject );
    
    if ( iView.FirstL() )
        {
        iView.GetL();
        value = iView.ColInt( iColSet->ColNo( aColumn ) );
        }
    
    _DBG_FILE("CNSmlDmFotaAdapterDb::IntValueL(): end");
    
    return value;
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::FetchRowL()
//  Prepares iView when getting a single row from DB.
// ---------------------------------------------------------------------------
// 
void CNSmlDmFotaAdapterDb::FetchRowL( const TNSmlDmFwObjectId aObject )
    {
    HBufC* sql = FwMgmtObjectRowSqlLC( aObject );
    
    PrepareViewL( *sql, iView.EReadOnly );

    CleanupStack::PopAndDestroy( sql );
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::FwMgmtObjectRowSqlLC()
//  Returns a buffer containing formatted SQL statement for getting a FW object 
//  identified by aId.
// ---------------------------------------------------------------------------
// 
HBufC* CNSmlDmFotaAdapterDb::FwMgmtObjectRowSqlLC( TNSmlDmFwObjectId aId ) const
    {
    HBufC* sql = HBufC::NewLC( KNSmlGetFwMgmtObject().Length() + 
                               KNSmlFwMgmtObjectIntegerLength );
    
    TPtr sqlPtr = sql->Des();
    sqlPtr.Format( KNSmlGetFwMgmtObject, aId );
    return sql;
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::PrepareViewL()
//  Closes and prepares the view
// ---------------------------------------------------------------------------
// 
void CNSmlDmFotaAdapterDb::PrepareViewL( const TDesC& aSql, 
                                         RDbRowSet::TAccess aAccess )
    {
    iView.Close();
    User::LeaveIfError( iView.Prepare( iDatabase, TDbQuery(aSql), aAccess ) );
    
    if ( iView.Unevaluated() )
        {
        User::LeaveIfError( iView.EvaluateAll() );
        }
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::CommitAndCompact()
//  Commits update and compacts the database
// ---------------------------------------------------------------------------
// 
void CNSmlDmFotaAdapterDb::CommitAndCompact() 
    {
    iDatabase.Commit();
    iDatabase.Compact();
    }

// ---------------------------------------------------------------------------
//  CNSmlDmFotaAdapterDb::ConvertTo16BitLC()
//  Returns a pointer to a unicode copy of the given 8-bit descriptor.
//  The pointer to the copy is allocated from heap and pushed on the 
//  CleanupStack.
// ---------------------------------------------------------------------------
// 
HBufC* CNSmlDmFotaAdapterDb::ConvertTo16BitLC( const TDesC8& aDes ) const
    {
    HBufC* buf = HBufC::NewLC( aDes.Length() );
    buf->Des().Copy( aDes );
    
    return buf;
    }
