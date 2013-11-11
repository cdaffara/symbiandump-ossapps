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
* Description:  SysInfoPlugin implementation
*
*/


// INCLUDE FILES
#include "HtiSysInfoServicePlugin.h"
#include "HtiLightsController.h"
#include "HtiPropertySubscriber.h"
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
#include <aknkeylock.h>
#include <ScreensaverInternalPSKeys.h>
#endif

#include <AknSkinsInternalCRKeys.h>
#include <AknsSkinUID.h>
#include <AknsSrvClient.h>
#include <AknFepInternalCRKeys.h>
#include <AknFepGlobalEnums.h> //For chinese input modes
#include <CommonEngineDomainCRKeys.h>
#include <featmgr.h>
#include <bautils.h>
#include <btengsettings.h>
#include <btengdomaincrkeys.h>
#include <bt_subscribe.h>
#include <btmanclient.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <DRMRightsClient.h>
#include <e32property.h>
#include <etel.h>
#include <etelmm.h>
#include <hal.h>
#include <ir_sock.h>
#include <mmtsy_names.h>
#include <rmmcustomapi.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <settingsinternalcrkeys.h>
#include <sysutil.h>
#include <tz.h>

// CONSTANTS
const static TUid KSysInfoServiceUid = { 0x10210CC7 };

// from irinternalpskey.h
const static TUid KPSUidIrdaActivation = { 0x2000276D };

const TInt KTimeDataLength = 7;
const TInt KMaxBtNameLength = 30;
const TInt KDateTimeFormatCmdLength = 6;

const TInt KFepChineseInputModeLength = 10;

_LIT( KTempFilePath, "\\" );
_LIT( KTempFileName, "HtiTempFile.tmp" );
_LIT( KMatchFileName, "HtiTempFile.tmp*" );
_LIT( KDateSeparatorChars, ".:/-" );
_LIT( KTimeSeparatorChars, ".:" );

_LIT8( KErrDescrArgument, "Invalid argument" );
_LIT8( KErrDescrNotSupported, "Command not supported" );
_LIT8( KErrDescrHAL, "Error retrieving HAL attribute" );
_LIT8( KErrDescrAttOutOfRange, "HAL attribute argument is out of range" );
_LIT8( KErrDescrFreeRAM, "Error retrieving the amount of free RAM" );
_LIT8( KErrDescrTotalRAM, "Error retrieving the amount of total RAM" );
_LIT8( KErrDescrAllocRAM, "Error allocating RAM" );
_LIT8( KErrDescrInvalidRAM, "Requested free RAM larger than currently free" );
_LIT8( KErrDescrVolInfo, "Error retrieving volume info" );
_LIT8( KErrDescrNotEnoughSpace, "Not enough disk space" );
_LIT8( KErrDescrCreateTempFile, "Error creating temp file" );
_LIT8( KErrDescrSetSizeTempFile, "Error allocating size for temp file" );
_LIT8( KErrDescrDeleteTempFile, "Error deleting temp file" );
_LIT8( KErrDescrSysUtil, "SysUtil failed" );
_LIT8( KErrDescrSetTime, "Setting time failed" );
_LIT8( KErrDescrDateTimeFormat, "Setting date and time formats failed" );
_LIT8( KErrDescrSetLanguage, "Setting language failed");
_LIT8( KErrDescrGetNetworkModes, "Getting network modes failed" );
_LIT8( KErrDescrSetNetworkMode, "Setting network mode failed" );
_LIT8( KErrDescrIrActivation, "IR activation failed" );
_LIT8( KErrDescrGetBtPower, "Getting BT power state failed" );
_LIT8( KErrDescrSetBtPower, "Setting BT power state failed" );
_LIT8( KErrDescrBtOnDenied, "Turning BT on not allowed (Offline mode)" );
_LIT8( KErrDescrBtOffDenied, "Turning BT off not allowed (active connections)" );
_LIT8( KErrDescrBtSettings, "Bluetooth settings failed" );
_LIT8( KErrDescrBtDeletePairings, "Deleting Bluetooth pairing(s) failed" );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
_LIT8( KErrDescrKeyLock, "Key lock toggle failed" );
_LIT8( KErrDescrScreenSaver, "Setting screen saver state failed" );
_LIT8( KErrDescrInvalidSSTimeout, "Invalid screen saver timeout value" );
_LIT8( KErrDescrSSTimeoutFailed, "Setting screen saver timeout failed" );
#endif
_LIT8( KErrDescrInvalidTime, "Auto key guard time value too large (max 3600)" );
_LIT8( KErrDescrAutoKeyGuardFailed, "Setting auto key guard time failed" );
_LIT8( KErrDescrDrmDbConnect, "DRM DB connect failed." );
_LIT8( KErrDescrDrmDbDelete,  "DRM DB delete failed." );
_LIT8( KErrDescrBatteryLevel, "Getting battery level failed." );
_LIT8( KErrDescrChargingStatus, "Getting charging status failed." );
_LIT8( KErrDescrSignalStrength, "Getting signal strength failed." );
_LIT8( KErrDescrMGUpdate, "Update Media Gallery failed" );
_LIT8( KErrDescrActivateSkin, "Activating Skin failed" );

enum TSysInfoCommand
    {
    ESysInfoHAL =             0x01,
    ESysInfoIMEI=             0x02,
    ESysInfoSWVersion =       0x03,
    ESysInfoLangVersion =     0x04,
    ESysInfoSWLangVersion =   0x05,
    ESysInfoUserAgent =       0x06,
    EFreeRAM =                0x07,
    EUsedRAM =                0x08,
    ETotalRAM =               0x09,
    EEatRAM =                 0x0A,
    EReleaseRAM =             0x0B,
    EFreeDiskSpace =          0x0C,
    EUsedDiskSpace =          0x0D,
    ETotalDiskSize =          0x0E,
    EEatDiskSpace =           0x0F,
    EReleaseDiskSpace =       0x10,

    ESysInfoSetHomeTime =     0x20,
    ESysInfoGetHomeTime =     0x21,
    ESetDateTimeFormat =      0x22,
    
    ESetLanguage =            0x25,

    ELightStatus =            0x30,
    ELightOn =                0x31,
    ELightOff =               0x32,
    ELightBlink =             0x33,
    ELightRelease =           0x3A,

    EScreenSaverDisable =     0x40,
    EScreenSaverEnable  =     0x41,
    EScreenSaverTimeout =     0x42,

    ENetworkModeGet =         0x50,
    ENetworkModeSet =         0x51,
    ENetworkModeSetNoReboot = 0x52,
    EHsdpaSet =               0x53,

    EIrActivate =             0x5A,
    EBtPower =                0x5B,
    EBtSettings =             0x5C,
    EBtDeletePairings =       0x5D,

    EKeylockToggle =          0x60,
    EAutoKeyGuardTime =       0x61,

    EEmtpyDrmRightsDb =       0x65,

    EBatteryStatus =          0x70,
    ESignalStrength =         0x71,

    EUpdateMediaGallery =     0x7A,

    EActivateSkin =           0x80
    };

enum TGSNumberModes
    {
    EGSNbrModeLatin, EGSNbrModeArabic = 1, EGSNbrModeIndic = 1
    };

// Number mode type
enum TGSNumberModeType
    {
    EGSNbrModeTypeArabic, EGSNbrModeTypeIndic, EGSNbrModeTypeEasternArabic
    // for Urdu & Farsi languages
    };

//------------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
//------------------------------------------------------------------------------
CHtiSysInfoServicePlugin* CHtiSysInfoServicePlugin::NewL()
    {
    CHtiSysInfoServicePlugin* self = new (ELeave) CHtiSysInfoServicePlugin;
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
CHtiSysInfoServicePlugin::CHtiSysInfoServicePlugin():
    iMemEater( NULL ), iReply( NULL ), iGalleryUpdateSupported( ETrue )
    {
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
        iAllowSSValue = -1;
        iAllowSSPropertyAttached = EFalse;
#endif
    }

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
CHtiSysInfoServicePlugin::~CHtiSysInfoServicePlugin()
    {
    HTI_LOG_TEXT( "CHtiSysInfoServicePlugin destroy" );
    delete iMemEater;
    delete iReply;

    CleanUpTempFiles();
    delete iFileMan;
    iFs.Close();
    delete iLightsController;

    if ( iAllowSSSubscriber )
        {
        iAllowSSSubscriber->Unsubscribe();
        }
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    iAllowSSProperty.Close();
#endif 
    delete iAllowSSSubscriber;
    
    FeatureManager::UnInitializeLib();
    }

//------------------------------------------------------------------------------
// Second phase construction
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::ConstructL()
    {
    HTI_LOG_TEXT( "CHtiSysInfoServicePlugin::ConstructL" );
    User::LeaveIfError( iFs.Connect() );
    iFileMan = CFileMan::NewL( iFs );
    
    FeatureManager::InitializeLibL();
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::ProcessMessageL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::ProcessMessageL(const TDesC8& aMessage,
                                THtiMessagePriority /*aPriority*/)
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::ProcessMessage" );
    HTI_LOG_FORMAT( "Message length: %d", aMessage.Length() );

    if ( aMessage.Length() > 0 )
        {
        HTI_LOG_FORMAT( "Command: %d", aMessage[0] );

        switch ( aMessage[0] )
            {
            case ESysInfoHAL:
                {
                HTI_LOG_TEXT( "ESysInfoHAL" );
                HandleGetHalAttrL( aMessage );
                }
                break;
            case ESysInfoIMEI:
                {
                HTI_LOG_TEXT( "ESysInfoIMEI" );
                HandleGetImeiL( aMessage );
                }
                break;
            case ESysInfoSWVersion:
                {
                HTI_LOG_TEXT( "ESysInfoSWVersion" );
                HandleGetSwVersionL( aMessage );
                }
                break;
            case ESysInfoLangVersion:
                {
                HTI_LOG_TEXT( "ESysInfoLangVersion" );
                HandleGetLangVersionL( aMessage );
                }
                break;
            case ESysInfoSWLangVersion:
                {
                HTI_LOG_TEXT( "ESysInfoSWLangVersion" );
                HandleGetSwLangVersionL( aMessage );
                }
                break;
            case ESysInfoUserAgent:
                {
                HTI_LOG_TEXT( "ESysInfoUserAgent" );
                HandleGetUserAgentStringL( aMessage );
                }
                break;
            case EFreeRAM:
                {
                HTI_LOG_TEXT( "EFreeRAM" );
                HandleGetFreeRamL( aMessage );
                }
                break;
            case EUsedRAM:
                {
                HTI_LOG_TEXT( "EUsedRAM" );
                HandleGetUsedRamL( aMessage );
                }
                break;
            case ETotalRAM:
                {
                HTI_LOG_TEXT( "ETotalRAM" );
                HandleGetTotalRamL( aMessage );
                }
                break;
            case EEatRAM:
                {
                HTI_LOG_TEXT( "EEatRAM" );
                HandleEatRamL( aMessage );
                }
                break;
            case EReleaseRAM:
                {
                HTI_LOG_TEXT( "EReleaseRAM" );
                HandleReleaseRamL( aMessage );
                }
                break;
            case EFreeDiskSpace:
                {
                HTI_LOG_TEXT( "EFreeDiskSpace" );
                HandleGetFreeDiskSpaceL( aMessage );
                }
                break;
            case EUsedDiskSpace:
                {
                HTI_LOG_TEXT( "EUsedDiskSpace" );
                HandleGetUsedDiskSpaceL( aMessage );
                }
                break;
            case ETotalDiskSize:
                {
                HTI_LOG_TEXT( "ETotalDiskSize" );
                HandleGetTotalDiskSpaceL( aMessage );
                }
                break;
            case EEatDiskSpace:
                {
                HTI_LOG_TEXT( "EEatDiskSpace" );
                HandleEatDiskSpaceL( aMessage );
                }
                break;
            case EReleaseDiskSpace:
                {
                HTI_LOG_TEXT( "EReleaseDiskSpace" );
                HandleReleaseDiskSpaceL( aMessage );
                }
                break;
            case ESysInfoSetHomeTime:
                {
                HTI_LOG_TEXT( "ESysInfoSetHomeTime" );
                HandleSetHomeTimeL( aMessage );
                }
                break;
            case ESysInfoGetHomeTime:
                {
                HTI_LOG_TEXT( "ESysInfoGetHomeTime" );
                HandleGetHomeTimeL( aMessage );
                }
                break;
            case ESetDateTimeFormat:
                {
                HTI_LOG_TEXT( "ESetDateTimeFormat" );
                HandleSetDateTimeFormatL( aMessage );
                }
                break;
            case ESetLanguage:
                {
                HTI_LOG_TEXT("ESetLanguage");
                HandleSetLanguageL( aMessage);
                }
                break;
            case ELightStatus:
            case ELightOn:
            case ELightOff:
            case ELightBlink:
            case ELightRelease:
                {
                HTI_LOG_TEXT( "ELight*" );
                HandleLightsCommandL( aMessage );
                }
                break;
            case EScreenSaverDisable:
            case EScreenSaverEnable:
                {
                HTI_LOG_TEXT( "EScreenSaver*" );
                HandleScreenSaverCommandL( aMessage );
                }
                break;
            case EScreenSaverTimeout:
                {
                HTI_LOG_TEXT( "EScreenSaverTimeout" );
                HandleScreenSaverTimeoutCommandL( aMessage );
                }
                break;
            case ENetworkModeSet:
            case ENetworkModeSetNoReboot:
            case ENetworkModeGet:
                {
                HTI_LOG_TEXT( "ENetworkMode*" );
                HandleNetworkModeCommandL( aMessage );
                }
                break;
            case EHsdpaSet:
                {
                HTI_LOG_TEXT( "EHsdpaSet" );
                HandleHsdpaCommandL( aMessage );
                }
                break;
            case EIrActivate:
                {
                HTI_LOG_TEXT( "EIrActivate" );
                HandleIrActivateCommandL( aMessage );
                }
                break;
            case EBtPower:
                {
                HTI_LOG_TEXT( "EBtPower" );
                HandleBtPowerCommandL( aMessage );
                }
                break;
            case EBtSettings:
                {
                HTI_LOG_TEXT( "EBtSettings" );
                HandleBtSettingsCommandL( aMessage );
                }
                break;
            case EBtDeletePairings:
                {
                HTI_LOG_TEXT( "EBtDeletePairings" );
                HandleBtDeletePairingsL( aMessage );
                }
                break;
            case EKeylockToggle:
                {
                HTI_LOG_TEXT( "EKeylockToggle" );
                HandleKeyLockToggleL( aMessage );
                }
                break;
            case EAutoKeyGuardTime:
                {
                HTI_LOG_TEXT( "EAutoKeyGuardTime" );
                HandleAutoKeyGuardTimeL( aMessage );
                }
                break;
            case EEmtpyDrmRightsDb:
                {
                HTI_LOG_TEXT( "EEmtpyDrmRightsDb" );
                HandleEmptyDrmRightsDbL( aMessage );
                }
                break;
            case EBatteryStatus:
                {
                HTI_LOG_TEXT( "EBatteryStatus" );
                HandleBatteryStatusL( aMessage );
                }
                break;
            case ESignalStrength:
                {
                HTI_LOG_TEXT( "ESignalStrength" );
                HandleSignalStrengthL( aMessage );
                }
                break;
            case EUpdateMediaGallery:
                {
                HTI_LOG_TEXT( "EUpdateMediaGallery" );
                HandleUpdateMediaGalleryL( aMessage );
                }
                break;
            case EActivateSkin:
                {
                HTI_LOG_TEXT( "EActivateSkin" );
                HandleActivateSkinL( aMessage );
                }
                break;
            default:
                {
                iDispatcher->DispatchOutgoingErrorMessage(
                    KErrArgument,
                    KErrDescrNotSupported,
                    KSysInfoServiceUid );
                }
            }
        }

    else // aMessage.Length() > 0
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        }

    if ( iReply )
        {
        TInt err = iDispatcher->DispatchOutgoingMessage( iReply,
                                                         KSysInfoServiceUid );
        if ( err == KErrNoMemory )
            {
            HTI_LOG_TEXT( "KErrNoMemory" );
            iDispatcher->AddMemoryObserver( this );
            }
        else
            {
            iReply = NULL;
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::ProcessMessage" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::NotifyMemoryChange
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::NotifyMemoryChange( TInt aAvailableMemory )
    {

    if ( iReply )
        {
        if ( aAvailableMemory >= iReply->Size() )
            {
            TInt err = iDispatcher->DispatchOutgoingMessage(
                iReply, KSysInfoServiceUid );

            if ( err == KErrNone )
                {
                iReply = NULL;
                iDispatcher->RemoveMemoryObserver( this );
                }
            else if ( err != KErrNoMemory ) //some other error
                {
                delete iReply;
                iReply = NULL;
                iDispatcher->RemoveMemoryObserver( this );
                }
            }
        }
    else
        {
        // some error, should not be called
        iDispatcher->RemoveMemoryObserver( this );
        }
    }

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleAllowSSPropertyChange
//------------------------------------------------------------------------------
TInt CHtiSysInfoServicePlugin::HandleAllowSSPropertyChange( TAny* aPtr )
    {
    HTI_LOG_TEXT( "Allow SS property was changed" );
    TInt newValue = -1;
    TInt wantedValue =
        STATIC_CAST( CHtiSysInfoServicePlugin*, aPtr )->iAllowSSValue;
    RProperty::Get( KPSUidScreenSaver,
                    KScreenSaverAllowScreenSaver, newValue );
    HTI_LOG_FORMAT( "New value is %d", newValue );

    TInt err = KErrNone;
    if ( newValue == 0 && wantedValue == 1 )
        {
        HTI_LOG_TEXT( "Restoring the SS disabled value" );
        err = RProperty::Set( KPSUidScreenSaver,
                              KScreenSaverAllowScreenSaver, wantedValue );
        }
    return err;
    }

#endif
/*
 * Private helper methods
 */

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetHalAttrL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetHalAttrL( const TDesC8& aMessage )
    {
    // check the message length
    if ( aMessage.Length() != 5 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    TInt att =  aMessage[1] +
              ( aMessage[2] << 8  ) +
              ( aMessage[3] << 16 ) +
              ( aMessage[4] << 24 );

    // check that requested HAL attribute is valid
    if ( att < 0 || att >= HALData::ENumHalAttributes )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrAttOutOfRange,
            KSysInfoServiceUid);
        return;
        }

    // get the HAL attribute
    TInt result;
    TInt err = HAL::Get( ( HALData::TAttribute ) att, result );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrHAL,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( 4 );
        iReply->Des().Append( ( TUint8* )( &result ), 4 );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetImeiL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetImeiL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleGetImeiL" );

    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

#if !defined (__WINS__) // no IMEI in emulator
    RTelServer server;
    User::LeaveIfError( server.Connect() );
    CleanupClosePushL( server );
    User::LeaveIfError( server.LoadPhoneModule( KMmTsyModuleName ) );

    RTelServer::TPhoneInfo info;
    TInt ret = KErrNotSupported;
    TInt count;

    RMobilePhone mobilePhone;

    User::LeaveIfError( server.EnumeratePhones( count ) );

    for ( TInt i = 0; i < count; i++ )
        {
        ret = server.GetPhoneInfo( i, info );
        if ( ret == KErrNone )
            {
            User::LeaveIfError( mobilePhone.Open( server, info.iName ) );
            CleanupClosePushL( mobilePhone );
            break;
            }
        }

    TRequestStatus status;
    RMobilePhone::TMobilePhoneIdentityV1 identity;

    mobilePhone.GetPhoneId( status, identity );
    User::WaitForRequest( status );

    CleanupStack::PopAndDestroy(); // mobilePhone

    server.UnloadPhoneModule( KMmTsyModuleName );
    CleanupStack::PopAndDestroy(); // server

    iReply = HBufC8::NewL( identity.iSerialNumber.Length() );
    iReply->Des().Copy( identity.iSerialNumber );

#else // __WINS__
    // no IMEI in emulator
    iDispatcher->DispatchOutgoingErrorMessage( KErrNotSupported,
        KErrDescrNotSupported, KSysInfoServiceUid );
#endif // __WINS__
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleGetImeiL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetSwVersionL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetSwVersionL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    TBuf<KSysUtilVersionTextLength> reply16;
    TInt err = SysUtil::GetSWVersion( reply16 );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrSysUtil,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( reply16.Size() );
        iReply->Des().Append( ( TUint8* )reply16.Ptr(), reply16.Size() );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetLangVersionL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetLangVersionL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    TBuf<KSysUtilVersionTextLength> reply16;
    TInt err = SysUtil::GetLangVersion( reply16 );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrSysUtil,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( reply16.Size() );
        iReply->Des().Append( ( TUint8* )reply16.Ptr(), reply16.Size() );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetSwLangVersionL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetSwLangVersionL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    TBuf<KSysUtilVersionTextLength> reply16;
    TInt err = SysUtil::GetLangSWVersion( reply16 );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrSysUtil,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( reply16.Size() );
        iReply->Des().Append( ( TUint8* )reply16.Ptr(), reply16.Size() );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetUserAgentStringL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetUserAgentStringL(
        const TDesC8& aMessage )
    {
    aMessage.Length(); // get rid of compiler warning
    iDispatcher->DispatchOutgoingErrorMessage(
        KErrNotSupported,
        KErrDescrNotSupported,
        KSysInfoServiceUid);
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetFreeRamL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetFreeRamL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    User::CompressAllHeaps();
    TInt result;
    TInt err = HAL::Get( HALData::EMemoryRAMFree, result );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrFreeRAM,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( 4 );
        iReply->Des().Append( ( TUint8* )( &result ), 4 );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetUsedRamL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetUsedRamL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    User::CompressAllHeaps();

    TInt totalRam;
    TInt freeRam;
    TInt usedRam;

    // first get the total RAM...
    TInt err = HAL::Get( HALData::EMemoryRAM, totalRam );
    if ( err != KErrNone )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrTotalRAM,
            KSysInfoServiceUid );
        return;
        }

    // ...then get the free RAM
    err = HAL::Get( HALData::EMemoryRAMFree, freeRam );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrFreeRAM,
            KSysInfoServiceUid );
        return;
        }

    // calculate used RAM from total and free RAM
    usedRam = totalRam - freeRam;
    iReply = HBufC8::NewL( 4 );
    iReply->Des().Append( ( TUint8* )( &usedRam ), 4 );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetTotalRamL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetTotalRamL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    User::CompressAllHeaps();

    TInt result;
    TInt err = HAL::Get( HALData::EMemoryRAM, result );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrTotalRAM,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( 4 );
        iReply->Des().Append( ( TUint8* )( &result ), 4 );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleEatRamL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleEatRamL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 5 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    // get the amount of memory to be left free from the message
    TInt memLeftFree =  aMessage[1] +
                      ( aMessage[2] << 8  ) +
                      ( aMessage[3] << 16 ) +
                      ( aMessage[4] << 24 );

    // if there's a previous memory eater, delete it
    if ( iMemEater != NULL )
        {
        delete iMemEater;
        iMemEater = NULL;
        }

    User::CompressAllHeaps();

    // get the current free memory
    TInt memFree;
    TInt err = HAL::Get( HALData::EMemoryRAMFree, memFree );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrFreeRAM,
            KSysInfoServiceUid );
        return;
        }

    // try to eat the memory
    TInt memToBeEaten = memFree - memLeftFree;

    if ( memToBeEaten < 0 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
                KErrUnderflow,
                KErrDescrInvalidRAM,
                KSysInfoServiceUid );
        return;
        }

    TRAP( err, iMemEater = HBufC8::NewL( memToBeEaten ) );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrAllocRAM,
            KSysInfoServiceUid );
        return;
        }

    // get the amount of memory left
    err = HAL::Get( HALData::EMemoryRAMFree, memFree );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrFreeRAM,
            KSysInfoServiceUid );
        return;
        }

    // send the amount of memory back
    iReply = HBufC8::NewL( 4 );
    iReply->Des().Append( ( TUint8* )( &memFree ), 4 );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleReleaseRamL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleReleaseRamL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    // if there's a memory eater, delete it
    if ( iMemEater != NULL )
        {
        delete iMemEater;
        iMemEater = NULL;
        }

    User::CompressAllHeaps();

    // query the amount of memory and send it back
    TInt memFree;
    TInt err = HAL::Get( HALData::EMemoryRAMFree, memFree );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrHAL,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( 4 );
        iReply->Des().Append( ( TUint8* )( &memFree ), 4 );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetFreeDiskSpaceL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetFreeDiskSpaceL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    TInt drive;
    RFs::CharToDrive( TChar( aMessage[1] ), drive );
    TVolumeInfo volInfo;
    TInt err = iFs.Volume( volInfo, drive );

    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrVolInfo,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( 8 );
        iReply->Des().Append( ( TUint8* )( &volInfo.iFree ), 8 );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetUsedDiskSpaceL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetUsedDiskSpaceL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    TInt drive;
    RFs::CharToDrive( TChar( aMessage[1] ), drive );
    TVolumeInfo volInfo;
    TInt err = iFs.Volume( volInfo, drive );

    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrVolInfo,
            KSysInfoServiceUid );
        }
    else
        {
        TInt64 used = volInfo.iSize - volInfo.iFree;
        iReply = HBufC8::NewL( 8 );
        iReply->Des().Append( ( TUint8* )( &used ), 8 );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetTotalDiskSpaceL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetTotalDiskSpaceL(
        const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    TInt drive;
    RFs::CharToDrive( TChar( aMessage[1] ), drive );
    TVolumeInfo volInfo;
    TInt err = iFs.Volume( volInfo, drive );

    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrVolInfo,
            KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( 8 );
        iReply->Des().Append( ( TUint8* )( &volInfo.iSize ), 8 );
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleEatDiskSpaceL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleEatDiskSpaceL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleEatDiskSpaceL" );
    
    if ( aMessage.Length() != 10 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleEatDiskSpaceL" );
        return;
        }

    TFileName commonpath;
    commonpath.Append( aMessage[1] );
    commonpath.Append( _L( ":" ) );
    commonpath.Append( KTempFilePath );
    commonpath.Append( KTempFileName );
    TFileName path;
        
    // get free disk space
    TInt drive;
    RFs::CharToDrive( TChar( aMessage[1] ), drive );
    TVolumeInfo volInfo;
    TInt err = iFs.Volume( volInfo, drive );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrVolInfo,
            KSysInfoServiceUid );
        HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleEatDiskSpaceL" );
        return;
        }

    // calculate how much we must eat the disk space
    TInt64 temp1 = aMessage[2] +
                 ( aMessage[3] << 8  ) +
                 ( aMessage[4] << 16 ) +
                 ( aMessage[5] << 24 );
    TInt64 temp2 = aMessage[6] +
                 ( aMessage[7] << 8  ) +
                 ( aMessage[8] << 16 ) +
                 ( aMessage[9] << 24 );

    TInt64 spaceLeftFree = temp1 + ( temp2 << 32) ;
    TInt64 spaceToEat = volInfo.iFree - spaceLeftFree;

    HTI_LOG_FORMAT( "Disk space to eat: %Ld", spaceToEat );

    // check that there is enough free disk space
    if ( spaceToEat < 0 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrDiskFull,
            KErrDescrNotEnoughSpace,
            KSysInfoServiceUid );
        HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleEatDiskSpaceL" );
        return;
        }

    // check if scaceToEat is greater than KMaxTInt
    //  --> it must be eaten in several chunks
    //  --> not yet supported.

    TInt64 size;
    for(TInt i=1;  spaceToEat>0; i++)
        {
        path.Zero();
        path.Copy(commonpath);
        path.AppendNum(i);
        if ( BaflUtils::FileExists( iFs, path ) )
            {
            continue;
            }
        
        if(spaceToEat > KMaxTInt)  
            size=KMaxTInt;
        else
            size=spaceToEat;
        
        err = CreatFileToEatDiskSpace(path, size);
        if(err)
            { 
            HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleEatDiskSpaceL CreateFile Fail" );
            return;  
            }        
        
        iFs.Volume( volInfo, drive );
        HTI_LOG_FORMAT( "current free space: %Ld", volInfo.iFree );
        spaceToEat = volInfo.iFree - spaceLeftFree;           
        }

    // all ok, send the remaining disk size back
    iReply = HBufC8::NewL( 8 );
    iReply->Des().Append( ( TUint8* )( &volInfo.iFree ), 8 );
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleEatDiskSpaceL" );
    }

TInt CHtiSysInfoServicePlugin::CreatFileToEatDiskSpace( TFileName aPath, TInt64 aSpaceToEat  )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::CreatFileToEatDiskSpace" );
    
    HTI_LOG_FORMAT( "Create file: %S", &aPath );
    HTI_LOG_FORMAT( "file size %Ld", aSpaceToEat );
    
    // create a temp file
    RFile diskEater;
    TInt err = diskEater.Replace( iFs, aPath, EFileWrite );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrCreateTempFile,
            KSysInfoServiceUid );
        HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::CreatFileToEatDiskSpace Replace error" );
        return err;
        }
    
    // set the size for temp file
    err = diskEater.SetSize( I64LOW( aSpaceToEat ) );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrSetSizeTempFile,
            KSysInfoServiceUid );
        diskEater.Close();
        HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::CreatFileToEatDiskSpace SetSize error" );
        return err;
        }
    diskEater.Close();
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::CreatFileToEatDiskSpace" );
    return 0;
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleReleaseDiskSpaceL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleReleaseDiskSpaceL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleReleaseDiskSpaceL" );
    if ( aMessage.Length() != 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleReleaseDiskSpaceL" );
        return;
        }

    TFileName path;
    path.Append( aMessage[1] );
    path.Append( _L( ":" ) );
    path.Append(KTempFilePath);
    path.Append(KMatchFileName);
    TInt err = iFileMan->Delete(path);
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
                err,
                KErrDescrDeleteTempFile,
                KSysInfoServiceUid );
        HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleReleaseDiskSpaceL" );
        return;
        } 
    
    // query the free disk space
    TInt drive;
    RFs::CharToDrive( TChar( aMessage[1] ), drive );
    TVolumeInfo volInfo;
    err = iFs.Volume( volInfo, drive );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrVolInfo,
            KSysInfoServiceUid );
        HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleReleaseDiskSpaceL" );
        return;
        }

    // all ok, send the free disk space back
    iReply = HBufC8::NewL( 8 );
    iReply->Des().Append( ( TUint8* )( &volInfo.iFree ), 8 );

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleReleaseDiskSpaceL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleSetHomeTimeL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleSetHomeTimeL( const TDesC8& aMessage )
    {
    TTime time;
    TRAPD( err, ParseTimeDataL( aMessage.Mid( 1 ), time ) );
    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage( err,
                               KErrDescrArgument,
                               KSysInfoServiceUid);
        return;
        }

    // User::SetHomeTime() does not work correctly with daylight saving time
    // in S60 3.0 - have to use time zone server instead.
    RTz tzServer;
    err = tzServer.Connect();
    if ( err == KErrNone )
        {
        err = tzServer.SetHomeTime( time );
        }
    tzServer.Close();

    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage( err,
                               KErrDescrSetTime,
                               KSysInfoServiceUid);
        return;
        }

    iReply = HBufC8::NewL( 1 );
    iReply->Des().Append( 0 );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleGetHomeTimeL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleGetHomeTimeL( const TDesC8& aMessage )
    {
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );

        return;
        }

    TTime time;
    time.HomeTime();
    TDateTime dateTime = time.DateTime();
    TUint year = dateTime.Year();
    iReply = HBufC8::NewL( KTimeDataLength );
    iReply->Des().Append( (TUint8*)(&year), 2 );
    iReply->Des().Append( dateTime.Month() + 1 );
    iReply->Des().Append( dateTime.Day() + 1 );
    iReply->Des().Append( dateTime.Hour() );
    iReply->Des().Append( dateTime.Minute() );
    iReply->Des().Append( dateTime.Second() );
    }


//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleSetDateTimeFormatL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleSetDateTimeFormatL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleSetDateTimeFormatL" );

    if ( aMessage.Length() != KDateTimeFormatCmdLength )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    // Parse values from message
    TDateFormat dateFormat = STATIC_CAST( TDateFormat, aMessage[1] );
    TChar dateSepar = aMessage[2];
    TTimeFormat timeFormat = STATIC_CAST( TTimeFormat, aMessage[3] );
    TChar timeSepar = aMessage[4];
    TClockFormat clockFormat = STATIC_CAST( TClockFormat, aMessage[5] );

    HTI_LOG_FORMAT( "Date format   : %d", dateFormat );
    HTI_LOG_FORMAT( "Date separator: %c", aMessage[2] );
    HTI_LOG_FORMAT( "Time format   : %d", timeFormat );
    HTI_LOG_FORMAT( "Time separator: %c", aMessage[4] );
    HTI_LOG_FORMAT( "Clock format  : %d", clockFormat );

    // Check validity of values
    if ( dateFormat < EDateAmerican || dateFormat > EDateJapanese ||
         timeFormat < ETime12 || timeFormat > ETime24 ||
         clockFormat < EClockAnalog || clockFormat > EClockDigital ||
         KDateSeparatorChars().Locate( dateSepar ) == KErrNotFound ||
         KTimeSeparatorChars().Locate( timeSepar ) == KErrNotFound )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    // Set the values
    TLocale locale;
    locale.SetDateFormat( dateFormat );
    locale.SetDateSeparator( dateSepar, 1 );
    locale.SetDateSeparator( dateSepar, 2 );
    locale.SetTimeFormat( timeFormat );
    locale.SetTimeSeparator( timeSepar, 1 );
    locale.SetTimeSeparator( timeSepar, 2 );
    locale.SetClockFormat( clockFormat );
    TInt err = locale.Set();

    if ( err != KErrNone )
        {
        iDispatcher->DispatchOutgoingErrorMessage( err,
            KErrDescrDateTimeFormat, KSysInfoServiceUid );
        }
    else
        {
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 0 );
        }

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleSetDateTimeFormatL" );
    }


//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleLightsCommandL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleLightsCommandL( const TDesC8& aMessage )
    {
    if ( aMessage[0] == ELightRelease )
        {
        if ( aMessage.Length() != 1 )
            {
            iDispatcher->DispatchOutgoingErrorMessage(
                KErrArgument,
                KErrDescrArgument,
                KSysInfoServiceUid);
            }

        else
            {
            HTI_LOG_TEXT( "ELightRelease" );
            delete iLightsController;
            iLightsController = NULL;
            iReply = HBufC8::NewL( 1 );
            iReply->Des().Append( 0 );
            }
        }

    else
        {
        if ( !iLightsController )
            {
            HTI_LOG_TEXT( "Creating lights controller" );
            iLightsController = CHtiLightsController::NewL(
                                    iDispatcher );
            }
        TBuf8<4> reply;
        iLightsController->ProcessMessageL( aMessage, reply );
        if ( reply.Length() > 0 )
            {
            iReply = HBufC8::NewL( reply.Length() );
            iReply->Des().Copy( reply );
            }
        }
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleScreenSaverCommandL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleScreenSaverCommandL(
                                    const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN(
            "CHtiSysInfoServicePlugin::HandleScreenSaverCommandL" );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 ) 
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    if ( aMessage[0] == EScreenSaverDisable ) iAllowSSValue = 1;
    else if ( aMessage[0] == EScreenSaverEnable ) iAllowSSValue = 0;
    else User::Leave( KErrArgument );

    HTI_LOG_FORMAT( "Setting allow screen saver state %d", iAllowSSValue );

    TInt err = KErrNone;

    if ( !iAllowSSPropertyAttached )
        {
        HTI_LOG_TEXT( "Attaching to KScreenSaverAllowScreenSaver property" );
        err = iAllowSSProperty.Attach(
                KPSUidScreenSaver, KScreenSaverAllowScreenSaver );

        if ( err )
            {
            iDispatcher->DispatchOutgoingErrorMessage(
                err, KErrDescrScreenSaver, KSysInfoServiceUid );
            return;
            }

        iAllowSSPropertyAttached = ETrue;
        }

    if ( iAllowSSValue == 1 )
        {
        iAllowSSProperty.Set( iAllowSSValue ); // ignore error
        // Screen saver disabled. We want to keep it disabled, so
        // subscribe to the property to get notified about changes in it.
        if ( !iAllowSSSubscriber )
            {
            iAllowSSSubscriber = new (ELeave) CHtiPropertySubscriber(
                TCallBack( HandleAllowSSPropertyChange, this ),
                iAllowSSProperty );
            iAllowSSSubscriber->Subscribe();
            }
        }

    else  // iAllowSSValue == 0
        {
        // Enabling screen saver. Cancel possible subscription so
        // other applications can control the property.
        if ( iAllowSSSubscriber )
            {
            iAllowSSSubscriber->Unsubscribe();
            }
        iAllowSSProperty.Set( iAllowSSValue ); // ignore error
        iAllowSSProperty.Close();
        iAllowSSPropertyAttached = EFalse;
        delete iAllowSSSubscriber;
        iAllowSSSubscriber = NULL;
        }

    iReply = HBufC8::NewL( 1 );
    iReply->Des().Append( 0 );
#else
    iDispatcher->DispatchOutgoingErrorMessage(KErrArgument,
            KErrDescrNotSupported, KSysInfoServiceUid);
#endif 
    HTI_LOG_FUNC_OUT(
                "CHtiSysInfoServicePlugin::HandleScreenSaverCommandL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleScreenSaverTimeoutCommandL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleScreenSaverTimeoutCommandL(
                                    const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN(
            "CHtiSysInfoServicePlugin::HandleScreenSaverTimeoutCommandL" );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    if ( aMessage.Length() != 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid);
        return;
        }

    TInt time = aMessage[1];
    HTI_LOG_FORMAT( "Requested timeout %d", time );
    if ( time < 5 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrInvalidSSTimeout, KSysInfoServiceUid );
        return;
        }

    CRepository* persRep = CRepository::NewL( KCRUidPersonalizationSettings );
    TInt err = persRep->Set( KSettingsScreenSaverPeriod, time );

    if ( err == KErrNone )
        {
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 0 );
        }

    else
        {
        iDispatcher->DispatchOutgoingErrorMessage( err,
            KErrDescrSSTimeoutFailed, KSysInfoServiceUid );
        }

    delete persRep;
#else
    iDispatcher->DispatchOutgoingErrorMessage(KErrArgument,
            KErrDescrNotSupported, KSysInfoServiceUid);
#endif 
    HTI_LOG_FUNC_OUT(
        "CHtiSysInfoServicePlugin::HandleScreenSaverTimeoutCommandL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleNetworkModeCommandL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleNetworkModeCommandL( const TDesC8& aMessage )
{
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleNetworkModeCommandL" );

    TInt err = StartC32();
    if ( ( err != KErrNone ) && ( err != KErrAlreadyExists ) )
        {
        HTI_LOG_FORMAT( "StartC32 failed %d", err );
        User::Leave( err );
        }

    // Connect to telephony server
    RTelServer telServer;
    err = telServer.Connect();
    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "RTelServer::Connect() failed %d", err );
        User::Leave( err );
        }
    CleanupClosePushL( telServer );

    // load phonetsy
    err = telServer.LoadPhoneModule( KMmTsyModuleName );
    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "RTelServer::LoadPhoneModule() failed %d", err );
        User::Leave( err );
        }

    // get phones
    TInt noOfPhones;
    err = telServer.EnumeratePhones( noOfPhones );
    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "RTelServer::EnumeratePhones() failed %d", err );
        User::Leave( err );
        }

    if ( noOfPhones == 0 )
        {
        HTI_LOG_TEXT( "No phones found" );
        User::Leave( KErrNotFound );
        }

    HTI_LOG_FORMAT( "noOfPhones %d", noOfPhones );

    RTelServer::TPhoneInfo phoneInfo;
    for ( TInt i = 0; i < noOfPhones; i++ )
        {
        TName phoneTsy;
        telServer.GetTsyName( i, phoneTsy );
        HTI_LOG_DES( phoneTsy );

        err = telServer.GetPhoneInfo( i, phoneInfo );
        if ( err != KErrNone )
            {
            HTI_LOG_FORMAT( "RTelServer::GetPhoneInfo() %d", i );
            HTI_LOG_FORMAT( "failed %d", err );
            User::Leave( err );
            }
        HTI_LOG_DES( phoneInfo.iName );
        }

    // open phone
    RMobilePhone phone;
    err = phone.Open( telServer, phoneInfo.iName );
    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "RMobilePhone::Open() failed %d", err );
        User::Leave( err );
        }
    CleanupClosePushL( phone );

    err = phone.Initialise();
    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "RMobilePhone::Initialise() failed %d", err );
        User::Leave( err );
        }

    // Open customapi
    RMmCustomAPI customAPI;
    err = customAPI.Open( phone );
    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "RMmCustomAPI::Open() %d", err );
        User::LeaveIfError( err );
        }
    CleanupClosePushL( customAPI );

    switch ( aMessage[0] )
    {
    case ENetworkModeGet:
        {
        HTI_LOG_TEXT( "ENetworkModeGet" );
        TUint32 networkModes = 0;
        err = customAPI.GetCurrentSystemNetworkModes( networkModes );
        if ( err )
            {
            HTI_LOG_FORMAT(
                    "RMmCustomAPI::GetCurrentSystemNetworkModes() failed %d",
                    err );
            User::LeaveIfError(
                iDispatcher->DispatchOutgoingErrorMessage(
                        err,
                        KErrDescrGetNetworkModes,
                        KSysInfoServiceUid ) );
            }
        else
            {
            HTI_LOG_FORMAT( "networkModes 0x%x", networkModes );
            TBuf8<5> okMsg;
            okMsg.Append( ENetworkModeGet );
            okMsg.Append( (TUint8*) &networkModes, 4 );
            iReply = okMsg.AllocL();
            }
        }
        break;

    case ENetworkModeSet:
        HTI_LOG_TEXT( "ENetworkModeSet" );
        if ( aMessage.Length() != 5 )
            {
            HTI_LOG_TEXT( "KErrArgument" );
            User::LeaveIfError(
                iDispatcher->DispatchOutgoingErrorMessage(
                        KErrArgument,
                        KErrDescrArgument,
                        KSysInfoServiceUid ) );
            }
        else
            {
            TUint32 mode = aMessage[1] + ( aMessage[2] << 8 ) +
                           ( aMessage[3] << 16 ) + ( aMessage[4] << 24 );

            HTI_LOG_FORMAT( "SetSystemNetworkMode 0x%x", mode );
            err = customAPI.SetSystemNetworkMode(
                    ( RMmCustomAPI::TNetworkModeCaps ) mode );
            if ( err )
                {
                HTI_LOG_FORMAT(
                        "RMmCustomAPI::SetSystemNetworkMode() failed %d", err );
                iDispatcher->DispatchOutgoingErrorMessage(
                    err,
                    KErrDescrSetNetworkMode,
                    KSysInfoServiceUid );
                }
            else
                {
                iDispatcher->ShutdownAndRebootDeviceL();
                }
            }
        break;

    case ENetworkModeSetNoReboot:
        {
        HTI_LOG_TEXT( "ENetworkModeSetNoReboot" );
        if ( aMessage.Length() != 5 )
            {
            HTI_LOG_TEXT( "KErrArgument" );
            User::LeaveIfError(
                iDispatcher->DispatchOutgoingErrorMessage(
                        KErrArgument,
                        KErrDescrArgument,
                        KSysInfoServiceUid ) );
            }
        else
            {
            TUint32 mode = aMessage[1] + ( aMessage[2] << 8 ) +
                           ( aMessage[3] << 16 ) + ( aMessage[4] << 24 );

            HTI_LOG_FORMAT( "SetSystemNetworkMode 0x%x", mode );
            err = customAPI.SetSystemNetworkMode(
                    ( RMmCustomAPI::TNetworkModeCaps ) mode );
            if ( err )
                {
                HTI_LOG_FORMAT(
                        "RMmCustomAPI::SetSystemNetworkMode() failed %d", err );
                iDispatcher->DispatchOutgoingErrorMessage(
                    err,
                    KErrDescrSetNetworkMode,
                    KSysInfoServiceUid );
                }
            else
                {
                iReply = HBufC8::NewL( 1 );
                iReply->Des().Append( 0 );
                }
            }
        break;
        }

    default:
        break;
    }

    CleanupStack::PopAndDestroy( 3 ); // telServer, phone, customAPI
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleNetworkModeCommandL" );
}

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleIrActivateCommandL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleIrActivateCommandL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleIrActivateCommandL" );

    // Message validation
    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    TInt irStatus = -1;
    TInt err = RProperty::Get( KIrdaPropertyCategory, KIrdaStatus, irStatus );
    if ( err != KErrNone || irStatus < TIrdaStatusCodes::EIrLoaded
                         || irStatus > TIrdaStatusCodes::EIrDisconnected )
        {
        // values from irinternalpskey.h
        err = RProperty::Set( KPSUidIrdaActivation, 1, 1 );
        if ( err != KErrNone )
            {
            iDispatcher->DispatchOutgoingErrorMessage(
                err, KErrDescrIrActivation, KSysInfoServiceUid );
            }
        else
            {
            // Activation OK
            iReply = HBufC8::NewL( 1 );
            iReply->Des().Append( 0 );
            }
        }
    else
        {
        // Already active - just send a message
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 1 );
        }

     HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleIrActivateCommandL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleBtPowerCommandL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleBtPowerCommandL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleBtPowerCommandL" );

    // Message validation
    if ( aMessage.Length() != 3 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    TInt err = KErrNone;
    TBool setBtOn = aMessage[1];
    TBool useForce = aMessage[2];
    TBool isBtOn = EFalse;

    TBTPowerStateValue powerState = EBTPowerOff;
    CBTEngSettings* btSettings = CBTEngSettings::NewLC();
    err = btSettings->GetPowerState( powerState );
    if ( err == KErrNone && powerState == EBTPowerOn )
        {
        isBtOn = ETrue;
        }

    if ( err )
        {
        HTI_LOG_FORMAT( "GetPowerState error %d", err );
        CleanupStack::PopAndDestroy(); // btSettings
        iDispatcher->DispatchOutgoingErrorMessage( err, KErrDescrGetBtPower,
                                                   KSysInfoServiceUid );
        return;
        }
    HTI_LOG_FORMAT( "Current BT power state %d", isBtOn );
    HTI_LOG_FORMAT( "Requested BT power state %d", setBtOn );

    if ( setBtOn == isBtOn )
        {
        // Already in requested state - just send message
        CleanupStack::PopAndDestroy(); // btSettings
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 1 );
        }

    else
        {
        if ( setBtOn && !CanTurnBluetoothOnL( useForce ) )
            {
            iDispatcher->DispatchOutgoingErrorMessage( KErrAccessDenied,
                KErrDescrBtOnDenied, KSysInfoServiceUid );
            CleanupStack::PopAndDestroy(); // btSettings
            return;
            }


        if ( !setBtOn )
            {
            // If we are setting BT off, do checks for active connections.
            TInt connCount = 0;
            // Ignore error.
            // If we cannot query, we act like there's no active connections.
            RProperty::Get( KPropertyUidBluetoothCategory,
                            KPropertyKeyBluetoothGetPHYCount,
                            connCount );
            // Check if there's Bluetooth audio accessory connected
            TBool isBtAacConnected = EFalse;

            // If there are connections, force flag is required in the
            // command to turn BT off.
            if ( ( connCount || isBtAacConnected ) && !useForce )
                {
                iDispatcher->DispatchOutgoingErrorMessage( KErrInUse,
                    KErrDescrBtOffDenied, KSysInfoServiceUid );
                CleanupStack::PopAndDestroy(); // btMcm/btSettings
                return;
                }
            }

        if ( setBtOn )
            {
            err = btSettings->SetPowerState( EBTPowerOn );
            }
        else
            {
            err = btSettings->SetPowerState( EBTPowerOff );
            }

        if ( err != KErrNone )
            {
            HTI_LOG_FORMAT( "CBTMCMSettings::SetPowerState error %d", err );
            iDispatcher->DispatchOutgoingErrorMessage( err, KErrDescrSetBtPower,
                                                       KSysInfoServiceUid );
            }
        else
            {
            iReply = HBufC8::NewL( 1 );
            iReply->Des().Append( 0 );
            }
        CleanupStack::PopAndDestroy(); // btSettings
        }
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleBtPowerCommandL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleBtSettingsCommandL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleBtSettingsCommandL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleBtSettingsCommandL" );

    // Message validation
    if ( aMessage.Length() < 4 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
    }
    TInt btNameLength = aMessage[3];
    if ( btNameLength > KMaxBtNameLength ||
         aMessage.Length() != ( btNameLength + 4 ) )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    TBTVisibilityMode visibilityMode = EBTVisibilityModeGeneral;
    if ( aMessage[1] == 0 )
        {
        visibilityMode = EBTVisibilityModeHidden;
        }
    HTI_LOG_FORMAT( "Visibility mode = %d", visibilityMode );

    TInt sapMode = 1;  // EBTSapEnabled
    if ( aMessage[2] == 0 )
        {
        sapMode = 0;   // EBTSapDisabled
        }
    HTI_LOG_FORMAT( "SAP mode = %d", sapMode );

    TBuf<KMaxBtNameLength> btName;
    if ( btNameLength > 0 )
        {
        btName.Copy( aMessage.Mid( 4, btNameLength ) );
        }
    HTI_LOG_FORMAT( "BT name = %S", &btName );

    TInt err = KErrNone;
    CBTEngSettings* btSettings = CBTEngSettings::NewLC();
    HTI_LOG_TEXT( "CBTEngSettings::NewLC done" );

    HTI_LOG_TEXT( "Setting visibility mode" );
    err = btSettings->SetVisibilityMode( visibilityMode );
    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "Setting SAP mode" );
        // CenRep UID and key value from btengprivatecrkeys.h
        // const TUid KCRUidBTEngPrivateSettings = { 0x10204DAC }
        // const TUint32 KBTSapEnabled  = 0x00000003
        CRepository* btEngRep = CRepository::NewL( TUid::Uid( 0x10204DAC ) );
        err = btEngRep->Set( 0x00000003, sapMode );
        delete btEngRep;
        btEngRep = NULL;
        }
    if ( err == KErrNone && btName.Length() > 0 )
        {
        HTI_LOG_TEXT( "Setting BT name" );
        err = btSettings->SetLocalName( btName );
        }

    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "All set successfully" );
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 0 );
        }
    else
        {
        HTI_LOG_FORMAT( "Error %d", err );
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrBtSettings,
            KSysInfoServiceUid );
        }

    CleanupStack::PopAndDestroy(); // btSettings
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleBtSettingsCommandL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleBtDeletePairingsL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleBtDeletePairingsL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleBtDeletePairingsL" );

    // Message validation
    if ( aMessage.Length() < 3 )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    TInt btNameLength = aMessage[2];
    if ( btNameLength > KMaxBluetoothNameLen ||
         aMessage.Length() != ( btNameLength + 3 ) )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument,
            KErrDescrArgument,
            KSysInfoServiceUid );
        return;
        }

    // Message parsing
    TBool closeConnections = aMessage[1];
    HTI_LOG_FORMAT( "Close connections = %d", closeConnections );
    TBTDeviceName8 btName8;
    if ( btNameLength > 0 )
        {
        btName8.Copy( aMessage.Mid( 3, btNameLength ) );
        }
    HTI_LOG_FORMAT( "BT name = %S",
        &( BTDeviceNameConverter::ToUnicodeL( btName8 ) ) );

    // Action
    TInt deleteCount = 0;
    TInt err = KErrNone;
    RBTRegServ regServ;
    RBTRegistry registry;
    User::LeaveIfError( regServ.Connect() );
    CleanupClosePushL( regServ );
    User::LeaveIfError( registry.Open( regServ ) );
    CleanupClosePushL( registry );
    TBTRegistrySearch searchPattern;
    searchPattern.FindBonded();

    TRequestStatus status;
    registry.CreateView( searchPattern, status );
    User::WaitForRequest( status );
    err = status.Int();
    HTI_LOG_FORMAT( "RBTRegistry::CreateView returned %d", err );

    if ( err > 0 )
        {
        CBTRegistryResponse* response = CBTRegistryResponse::NewL( registry );
        CleanupStack::PushL( response );
        HTI_LOG_TEXT( "Creating AsyncWaiter" );
        CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
        HTI_LOG_TEXT( "Calling response->Start()" );
        response->Start( waiter->iStatus );
        HTI_LOG_TEXT( "Calling waiter->StartAndWait()" );
        waiter->StartAndWait();
        err = waiter->Result();
        CleanupStack::PopAndDestroy( waiter );

        if ( err == KErrNone )
            {
            RBTDeviceArray results = response->Results();
            TInt count = results.Count();
            for ( TInt i = 0; i < count; i++ )
                {
                HTI_LOG_FORMAT( "Device %d", i + 1 );
                CBTDevice* device = results[i];
                if ( btNameLength == 0 ||
                     device->DeviceName().Match( btName8 ) != KErrNotFound )
                    {
                    HTI_LOG_TEXT( "Name qualifies for deletion" );
                    registry.UnpairDevice( device->BDAddr(), status );
                    User::WaitForRequest( status );
                    err = status.Int();
                    if ( err == KErrNone )
                        {
                        deleteCount++; // one deletion successfully done
                        }
                    }
                if ( err != KErrNone )
                    {
                    // Break out if any failure occurs - the command has not
                    // been able to do what it is expected to do.
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( response );
        }

    CleanupStack::PopAndDestroy( &registry );
    CleanupStack::PopAndDestroy( &regServ );

    // Create OK response or send error
    if ( err == KErrNone )
        {
        HTI_LOG_FORMAT( "%d pairings deleted successfully", deleteCount );
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( deleteCount );
        }
    else
        {
        HTI_LOG_FORMAT( "Error %d", err );
        iDispatcher->DispatchOutgoingErrorMessage(
            err,
            KErrDescrBtDeletePairings,
            KSysInfoServiceUid );
        }

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleBtDeletePairingsL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleKeyLockToggleL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleKeyLockToggleL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleKeyLockToggleL" );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 ) 
    if ( aMessage.Length() != 3 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    RAknKeyLock keyLock;
    User::LeaveIfError( keyLock.Connect() );
    HTI_LOG_TEXT( "RAknKeyLock connect OK" );

    TBool isKeyLockOn = keyLock.IsKeyLockEnabled();
    HTI_LOG_FORMAT( "Keylock status = %d", isKeyLockOn );

    TBool requested = aMessage[1];
    HTI_LOG_FORMAT( "Requested status = %d", requested );

    if ( requested == isKeyLockOn )
        {
        // Already in requested state - just send message
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 1 );
        }

    else
        {
        TBool showNote = aMessage[2];
        HTI_LOG_FORMAT( "Note request = %d", showNote );
        if ( requested )
            {
            if ( showNote )
                {
                keyLock.EnableKeyLock();
                }
            else
                {
                keyLock.EnableWithoutNote();
                }
            }
        else
            {
            if ( showNote )
                {
                keyLock.DisableKeyLock();
                }
            else
                {
                keyLock.DisableWithoutNote();
                }
            }
        User::After( 500000 );
        isKeyLockOn = keyLock.IsKeyLockEnabled();
        HTI_LOG_FORMAT( "New keylock status = %d", isKeyLockOn );
        if ( isKeyLockOn == requested )
            {
            iReply = HBufC8::NewL( 1 );
            iReply->Des().Append( 0 );
            }
        else
            {
            iDispatcher->DispatchOutgoingErrorMessage( KErrGeneral,
                KErrDescrKeyLock, KSysInfoServiceUid );
            }
        }

    keyLock.Close();
#else
    iDispatcher->DispatchOutgoingErrorMessage(KErrArgument,
            KErrDescrNotSupported, KSysInfoServiceUid);
#endif    
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleKeyLockToggleL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleAutoKeyGuardTimeL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleAutoKeyGuardTimeL( const TDesC8& aMessage )
    {

    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleAutoKeyGuardTimeL" );

    if ( aMessage.Length() != 3 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    TInt time = aMessage[1] + ( aMessage[2] << 8 );
    HTI_LOG_FORMAT( "Requested auto key guard time %d", time );
    if ( time > 3600 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrInvalidTime, KSysInfoServiceUid );
        return;
        }

    CRepository* secRep = CRepository::NewL( KCRUidSecuritySettings );
    TInt err = secRep->Set( KSettingsAutomaticKeyguardTime, time );

    if ( err == KErrNone )
        {
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 0 );
        }

    else
        {
        iDispatcher->DispatchOutgoingErrorMessage( err,
            KErrDescrAutoKeyGuardFailed, KSysInfoServiceUid );
        }

    delete secRep;

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleAutoKeyGuardTimeL" );
    }


//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleEmptyDrmRightsDbL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleEmptyDrmRightsDbL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleEmptyDrmRightsDbL" );

    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    RDRMRightsClient rightsClient;
    TInt err = rightsClient.Connect();

    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "RDRMRightsClient connect failed %d", err );
        iDispatcher->DispatchOutgoingErrorMessage( err,
            KErrDescrDrmDbConnect, KSysInfoServiceUid );
        }

    else
        {
        HTI_LOG_TEXT( "RDRMRightsClient connect OK, clearing DB..." );
        err = rightsClient.DeleteAll();
        if ( err == KErrNone )
            {
            HTI_LOG_TEXT( "DB cleared OK" );
            iReply = HBufC8::NewL( 1 );
            iReply->Des().Append( 0 );
            }
        else
            {
            HTI_LOG_FORMAT( "DB clear failed %d", err );
            iDispatcher->DispatchOutgoingErrorMessage( err,
                KErrDescrDrmDbDelete, KSysInfoServiceUid );
            }
        rightsClient.Close();
        }

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleEmptyDrmRightsDbL" );
    }


//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleBatteryStatusL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleBatteryStatusL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleBatteryStatusL" );

    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    TInt err = KErrNone;
    TInt batteryLevel = EBatteryLevelUnknown;
    TInt chargingStatus = EChargingStatusError;

    err = RProperty::Get( KPSUidHWRMPowerState,
                          KHWRMBatteryLevel, batteryLevel );
    HTI_LOG_FORMAT( "Battery level = %d", batteryLevel );
    if ( err != KErrNone || batteryLevel == EBatteryLevelUnknown )
        {
        if ( err == KErrNone ) err = KErrGeneral;
        iDispatcher->DispatchOutgoingErrorMessage( err,
            KErrDescrBatteryLevel, KSysInfoServiceUid );
        return;
        }

    err = RProperty::Get( KPSUidHWRMPowerState,
                          KHWRMChargingStatus, chargingStatus );
    HTI_LOG_FORMAT( "Charging status = %d", chargingStatus );
    if ( err != KErrNone || chargingStatus == EChargingStatusError )
        {
        if ( err == KErrNone ) err = KErrGeneral;
        iDispatcher->DispatchOutgoingErrorMessage( err,
            KErrDescrChargingStatus, KSysInfoServiceUid );
        return;
        }

    iReply = HBufC8::NewL( 2 );
    iReply->Des().Append( batteryLevel );
    iReply->Des().Append( chargingStatus );

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleBatteryStatusL" );
    }


//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleSignalStrengthL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleSignalStrengthL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleSignalStrengthL" );

    if ( aMessage.Length() != 1 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

#if defined(__WINS__)
    iDispatcher->DispatchOutgoingErrorMessage( KErrNotSupported,
        KErrDescrNotSupported, KSysInfoServiceUid );
#else
    TInt err = KErrNone;
    TInt popCount = 0;
    RTelServer server;
    err = server.Connect();

    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "Connected to RTelServer" );
        CleanupClosePushL( server );
        popCount++;
        err = server.LoadPhoneModule( KMmTsyModuleName );
        if ( err == KErrAlreadyExists ) err = KErrNone; // ok if already loaded
        }

    RMobilePhone mobilePhone;

    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "Phone module loaded" );
        err = mobilePhone.Open( server, KMmTsyPhoneName );
        }

    TInt8  signalBars;
    TInt32 signalStrength;

    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "RMobilePhone open" );
        CleanupClosePushL( mobilePhone );
        popCount++;
        TRequestStatus status;
        mobilePhone.GetSignalStrength( status, signalStrength, signalBars );
        User::WaitForRequest( status );
        HTI_LOG_FORMAT( "GetSignalStrength return value %d", status.Int() );
        err = status.Int();
        }

    if ( err == KErrNone )
        {
        HTI_LOG_FORMAT( "Signal bars = %d", signalBars );
        HTI_LOG_FORMAT( "Signal strength = %d", signalStrength );
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( signalBars );
        }

    else
        {
        iDispatcher->DispatchOutgoingErrorMessage( err,
            KErrDescrSignalStrength, KSysInfoServiceUid );
        }

    if ( popCount > 0 )
        {
        CleanupStack::PopAndDestroy( popCount );
        }
#endif // __WINS__
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleSignalStrengthL" );
    }


//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleHsdpaCommandL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleHsdpaCommandL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleHsdpaCommandL" );

    if ( aMessage.Length() != 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }
    TBool enableHsdpa = aMessage[1];

    RTelServer telServer;
    RMmCustomAPI customAPI;
    RMobilePhone mobilePhone;
    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL( telServer );
    User::LeaveIfError( mobilePhone.Open( telServer, KMmTsyPhoneName ) );
    CleanupClosePushL( mobilePhone );
    User::LeaveIfError( customAPI.Open( mobilePhone ) );
    CleanupClosePushL( customAPI );

    // Get current HSDPA status
    TBool isHsdpaEnabled = EFalse;
    TRequestStatus status;
    RMmCustomAPI::THSxPAStatus hSxPAStatus;
    customAPI.ReadHSxPAStatus( status, hSxPAStatus );
    User::WaitForRequest( status );
    HTI_LOG_FORMAT( "Reading HSxPA status returned %d", status.Int() );
    User::LeaveIfError( status.Int() );
    if ( hSxPAStatus == RMmCustomAPI::EHSxPAEnabled )
        {
        isHsdpaEnabled = ETrue;
        }

    HTI_LOG_FORMAT( "Current HSDPA status   = %d", isHsdpaEnabled );
    HTI_LOG_FORMAT( "Requested HSDPA status = %d", enableHsdpa );

    if ( isHsdpaEnabled == enableHsdpa )
        {
        // Already in requested state - just send message
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 1 );
        }

    else
        {
        // Try to change status
        if ( enableHsdpa )
            {
            hSxPAStatus = RMmCustomAPI::EHSxPAEnabled;
            }
        else
            {
            hSxPAStatus = RMmCustomAPI::EHSxPADisabled;
            }
        customAPI.WriteHSxPAStatus( status, hSxPAStatus );
        User::WaitForRequest( status );
        HTI_LOG_FORMAT( "Writing HSxPA status returned %d", status.Int() );
        User::LeaveIfError( status.Int() );
        iReply = HBufC8::NewL( 1 );
        iReply->Des().Append( 0 );
        }

    CleanupStack::PopAndDestroy( 3 ); // mobilePhone, customAPI, telServer

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleHsdpaCommandL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleUpdateMediaGalleryL()
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleUpdateMediaGalleryL(
        const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleUpdateMediaGalleryL" );

    if ( !iGalleryUpdateSupported )
        {
        HTI_LOG_TEXT( "Media Gallery update not supported" );
        iDispatcher->DispatchOutgoingErrorMessage( KErrNotSupported,
                KErrDescrNotSupported, KSysInfoServiceUid );
        return;
        }

    if ( aMessage.Length() < 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
            KErrDescrArgument, KSysInfoServiceUid );
        return;
        }
    TInt filePathLength = aMessage[1];
    // Check that given file path length is valid: Index 0 is the
    // command code, index 1 is the path length -> hence the + 2
    if ( aMessage.Length() != filePathLength + 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
                KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    // Try to load the Media File API DLL
    TInt err = KErrNone;
    RLibrary galleryUpdaterDLL;
    err = galleryUpdaterDLL.Load( _L( "MGXMediaFileAPI.dll" ) );
    if ( err == KErrNotFound )  // DLL does not exist
        {
        HTI_LOG_TEXT( "MGXMediaFileAPI.dll file not found" );
        iGalleryUpdateSupported = EFalse;
        iDispatcher->DispatchOutgoingErrorMessage( KErrNotSupported,
                KErrDescrNotSupported, KSysInfoServiceUid );
        return;
        }
    User::LeaveIfError( err ); // Some other error in loading the DLL

    // DLL loaded successfully
    CleanupClosePushL( galleryUpdaterDLL );

    // Construct the CMGXFileManager object from the DLL
    typedef CMGXFileManager* ( *TNewFileManagerFunc )( RFs& aFs );
    TNewFileManagerFunc newFileManFunc =
            ( TNewFileManagerFunc ) galleryUpdaterDLL.Lookup( 1 );
    if ( newFileManFunc == NULL )
        {
        HTI_LOG_TEXT( "Function not found from DLL" );
        iGalleryUpdateSupported = EFalse;
        User::Leave( KErrNotSupported );
        }

    CMGXFileManager* mgManager = NULL;
    TRAP( err, mgManager = newFileManFunc( iFs ) );
    HTI_LOG_FORMAT( "NewFileManagerL returned %d", err );
    User::LeaveIfError( err );
    User::LeaveIfNull( mgManager );
    CleanupStack::PushL( mgManager );

    if ( filePathLength > 0 )
        {
        TBuf<KMaxFileName> path;
        path.Copy( aMessage.Mid( 2 ) );
        HTI_LOG_FORMAT( "Updating file %S to gallery", &path );
        TRAP( err, mgManager->UpdateL( path ) );
        }
    else
        {
        HTI_LOG_TEXT( "Updating all files to gallery" );
        TRAP( err, mgManager->UpdateL() );
        }

    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "Gallery update failed with %d", err );
        iDispatcher->DispatchOutgoingErrorMessage( err, KErrDescrMGUpdate,
                KSysInfoServiceUid );
        }

    CleanupStack::PopAndDestroy( 2 ); // mgManager, galleryUpdaterDLL
    iReply = HBufC8::NewL( 1 );
    iReply->Des().Append( 0 );
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleUpdateMediaGalleryL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleActivateSkinL()
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleActivateSkinL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleActivateSkinL" );

    // Must be at least command code + name length byte
    if ( aMessage.Length() < 2 || aMessage.Length() != aMessage[1] + 2 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
                KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    TFileName skinName;
    if ( aMessage[1] > 0 )
        {
        skinName.Copy( aMessage.Mid( 2 ) );
        }
    HTI_LOG_FORMAT( "Skin name: %S", &skinName );

    // Check if we got full path to skn file
    TParse fileParse;
    fileParse.Set( skinName, NULL, NULL );
    TBool isFullPath = fileParse.DrivePresent() && fileParse.PathPresent() &&
            fileParse.NamePresent() && fileParse.ExtPresent();
    HTI_LOG_FORMAT( "Is full path = %d", isFullPath );

    // Check current skin
    TAknsPkgIDBuf pidBuf;
    TInt currentSkinLocation; // TAknSkinSrvSkinPackageLocation
    CRepository* repository =
            CRepository::NewL( KCRUidPersonalisation );
    CleanupStack::PushL( repository );
    repository->Get( KPslnActiveSkinUid, pidBuf );
    repository->Get( KPslnActiveSkinLocation, currentSkinLocation );
    TAknsPkgID currentSkinPid;
    currentSkinPid.SetFromDesL( pidBuf );
    HTI_LOG_FORMAT( "Current skin pkg ID buf = %S", &pidBuf );
    HTI_LOG_FORMAT( "Current skin location = %d", currentSkinLocation );

    // Connect to the skins server
    RAknsSrvSession skinsSession;
    User::LeaveIfError( skinsSession.Connect() );
    CleanupClosePushL( skinsSession );

    // Resolve the ID for the requested skin
    TAknsPkgID requestedSkinPid = KAknsNullPkgID;
    TInt requestedSkinLocation = EAknsSrvPhone;

    if ( skinName.Length() == 0 )
        {
        // Default skin requested - resolve default skin ID
        // Use KAknsPIDS60DefaultSkin if nothing else found from CenRep
        requestedSkinPid.Set( KAknsPIDS60DefaultSkin );
        TAknsPkgID defaultSkin = KAknsNullPkgID;
        TAknsPkgIDBuf defaultPidBuf;
        TInt ret = repository->Get( KPslnDefaultSkinUID, defaultPidBuf );
        if ( ret != KErrNone || defaultPidBuf.Length() == 0 )
            {
            HTI_LOG_TEXT( "KPslnDefaultSkinUID not found" );
            TInt defaultID = 0;
            ret = repository->Get( KPslnDefaultSkinID, defaultID );
            if ( ret == KErrNone && defaultID != 0 )
                {
                HTI_LOG_FORMAT( "KPslnDefaultSkinID found: %d", defaultID );
                defaultSkin.Set( TUid::Uid( defaultID ) );
                }
            }
        else
            {
            HTI_LOG_FORMAT( "KPslnDefaultSkinUID found: %S", &defaultPidBuf );
            TLex lexer( defaultPidBuf );
            TPtrC pidToken( lexer.NextToken() );
            TUint pid = 0;
            TUint timeStamp = 0;
            // as hex UID is 8 characters
            // as decimal UID is 9 or 10 characters
            if ( pidToken.Length() == 8 )
                {
                ret = TLex( pidToken ).Val( pid, EHex ); // value is in hex
                }
            else
                {
                ret = TLex( pidToken ).Val( pid ); // value is in decimal
                }
            if ( ret == KErrNone )
                {
                TPtrC stampToken( lexer.NextToken() );
                // Timestamp doesn't exist if PID is an UID
                if ( stampToken.Length() )
                    {
                    if ( stampToken.Length() == 8 )
                        {
                        // value is in hex
                        ret = TLex( stampToken ).Val( timeStamp, EHex );
                        }
                    else
                        {
                        // value is decimal
                        ret = TLex( stampToken ).Val( timeStamp );
                        }
                    }
                }
            if ( ret == KErrNone )
                {
                // We have found some valid values.
                // Timestamp is 0 if pid is UID value
                HTI_LOG_FORMAT( "PID %d", pid );
                HTI_LOG_FORMAT( "Timestamp %d", timeStamp );
                defaultSkin.Set( timeStamp, pid );
                }
            }
        // Did we find something from CenRep
        if ( defaultSkin != KAknsNullPkgID )
            {
            requestedSkinPid.Set( defaultSkin );
            }
        }

    else
        {
        // We have skin name - try to find it
        CArrayPtr<CAknsSrvSkinInformationPkg>* skinInfoArray =
                skinsSession.EnumerateSkinPackagesL( EAknsSrvAll );
        HTI_LOG_FORMAT( "Skins found: %d", skinInfoArray->Count() );
        TInt i = 0;
        for ( ; i < skinInfoArray->Count(); i++ )
            {
            if ( isFullPath )
                {
                if ( skinName.CompareF(
                        skinInfoArray->At( i )->FullName() ) == 0 )
                    {
                    requestedSkinPid = skinInfoArray->At( i )->PID();
                    }
                }
            else
                {
                if ( skinName.CompareF( skinInfoArray->At( i )->Name() ) == 0 )
                    {
                    requestedSkinPid = skinInfoArray->At( i )->PID();
                    }
                }
            if ( requestedSkinPid != KAknsNullPkgID )
                {
                // Requested skin was found - check the location
                TUint16 drive = ( skinInfoArray->At( i )->Directory() )[0];
                if (  drive == 'E' || drive == 'e' )
                    {
                    requestedSkinLocation = EAknsSrvMMC;
                    }
                else
                    {
                    requestedSkinLocation = EAknsSrvPhone;
                    }
                break;
                }
            }
        skinInfoArray->ResetAndDestroy(); // not needed anymore
        delete skinInfoArray;
        skinInfoArray = NULL;
        }

    if ( requestedSkinPid != KAknsNullPkgID )
        {
        // Do we need to change skin
        if ( requestedSkinPid != currentSkinPid )
            {
            HTI_LOG_FORMAT( "Activating skin %d", requestedSkinPid.iNumber );
            TInt err = skinsSession.SetAllDefinitionSets( requestedSkinPid );
            HTI_LOG_FORMAT( "Activation returned %d", err );
            if ( err == KErrNone )
                {
                TAknsPkgIDBuf newPidBuf;
                requestedSkinPid.CopyToDes( newPidBuf );
                err = repository->Set( KPslnActiveSkinUid, newPidBuf );
                HTI_LOG_FORMAT( "Set KPslnActiveSkinUid returned %d", err );
                if ( err == KErrNone &&
                        requestedSkinLocation != currentSkinLocation )
                    {
                    err = repository->Set(
                            KPslnActiveSkinLocation, requestedSkinLocation );
                    HTI_LOG_FORMAT( "Set KPslnActiveSkinLocation returned %d",
                            err );
                    }
                if ( err == KErrNone )
                    {
                    // Send OK message
                    iReply = HBufC8::NewL( 1 );
                    iReply->Des().Append( 0 ); // 0 means OK
                    }
                }
            if ( err != KErrNone )
                {
                HTI_LOG_FORMAT( "Skin activation failed with %d", err );
                iDispatcher->DispatchOutgoingErrorMessage( err,
                        KErrDescrActivateSkin, KSysInfoServiceUid );
                }
            }
        else
            {
            // Requested skin already active - just send message
            HTI_LOG_TEXT( "Already active - no need to change" );
            iReply = HBufC8::NewL( 1 );
            iReply->Des().Append( 1 );  // 1 means "already active"
            }
        }

    else
        {
        // Skin was not found
        HTI_LOG_TEXT( "Skin was not found" );
        iDispatcher->DispatchOutgoingErrorMessage( KErrNotFound,
                KErrDescrActivateSkin, KSysInfoServiceUid );
        }

    CleanupStack::PopAndDestroy( 2 ); // skinsSession, repository

    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleActivateSkinL" );
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::HandleSetLanguageL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::HandleSetLanguageL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::HandleSetLanguageL" );
    if ( aMessage.Length() != 3 )
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
                KErrDescrArgument, KSysInfoServiceUid );
        return;
        }
    
    TInt language = aMessage[1] + ( aMessage[2] << 8 );
    if(language < 0)
        {
        iDispatcher->DispatchOutgoingErrorMessage( KErrArgument,
                KErrDescrSetLanguage, KSysInfoServiceUid );
        return;
        }
    HTI_LOG_FORMAT( "Set language to %d", language );
    
    // Never set Language code 0 to HAL
    if (language != 0)
        {
        User::LeaveIfError(HAL::Set(HAL::ELanguageIndex, language));
        }
    
    CRepository* commonEngineRepository = CRepository::NewL(
            KCRUidCommonEngineKeys);
    CleanupStack::PushL(commonEngineRepository);
    
    User::LeaveIfError(commonEngineRepository->Set(KGSDisplayTxtLang, language));

    CleanupStack::PopAndDestroy();

    TBool nbrModeSaved = EFalse;
    if (language == ELangArabic || User::Language() == ELangArabic)
        {
        //numberMode = EGSNumberModeArabicIndic;
        SetDefaultNumberModeL(EGSNbrModeArabic, EGSNbrModeTypeArabic);
        nbrModeSaved = ETrue;
        }
    else if ((language == ELangUrdu || User::Language() == ELangUrdu)
            || (language == ELangFarsi || User::Language() == ELangFarsi))
        {
        //numberMode = EGSNumberModeEasternArabicIndic;
        SetDefaultNumberModeL(EGSNbrModeLatin, EGSNbrModeTypeEasternArabic);
        nbrModeSaved = ETrue;
        }
    else if (language == ELangHindi || User::Language() == ELangHindi
            || language == ELangMarathi || User::Language() == ELangMarathi)
        {
        //numberMode = EGSNumberModeIndic;
        SetDefaultNumberModeL(EGSNbrModeLatin, EGSNbrModeTypeIndic);
        nbrModeSaved = ETrue;
        }

    //if number mode is not set above, then set it to Latin with respective
    //number mode types. This part might be executed when Automatic is
    //selected and the SIM card does not support the language.
    if (!nbrModeSaved)
        {
        TInt nbrModeType = EGSNbrModeTypeIndic;
        if (language == ELangArabic || User::Language() == ELangArabic)
            {
            nbrModeType = EGSNbrModeTypeArabic;
            }
        else if ((language == ELangUrdu || User::Language() == ELangUrdu)
                || (language == ELangFarsi || User::Language() == ELangFarsi))
            {
            nbrModeType = EGSNbrModeTypeEasternArabic;
            }

        //EGSNumberModeLatin is true in both cases;
        SetDefaultNumberModeL(EGSNbrModeLatin, nbrModeType);
        }
    
    // Change input language
    CRepository* aknFepRepository = CRepository::NewL( KCRUidAknFep );
    CleanupStack::PushL(aknFepRepository);
    User::LeaveIfError(  aknFepRepository->Set( KAknFepInputTxtLang,
                                                language ));
    // Change input method for Chinese variants
    if( FeatureManager::FeatureSupported( KFeatureIdChinese ) )
        {
        TBuf<KFepChineseInputModeLength> conversion;
        if( language == ELangPrcChinese )
            {
            conversion.Num( EPinyin, EHex );
            User::LeaveIfError( aknFepRepository->Set( KAknFepChineseInputMode, conversion ) );
            }
        else if( language == ELangHongKongChinese )
            {
            conversion.Num( EStroke, EHex );
            User::LeaveIfError( aknFepRepository->Set( KAknFepChineseInputMode, conversion ) );
            }
        else if( language == ELangTaiwanChinese )
            {
            conversion.Num( EZhuyin, EHex );
            User::LeaveIfError( aknFepRepository->Set( KAknFepChineseInputMode, conversion ) );
            }
        }
    CleanupStack::PopAndDestroy();
    
    iReply = HBufC8::NewL( 1 );
    iReply->Des().Append( 0 );
    HTI_LOG_FUNC_OUT( "CHtiSysInfoServicePlugin::HandleSetLanguageL" );
    }

void CHtiSysInfoServicePlugin::SetDefaultNumberModeL(TInt aMode, TInt aNbrModeType)
    {

    CRepository* localeRepository = CRepository::NewL(KCRUidLocaleSettings);
    CleanupStack::PushL(localeRepository);
    if (aNbrModeType == EGSNbrModeTypeArabic || aNbrModeType
            == EGSNbrModeTypeEasternArabic)
        {
        localeRepository->Set(KSettingsDefaultNumberMode, aMode);
        }
    else
        {
        localeRepository->Set(KSettingsIndicDefaultNumberMode, aMode);
        }
    CleanupStack::PopAndDestroy();

    TLocale locale;
    if (aMode == EGSNbrModeLatin)
        {
        locale.SetDigitType(EDigitTypeWestern);
        }
    else
        {
        //if aMode <> EGSNbrModeLatin, then it should be either latin or arabic. However
        //as EGSNbrModeArabic and EGsNbrModeIndic both have a value = 1, we can't use
        //that constant for below comparison. Hence, need to depend on the 2nd param.
        switch (aNbrModeType)
            {
            case EGSNbrModeTypeArabic:
                locale.SetDigitType(EDigitTypeArabicIndic);
                break;
            case EGSNbrModeTypeIndic:
                locale.SetDigitType(EDigitTypeDevanagari);
                break;
            case EGSNbrModeTypeEasternArabic:
                locale.SetDigitType(EDigitTypeEasternArabicIndic);
                break;
            default:
                break;
            }
        }

    locale.Set();
    }
//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::ParseTimeDataL
//------------------------------------------------------------------------------
void CHtiSysInfoServicePlugin::ParseTimeDataL( const TDesC8& aTimeData,
                                               TTime& aResult )
    {
    /*
      aTimeData =
        bytes 0 - 1 = year
                  2 = month
                  3 = day
                  4 = hour
                  5 = minute
                  6 = second
    */
    if ( aTimeData.Length() != KTimeDataLength )
        {
        User::Leave( KErrBadDescriptor );
        }

    TInt year   = aTimeData[0] + ( aTimeData[1] << 8 );
    TInt month  = aTimeData[2];
    TInt day    = aTimeData[3];
    TInt hour   = aTimeData[4];
    TInt minute = aTimeData[5];
    TInt second = aTimeData[6];

    TDateTime dateTime;
    User::LeaveIfError( dateTime.Set(
        year, TMonth( month - 1 ), day - 1, hour, minute, second, 0 ) );
    aResult = dateTime;
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::CleanUpTempFiles
//------------------------------------------------------------------------------
TInt CHtiSysInfoServicePlugin::CleanUpTempFiles()
    {
    HTI_LOG_FUNC_IN( "CHtiSysInfoServicePlugin::CleanUpTempFiles" );
    TFindFile finder( iFs );
    CDir* dir = NULL;
    TInt err = finder.FindWildByDir(KMatchFileName, KTempFilePath, dir);
    TInt safeDeleteCount = 0;
    while ( err == KErrNone && safeDeleteCount < 20)
        {
        safeDeleteCount++;
        TFileName path;
        path.Copy(finder.File());
        HTI_LOG_FORMAT( "found file: %S", &path );
        TInt ret = iFileMan->Delete(path);
        delete dir;
        dir = NULL;
        if(ret != KErrNone)
            {
            break;
            }
        err = finder.FindWildByDir(KMatchFileName, KTempFilePath, dir);
        }
    HTI_LOG_FUNC_OUT("CHtiSysInfoServicePlugin::CleanUpTempFiles");
    return KErrNone;
    }

//------------------------------------------------------------------------------
// CHtiSysInfoServicePlugin::CanTurnBluetoothOnL
//------------------------------------------------------------------------------
TBool CHtiSysInfoServicePlugin::CanTurnBluetoothOnL( const TBool aUseForce )
    {
    HTI_LOG_FUNC_IN("CHtiSysInfoServicePlugin::CanTurnBluetoothOnL");
    TInt isInNetwork = 0;
    CRepository* repository = CRepository::NewL( KCRUidCoreApplicationUIs );
    repository->Get( KCoreAppUIsNetworkConnectionAllowed, isInNetwork );
    HTI_LOG_FORMAT( "isInNetwork = %d", isInNetwork );
    delete repository;

    if ( isInNetwork )
        {
        return ETrue;
        }

    // Phone is offline - check if it's allowed to turn BT on.

    // If the force flag was not set in command, we won't turn BT on in offline.
    if ( !aUseForce )
        {
        return EFalse;
        }

    // Check if it's possible to turn BT on in offline mode.
    TInt btOfflineEnabled = 0;
    CRepository* repository2 = CRepository::NewL( KCRUidBluetoothEngine );
    repository2->Get( KBTEnabledInOffline, btOfflineEnabled );
    HTI_LOG_FORMAT( "btOfflineEnabled = %d", btOfflineEnabled );
    delete repository2;

    if ( btOfflineEnabled )
        {
        return ETrue;
        }
    HTI_LOG_FUNC_OUT("CHtiSysInfoServicePlugin::CanTurnBluetoothOnL");
    return EFalse;
    }


// ----------------------------------------------------------------------------
CAsyncWaiter* CAsyncWaiter::NewL( TInt aPriority )
    {
    CAsyncWaiter* self = new(ELeave) CAsyncWaiter( aPriority );
    return self;
    }

// ----------------------------------------------------------------------------
CAsyncWaiter* CAsyncWaiter::NewLC( TInt aPriority )
    {
    CAsyncWaiter* self = new ( ELeave ) CAsyncWaiter( aPriority );
    CleanupStack::PushL( self );
    return self;
    }

// ----------------------------------------------------------------------------
CAsyncWaiter::CAsyncWaiter( TInt aPriority ) : CActive( aPriority )
    {
    iWait = new CActiveSchedulerWait();
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
CAsyncWaiter::~CAsyncWaiter()
    {
    Cancel();
    delete iWait;
    }

// ----------------------------------------------------------------------------
void CAsyncWaiter::StartAndWait()
    {
    HTI_LOG_FUNC_IN( "CAsyncWaiter::StartAndWait" );
    iStatus = KRequestPending;
    SetActive();
    iWait->Start();
    HTI_LOG_FUNC_OUT( "CAsyncWaiter::StartAndWait" );
    }

// ----------------------------------------------------------------------------
TInt CAsyncWaiter::Result() const
    {
    return iResult;
    }

// ----------------------------------------------------------------------------
void CAsyncWaiter::RunL()
    {
    HTI_LOG_FUNC_IN( "CAsyncWaiter::RunL" );
    iResult = iStatus.Int();
    iWait->AsyncStop();
    HTI_LOG_FUNC_OUT( "CAsyncWaiter::RunL" );
    }

// ----------------------------------------------------------------------------
void CAsyncWaiter::DoCancel()
    {
    iResult = KErrCancel;
    if ( iStatus == KRequestPending )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrCancel );
        }
    iWait->AsyncStop();
    }


// End of file
