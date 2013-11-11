/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Base class for different media type info classes.
*
*/




// INCLUDE FILES
#include <e32std.h>
#include <e32base.h> // CBase
#include <apmstd.h>  // TDataType
#include <f32file.h> // RFs, RFile
#include <apgcli.h>  // RApaLsSession

#include <caf/caf.h>
#include <DRMCommon.h> // Just for error codes
#include <DRMHelper.h>
#include <fileprotectionresolver.h>

#include "MsgMediaInfo.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMediaInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMsgMediaInfo* CMsgMediaInfo::NewL( RFile& aFile, 
                                            TDataType& aMimeType,
                                            TMsgMediaType aMediaType )
    {
    CMsgMediaInfo* self = new( ELeave ) CMsgMediaInfo( aMimeType, aMediaType );
    CleanupStack::PushL( self );
    self->ConstructL( aFile );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::CMsgMediaInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMediaInfo::CMsgMediaInfo( TDataType& aDataType, TMsgMediaType aMediaType ):
    CActive( EPriorityLow ),
    iMimeType( aDataType ),
    iFileSize( 0 ),
    iModificationTime(),
    iMediaType( aMediaType ),
    iDuration( 0 ),
    iProtection( EFileProtNoProtection ),
    iParsed( EFalse ),
    iHandlerAppUid( KNullUid )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMediaInfo::ConstructL( RFile& aFile )
    {
    TFileName fullName;
    User::LeaveIfError( aFile.FullName( fullName ) );
    User::LeaveIfError( aFile.Size( iFileSize ) );
    User::LeaveIfError( aFile.Modified( iModificationTime ) );
    iFullFilePath = fullName.AllocL();
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CMsgMediaInfo::~CMsgMediaInfo()
    {
    Cancel();
    iObserver = NULL;
    iDRMHelper = NULL;
    iFile.Close();
    delete iFullFilePath;
    delete iContentURI;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::ParseInfoDetails
// -----------------------------------------------------------------------------
//
EXPORT_C void CMsgMediaInfo::ParseInfoDetails( RFile& aFile, CDRMHelper& aDRMHelper, MMediaInfoObserver& aObserver )
    {
    iObserver = &aObserver;
    iDRMHelper = &aDRMHelper;
    iFile.Duplicate( aFile );
    // Actual parsing is done in RunL (of each media info class).
    CompleteSelf( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::ParseInfoDetailsL
//
// Deprecated. Use the non-leaving version instead.
// -----------------------------------------------------------------------------
//
EXPORT_C void CMsgMediaInfo::ParseInfoDetailsL( RFile& aFile, CDRMHelper& aDRMHelper, MMediaInfoObserver& aObserver )
    {
    ParseInfoDetails( aFile, aDRMHelper, aObserver );
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::SetFileL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMsgMediaInfo::SetFileL( RFile& aFile )
    {
    TBool oldParsed = iParsed;
    iParsed = EFalse;

    delete iFullFilePath;
    iFullFilePath = NULL;
    
    TFileName fullName;
    User::LeaveIfError( aFile.FullName( fullName ) );
    User::LeaveIfError( aFile.Size( iFileSize ) );
    User::LeaveIfError( aFile.Modified( iModificationTime ) );
    iFullFilePath = fullName.AllocL();

    iParsed = oldParsed;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::Corrupt
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMsgMediaInfo::Corrupt() const
    {
    TBool corrupt( ETrue );
    switch ( iParseError )
        {
        case KErrNone:
        case KErrNoMemory:
        case KErrInUse:
        case DRMCommon::ENoRights:
        case DRMCommon::ERightsExpired:
        case DRMCommon::EInvalidRights:
            corrupt = EFalse;
            break;
        default:
            // All other errors are interpreted as corrupt
            break;
        }
    return corrupt;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::DoCancel
// -----------------------------------------------------------------------------
//
void CMsgMediaInfo::DoCancel()
    {
    iFile.Close();
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::RunL
// -----------------------------------------------------------------------------
//
void CMsgMediaInfo::RunL()
    {
    // Never fails since does actually nothing.
    iParsed = ETrue;
    CompleteObserver();
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::RunError
// -----------------------------------------------------------------------------
//
TInt CMsgMediaInfo::RunError( TInt aError )
    {
    CompleteSelf( aError );
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// CMsgMediaInfo::CompleteSelf
// -----------------------------------------------------------------------------
//
void CMsgMediaInfo::CompleteSelf( TInt aError )
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, aError );
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::CompleteSelf
// -----------------------------------------------------------------------------
//
void CMsgMediaInfo::CompleteObserver()
    {
    iFile.Close();
    ReleaseRights();
    iDRMHelper = NULL;
    iObserver->MediaInfoParsed();
    iObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::FreezeRights
// -----------------------------------------------------------------------------
//
void CMsgMediaInfo::FreezeRights()
    {
    if ( iProtection & ( EFileProtForwardLocked | EFileProtSuperDistributable ) &&
        iContentURI && iContentURI->Length() )
        {
        TInt err = iDRMHelper->Consume2(
            *iContentURI,
            ContentAccess::EInstall,
            CDRMHelper::EStart );
        if ( !err )
            {
            iConsumed = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMsgMediaInfo::ReleaseRights
// -----------------------------------------------------------------------------
//
void CMsgMediaInfo::ReleaseRights()
    {
    if ( iConsumed )
        {
        /*TInt ignore =*/ 
        iDRMHelper->Consume2(
            *iContentURI,
            ContentAccess::EInstall,
            CDRMHelper::EFinish );
        iConsumed = EFalse;
        }
    }

//  End of File  
