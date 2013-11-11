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
* Description:  Implementation of RFS (Restore Factory Settings) Plugin i/f.
*
*/
//  Include Files
#include <driveinfo.h>
#include <f32file.h>

#include "RfsMsgStorePlugin.h"
#include "emailstoreuids.hrh"
#include <bautils.h>  
#include <s32file.h>

// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::NewL()
// -----------------------------------------------------------------------------
//
CRfsMsgStorePlugin* CRfsMsgStorePlugin::NewL()
    {
    CRfsMsgStorePlugin* self = new ( ELeave ) CRfsMsgStorePlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::~CRfsMsgStorePlugin()
// -----------------------------------------------------------------------------
//
CRfsMsgStorePlugin::~CRfsMsgStorePlugin()
    {
    iFs.Close();
    }

// ---------------------------------------------------------------------------
// CRfsMsgStorePlugin::RestoreFactorySettingsL
// Handle rfs.
// ---------------------------------------------------------------------------
//
void CRfsMsgStorePlugin::RestoreFactorySettingsL( const TRfsReason aType )
    {
    if ( aType == ENormalRfs || aType == EDeepRfs || aType == EInitRfs )
        {
        TUint driveStatus( 0 );
        TDriveNumber drive( EDriveC );
        
        if ( DriveInfo::GetDriveStatus( iFs, drive, driveStatus ) == KErrNone )
            {
            _LIT( KPrivate, ":\\Private\\" );    
            TChar driveChar;
            RFs::DriveToChar(drive, driveChar);
            TFileName msgStorePath;
            TBuf<5> driveLetter;
            driveLetter.Append(driveChar);
            msgStorePath.Append(driveLetter);
            msgStorePath.Append(KPrivate);                
            msgStorePath.AppendNum( KUidMessageStoreExe, EHex );
            msgStorePath.Append( KPathDelimiter );
            CFileMan* fileManager = CFileMan::NewL( iFs );
            fileManager->RmDir( msgStorePath );
            delete fileManager;
            }
        }
    }

// ---------------------------------------------------------------------------
// CRfsMsgStorePlugin::GetScriptL
// ---------------------------------------------------------------------------
//
void CRfsMsgStorePlugin::GetScriptL( const TRfsReason /*aType*/, TDes& aPath )
    {
    // no-op
    // there is no script for RFS Server to execute,
    // just populate the aPath to null string
    aPath.Copy( KNullDesC );
    }

// ---------------------------------------------------------------------------
// CRfsMsgStorePlugin::ExecuteCustomCommandL
// ---------------------------------------------------------------------------
void CRfsMsgStorePlugin::ExecuteCustomCommandL( const TRfsReason /*aType*/, TDesC& /*aCommand*/ )
    {
    // no-op
    // there is no custom command to be executed by this plugin
    }

// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::CRfsMsgStorePlugin()
// -----------------------------------------------------------------------------
//
CRfsMsgStorePlugin::CRfsMsgStorePlugin()
    {
    }

// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::ConstructL()
// -----------------------------------------------------------------------------
//
void CRfsMsgStorePlugin::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }
    

// END FILE RfsMsgStorePlugin.cpp
