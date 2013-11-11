/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConSBEClient implementation
*
*/


// INCLUDE FILES

#include <pathinfo.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "sconsbeclient.h"
#include "debug.h"

const TInt KDataBufExpandSize( 1024 );
const TInt KMaxObjectSize( 1048576 - 1024 ); // Max chunk size is 1MB, but give some space for ConML wbxml
_LIT( KSConNoDrive, "\x0" );
_LIT( KSConDriveExists, "\x1" );
// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CSConSBEClient::NewL( cosnt TInt aMaxObjectSize )
// Two-phase constructor
// -----------------------------------------------------------------------------
//  
CSConSBEClient* CSConSBEClient::NewL( RFs& aFs )
    {
    TRACE_FUNC;
    CSConSBEClient* self = new (ELeave) CSConSBEClient( aFs );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::CSConSBEClient( const TInt aMaxObjectSize )
// Constructor
// -----------------------------------------------------------------------------
//
CSConSBEClient::CSConSBEClient( RFs& aFs ) : 
        CActive( EPriorityStandard ), iBURModeNormal( ETrue ), iFs( aFs )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::~CSConSBEClient()
// Initializes member data
// -----------------------------------------------------------------------------
//  
CSConSBEClient::~CSConSBEClient()
    {
    TRACE_FUNC_ENTRY;
    Cancel();
    TInt err( KErrNone );
    
    if( !iBURModeNormal && iSBEClient )
        {
        TDriveList driveList;
        
        for( TInt i = 0; i<KMaxDrives; i++ )
            {
            if( i == EDriveC || i == EDriveE )
                {
                driveList.Append( KSConDriveExists );
                }
            else
                {
                driveList.Append( KSConNoDrive );
                }
            }
        
        TRAP( err, iSBEClient->SetBURModeL( driveList, EBURNormal, ENoBackup ) );
        }
    
    if( iSBEClient )
        {
        delete iSBEClient;
        iSBEClient = NULL;
        }
    
    delete iDataBuffer;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::SetBURMode( TRequestStatus& aStatus, CSConTask*& aTask )
// Set backup/restore mode
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::SetBURMode( TRequestStatus& aStatus, CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iCurrentTask = aTask;

    TRequestStatus* status = &iStatus; 
    User::RequestComplete( status, KErrNone ); 
    SetActive();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::ListPublicFiles( TRequestStatus& aStatus, 
//                                  CSConTask*& aTask )
// Lists public files from data owners
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::ListPublicFiles( TRequestStatus& aStatus, 
                                    CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iCurrentTask = aTask;
    
    TRequestStatus* status = &iStatus; 
    User::RequestComplete( status, KErrNone ); 
    SetActive();    
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::ListDataOwners( TRequestStatus& aStatus, CSConTask*& aTask )
// Lists participant data owners
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::ListDataOwners( TRequestStatus& aStatus, CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iCurrentTask = aTask;
    
    TRequestStatus* status = &iStatus; 
    User::RequestComplete( status, KErrNone ); 
    SetActive();
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::GetDataSize( TRequestStatus& aStatus, CSConTask*& aTask )
// Lists data sizes from data owners
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::GetDataSize( TRequestStatus& aStatus, CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iCurrentTask = aTask;
    
    TRequestStatus* status = &iStatus; 
    User::RequestComplete( status, KErrNone ); 
    SetActive();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::RequestData( TRequestStatus& aStatus, CSConTask*& aTask )
// Request a data from a data owner
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::RequestData( TRequestStatus& aStatus, CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iCurrentTask = aTask;
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );          
    SetActive();    
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::GetDataOwnerStatus( TRequestStatus& aStatus, 
//                                      CSConTask*& aTask )
// Lists the status of data owners
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::GetDataOwnerStatus( TRequestStatus& aStatus, 
                                        CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iCurrentTask = aTask;
    
    TRequestStatus* status = &iStatus; 
    User::RequestComplete( status, KErrNone ); 
    SetActive();
        
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::SupplyData( TRequestStatus& aStatus, CSConTask*& aTask )
// Supplies a data to a data owner
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::SupplyData( TRequestStatus& aStatus, CSConTask*& aTask )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iCurrentTask = aTask;
    
    TRequestStatus* status = &iStatus; 
    User::RequestComplete( status, KErrNone ); 
    SetActive();    
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::DoCancel()
// Implementation of CActive::DoCancel()
// -----------------------------------------------------------------------------
//          
void CSConSBEClient::DoCancel()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::RunL()
// Implementation of CActive::RunL()
// -----------------------------------------------------------------------------
//              
void CSConSBEClient::RunL()
    {
    TRACE_FUNC_ENTRY;
    TInt err( KErrNone );
    TInt ret( KErrNone );
    
    //Initialize a connection to the backup server
    if( !iSBEClient )
        {
        iSBEClient = CSBEClient::NewL();
        }   
    
    switch( iCurrentTask->GetServiceId() )
        {
        case ESetBURMode :
            LOGGER_WRITE( "CSConSBEClient::RunL() : ESetBURMode" );
            TRAP( err, ProcessSetBURModeL() );
            LOGGER_WRITE_1( "CSConSBEClient::RunL() : ProcessSetBURModeL() : returned %d", err );
            break;
        case EListPublicFiles :
            LOGGER_WRITE( "CSConSBEClient::RunL() : EListPublicFiles" );
            TRAP( err, ProcessListPublicFilesL() );
            LOGGER_WRITE_1( "CSConSBEClient::RunL() : ProcessListPublicFilesL() : returned %d", err );
            break;
        case EListDataOwners :
            LOGGER_WRITE( "CSConSBEClient::RunL() : EListDataOwners" ); 
            TRAP( err, ProcessListDataOwnersL() );
            LOGGER_WRITE_1( "CSConSBEClient::RunL() : ProcessListDataOwnersL() : returned %d", err );
            break;
        case EGetDataSize :
            LOGGER_WRITE( "CSConSBEClient::RunL() : EGetDataSize" );    
            TRAP( err, ProcessGetDataSizeL() );
            LOGGER_WRITE_1( "CSConSBEClient::RunL() : ProcessGetDataSizeL() : returned %d", err );
            break;
        case ERequestData :
            LOGGER_WRITE( "CSConSBEClient::RunL() : ERequestData" );
            TRAP( err, ret = ProcessRequestDataL() );
            LOGGER_WRITE_1( "CSConSBEClient::RunL() : ProcessRequestDataL() : returned %d", err );
            break;  
        case EGetDataOwnerStatus :
            LOGGER_WRITE( "CSConSBEClient::RunL() : EGetDataOwnerStatus" );
            TRAP( err, ProcessGetDataOwnerStatusL() );
            LOGGER_WRITE_1( "CSConSBEClient::RunL() : ProcessGetDataOwnerStatusL() : returned %d", err );
            break;
        case ESupplyData :
            LOGGER_WRITE( "CSConSBEClient::RunL() : ESupplyData" ); 
            TRAP( err, ret = ProcessSupplyDataL() );
            LOGGER_WRITE_1( "CSConSBEClient::RunL() : SupplyDataL() : returned %d", err );
            break;
        default :
            err = KErrNotSupported;
            LOGGER_WRITE( "CSConSBEClient::RunL() : ERROR! Unknown!" ); 
            break;
        }
    
    if ( err )
        {
        HandleSBEErrorL( err );
        }
    else
        {
        err = ret;
        }
    User::RequestComplete( iCallerStatus, err );
            
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::ProcessSetBURModeL()
// Executes SetBURMode task
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::ProcessSetBURModeL()
    {
    TRACE_FUNC_ENTRY;
    
    iAllSnapshotsSuppliedCalled = EFalse;
    iLastChunk = EFalse;
    
    TDriveList driveList = iCurrentTask->iBURModeParams->iDriveList;
    
    TBURPartType partialType = EBURUnset;
    iRestoreMode = EFalse;
    
    switch( iCurrentTask->iBURModeParams->iPartialType )
        {
        case ESConBurNormal :
            partialType = EBURNormal;
            break;
        case ESConBurBackupFull :
            partialType = EBURBackupFull;
            break;
        case ESConBurBackupPartial :
            partialType = EBURBackupPartial;
            break;
        case ESConBurRestoreFull :
            partialType = EBURRestoreFull;
            iRestoreMode = ETrue;
            break;
        case ESConBurRestorePartial :
            partialType = EBURRestorePartial;
            iRestoreMode = ETrue;
            break;
        default :
            partialType = EBURUnset;
            break;
        }
    
    if ( partialType == EBURBackupFull || partialType == ESConBurBackupPartial )
        {
        // initialize buffer
        if ( !iDataBuffer )
            iDataBuffer = CBufFlat::NewL( KDataBufExpandSize );
        else
            iDataBuffer->Reset();
        iDataBufferSize=0;
        }
    else
        {
        delete iDataBuffer;
        iDataBuffer = 0;
        }
    
    TBackupIncType incType = ENoBackup;
    
    switch( iCurrentTask->iBURModeParams->iIncType )    
        {
        case ESConNoBackup :
            incType = ENoBackup;
            break;
        case ESConBackupBase :
            incType = EBackupBase;
            break;
        case ESConBackupIncrement :
            incType = EBackupIncrement;
            break;
        default :
            incType = ENoBackup;
            break;
        }
    
    iSBEClient->SetBURModeL( driveList, partialType, incType );
    
    if( partialType != EBURNormal )
        {
        iBURModeNormal = EFalse;
        }
    else
        {
        iBURModeNormal = ETrue;
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::ProcessListPublicFilesL()
// Executes ListPublicFiles task
// -----------------------------------------------------------------------------
//
void CSConSBEClient::ProcessListPublicFilesL()
    {
    TRACE_FUNC_ENTRY;
    //Get the secure id
    TSecureId sid = iCurrentTask->iPubFilesParams->iDataOwners[0]->iUid;
    TSecureId packageSid;
    LOGGER_WRITE_1("DO: 0x%08x", sid.iId);
    TPtrC packageName(KNullDesC);
    const TUint KSConUidLength = 8; // 8 digits (hex)
    const TUint KSConUidPrefixLength = 3; // prefix is '#0x'
    
    TInt packageNameLength = iCurrentTask->iPubFilesParams->iDataOwners[0]->iPackageName.Length();
    
    if ( packageNameLength > KSConUidLength )
        {
        // remove uid from package name
        packageName.Set( 
                iCurrentTask->iPubFilesParams->iDataOwners[0]->
                iPackageName.Left( packageNameLength - (KSConUidLength + KSConUidPrefixLength) ) );
        LOGGER_WRITE_1("PackageName: %S", &packageName );
        
        TPtrC sidPtr( iCurrentTask->iPubFilesParams->iDataOwners[0]->
                iPackageName.Right( KSConUidLength ) );
        TLex lex(sidPtr);
        TInt err = lex.Val( packageSid.iId, EHex );
        if ( err )
            {
            LOGGER_WRITE_1("lex.Val error: %d", err);
            packageName.Set( KNullDesC );
            packageSid.iId = 0;
            }
        LOGGER_WRITE_1("packageSid: 0x%08x", packageSid.iId );
        }
    
    //Get the drive list
    TDriveList driveList = 
    iCurrentTask->iPubFilesParams->iDataOwners[0]->iDriveList;
    
    RFileArray files;
    CleanupClosePushL( files );
    //Go through all the drives in the list
    for( TInt i = 0; i < KMaxDrives; i++ )
        {
        if( driveList[i] )
            {
            //Get files from the drive
            TDriveNumber driveNumber = GetDriveNumber( i );
            LOGGER_WRITE_1("driveNumber: %d", driveNumber);
            
            LOGGER_WRITE("List public files using CSBSecureId");
            CSBSecureId* secureId = CSBSecureId::NewL( sid );
            CleanupStack::PushL( secureId );
            
            TRAPD( err, iSBEClient->PublicFileListL( driveNumber, *secureId, files ) );
            
            if( err != KErrNone && err != KErrNotFound )
                {
                LOGGER_WRITE_1("iSBEClient->PublicFileListL leaved: %d", err);
                User::LeaveIfError( err );
                }
            
            CleanupStack::PopAndDestroy( secureId );
            LOGGER_WRITE_1("public files: %d", files.Count());
            AppendFilesToFilelistL( files, iCurrentTask->iPubFilesParams->iFiles) ;
            
            if ( packageName.Length() > 0 )
                {
                LOGGER_WRITE("Append files registered to packages. Using CSBPackageId");
                
                CSBPackageId* packageId = CSBPackageId::NewL(
                        iCurrentTask->iPubFilesParams->iDataOwners[0]->iUid,
                        packageSid,
                        packageName );
                CleanupStack::PushL( packageId );
                
                TRAPD( err, iSBEClient->PublicFileListL( driveNumber, *packageId, files ) );
                if( err != KErrNone && err != KErrNotFound )
                    {
                    LOGGER_WRITE_1("iSBEClient->PublicFileListL leaved: %d", err);
                    User::LeaveIfError( err );
                    }
                CleanupStack::PopAndDestroy( packageId );
                
                LOGGER_WRITE_1("public files: %d", files.Count());
                AppendFilesToFilelistL( files, iCurrentTask->iPubFilesParams->iFiles) ;
                }
            
            files.Reset();
            }
        }
    CleanupStack::PopAndDestroy( &files );
    TRACE_FUNC_EXIT;
    }

    
// -----------------------------------------------------------------------------
// CSConSBEClient::ProcessListDataOwnersL()
// Executes ListDataOwners task
// -----------------------------------------------------------------------------
//
void CSConSBEClient::ProcessListDataOwnersL()
    {
    TRACE_FUNC_ENTRY;
    TBool includeToList( ETrue );
    RPointerArray<CDataOwnerInfo> dataOwners;
    RArray<TUid> packageArray;
    CleanupResetAndDestroyPushL( dataOwners );
    CleanupClosePushL( packageArray );
    
    iSBEClient->ListOfDataOwnersL( dataOwners );
    LOGGER_WRITE_1( "dataOwners count: %d", dataOwners.Count() );
    
    
    for( TInt i = 0; i < dataOwners.Count(); i++ )
        {
        LOGGER_WRITE_2("handling do: %d, count: %d", i, dataOwners.Count());
        CSConDataOwner* dataOwner = new (ELeave) CSConDataOwner();
        CleanupStack::PushL( dataOwner );
        // default values
        TSecureId sid( 0 );
        includeToList = ETrue;
        
        TCommonBURSettings commonOptions = dataOwners[i]->CommonSettings();
        TPassiveBURSettings passiveOptions = dataOwners[i]->PassiveSettings();
        TActiveBURSettings activeOptions = dataOwners[i]->ActiveSettings();
        
        //active, passive or hybrid? (common)
        if( commonOptions & EActiveBUR && commonOptions & EPassiveBUR )
            {
            dataOwner->iType = EHybridDataOwner;
            }       
        else if( commonOptions & EActiveBUR )
            {
            dataOwner->iType = EActiveDataOwner;
            }
        else if( commonOptions & EPassiveBUR )
            {
            dataOwner->iType = EPassiveDataOwner;
            }
                
        //requires reboot? (common)
        if( commonOptions & ERequiresReboot )
            {
            dataOwner->iReqReboot = ETrue;
            }
        else
            {
            dataOwner->iReqReboot = EFalse;
            }
            
        //has system files? (common)
        if( commonOptions & EHasSystemFiles )
            {
            dataOwner->iHasFiles = ESystemFiles;
            }
            
        //supports selective? (common)
        if( commonOptions & ESupportsSelective )
            {
            dataOwner->iSupportsSel = ETrue;
            }
        else
            {
            dataOwner->iSupportsSel = EFalse;
            }
            
        //has public files? (passive)
        if( passiveOptions & EHasPublicFiles )
            {
            if( dataOwner->iHasFiles == ESystemFiles )
                {
                dataOwner->iHasFiles = EPublicSystemFiles;
                }
            else
                {
                dataOwner->iHasFiles = EPublicFiles;    
                }
            }
            
        //supports incremental? (active and passive)
        if( activeOptions & EActiveSupportsInc || 
                passiveOptions & EPassiveSupportsInc  )
            {
            dataOwner->iSupportsInc = ETrue;
            }
        else
            {
            dataOwner->iSupportsInc = EFalse;
            }
            
        //delay to prepare data? (active)
        if( activeOptions & EDelayToPrepareData )
            {
            dataOwner->iDelayToPrep = ETrue;
            }
        else
            {
            dataOwner->iDelayToPrep = EFalse;
            }
            
        //fill drivelist
        dataOwner->iDriveList = dataOwners[i]->DriveList();
        //filter the drive list
        FilterDriveList( dataOwner->iDriveList );
                        
        CSBGenericDataType* genericData = &( dataOwners[i]->Identifier() );
        TSBDerivedType derived = genericData->DerivedTypeL();
        
        if( derived == ESIDDerivedType )
            {
            //Uif of the data owner
            CSBSecureId* secureId = CSBSecureId::NewL( genericData );
            CleanupStack::PushL( secureId );
            sid = secureId->SecureIdL();
        
            dataOwner->iUid.iUid = (TInt32)sid.iId;
            LOGGER_WRITE_1("ESIDDerivedType, sid: 0x%08x", dataOwner->iUid.iUid );
        
            CleanupStack::PopAndDestroy( secureId );
            }
        else if( derived == EPackageDerivedType )
            {
            //Package name and uid of the data owner
            CSBPackageId* packageId = CSBPackageId::NewL( genericData );
            CleanupStack::PushL( packageId );
        
            dataOwner->iPackageName = packageId->PackageNameL();
            dataOwner->iUid.iUid = packageId->PackageIdL().iUid;
            LOGGER_WRITE_1("EPackageDerivedType, uid: 0x%08x", dataOwner->iUid.iUid );
            sid = packageId->SecureIdL();
            LOGGER_WRITE_1("sid: 0x%08x", sid.iId );
            
            // owerload the package name, also add sid information.
            _LIT(KSidFormat, "#0x%08x");
            const TInt KSidFormatLength = 11;
            TBuf<KSidFormatLength> sidFormat;
            sidFormat.Format( KSidFormat, sid.iId );
            if ( dataOwner->iPackageName.Length() + sidFormat.Length()
                <= dataOwner->iPackageName.MaxLength() )
                {
                dataOwner->iPackageName.Append( sidFormat );
                }
            else
                {
                LOGGER_WRITE("WARNING: Package name too long, sid not included.");
                }
            
            LOGGER_WRITE_1("Package name: %S", &dataOwner->iPackageName );
            if( sid )
                {
                //Find if the package is already included to dataowner list
                if( packageArray.Find( packageId->PackageIdL(), 
                                        CSConSBEClient::Match ) != KErrNotFound )
                    {
                    LOGGER_WRITE("dataowner was already included to list");
                    //Don't include dataowner to list again
                    includeToList = EFalse;
                    }
                else
                    {
                    //Store package's id for filtering
                    LOGGER_WRITE_1( "Storing package id for filtering, uid: 0x%08x", dataOwner->iUid.iUid );
                    packageArray.Append( dataOwner->iUid ); 
                    }
                }
            
            CleanupStack::PopAndDestroy( packageId );
            }
        else if( derived == EJavaDerivedType )
            {
            //Hash
            CSBJavaId* javaId = CSBJavaId::NewL( genericData );
            CleanupStack::PushL( javaId );
            
            //Initialize with hash data length and copy hash
            dataOwner->iJavaHash = HBufC::NewL( javaId->SuiteHashL().Length() );
            dataOwner->iJavaHash->Des().Copy( javaId->SuiteHashL() );
            
            //For ConML: set as passive package
            dataOwner->iType = EPassiveDataOwner;
            dataOwner->iPackageName.Copy( javaId->SuiteNameL() );
            
#ifdef _DEBUG
            LOGGER_WRITE("EJavaDerivedType" );
            LOGGER_WRITE_1("Package name: %S", &dataOwner->iPackageName );
            TPtrC hash( dataOwner->iJavaHash->Des() );
            LOGGER_WRITE_1("JavaHash: %S", &hash );
#endif
            
            CleanupStack::PopAndDestroy( javaId );
            }
        else
            {
            LOGGER_WRITE("Unknown type, ignored from list" );
            //Unknown type => ignore from list
            includeToList = EFalse;
            }
        
        if( includeToList ) 
            {
            LOGGER_WRITE_1( "Appending to list, uid: 0x%08x", dataOwner->iUid.iUid );
            iCurrentTask->iListDataOwnersParams->iDataOwners.AppendL( dataOwner );
            CleanupStack::Pop( dataOwner );
            }
        
        if( sid )
            {
            LOGGER_WRITE_1( "Appending package sid to list, sid: 0x%08x", sid.iId );
            CSConDataOwner* packageDataOwner = dataOwner->CopyL();
            CleanupStack::PushL( packageDataOwner );
            //Clear package name
            packageDataOwner->iPackageName = KNullDesC();
            //Add sid
            packageDataOwner->iUid.iUid = sid.iId;
            iCurrentTask->iListDataOwnersParams->iDataOwners.AppendL( packageDataOwner );
            CleanupStack::Pop( packageDataOwner );
            }
        
        if( !includeToList )
            {
            //Not included to list => delete memory allocation
            CleanupStack::PopAndDestroy( dataOwner );
            }
        }
        
    packageArray.Reset();
    CleanupStack::PopAndDestroy( &packageArray );
    CleanupStack::PopAndDestroy( &dataOwners );
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::ProcessGetDataSizeL()
// Executes GetDataSize task
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::ProcessGetDataSizeL()
    {
    TRACE_FUNC_ENTRY;
    
    if ( !iAllSnapshotsSuppliedCalled )
        {
        TInt err(KErrNone);
        // we have to call AllSnapshotsSupplied() to inform active
        // data owners to start prepare their base data.
        TRAP( err, iSBEClient->AllSnapshotsSuppliedL() );
        LOGGER_WRITE_1( "AllSnapshotsSuppliedL() leaved: %d", err );
        iAllSnapshotsSuppliedCalled = ETrue;
        }
    
    
    TBool packageData( EFalse );
    
    //Calculate data size for every data owner received from the task
    for( TInt i = 0; i < iCurrentTask->iGetDataSizeParams->iDataOwners.Count();
        i++ )
        {
        packageData = EFalse;
        CSConDataOwner* dataOwner = 
            iCurrentTask->iGetDataSizeParams->iDataOwners[i];
            
        TTransferDataType tdt( ERegistrationData );
        TPackageDataType pdt( ESystemData );
        
        LOGGER_WRITE_2( "ProcessGetDataSizeL DO %d of %d", i, iCurrentTask->iGetDataSizeParams->iDataOwners.Count() );
        LOGGER_WRITE_1( "ProcessGetDataSizeL SID 0x%08x", dataOwner->iUid );
        switch( dataOwner->iTransDataType )
            {
            case ESConRegistrationData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : ERegistrationData" );
                tdt = ERegistrationData;
                break;
            case ESConPassiveSnapshotData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : EPassiveSnapshotData" );
                tdt = EPassiveSnapshotData;
                break;
            case ESConPassiveBaseData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : EPassiveBaseData" );
                tdt = EPassiveBaseData;
                break;
            case ESConPassiveIncrementalData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : EPassiveIncrementalData" );
                tdt = EPassiveIncrementalData;
                break;
            case ESConActiveSnapshotData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : EActiveSnapshotData" );
                tdt = EActiveSnapshotData;
                break;
            case ESConActiveBaseData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : EActiveBaseData" );
                tdt = EActiveBaseData;
                break;
            case ESConActiveIncrementalData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : EActiveIncrementalData" );
                tdt = EActiveIncrementalData;
                break;
            case ESConSystemData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : ESystemData" );
                pdt = ESystemData;
                packageData = ETrue;
                break;
            case ESConSystemSnapshotData :
                LOGGER_WRITE( "CSConSBEClient::ProcessGetDataSize() : ESystemSnapshotData" );
                pdt = ESystemSnapshotData;
                packageData = ETrue;
                break;
            default :
                break;
            }
        
        if( packageData && !dataOwner->iJavaHash )
            {
            dataOwner->iSize = PackageDataSizeL( dataOwner->iUid, dataOwner->iDriveList, pdt );
            }
        else if( !dataOwner->iJavaHash )
            {
            dataOwner->iSize = SidDataSizeL( dataOwner->iUid, dataOwner->iDriveList, tdt );
            }
        else
            {
            dataOwner->iSize = JavaDataSizeL( dataOwner->iJavaHash->Des(),
                    dataOwner->iDriveList );
            }
        }
    
    TRACE_FUNC_EXIT;
    }


// -----------------------------------------------------------------------------
// CSConSBEClient::ProcessRequestDataL()
// Executes RequestData task
// -----------------------------------------------------------------------------
//      
TInt CSConSBEClient::ProcessRequestDataL()
    {
    TRACE_FUNC_ENTRY;
    TInt ret( KErrNone );
        
    TBool packageData = EFalse;
    TTransferDataType transferDataType( ERegistrationData );
    TPackageDataType packageDataType( ESystemSnapshotData );    
    LOGGER_WRITE_1("CSConSBEClient::ProcessRequestDataL() iTransDataType: %d",
        (TInt)iCurrentTask->iRequestDataParams->iDataOwner->iTransDataType);
    
    switch( iCurrentTask->iRequestDataParams->iDataOwner->iTransDataType )
        {
        case ESConRegistrationData :
            transferDataType = ERegistrationData;
            break;
        case ESConPassiveSnapshotData :
            transferDataType = EPassiveSnapshotData;
            break;
        case ESConPassiveBaseData :
            transferDataType = EPassiveBaseData;
            break;
        case ESConPassiveIncrementalData :
            transferDataType = EPassiveIncrementalData;
            break;
        case ESConActiveSnapshotData :
            transferDataType = EActiveSnapshotData;
            break;
        case ESConActiveBaseData :
            transferDataType = EActiveBaseData;
            break;
        case ESConActiveIncrementalData :
            transferDataType = EActiveIncrementalData;
            break;
        case ESConSystemData :
            packageDataType = ESystemData;
            packageData = ETrue;
            break;
        case ESConSystemSnapshotData:
            packageDataType = ESystemSnapshotData;
            packageData = ETrue;
            break;
        default :
            break;
        }
        
    
    //Get the drive number  
    TDriveNumber driveNumber = EDriveC;
    TDriveList driveList = iCurrentTask->iRequestDataParams->
        iDataOwner->iDriveList;
    
    for( TInt i = 0; i < KMaxDrives; i++ )
        {
        if( driveList[i] )
            {
            driveNumber = GetDriveNumber( i );
            break;
            }
        }
        
    //Get UID, SID or Java hash
    TUid uid;
    TSecureId sid;
            
    uid.iUid = 0;
    sid.iId = 0;
    
    if( packageData && !iCurrentTask->iRequestDataParams->iDataOwner->iJavaHash )
        {
        uid = iCurrentTask->iRequestDataParams->iDataOwner->iUid;
        LOGGER_WRITE_1("CSConSBEClient::ProcessRequestDataL() uid: 0x%08x", uid.iUid);
        }
    else if( !iCurrentTask->iRequestDataParams->iDataOwner->iJavaHash )
        {
        sid = iCurrentTask->iRequestDataParams->iDataOwner->iUid;
        LOGGER_WRITE_1("CSConSBEClient::ProcessRequestDataL() sid: 0x%08x", sid.iId);
        }
    

    LOGGER_WRITE_1( "CSConSBEClient::ProcessRequestDataL() : \
    Begin - Package data: %d", packageData );

    LOGGER_WRITE_1("iLastChunk: %d", (TInt) iLastChunk );
    LOGGER_WRITE_1("iDataBufferSize: %d", iDataBufferSize );
        
    // if was't last chunk and there are free space left on our packet
    if( !iLastChunk && iDataBufferSize < KMaxObjectSize ) 
        {
        //No data left, request more from the server
        if( packageData && !iCurrentTask->iRequestDataParams->iDataOwner->iJavaHash )
            {
            CSBPackageTransferType* ptt = CSBPackageTransferType::NewL(
                    uid, driveNumber, packageDataType );
            CleanupStack::PushL( ptt );
            
            RequestDataL( *ptt );
            
            CleanupStack::PopAndDestroy( ptt );
            }
        else if( !iCurrentTask->iRequestDataParams->iDataOwner->iJavaHash )
            {
            CSBSIDTransferType* stt = CSBSIDTransferType::NewL(
                    sid, driveNumber, transferDataType );
            CleanupStack::PushL( stt );
            
            RequestDataL( *stt );
            
            CleanupStack::PopAndDestroy( stt );
            }
        else
            {
            TPtr javaHashPtr = iCurrentTask->iRequestDataParams->iDataOwner->iJavaHash->Des();
            CSBJavaTransferType* jtt( NULL );
            //When ESystemData is requested, request EJavaMIDlet
            if( packageDataType == ESystemData )
                {
                jtt = CSBJavaTransferType::NewL( javaHashPtr, driveNumber, EJavaMIDlet );
                }
            //When EPassiveBaseData is requested, request EJavaMIDletData
            else if( transferDataType == EPassiveBaseData )
                {
                jtt = CSBJavaTransferType::NewL( javaHashPtr, driveNumber, EJavaMIDletData );
                }
                        
            if( packageDataType == ESystemData || transferDataType == EPassiveBaseData )
                {
                CleanupStack::PushL( jtt );
                
                RequestDataL( *jtt );
                
                CleanupStack::PopAndDestroy( jtt );   
                }
            else
                {
                //No data
                iDataBuffer->Reset();
                iDataBufferSize = 0;
                iLastChunk = ETrue;
                }
            }
        }
    
    LOGGER_WRITE_1("readed iLastChunk: %d", (TInt) iLastChunk );
    LOGGER_WRITE_1("readed iDataBufferSize: %d", iDataBufferSize );
        
    
    
    if( iCurrentTask->iRequestDataParams->iBackupData )
        {
        delete iCurrentTask->iRequestDataParams->iBackupData;
        iCurrentTask->iRequestDataParams->iBackupData = NULL;
        }
    
    TInt dataToRead = KMaxObjectSize;
    if ( dataToRead > iDataBufferSize )
        dataToRead = iDataBufferSize;
    
    //Initialize the task data buffer
    iCurrentTask->iRequestDataParams->iBackupData = HBufC8::NewL( dataToRead );
    //Get descriptor task's buffer
    TPtr8 backupDataPtr = iCurrentTask->iRequestDataParams->iBackupData->Des();
    
    
    iDataBuffer->Read(0, backupDataPtr, dataToRead );
    iDataBuffer->Delete(0, dataToRead);
    iDataBufferSize -= dataToRead;
    
    if ( !iLastChunk || iDataBufferSize>0 )
        {
        LOGGER_WRITE( "CSConSBEClient::ProcessRequestDataL() : There are more data available" );
        iCurrentTask->iRequestDataParams->iMoreData = ETrue;
        //Another task is needed to transfer the data to the client
        ret = KErrCompletion;
        }
    else
        {
        LOGGER_WRITE( "CSConSBEClient::ProcessRequestDataL() : All data readed" );
        iDataBuffer->Reset();
        iDataBufferSize = 0;
        // task will be completed, initialize iLastChunk value for next operation
        iLastChunk = EFalse;
        }
    
        
    LOGGER_WRITE_1( "CSConSBEClient::ProcessRequestDataL() :  returned %d", ret );
    return ret;
    }

void CSConSBEClient::RequestDataL( CSBGenericTransferType& aGenericTransferType )
    {
    TRACE_FUNC_ENTRY;
    if ( !iDataBuffer )
        User::Leave( KErrArgument );
    
    do
        {
        LOGGER_WRITE( "iSBEClient->RequestDataL() : start" );
        TRequestStatus status;
        iSBEClient->RequestDataL( aGenericTransferType, status );
        User::WaitForRequest( status );
        LOGGER_WRITE_1( "iSBEClient->RequestDataL() : status.Int() %d", status.Int() );
        User::LeaveIfError( status.Int() );
                    
        //Get the data and store the handle
        CSBGenericTransferType* gtt = NULL;
        const TPtrC8& dataPtr = iSBEClient->TransferDataInfoL( gtt, iLastChunk );
        LOGGER_WRITE_1("data size: %d", dataPtr.Length());
        delete gtt;
        iDataBuffer->ExpandL( iDataBufferSize, dataPtr.Length() );
        iDataBuffer->Write(iDataBufferSize, dataPtr);
        iDataBufferSize += dataPtr.Length();
        LOGGER_WRITE_1("total buffer size: %d", iDataBufferSize);
        }
    // Continue if there are more data, and our packet is not full
    while ( !iLastChunk && iDataBufferSize < KMaxObjectSize );
    
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::ProcessGetDataOwnerStatusL()
// Executes GetDataOwnerStatus task
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::ProcessGetDataOwnerStatusL()
    {
    TRACE_FUNC_ENTRY;
    
    //If restore-mode, call AllSystemFilesRestored()
    if( iRestoreMode )
        {
        LOGGER_WRITE( "CSConSBEClient::ProcessGetDataOwnerStatusL() : iSBEClient->AllSystemFilesRestored() start" );
        iSBEClient->AllSystemFilesRestored();
        LOGGER_WRITE( "CSConSBEClient::ProcessGetDataOwnerStatusL() : iSBEClient->AllSystemFilesRestored() end" );
        }
    
    RSIDStatusArray sidStatus;
    CleanupClosePushL(sidStatus);
        
    for( TInt i = 0; i < iCurrentTask->iGetDataOwnerParams->iDataOwners.Count();
     i++ )
        {
        CSConDataOwner* dataOwner = 
            iCurrentTask->iGetDataOwnerParams->iDataOwners[i];
        TSecureId sid( dataOwner->iUid );
        TDataOwnerStatus dataOwnerStatus = EUnset;
        TInt dataOwnerError = 0;
        
        TDataOwnerAndStatus dataOwnerAndStatus( sid, dataOwnerStatus, 
            dataOwnerError );
                
        sidStatus.Append( dataOwnerAndStatus );
        }
    
    iSBEClient->SIDStatusL( sidStatus );
    
    for( TInt j = 0; j < sidStatus.Count(); j++ )
        {
        switch( sidStatus[j].iStatus )
            {
            case EUnset :
                iCurrentTask->iGetDataOwnerParams->iDataOwners[j]->
                iDataOwnStatus = ESConUnset;
                break;
            case EDataOwnerNotFound :
                iCurrentTask->iGetDataOwnerParams->iDataOwners[j]->
                iDataOwnStatus = ESConNotFound;
                break;
            case EDataOwnerReady :
                iCurrentTask->iGetDataOwnerParams->iDataOwners[j]->
                iDataOwnStatus = ESConReady;
                break;
            case EDataOwnerNotReady :
                iCurrentTask->iGetDataOwnerParams->iDataOwners[j]->
                iDataOwnStatus = ESConNotReady;
                break;
            case EDataOwnerFailed :
                iCurrentTask->iGetDataOwnerParams->iDataOwners[j]->
                iDataOwnStatus = ESConFailed;
                break;
            case EDataOwnerNotConnected :
                iCurrentTask->iGetDataOwnerParams->iDataOwners[j]->
                iDataOwnStatus = ESConNotConnected;
                break;
            case EDataOwnerReadyNoImpl :
                iCurrentTask->iGetDataOwnerParams->iDataOwners[j]->
                iDataOwnStatus = ESConNotImplemented;
                break;
            default :
                break;
            }
        }
    sidStatus.Reset();  
    CleanupStack::PopAndDestroy( &sidStatus );

    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::ProcessSupplyDataL()
// Executes SupplyData task
// -----------------------------------------------------------------------------
//  
TInt CSConSBEClient::ProcessSupplyDataL()
    {
    TRACE_FUNC_ENTRY;
    TBool packageData = EFalse;
    TBool lastChunk = ETrue;
    TTransferDataType transferDataType( ERegistrationData );
    TPackageDataType packageDataType( ESystemSnapshotData );    
    LOGGER_WRITE_1("CSConSBEClient::ProcessSupplyDataL() iTransDataType: %d",
        (TInt)iCurrentTask->iSupplyDataParams->iDataOwner->iTransDataType);
    switch( iCurrentTask->iSupplyDataParams->iDataOwner->iTransDataType )
        {
        case ESConRegistrationData :
            transferDataType = ERegistrationData;
            break;
        case ESConPassiveSnapshotData :
            transferDataType = EPassiveSnapshotData;
            break;
        case ESConPassiveBaseData :
            transferDataType = EPassiveBaseData;
            break;
        case ESConPassiveIncrementalData :
            transferDataType = EPassiveIncrementalData;
            break;
        case ESConActiveSnapshotData :
            transferDataType = EActiveSnapshotData;
            break;
        case ESConActiveBaseData :
            transferDataType = EActiveBaseData;
            break;
        case ESConActiveIncrementalData :
            transferDataType = EActiveIncrementalData;
            break;
        case ESConSystemData :
            packageDataType = ESystemData;
            packageData = ETrue;
            break;
        case ESConSystemSnapshotData:
            packageDataType = ESystemSnapshotData;
            packageData = ETrue;
            break;
        default :
            break;
        }
        
    //Get the drive number  
    TDriveNumber driveNumber = EDriveC;
    TDriveList driveList = iCurrentTask->iSupplyDataParams->iDataOwner->
        iDriveList;
    
    for( TInt i = 0; i < KMaxDrives; i++ )
        {
        if( driveList[i] )
            {
            driveNumber = GetDriveNumber( i );
            break;
            }
        }
        
    //Get UID or SID
    TUid uid;
    TSecureId sid;  
    
    uid.iUid = 0;
    sid.iId = 0;
    
    if( packageData && !iCurrentTask->iSupplyDataParams->iDataOwner->iJavaHash )
        {
        uid = iCurrentTask->iSupplyDataParams->iDataOwner->iUid;
        LOGGER_WRITE_1( "CSConSBEClient::ProcessSupplyDataL() uid: 0x%08x", uid.iUid );
        }
    else if( !iCurrentTask->iSupplyDataParams->iDataOwner->iJavaHash )
        {
        sid = iCurrentTask->iSupplyDataParams->iDataOwner->iUid;
        LOGGER_WRITE_1( "CSConSBEClient::ProcessSupplyDataL() sid: 0x%08x", sid.iId );
        }
    
    
    if( packageData && !iCurrentTask->iSupplyDataParams->iDataOwner->iJavaHash )
        {
        CSBPackageTransferType* ptt = CSBPackageTransferType::NewL( uid, driveNumber, 
            packageDataType );
        CleanupStack::PushL( ptt );
        
        //Write restore data to chunk
        LOGGER_WRITE( "iSBEClient->TransferDataAddressL().Copy() : start" );
        LOGGER_WRITE_1( "CSConSBEClient::ProcessSupplyDataL() :\
         Received data length: %d", iCurrentTask->iSupplyDataParams->iRestoreData->Length() );
        iSBEClient->TransferDataAddressL().Copy( iCurrentTask->
        iSupplyDataParams->iRestoreData->Des() );
        LOGGER_WRITE( "iSBEClient->TransferDataAddressL().Copy()" );
        
        delete iCurrentTask->iSupplyDataParams->iRestoreData;
        iCurrentTask->iSupplyDataParams->iRestoreData = NULL;
        
        //Supply data
        lastChunk = !( iCurrentTask->iSupplyDataParams->iMoreData );
        LOGGER_WRITE( "iSBEClient->SupplyDataL( ptt, lastChunk ) : start" );
        TRequestStatus status;
        iSBEClient->SupplyDataL( *ptt, lastChunk, status );
        User::WaitForRequest( status );
        LOGGER_WRITE( "iSBEClient->SupplyDataL( ptt, lastChunk ) : stop" );
        
        User::LeaveIfError( status.Int() );
        
        CleanupStack::PopAndDestroy( ptt );
        }
    else if( !iCurrentTask->iSupplyDataParams->iDataOwner->iJavaHash )
        {
        CSBSIDTransferType* stt = CSBSIDTransferType::NewL( sid, driveNumber, transferDataType );
        CleanupStack::PushL( stt );
        
        //Write restore data to chunk
        LOGGER_WRITE( "iSBEClient->TransferDataAddressL().Copy() : start" );
        LOGGER_WRITE_1( "CSConSBEClient::ProcessSupplyDataL() : Received data length: %d",
         iCurrentTask->iSupplyDataParams->iRestoreData->Length() );
        iSBEClient->TransferDataAddressL().Copy( iCurrentTask->
        iSupplyDataParams->iRestoreData->Des() );
        LOGGER_WRITE( "iSBEClient->TransferDataAddressL().Copy()" );
        
        delete iCurrentTask->iSupplyDataParams->iRestoreData;
        iCurrentTask->iSupplyDataParams->iRestoreData = NULL;
        
        //Supply data
        lastChunk = !( iCurrentTask->iSupplyDataParams->iMoreData );
        LOGGER_WRITE( "iSBEClient->SupplyDataL( stt, lastChunk ) : start" );
        TRequestStatus status;
        iSBEClient->SupplyDataL( *stt, lastChunk, status );
        User::WaitForRequest( status );
        LOGGER_WRITE( "iSBEClient->SupplyDataL( stt, lastChunk ) : stop" );
        
        User::LeaveIfError( status.Int() );
        
        CleanupStack::PopAndDestroy( stt );
        }
    else
        {
        TPtr javaHashPtr = iCurrentTask->iSupplyDataParams->iDataOwner->iJavaHash->Des();
        CSBJavaTransferType* jtt( NULL );
        if( packageDataType == ESystemData )
            {
            LOGGER_WRITE( "iSBEClient->SupplyDataL java ESystemData" );
            jtt = CSBJavaTransferType::NewL( javaHashPtr, driveNumber, EJavaMIDlet );
            }
        else if( transferDataType == EPassiveBaseData )
            {
            LOGGER_WRITE( "iSBEClient->SupplyDataL java EPassiveBaseData" );
            jtt = CSBJavaTransferType::NewL( javaHashPtr, driveNumber, EJavaMIDletData );
            }
                
        if( packageDataType == ESystemData || transferDataType == EPassiveBaseData )
            {
            CleanupStack::PushL( jtt );
            //Write restore data to chunk
            LOGGER_WRITE( "iSBEClient->TransferDataAddressL().Copy() : start" );
            LOGGER_WRITE_1( "CSConSBEClient::ProcessSupplyDataL() : Received data length: %d",
             iCurrentTask->iSupplyDataParams->iRestoreData->Length() );
            iSBEClient->TransferDataAddressL().Copy( iCurrentTask->
            iSupplyDataParams->iRestoreData->Des() );
            LOGGER_WRITE( "iSBEClient->TransferDataAddressL().Copy()" );
            
            delete iCurrentTask->iSupplyDataParams->iRestoreData;
            iCurrentTask->iSupplyDataParams->iRestoreData = NULL;
            
            //Supply data
            lastChunk = !( iCurrentTask->iSupplyDataParams->iMoreData );
            LOGGER_WRITE( "iSBEClient->SupplyDataL( jtt, lastChunk ) : start" );
            TRequestStatus status;
            iSBEClient->SupplyDataL( *jtt, lastChunk, status );
            User::WaitForRequest( status );
            LOGGER_WRITE( "iSBEClient->SupplyDataL( jtt, lastChunk ) : stop" );
            
            User::LeaveIfError( status.Int() );
            
            CleanupStack::PopAndDestroy( jtt );
            }
        }
    
    TInt ret( KErrNone );
        
    if( !lastChunk )
        {
        ret = KErrCompletion;
        }
    
    LOGGER_WRITE_1( "CSConSBEClient::ProcessSupplyDataL() : returned %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::GetDriveNumber( TInt aDrive ) const
// Maps TInt drive number to TDriveNumber
// -----------------------------------------------------------------------------
//  
TDriveNumber CSConSBEClient::GetDriveNumber( TInt aDrive ) const
    {
    TDriveNumber driveNumber;
    switch( aDrive )
        {
        case 0 :
            driveNumber = EDriveA;
            break;
        case 1 :
            driveNumber = EDriveB;
            break;
        case 2 : 
            driveNumber = EDriveC;
            break;
        case 3 :
            driveNumber = EDriveD;
            break;
        case 4 : 
            driveNumber = EDriveE;
            break;
        case 5 :
            driveNumber = EDriveF;
            break;
        case 6 :
            driveNumber = EDriveG;
            break;
        case 7 :
            driveNumber = EDriveH;
            break;
        case 8 :
            driveNumber = EDriveI;
            break;
        case 9 :
            driveNumber = EDriveJ;
            break;
        case 10 :
            driveNumber = EDriveK;
            break;
        case 11 :
            driveNumber = EDriveL;
            break;
        case 12 :
            driveNumber = EDriveM;
            break;
        case 13 :
            driveNumber = EDriveN;
            break;
        case 14 :
            driveNumber = EDriveO;
            break;
        case 15 :
            driveNumber = EDriveP;
            break;
        case 16 :
            driveNumber = EDriveQ;
            break;
        case 17 :
            driveNumber = EDriveR;
            break;
        case 18 :
            driveNumber = EDriveS;
            break;
        case 19 :
            driveNumber = EDriveT;
            break;
        case 20 :
            driveNumber = EDriveU;
            break;
        case 21 :
            driveNumber = EDriveV;
            break;
        case 22 :
            driveNumber = EDriveW;
            break;
        case 23 :
            driveNumber = EDriveX;
            break;
        case 24 :
            driveNumber = EDriveY;
            break;
        case 25 :
            driveNumber = EDriveZ;
            break;
        default :
            driveNumber = EDriveC;
            break;
        }
        
    return driveNumber;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::FilterDriveList( TDriveList& aDriveList ) const
// Filters the drive list
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::FilterDriveList( TDriveList& aDriveList ) const
    {
    TDriveInfo info;
    
    for ( TInt i = 0; i < aDriveList.Length(); i++ )
        {
        if ( aDriveList[i] )
            {
            iFs.Drive( info, i );
#ifdef __WINS__
            if ( i == EDriveD )
                {
                LOGGER_WRITE( "EDriveD skipped on WINS build" );
                aDriveList[i] = '\x0';
                }
#else
            if ( info.iType == EMediaRam )
                {
                aDriveList[i] = '\x0';
                }
#endif
            }
        }
    
    }
    
// -----------------------------------------------------------------------------
// CSConSBEClient::Match( const CSConTask& aFirst, const CSConTask& aSecond )
// Matches the uids
// -----------------------------------------------------------------------------
//
TInt CSConSBEClient::Match( const TUid& aFirst, const TUid& aSecond )
    {
    if( aFirst == aSecond )
        {
        return ETrue;
        }
        
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::HandleSBEErrorL( TInt& aErr )
// Handle error received from Secure Backup Engine
// -----------------------------------------------------------------------------
//  
void CSConSBEClient::HandleSBEErrorL( TInt& aErr )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( " aErr: %d", aErr );
    if( aErr == KErrServerTerminated )
        {
        LOGGER_WRITE( "Re-connecting to SBE..." );
        //Session has been terminated, recreate it..
        delete iSBEClient;
        iSBEClient = NULL;
        iSBEClient = CSBEClient::NewL();
        }
    else if ( aErr ) // error
        {
        if ( iDataBuffer )
            {
            iDataBuffer->Reset();
            iDataBufferSize = 0;
            }
        iLastChunk = EFalse;
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::AppendFilesToFilelistL()
// Appends files from RFileArray to RPointerArray<CSConFile>
// -----------------------------------------------------------------------------
//
void CSConSBEClient::AppendFilesToFilelistL( const RFileArray& aFiles, RPointerArray<CSConFile>& aSconFiles )
    {
    TRACE_FUNC_ENTRY;
    _LIT( KSConXmlDate, "%F%Y%M%DT%H%T%SZ" );
    for( TInt j = 0; j < aFiles.Count(); j++ )
        {
        CSConFile* fileInfo = new (ELeave) CSConFile();
        CleanupStack::PushL( fileInfo );
        fileInfo->iPath = aFiles[j].iName;
        LOGGER_WRITE_1("file: %S", &fileInfo->iPath );                    
        
        fileInfo->iSize = aFiles[j].iSize;
        
        switch( aFiles[j].iAtt )
            {
            case KEntryAttNormal :
                fileInfo->iUserPerm = EPermNormal;
                break;
            case KEntryAttReadOnly :
                fileInfo->iUserPerm = EPermReadOnly;
                break;
            default :
                fileInfo->iUserPerm = EPermNormal;
                break;
            }
                        
        TTime time( aFiles[j].iModified );
        HBufC* timeBuf = HBufC::NewLC( KMaxTimeFormatSpec );
        TPtr timePtr = timeBuf->Des();
        time.FormatL( timePtr, KSConXmlDate );

        fileInfo->iModified.Copy( timePtr );

        CleanupStack::PopAndDestroy( timeBuf );    

        aSconFiles.Append( fileInfo );
        CleanupStack::Pop( fileInfo );
        }
    TRACE_FUNC_EXIT;
    }


// -----------------------------------------------------------------------------
// CSConSBEClient::PackageDataSizeL()
// Gets Package dataowner size
// -----------------------------------------------------------------------------
//
TUint CSConSBEClient::PackageDataSizeL( TUid aPackageId, const TDriveList& aDriveList,
        TPackageDataType aPackageDataType ) const
    {
    TUint dataSize(0);
    //Ask data size from every drive
    for( TInt j = 0; j < KMaxDrives; j++ )
        {
        if( aDriveList[j] )
            {
            CSBPackageTransferType* ptt( NULL );
            TDriveNumber driveNumber = GetDriveNumber( j );
            LOGGER_WRITE_1( "Drive %d", driveNumber );
            ptt = CSBPackageTransferType::NewL( aPackageId, driveNumber, aPackageDataType );
            CleanupStack::PushL( ptt );
            //Get the size and store it to a variable
            TRAPD( err, dataSize += iSBEClient->ExpectedDataSizeL( *ptt ) );
            CleanupStack::PopAndDestroy( ptt );
            LOGGER_WRITE_1( "ExpectedDataSizeL returned for package %d", err );
            if( err != KErrNone && err != KErrNotFound )
                {
                LOGGER_WRITE_1( "ExpectedDataSizeL LEAVE %d", err );
                User::Leave( err );
                }
            }
        }
    return dataSize;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::SidDataSizeL()
// Gets Sid (SecureId) dataowner size
// -----------------------------------------------------------------------------
//
TUint CSConSBEClient::SidDataSizeL( TUid aSid, const TDriveList& aDriveList,
        TTransferDataType aTransferDataType ) const
    {
    TUint dataSize(0);
    //Ask data size from every drive
    for( TInt j = 0; j < KMaxDrives; j++ )
        {
        if( aDriveList[j] )
            {
            CSBSIDTransferType* stt( NULL );
            TDriveNumber driveNumber = GetDriveNumber( j );
            LOGGER_WRITE_1( "Drive %d", driveNumber );
            stt = CSBSIDTransferType::NewL( aSid, driveNumber, aTransferDataType );
            CleanupStack::PushL( stt );
            //Get the size and store it to a variable
            TRAPD( err, dataSize += iSBEClient->ExpectedDataSizeL( *stt ) ); 
            CleanupStack::PopAndDestroy( stt );
            LOGGER_WRITE_1( "ExpectedDataSizeL returned for DO %d", err );
            if( err != KErrNone && err != KErrNotFound )
                {
                LOGGER_WRITE_1( "ExpectedDataSizeL LEAVE %d", err );
                User::Leave( err );
                }
            }
        }
    return dataSize;
    }

// -----------------------------------------------------------------------------
// CSConSBEClient::JavaDataSizeL()
// Gets Java dataowner size
// -----------------------------------------------------------------------------
//
TUint CSConSBEClient::JavaDataSizeL( const TDesC& aJavaHash, const TDriveList& aDriveList ) const
    {
    TUint dataSize(0);
    //Ask data size from every drive
    for( TInt j = 0; j < KMaxDrives; j++ )
        {
        if( aDriveList[j] )
            {
            CSBJavaTransferType* jtt( NULL );
            
            TDriveNumber driveNumber = GetDriveNumber( j );
            LOGGER_WRITE_1( "Drive %d", driveNumber );
            jtt = CSBJavaTransferType::NewL( aJavaHash, 
                                            driveNumber, 
                                            EJavaMIDlet );
            CleanupStack::PushL( jtt );
            //Get the size and store it to a variable
            TRAPD( err, dataSize += iSBEClient->ExpectedDataSizeL( *jtt ) ); 
            CleanupStack::PopAndDestroy( jtt );
            LOGGER_WRITE_1( "ExpectedDataSizeL returned for Java(EJavaMIDlet) DO %d", err );
            if( err != KErrNone && err != KErrNotFound )
                {
                LOGGER_WRITE_1( "ExpectedDataSizeL LEAVE %d", err );
                User::Leave( err );
                }
            
            jtt = NULL;
            jtt = CSBJavaTransferType::NewL( aJavaHash, 
                                            driveNumber, 
                                            EJavaMIDletData );
            CleanupStack::PushL( jtt );
            //Get the size and store it to a variable
            TRAP( err, dataSize += iSBEClient->ExpectedDataSizeL( *jtt ) ); 
            CleanupStack::PopAndDestroy( jtt );
            LOGGER_WRITE_1( "ExpectedDataSizeL returned for Java(EJavaMIDletData) DO %d", err );
            if( err != KErrNone && err != KErrNotFound )
                {
                LOGGER_WRITE_1( "ExpectedDataSizeL LEAVE %d", err );
                User::Leave( err );
                }
            }
        }
    
    return dataSize;
    }
// End of file
