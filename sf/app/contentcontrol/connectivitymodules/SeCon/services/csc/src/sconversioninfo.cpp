/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSconVersionInfo implementation.
*
*/


#include "sconversioninfo.h"

#include <centralrepository.h>
#include <DSCapabilityManagementCRKeys.h>
#include <etel3rdparty.h> // read imsi
#include <etel.h>
#include <etelmm.h>
#include <hal.h>

#include "caputils.h"
#include "debug.h"

CSconVersionInfo::CSconVersionInfo()
    {
    iSymbianVersionError = KErrNotReady;
    iS60VersionError = KErrNotReady;
    }

CSconVersionInfo::~CSconVersionInfo()
    {
    delete iManufacturer;
    delete iModel;
    delete iProduct;
    delete iRevision;
    delete iSWVersion;
    delete iSerialNumber;
    delete iSysVersionInfo;
    delete iLangVersion;
    delete iLangSWVersion;
    delete iOPVersion;
    delete iProductCode;
    }

CSconVersionInfo* CSconVersionInfo::NewL()
    {
    CSconVersionInfo* self = new (ELeave) CSconVersionInfo;
    return self;
    }

// -----------------------------------------------------------------------------
// CSconVersionInfo::FetchInfoL()
// fetch device info
// -----------------------------------------------------------------------------
//
void CSconVersionInfo::FetchInfoL( RFs& aFs )
    {
    TRACE_FUNC_ENTRY;
    
    iSymbianVersionError = SysVersionInfo::GetVersionInfo( iSymbianVersion, aFs );
    iS60VersionError = VersionInfo::GetVersion( iS60Version, aFs );
    
    TBuf<KSysVersionInfoTextLength> info;
    TBuf<KSysVersionInfoTextLength> productBuf;
    TInt err(KErrNone);
    delete iSWVersion;
    iSWVersion = NULL;
    delete iProduct;
    iProduct = NULL;
    // get SW version, SW version date and model
    TRAP( err, CapUtil::GetSWVersionL( info, iDate, productBuf ) );
    iSWVersion = info.AllocL();
    iProduct = productBuf.AllocL();
    LOGGER_WRITE_1("CapUtil::GetSWVersionL err: %d", err);
    
    
    SysVersionInfo::TProductVersion productVersion;
    TInt sysVersionError = SysVersionInfo::GetVersionInfo( productVersion, aFs );
    LOGGER_WRITE_1( "SysVersionInfo::GetVersionInfo returned : %d", sysVersionError );
    
    // Use TelServer to get IMEI and also other info if SysVersionInfo is not supported
    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL( telServer );
    RTelServer::TPhoneInfo teleinfo;
    User::LeaveIfError( telServer.GetPhoneInfo( 0, teleinfo ) );
    RMobilePhone phone;
    User::LeaveIfError( phone.Open( telServer, teleinfo.iName ) );
    CleanupClosePushL( phone );
    User::LeaveIfError(phone.Initialise()); 
    TUint32 teleidentityCaps;
    phone.GetIdentityCaps( teleidentityCaps );
    RMobilePhone::TMobilePhoneIdentityV1 telid;
    TRequestStatus status;
    phone.GetPhoneId( status, telid );
    User::WaitForRequest( status );
    if ( status == KErrNone )
        {
        if ( sysVersionError )
            {
            LOGGER_WRITE("Use info from TMobilePhoneIdentityV1");
            delete iModel;
            iModel = NULL;
            delete iRevision;
            iRevision = NULL;
            
            // phone model sales name. For example "N01".
            iModel = telid.iModel.AllocL();
            // product revision. For example "01"
            iRevision = telid.iRevision.AllocL();
            }
        delete iSerialNumber;
        iSerialNumber = NULL;
        // Phone serial number (IMEI or ESN), in character string format.
        iSerialNumber = telid.iSerialNumber.AllocL();
        }
    
    CleanupStack::PopAndDestroy( &phone );
    CleanupStack::PopAndDestroy( &telServer );
        
    if ( sysVersionError == KErrNone )
        {
        // use information from SysVersionInfo instead of previous APIs.
        LOGGER_WRITE("Using SysVersionInfo");
        
        // phone model sales name. For example "N01".
        delete iModel;
        iModel = NULL;
        iModel = productVersion.iModel.AllocL();
        // product revision. For example "01"
        delete iRevision;
        iRevision = NULL;
        iRevision = productVersion.iRevision.AllocL();
        // manufacturer name. For example "Nokia"
        delete iManufacturer;
        iManufacturer = NULL;
        iManufacturer = productVersion.iManufacturer.AllocL();
        // product code name. For example "RM-1"
        delete iProduct;
        iProduct = NULL;
        iProduct = productVersion.iProduct.AllocL();
        }
    else
        {
        CapUtil::GetManufacturer( info );
        delete iManufacturer;
        iManufacturer = NULL;
        iManufacturer = info.AllocL();
        }
        
    CapUtil::GetLanguage( iLanguage );
    
    
    err = SysVersionInfo::GetVersionInfo( SysVersionInfo::EFWVersion, info, aFs );
    delete iSysVersionInfo;
    iSysVersionInfo = NULL;
    if ( !err )
        {
        iSysVersionInfo = info.AllocL();
        }
    
    err = SysUtil::GetLangVersion( info );
    delete iLangVersion;
    iLangVersion = NULL;
    if ( !err )
        {
        iLangVersion = info.AllocL();
        }
    
    sysVersionError = SysUtil::GetLangSWVersion( info );
    delete iLangSWVersion;
    iLangSWVersion = NULL;
    if ( !sysVersionError )
        {
        iLangSWVersion = info.AllocL();
        }
                
    sysVersionError = SysVersionInfo::GetVersionInfo( SysVersionInfo::EOPVersion, info, aFs );
    delete iOPVersion;
    iOPVersion = NULL;
    if ( !sysVersionError )
        {
        iOPVersion = info.AllocL();
        }
    
    
    sysVersionError = SysVersionInfo::GetVersionInfo( SysVersionInfo::EProductCode, info, aFs );
    delete iProductCode;
    iProductCode = NULL;
    if ( !sysVersionError )
        {
        iProductCode = info.AllocL();
        }
    
    // read DesktopSync key value
    CRepository* repository(NULL);
    TRAP( iDesktopSyncError, repository = CRepository::NewL( KCRUidDSDCMOConfig ));
    if ( !iDesktopSyncError )
        {
        iDesktopSyncError = repository->Get( KNsmlDesktopSync, iDesktopSync );
        LOGGER_WRITE_1("iDesktopSyncError: %d", iDesktopSyncError );
        LOGGER_WRITE_1("iDesktopSync: %d", iDesktopSync );
        delete repository;
        }
    else
        {
        LOGGER_WRITE_1("Could not create CRepository, err: %d", iDesktopSyncError );
        }
    
    // screen size
    HAL::Get(HAL::EDisplayXPixels, iScreenSize.iWidth);
    HAL::Get(HAL::EDisplayYPixels, iScreenSize.iHeight);
    
    iInfoFetched = ETrue;
    TRACE_FUNC_EXIT;   
    }

// -----------------------------------------------------------------------------
// CSconVersionInfo::IsReady()
// 
// -----------------------------------------------------------------------------
//
TBool CSconVersionInfo::IsReady()
    {
    return iInfoFetched;
    }

// -----------------------------------------------------------------------------
// CSconVersionInfo::GetSymbianVersion()
// Get Symbian OS version
// -----------------------------------------------------------------------------
//
TInt CSconVersionInfo::GetSymbianVersion( SysVersionInfo::TSymbianOSVersion& aSymbianVersion )
    {
    if ( iSymbianVersionError )
        {
        return iSymbianVersionError;
        }
    else
        {
        aSymbianVersion = iSymbianVersion;
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CSconVersionInfo::GetS60Version()
// Get S60 platform version
// -----------------------------------------------------------------------------
//
TInt CSconVersionInfo::GetS60Version( VersionInfo::TPlatformVersion& aS60Version )
    {
    if ( iS60VersionError )
        {
        return iS60VersionError;
        }
    else
        {
        aS60Version = iS60Version;
        return KErrNone;
        }
    }

