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
* Description:  HtiAppControl implementation
*
*/


// INCLUDE FILES
#include <apacmdln.h>
#include <apgtask.h>
#include <apgwgnam.h>
#include <apmstd.h>

#include <badesca.h>
#include <f32file.h>

#include "HtiAppControl.h"
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
#include <SWInstApi.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistrypackage.h>
#include <widgetregistryclient.h>
#else
#include <usif/sif/sif.h>
#include <usif/scr/scr.h>    //RSoftwareComponentRegistry
#endif
#include <javaregistryincludes.h>

using namespace Java;
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_4 )
_LIT8( KSisxMimeType, "x-epoc/x-sisx-app" );
_LIT8( KSisMimeType, "application/vnd.symbian.install" );
_LIT8( KPipMimeType, "application/x-pip" );
_LIT8( KJadMIMEType, "text/vnd.sun.j2me.app-descriptor" );
_LIT8( KJarMIMEType, "application/java-archive" );
_LIT8( KJavaMIMEType, "application/java");
_LIT8( KJarxMIMEType, "application/x-java-archive");
_LIT8( KWidgetMime, "application/x-nokia-widget");
#endif

// CONSTANTS
const static TUid KAppServiceUid = { 0x1020DEC7 }; //This is Uid of AppServiceOS

//error descriptions
_LIT8( KErrDescrNoMemory, "No memory" );
_LIT8( KErrDescrInvalidCmd, "Invalid command" );
_LIT8( KErrDescrInvalidArguments, "Invalid arguments" );
_LIT8( KErrDescrFailedCreateProcess, "Failed create process" );
_LIT8( KErrDescrNotSupported, "Command not supported" );

_LIT8( KErrDescrFailedStartApp, "Failed start app" );
_LIT8( KErrDescrInvalidUid, "Invalid application uid" );
_LIT8( KErrDescrFailedFindApp, "Failed find app" );

_LIT8( KErrDescrFailedConnectSilentInstaller, "Failed to connect to silent installer" );
_LIT8( KErrDescrFailedInstall, "Failed to install" );
_LIT8( KErrDescrFailedUnInstall, "Failed to uninstall" );
_LIT8( KErrDescrFailedFindPackage, "Failed to find the package" );
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_4 )
_LIT8( KErrDescrBadComponentId, "Bad component id");
#endif
_LIT8( KErrDescrFailedListInstApps, "Failed to list installed apps" );

const static TUint8 KUnicodeMask = 0x01;
const static TInt KTerminateReason = 0;
const static TInt KTUintLength = sizeof(TUint);
const static TInt KCloseTaskDelay = 250000; // microseconds

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================
CHtiAppControl* CHtiAppControl::NewL()
    {
    CHtiAppControl* self = new ( ELeave ) CHtiAppControl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Constructor
CHtiAppControl::CHtiAppControl():iIsAppCmdsSupported( EFalse )
    {
    }

EXPORT_C CHtiAppControl* NewHtiAppControl()
    {
    return CHtiAppControl::NewL();
    }

CHtiAppControl::~CHtiAppControl()
    {
    HTI_LOG_FUNC_IN( "~CHtiAppControl" );

    for ( TInt i = 0; i < iProcessHandleArray.Count(); i++ )
        {
        iProcessHandleArray[i].Close();
        }
    iProcessHandleArray.Close();

    iAppServer.Close();
    iWs.Close();

    if ( iMimeTypes )
        {
        iMimeTypes->Reset();
        delete iMimeTypes;
        }

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    iAugmentations.ResetAndDestroy();
#endif

    HTI_LOG_FUNC_OUT( "~CHtiAppControl" );
    }

// Second phase construction.
void CHtiAppControl::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::ConstructL" );
    iMimeTypes = new (ELeave) CDesC8ArrayFlat( 8 );
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    iMimeTypes->AppendL( SwiUI::KSisxMimeType() );
    iMimeTypes->AppendL( SwiUI::KSisMimeType() );
    iMimeTypes->AppendL( SwiUI::KPipMimeType() );
    iMimeTypes->AppendL( SwiUI::KJadMIMEType() );
    iMimeTypes->AppendL( SwiUI::KJarMIMEType() );
    iMimeTypes->AppendL( SwiUI::KJavaMIMEType() );
    iMimeTypes->AppendL( SwiUI::KJarxMIMEType() );
#else
    iMimeTypes->AppendL( KSisxMimeType() );
    iMimeTypes->AppendL( KSisMimeType() );
    iMimeTypes->AppendL( KPipMimeType() );
    iMimeTypes->AppendL( KJadMIMEType() );
    iMimeTypes->AppendL( KJarMIMEType() );
    iMimeTypes->AppendL( KJavaMIMEType() );
    iMimeTypes->AppendL( KJarxMIMEType() );
#endif
    iMimeTypes->AppendL( KWidgetMime() ); // from widgetregistryconstants.h
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    iAugmentationIndex = 0;
#endif

    TInt err = iAppServer.Connect();
    if ( err == KErrNone )
        {
        err = iWs.Connect();
        }
    if ( err == KErrNone )
        {
        iIsAppCmdsSupported = ETrue;
        }
    else
        {
        iAppServer.Close();
        }
    HTI_LOG_FUNC_OUT( "CHtiAppControl::ConstructL" );
    }

TInt CHtiAppControl::ParseString( const TDesC8& aRequest,
                                        TInt anOffset,
                                        TBool aUnicode,
                                        TDes& aResult )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::ParseString" );
    //validate parameters
    //if offset outside the string return empty string
    if ( anOffset >= aRequest.Size() )
        {
        return anOffset;
        }

    TInt len = aRequest[anOffset];
    HTI_LOG_FORMAT( "length %d", len );

    if ( len > aResult.MaxLength() )
        {
        return KErrBadDescriptor;
        }

    TInt nextOffset = ( aUnicode ? len * 2 : len ) + anOffset + 1;
    HTI_LOG_FORMAT( "nextOffset %d", nextOffset );
    HTI_LOG_FORMAT( "reqSize %d", aRequest.Size() );
    if ( nextOffset > aRequest.Size() )
        {
        return KErrArgument;
        }

    if ( aUnicode )
        {
        const TPtrC8 aFrom( aRequest.Mid( anOffset + 1, len * 2 ) );
        aResult.SetLength( len );
        for ( TInt i = 0; i < len; ++i )
            {
            aResult[i] = ( TUint16 ) aFrom[i << 1] +
                ( ( ( TUint16 ) aFrom[( i << 1 ) + 1] ) << 8 );
            }
        }
    else
        {
        aResult.Copy( aRequest.Mid( anOffset + 1, len ) );
        }

    HTI_LOG_FUNC_OUT( "CHtiAppControl::ParseString" );
    return nextOffset;
    }

void CHtiAppControl::ProcessMessageL( const TDesC8& aMessage,
                THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::ProcessMessage" );
    if ( aMessage.Length() < 1 )
        {
        // no command
        SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
        return;
        }

    if ( aMessage.Length() < 2 &&
         aMessage[0] !=  EListInstalledApps &&
         aMessage[0] !=  EListInstalledApps_u)
        {
         // parameter is required with all commands except
         // listing processes or installed applications
        SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
        return;
        }

    HTI_LOG_FORMAT( "cmd %d", aMessage[0] );

    if ( aMessage[0] > EProcessLastCommand &&
              aMessage[0] < EAppLastCommand)
        {
        if ( iIsAppCmdsSupported )
            {
            HandleAppControlL( aMessage );
            }
        else
            {
            SendErrorMsg( KErrNotSupported, KErrDescrNotSupported );
            }
        }
    else if ( aMessage[0] > EAppLastCommand &&
              aMessage[0] < ESisLastCommand )
        {
        HandleInstallerControlL( aMessage );
        }
    else
        {
        SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
        }
    HTI_LOG_FUNC_OUT( "CHtiAppControl::ProcessMessage" );
    }

void CHtiAppControl::HandleAppControlL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::HandleAppControl" );

    TBool unicode = aMessage[0] & KUnicodeMask;

    HTI_LOG_FORMAT( "unicode %d", unicode );

    TApaAppInfo appInfo;
    TFileName appName;
    TFileName docName;

    switch ( aMessage[0] )
        {
        case EStartApp_uid:
        case EStartApp_uid_u:
            {
            TPtrC8 parameters = aMessage.Mid( 1 );
            if ( parameters.Length() >= 4 )
                {
                TInt32 uid = Parse32<TInt32>( parameters );

                TInt offset = ParseString( parameters, 4, unicode, docName );
                if ( offset >= 0)
                    {
                    if ( FindAppL( appInfo, uid ) )
                        {
                        HandleStartAppL( appInfo, docName );
                        }
                    }
                else
                    {
                    SendErrorMsg( offset , KErrDescrInvalidArguments );
                    }
                }
            else
                {
                SendErrorMsg( KErrArgument , KErrDescrInvalidUid );
                }
            }
            break;
        case EStartApp:
        case EStartApp_u:
            {
            TInt offset = ParseString( aMessage, 1, unicode, appName );
            if ( offset >= 0 )
                {
                offset = ParseString( aMessage, offset, unicode, docName );
                if ( offset >= 0 )
                    {
                    //find appInfo by app full name
                    if ( FindAppL( appInfo, appName ) )
                        {
                        HTI_LOG_TEXT( "call HandleStartAppL" );
                        HandleStartAppL( appInfo, docName );
                        }
                    }
                }

            if ( offset < 0 )
                {
                SendErrorMsg( offset , KErrDescrInvalidArguments );
                }
            }
            break;
        case EStartDoc:
        case EStartDoc_u:
            {
            TInt offset = ParseString( aMessage, 1, unicode, docName );
            if ( offset >= 0 )
                {
                HandleStartAppL( appInfo, docName ); //appInfo is empty
                }
            else
                {
                SendErrorMsg( offset , KErrDescrInvalidArguments );
                }
            }
            break;
        case EStatusApp_uid:
            {
            TPtrC8 appUid8 = aMessage.Mid( 1 );
            if ( appUid8.Length() == 4 )
                {
                TApaTaskList tl( iWs );
                TInt32 appUid = Parse32<TInt32>( appUid8 );
                TApaTask task = tl.FindApp( TUid::Uid( appUid ) );
                if ( task.Exists() )
                    {
                    SendMessageL( ERunning );
                    }
                else
                    {
                    SendMessageL( ENotFound );
                    }
                }
            else
                {
                SendErrorMsg( KErrArgument, KErrDescrInvalidUid );
                }
            }
            break;
        case EStatusApp:
        case EStatusApp_u:
            {
            TInt offset = ParseString( aMessage, 1, unicode, appName );
            if ( offset >= 0 )
                {
                TApaTaskList tl( iWs );
                TApaTask task = tl.FindApp( appName );
                if ( task.Exists() )
                    {
                    SendMessageL( ERunning );
                    break; // we are done
                    }

                // Maybe the user gave path or filename instead of caption
                if ( FindAppL( appInfo, appName ) )
                    {
                    TApaTask task2 = tl.FindApp( appInfo.iUid );
                    if ( task2.Exists() )
                        {
                        SendMessageL( ERunning );
                        }
                    else
                        {
                        SendMessageL( ENotFound );
                        }
                    }
                // If app was not found by FindAppL method, a response message
                // has already been sent there - no need to send anything here.
                }
            else
                {
                SendErrorMsg( offset , KErrDescrInvalidArguments );
                }
            }
            break;
        case EStatusDoc:
        case EStatusDoc_u:
            {
            TInt offset = ParseString( aMessage, 1, unicode, docName );
            if ( offset >= 0 )
                {
                TApaTaskList tl( iWs );
                TApaTask task = tl.FindDoc( docName );
                if ( task.Exists() )
                    {
                    SendMessageL( ERunning );
                    }
                else
                    {
                    SendMessageL( ENotFound );
                    }
                }
            else
                {
                SendErrorMsg( offset , KErrDescrInvalidArguments );
                }
            }
            break;
        case EStopApp:
        case EStopApp_u:
            {
            TInt offset = ParseString( aMessage, 1, unicode, appName );
            if ( offset >= 0 )
                {
                TApaTaskList tl( iWs );
                TApaTask task = tl.FindApp( appName );
                if ( task.Exists() )
                    {
                    task.EndTask();
                    User::After( KCloseTaskDelay );
                    TApaTask task = tl.FindApp( appName );
                    if ( task.Exists() )
                        {
                        task.KillTask();
                        }
                    SendMessageL( EOk );
                    break; // we are done
                    }

                // Maybe the user gave path or filename instead of caption
                if ( FindAppL( appInfo, appName ) )
                    {
                    TApaTask task2 = tl.FindApp( appInfo.iUid );
                    if ( task2.Exists() )
                        {
                        task2.EndTask();
                        User::After( KCloseTaskDelay );
                        TApaTask task2 = tl.FindApp( appInfo.iUid );
                        if ( task2.Exists() )
                            {
                            task2.KillTask();
                            }
                        SendMessageL( EOk );
                        }
                    else
                        {
                        SendMessageL( ENotFound );
                        }
                    }
                // If app was not found by FindAppL method, a response message
                // has already been sent there - no need to send anything here.
                }
            else
                {
                SendErrorMsg( offset , KErrDescrInvalidArguments );
                }
            }
            break;
        case EStopDoc:
        case EStopDoc_u:
            {
            TInt offset = ParseString( aMessage, 1, unicode, docName );
            if ( offset >= 0 )
                {
                TApaTaskList tl( iWs );
                TApaTask task = tl.FindDoc( docName );
                if ( task.Exists() )
                    {
                    task.EndTask();
                    User::After( KCloseTaskDelay );
                    TApaTask task = tl.FindDoc( docName );
                    if ( task.Exists() )
                        {
                        task.KillTask();
                        }
                    SendMessageL( EOk );
                    }
                else
                    {
                    SendMessageL( ENotFound );
                    }
                }
            else
                {
                SendErrorMsg( offset , KErrDescrInvalidArguments );
                }
            }
            break;
        case EStopApp_uid:
            {
            TPtrC8 appUid8 = aMessage.Mid( 1 );
            if ( appUid8.Length() == 4 )
                {
                TApaTaskList tl( iWs );
                TInt32 appUid = Parse32<TInt32>( appUid8 );
                TApaTask task = tl.FindApp( TUid::Uid( appUid ) );
                if ( task.Exists() )
                    {
                    task.EndTask();
                    User::After(KCloseTaskDelay);
                    TApaTask task = tl.FindApp( TUid::Uid( appUid ) );
                    if ( task.Exists() )
                        {
                        task.KillTask();
                        }
                    SendMessageL( EOk );
                    }
                else
                    {
                    SendMessageL( ENotFound );
                    }
                }
            else
                {
                SendErrorMsg( KErrArgument , KErrDescrInvalidUid );
                }
            }
            break;
        case EListApps:
        case EListApps_u:
            {
            if ( aMessage.Length() != 3 )
                {
                SendErrorMsg( KErrArgument, KErrDescrInvalidArguments );
                }
            else
                {
                HandleListAppsL( aMessage[1], aMessage[2], unicode );
                }
            }
            break;
        case EListInstalledApps:
        case EListInstalledApps_u:
            {
            if ( aMessage.Length() != 1 )
                {
                SendErrorMsg( KErrArgument, KErrDescrInvalidArguments );
                }
            else
                {
                HandleListInstalledAppsL( unicode );
                }
            }
            break;
        default:
            {
            SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
            }
        }

    HTI_LOG_FUNC_OUT("CHtiAppControl::HandleAppControl");
    }

void CHtiAppControl::HandleInstallerControlL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::HandleInstallerControlL" );

    if ( aMessage.Length() < 2 )
        {
        HTI_LOG_TEXT( "Command parameters missing" );
        SendErrorMsg( KErrArgument , KErrDescrInvalidCmd );
        return;
        }

    TBool unicode = aMessage[0] & KUnicodeMask;
    HTI_LOG_FORMAT( "Unicode flag: %d", unicode );

    TFileName path;
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    TBuf<SwiUI::KSWInstMaxPasswordLength> login;
    TBuf<SwiUI::KSWInstMaxPasswordLength> password;
#endif

    switch ( aMessage[0] )
        {
        case EInstall:
        case EInstall_u:
            {
            TPtrC8 parameters = aMessage.Mid( 1 );
            if ( ValidateInstallParams( parameters, unicode ) )
                {
                TInt offset = ParseString( parameters, 0, unicode, path );

                HTI_LOG_TEXT( "Parsed path to install package:" );
                HTI_LOG_DES( path )

                if ( offset >= 0)
                    {
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
                    iInstOpts = SwiUI::TInstallOptions();

                    iInstOpts.iUpgrade =            ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iOptionalItems =      ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iOCSP =               ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iIgnoreOCSPWarnings = ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iUntrusted =          ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iPackageInfo =        ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iCapabilities =       ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iKillApp =            ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iOverwrite =          ConvertToPolicy( parameters[offset] ); offset++;
                    iInstOpts.iDownload =           ConvertToPolicy( parameters[offset] ); offset++;

                    HTI_LOG_FORMAT( "iUpgrade: %d",            iInstOpts.iUpgrade );
                    HTI_LOG_FORMAT( "iOptionalItems: %d",      iInstOpts.iOptionalItems );
                    HTI_LOG_FORMAT( "iOCSP: %d",               iInstOpts.iOCSP );
                    HTI_LOG_FORMAT( "iIgnoreOCSPWarnings: %d", iInstOpts.iIgnoreOCSPWarnings );
                    HTI_LOG_FORMAT( "iUntrusted: %d",          iInstOpts.iUntrusted );
                    HTI_LOG_FORMAT( "iPackageInfo: %d",        iInstOpts.iPackageInfo );
                    HTI_LOG_FORMAT( "iCapabilities: %d",       iInstOpts.iCapabilities );
                    HTI_LOG_FORMAT( "iKillApp: %d",            iInstOpts.iKillApp );
                    HTI_LOG_FORMAT( "iOverwrite: %d",          iInstOpts.iOverwrite );
                    HTI_LOG_FORMAT( "iDownload: %d",           iInstOpts.iDownload );

                    offset = ParseString( parameters, offset, unicode, login );
                    iInstOpts.iLogin.Copy( login );
                    HTI_LOG_TEXT( "Parsed login:" );
                    HTI_LOG_DES( login )

                    offset = ParseString( parameters, offset, unicode, password );
                    iInstOpts.iPassword.Copy( password );
                    HTI_LOG_TEXT( "Parsed password:" );
                    HTI_LOG_DES( password )

                    iInstOpts.iDrive =        (TChar) parameters[offset]; offset++;
                    iInstOpts.iLang =         (TLanguage) parameters[offset]; offset++;
                    iInstOpts.iUsePhoneLang = (TBool) parameters[offset]; offset++;
                    iInstOpts.iUpgradeData =  ConvertToPolicy( parameters[offset] );
                    offset++;
                    HTI_LOG_FORMAT( "parameters length: %d", parameters.Length());
                    HTI_LOG_FORMAT( "next offset: %d", offset);

                    if(parameters.Length() > offset)
                        {
                        TInt language = iInstOpts.iLang;
                        if (parameters.Length() == offset+2)
                            {
                            language = parameters[offset] + (parameters[offset+1]<<8);
                            }
                        if (parameters.Length() == offset+1)
                            {
                            language = parameters[offset];
                            }
                        if ((language > ELangTest) && (language < ELangMaximum))
                            {
                            iInstOpts.iLang =         (TLanguage) language;
                            }
                        }
                    HTI_LOG_FORMAT( "iDrive: %c",        iInstOpts.iDrive.GetLowerCase() );
                    HTI_LOG_FORMAT( "iLang: %d",         iInstOpts.iLang );
                    HTI_LOG_FORMAT( "iUsePhoneLang: %d", iInstOpts.iUsePhoneLang );
                    HTI_LOG_FORMAT( "iUpgradeData: %d",  iInstOpts.iUpgradeData );

                    iInstOptsPckg = iInstOpts;

                    // Connect to silent installer
                    SwiUI::RSWInstSilentLauncher launcher;
                    TInt err = launcher.Connect();
                    if ( err )
                        {
                        HTI_LOG_FORMAT( "Error connecting to silent installer, err: %d", err );
                        SendErrorMsg( err , KErrDescrFailedConnectSilentInstaller );
                        break;
                        }

                    err = launcher.SilentInstall( path, iInstOptsPckg );
                    if ( err )
                        {
                        HTI_LOG_FORMAT( "Error installing software, err: %d", err );
                        launcher.Close();
                        SendErrorMsg( err , KErrDescrFailedInstall );
                        break;
                        }

                    launcher.Close();
                    SendMessageL( EOk );
#else
                    HTI_LOG_TEXT( "============RSoftwareInstall::Install=============" );
                    Usif::RSoftwareInstall installer;
                    TInt err = installer.Connect();
                    if(err)
                        {
                        SendErrorMsg( err , KErrDescrFailedConnectSilentInstaller );
                        HTI_LOG_FORMAT("cannot connect to SIF server, err %d", err);
                        User::Leave(err);
                        }
                    CleanupClosePushL(installer);
                    TRequestStatus status;
                    Usif::COpaqueNamedParams *arguments = Usif::COpaqueNamedParams::NewL();
                    CleanupStack::PushL(arguments);
                    Usif::COpaqueNamedParams *results = Usif::COpaqueNamedParams::NewL();
                    CleanupStack::PushL(results);
                    
                    arguments->AddIntL(Usif::KSifInParam_InstallSilently, ETrue);
                    
                    TInt intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_AllowUpgrade, intValue);
                    HTI_LOG_FORMAT( "iUpgrade: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++; 
                    if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_InstallOptionalItems, intValue);
                    HTI_LOG_FORMAT( "iOptionalItems: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_PerformOCSP, intValue);
                    HTI_LOG_FORMAT( "iOCSP: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_IgnoreOCSPWarnings, intValue);
                    HTI_LOG_FORMAT( "iIgnoreOCSPWarnings: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::ENotAllowed)
                        arguments->AddIntL(Usif::KSifInParam_AllowUntrusted, intValue);
                    HTI_LOG_FORMAT( "iUntrusted: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::EAllowed)
						arguments->AddIntL(Usif::KSifInParam_PackageInfo, intValue);
                    HTI_LOG_FORMAT( "iPackageInfo: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_GrantCapabilities,intValue);
                    HTI_LOG_FORMAT( "iCapabilities: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_AllowAppShutdown,intValue);
                    HTI_LOG_FORMAT( "iKillApp: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_AllowOverwrite,intValue);
                    HTI_LOG_FORMAT( "iOverwrite: %d", intValue );
                    intValue = ConvertToSifPolicy(parameters[offset]); offset++;
                    if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_AllowDownload, intValue);
                    HTI_LOG_FORMAT( "iDownload: %d", intValue );

                    TBuf<32> login;
                    offset = ParseString( parameters, offset, unicode, login );
                    HTI_LOG_FORMAT( "login length: %d", login.Length() );
                    if(login != KNullDesC)
                        {
                        arguments->AddStringL(Usif::KSifInParam_UserName, login);
                        }
                    HTI_LOG_TEXT( "Parsed login:" );
                    HTI_LOG_DES( login )

                    TBuf<32> password;
                    offset = ParseString( parameters, offset, unicode, password );
                    HTI_LOG_FORMAT( "password length: %d", password.Length() );
                    if(password != KNullDesC)
                        {
                        arguments->AddStringL(Usif::KSifInParam_Password, password );
                        }
                    HTI_LOG_TEXT( "Parsed password:" );
                    HTI_LOG_DES( password )

                    TChar driver = (TChar) parameters[offset]; offset++;
                    if(driver >= 'A' && driver <= 'Z')
                        {
                        intValue = driver - (TChar)'A';
                        arguments->AddIntL(Usif::KSifInParam_Drive, intValue);
                        HTI_LOG_FORMAT( "iDrive: %c", intValue + 'a' );
                        }
                    else if(driver >= 'a' && driver <= 'z')
                        {
                        intValue = driver - (TChar)'a';
                        arguments->AddIntL(Usif::KSifInParam_Drive, intValue);
                        HTI_LOG_FORMAT( "iDrive: %c", intValue + 'a' );
                        }

                    TLanguage oldFormatLanuage = (TLanguage)parameters[offset]; offset++;

                    TBool usePhoneLang = (TBool)parameters[offset]; offset++;
                    HTI_LOG_FORMAT( "iUsePhoneLang: %d", usePhoneLang );

                    intValue =  ConvertToSifPolicy( parameters[offset] );offset++;
                    /*if(intValue != Usif::EAllowed)
                        arguments->AddIntL(Usif::KSifInParam_AllowUpgrade, intValue);*/
                    HTI_LOG_FORMAT( "iUpgradeData: %d",  intValue ); 
                    
                    HTI_LOG_FORMAT( "parameters length: %d", parameters.Length());
                    HTI_LOG_FORMAT( "next offset: %d", offset);

                    if(usePhoneLang == EFalse)
                        {
                        if(parameters.Length() > offset)
                            {
                            TLanguage language = ELangTest;
                            if (parameters.Length() == offset+2)
                                {
                                language = (TLanguage)(parameters[offset] + (parameters[offset+1]<<8));
                                }
                            else if (parameters.Length() == offset+1)
                                {
                                language = (TLanguage)(parameters[offset]);
                                }
                            if ((language > ELangTest) && (language < ELangMaximum))
                                {
                                arguments->AddIntL(Usif::KSifInParam_Languages, language);
                                HTI_LOG_FORMAT( "iLang: %d", language );
                                }
                            }
                        else
                            {
                            arguments->AddIntL(Usif::KSifInParam_Languages, oldFormatLanuage);
                            HTI_LOG_FORMAT( "iLang0: %d", oldFormatLanuage );
                            }
                        }

                    installer.Install(path, *arguments, *results, status);
                    User::WaitForRequest(status);
                    HTI_LOG_FORMAT("install status: %d", status.Int());
                    TInt componentId = 0;
                    TBool idExisted = results->GetIntByNameL(Usif::KSifOutParam_ComponentId, componentId);
                    if ( idExisted )
                        {
                        HTI_LOG_FORMAT( "componentId:%d", componentId);
                        }
                    CleanupStack::PopAndDestroy(3);
                    if (status != KErrNone)
                        {
                        SendErrorMsg( status.Int(), KErrDescrFailedInstall );
                        break;
                        }
                    SendMessageL( EOk );
#endif
                    }
                else
                    {
                    HTI_LOG_TEXT( "Error parsing path" );
                    SendErrorMsg( offset , KErrDescrInvalidArguments );
                    }
                }
            else
                {
                SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
                }
            }
            break;

        case EUnInstall:
        case EUnInstallName:
        case EUnInstallName_u:
            {
            TPtrC8 parameters = aMessage.Mid( 1 );
            if ( ( aMessage[0] == EUnInstall && parameters.Length() != 7 ) ||
                 ( aMessage[0] == EUnInstallName &&
                   parameters.Length() != parameters[0] + 4 ) ||
                 ( aMessage[0] == EUnInstallName_u &&
                   parameters.Length() != parameters[0] * 2 + 4 ) )
                {
                HTI_LOG_FORMAT( "Invalid command length: %d",
                    parameters.Length() );
                SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
                break;
                }

            TInt offset = 0;
            TInt32 uid = KErrNotFound;
            HBufC* packageName = NULL;
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_4 )
            Usif::TComponentId cid = KErrNotFound; 
            HTI_LOG_TEXT( "============RSoftwareInstall::Uninstall=============" );
            Usif::RSoftwareInstall installer;
            TInt err = installer.Connect();
            if(err)
                {
                SendErrorMsg( err , KErrDescrFailedConnectSilentInstaller );
                HTI_LOG_FORMAT("cannot connect to SIF server, err %d", err);
                User::Leave(err);
                }
            CleanupClosePushL(installer);
            TRequestStatus status;
            Usif::COpaqueNamedParams *arguments = Usif::COpaqueNamedParams::NewL();
            CleanupStack::PushL(arguments);
            Usif::COpaqueNamedParams *results = Usif::COpaqueNamedParams::NewL();
            CleanupStack::PushL(results);
#endif
            if ( aMessage[0] == EUnInstall )
                {
                uid = Parse32<TInt32>( parameters );
                offset += 4;
                HTI_LOG_FORMAT( "Uninstall by uid: %d", uid );
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_4 )
                cid = GetComponentIdFromUid(uid);
                if ( cid == KErrNotFound )
                    {
                    HTI_LOG_FORMAT( "cid: %d", cid );
                    SendErrorMsg( KErrNotFound, KErrDescrBadComponentId );
                    CleanupStack::PopAndDestroy(3); //results, arguments, installer
                    break;
                    }
#endif
                }
            else
                {
                packageName = HBufC::NewLC( parameters[offset] );
                TPtr namePtr = packageName->Des();
                offset = ParseString( parameters, offset, unicode, namePtr );
                HTI_LOG_FORMAT( "Uninstall by name: %S", packageName );
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_4 )
                cid = GetComponentIdFromPackageName(*packageName);
                CleanupStack::PopAndDestroy(); // packageName
                if ( cid == KErrNotFound )
                    {
                    HTI_LOG_FORMAT( "cid: %d", cid );
                    SendErrorMsg( KErrNotFound, KErrDescrFailedFindPackage );
                    CleanupStack::PopAndDestroy(3); //results, arguments, installer
                    break;
                    }
#endif
                }
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
            iUnInstOpts = SwiUI::TUninstallOptions();
            iUnInstOpts.iKillApp = ConvertToPolicy( parameters[offset] );
            offset++;
            iUnInstOpts.iBreakDependency = ConvertToPolicy( parameters[offset] );
            offset++;
            HTI_LOG_FORMAT( "iKillApp: %d",         iUnInstOpts.iKillApp );
            HTI_LOG_FORMAT( "iBreakDependency: %d", iUnInstOpts.iBreakDependency );

            TInt mimeIndex = parameters[offset];
            if ( mimeIndex > iMimeTypes->Count() - 1 )
                {
                HTI_LOG_FORMAT( "Invalid mime type: %d", mimeIndex );
                SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
                break;
                }

            HTI_LOG_TEXT( "Uninstall mime type:" );
            HTI_LOG_DES( (*iMimeTypes)[mimeIndex] );

            TInt err = KErrNone;
            iAugmentationIndex = 0;
            if ( packageName )
                {
                TRAP( err, uid = GetPackageUidL( *packageName, mimeIndex ) );
                CleanupStack::PopAndDestroy(); // packageName
                iAugmentations.ResetAndDestroy();
                if ( err != KErrNone )
                    {
                    SendErrorMsg( err, KErrDescrFailedFindPackage );
                    break;
                    }
                }
            HTI_LOG_FORMAT( "UID = %d", uid );

            iUnInstOptsPckg = iUnInstOpts;

            // Connect to silent installer
            SwiUI::RSWInstSilentLauncher launcher;
            err = launcher.Connect();
            if ( err )
                {
                HTI_LOG_FORMAT( "Error connecting to silent installer, err: %d", err );
                SendErrorMsg( err , KErrDescrFailedConnectSilentInstaller );
                break;
                }

            if ( iAugmentationIndex > 0 )
                {
                SwiUI::TOpUninstallIndexParam params;
                params.iUid = TUid::Uid( uid );
                params.iIndex = iAugmentationIndex;
                SwiUI::TOpUninstallIndexParamPckg paramPckg( params );
                SwiUI::TOperation operation( SwiUI::EOperationUninstallIndex );
                err = launcher.SilentCustomUninstall( operation, iUnInstOptsPckg,
                    paramPckg, (*iMimeTypes)[mimeIndex] );
                }
            else
                {
                err = launcher.SilentUninstall( TUid::Uid( uid ), iUnInstOptsPckg,
                        (*iMimeTypes)[mimeIndex] );
                }

            if ( err )
                {
                HTI_LOG_FORMAT( "Error uninstalling software, err: %d", err );
                launcher.Close();
                SendErrorMsg( err , KErrDescrFailedUnInstall );
                break;
                }

            launcher.Close();
#else
            
            TInt intValue = ConvertToSifPolicy( parameters[offset] );
            offset++;
            arguments->AddIntL(Usif::KSifInParam_AllowAppShutdown, intValue);
            HTI_LOG_FORMAT( "iKillApp: %d", intValue );
            intValue = ConvertToSifPolicy( parameters[offset] );
            offset++;
            arguments->AddIntL(Usif::KSifInParam_AllowAppBreakDependency, intValue);
            HTI_LOG_FORMAT( "iBreakDependency: %d", intValue );

            TInt mimeIndex = parameters[offset];
            if ( mimeIndex > iMimeTypes->Count() - 1 )
                {
                HTI_LOG_FORMAT( "Invalid mime type: %d", mimeIndex );
                SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
                break;
                }

            HTI_LOG_TEXT( "Uninstall mime type:" );
            HTI_LOG_DES( (*iMimeTypes)[mimeIndex] );

            HBufC* buf = HBufC::NewLC((*iMimeTypes)[mimeIndex].Length());
            TPtr ptr = buf->Des();
            ptr.Copy((*iMimeTypes)[mimeIndex]);
            arguments->AddStringL(Usif::KSifInParam_MimeType, *buf);
            CleanupStack::PopAndDestroy(); // buf
      
            HTI_LOG_FORMAT( "Component ID = %d", cid );
            
            arguments->AddIntL(Usif::KSifInParam_InstallSilently, ETrue);
            
            installer.Uninstall(cid, *arguments, *results, status, EFalse);
            User::WaitForRequest(status);
            HTI_LOG_FORMAT("uninstall status:%d", status.Int());
            CleanupStack::PopAndDestroy(3); //results, arguments, installer
            if (status != KErrNone)
                {
                SendErrorMsg(status.Int(), KErrDescrFailedUnInstall);
                break;
                }            
#endif
            SendMessageL( EOk );
            }
            break;

        default:
            {
            SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiAppControl::HandleInstallerControlL" );
    }


void CHtiAppControl::HandleStartProcessL( const TDesC& aProgramName,
                                                const TDesC& aCmdLine,
                                                TBool aStoreProcessHandle )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::HandleStartProcessL" );
    HTI_LOG_FORMAT( "progr name %d", aProgramName.Length() );
    HTI_LOG_DES( aProgramName );

    RProcess process;
    TInt err = process.Create( aProgramName, aCmdLine ); // command parameters

    if ( err == KErrNone )
        {
        CleanupClosePushL( process );

        //convert process id to binary des
        TUint processId = process.Id();
        HTI_LOG_FORMAT( "process id %d", processId );

        TBuf8<KTUintLength> processIdDes;
        processIdDes.Append(
                ( TUint8* )( &processId ), KTUintLength );

        SendMessageL( EOk, processIdDes );

        process.Resume();

        if ( aStoreProcessHandle )
            {
            HTI_LOG_TEXT( "Storing the process handle" );
            iProcessHandleArray.Append( process );
            CleanupStack::Pop();
            }
        else
            {
            CleanupStack::PopAndDestroy();
            }
        }
    else if ( err == KErrNotFound )
        {
        SendMessageL( ENotFound );
        }
    else
        {
        SendErrorMsg( err ,KErrDescrFailedCreateProcess );
        }

    HTI_LOG_FUNC_OUT( "CHtiAppControl::HandleStartProcessL" );
    }

void CHtiAppControl::HandleStopProcessL( RProcess& aProcess )
    {
    if ( aProcess.ExitType() == EExitPending )
        {
        aProcess.Kill( KTerminateReason );
        SendMessageL( EOk );
        }
    else
        {
        SendMessageL( EAlreadyStopped );
        }
    }

void CHtiAppControl::HandleStatusProcessL( RProcess& aProcess )
    {
    TExitType exT = aProcess.ExitType();

    switch ( exT )
        {
        case EExitPending:
            {
            SendMessageL( ERunning );
            }
            break;
        case EExitKill:
        case EExitTerminate:
            {
            SendMessageL( EKilled );
            }
            break;
        case EExitPanic:
            {
            SendMessageL( EPanic );
            }
            break;
        };
    }


void CHtiAppControl::HandleListProcessesL( const TDesC& aMatch )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::HandleListProcessesL" );

    RProcess process;
    TFullName processName;
    TUint processId;
    TBuf8<KTUintLength> processIdDes;
    TExitType exitType;

    TBuf8<128> buf;
    CBufFlat* processListBuf = NULL;
    TRAPD( err, processListBuf = CBufFlat::NewL( 128 ) );
    if ( err )
        {
        SendErrorMsg( err ,KErrDescrNoMemory );
        return;
        }

    CleanupStack::PushL( processListBuf );

    // The default match pattern is the single character *
    TFindProcess finder;

    // if some real match pattern is defined, use it
    if ( aMatch.Length() > 0 )
        {
        HTI_LOG_TEXT( "Match pattern was given:" );
        HTI_LOG_DES( aMatch );
        finder.Find( aMatch );
        }

    TInt pos = 0;
    TUint numberOfEntries = 0;

    while ( finder.Next( processName ) == KErrNone )
        {
        err = process.Open( finder );

        //convert process id to binary des
        processId = process.Id();
        processIdDes.Append( ( TUint8* ) ( &processId ), KTUintLength );
        buf.Append( processIdDes );

        // status
        exitType = process.ExitType();
        switch ( exitType )
            {
            case EExitPending:
                {
                buf.Append( ERunning );
                }
                break;
            case EExitKill:
            case EExitTerminate:
                {
                buf.Append( EKilled );
                }
                break;
            case EExitPanic:
                {
                buf.Append( EPanic );
                }
                break;
            };

        // name length
        buf.Append( processName.Length() );

        // name
        buf.Append( processName );

        process.Close();

        TRAP( err, processListBuf->ExpandL( pos, buf.Length() ) );
        if ( err )
            {
            SendErrorMsg( err , KErrDescrNoMemory );
            delete processListBuf;
            return;
            }
        processListBuf->Write( pos, buf, buf.Length() );

        pos += buf.Length();
        buf.Zero();
        processIdDes.Zero();
        numberOfEntries++;
        }

    // insert the number of entries in the beginning
    TBuf8<2> entries;
    entries.Append( ( TUint8* ) ( &numberOfEntries ), 2 );
    processListBuf->ExpandL( 0, 2 );
    processListBuf->Write( 0, entries, 2 );

    SendMessageL( EOk, processListBuf->Ptr( 0 ) );

    CleanupStack::PopAndDestroy( processListBuf );

    HTI_LOG_FUNC_OUT( "CHtiAppControl::HandleListProcessesL" );
    }


void CHtiAppControl::HandleStartAppL( TApaAppInfo &aAppInfo,
                                            const TDesC& aDocName )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::HandleStartAppL" );

    // Check if app is running as a root app (not embedded)
    TApaTask task = FindRunningRootApp( aAppInfo.iUid );
    if ( task.Exists() )
        {
        User::ResetInactivityTime();
        task.BringToForeground();
        SendMessageL( EAlreadyRunning );
        }
    else
        {
        TThreadId threadId;
        TInt err = KErrNone;
        if ( aAppInfo.iUid != TUid::Null() )
            {
            TApaAppCapabilityBuf capBuf;
            err = iAppServer.GetAppCapability( capBuf, aAppInfo.iUid );
            TApaAppCapability& caps = capBuf();
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( aAppInfo.iFullName );
            if ( caps.iLaunchInBackground )
                {
                cmdLine->SetCommandL( EApaCommandBackground );
                HTI_LOG_TEXT( "Launching in background" );
                }
            else
                {
                if ( aDocName != KNullDesC )
                    {
                    cmdLine->SetCommandL( EApaCommandOpen );
                    cmdLine->SetDocumentNameL( aDocName );
                    HTI_LOG_TEXT( "Launching with document" );
                    }
                else
                    {
                    cmdLine->SetCommandL( EApaCommandRun );
                    HTI_LOG_TEXT( "Launching without document" );
                    }
                }
            err = iAppServer.StartApp( *cmdLine, threadId );
            CleanupStack::PopAndDestroy( cmdLine );
            }
        else if ( aDocName != KNullDesC )
            {
            HTI_LOG_TEXT( "Launching a document" );
            /*
             * If the app that should handle the doc is already running, we
             * try to switch the open file and bring it to foreground. Whether
             * switching the file works depends on the application: It must
             * implement CEikAppUi::OpenFileL() or CAknAppUi::OpenFileL() in S60.
             */
            TUid appUid;
            TDataType dataType;
            err = iAppServer.AppForDocument( aDocName, appUid, dataType );
            if ( err == KErrNone )
                {
                TApaTask task = FindRunningRootApp( appUid );
                if ( task.Exists() )
                    {
                    HTI_LOG_TEXT( "App for doc already running, switch file" );
                    err = task.SwitchOpenFile( aDocName );
                    task.BringToForeground();
                    }
                else
                    {
                    err = iAppServer.StartDocument( aDocName, threadId );
                    }
                }
            }
        else
            {
            User::Leave( KErrArgument );
            }

        if ( err == KErrNone )
            {
            User::ResetInactivityTime();
            //convert thread id to string
            TUint threadIdUint = threadId; //cast to UInt
            TBuf8<KTUintLength> threadIdDes;
            threadIdDes.Append( ( TUint8* )( &threadIdUint ), KTUintLength );
            SendMessageL( EOk, threadIdDes );
            }
        else if ( err == KErrNotFound )
            {
            SendMessageL( ENotFound );
            }
        else
            {
            SendErrorMsg( err, KErrDescrFailedStartApp );
            }
        }
    HTI_LOG_FUNC_OUT( "CHtiAppControl::HandleStartAppL" );
    }

void CHtiAppControl::HandleListInstalledAppsL( TBool aUnicode )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::HandleListInstalledAppsL" );

    // max file name + max caption + max short caption + 7 constant bytes
    TInt bufSize = KMaxFileName + 2 * KApaMaxAppCaption + 7;
    if ( aUnicode )
        {
        bufSize *= 2;
        }

    CBufFlat* appListBuf = CBufFlat::NewL( bufSize );
    CleanupStack::PushL( appListBuf );

    HBufC8* appsArray = HBufC8::NewLC( bufSize );
    TPtr8 appsArrayPtr = appsArray->Des();

    TInt err = KErrNone;
    err = iAppServer.GetAllApps();
    if ( err != KErrNone )
        {
        SendErrorMsg( err, KErrDescrFailedListInstApps );
        }
    else
        {
        TApaAppInfo appInfo;
        TInt pos( 0 );

        // Add application count
        TInt numOfEntries = 0;
        iAppServer.AppCount( numOfEntries );

        appsArrayPtr.Append( ( TUint8*) ( &numOfEntries ), 2 );

        while ( iAppServer.GetNextApp( appInfo ) == KErrNone )
            {
            TUint appUidUint = appInfo.iUid.iUid;
            appsArrayPtr.Append( ( TUint8* ) &appUidUint, KTUintLength );

            // Add length of full name
            appsArrayPtr.Append( appInfo.iFullName.Length() );
            // Add full name if there is one
            if ( appInfo.iFullName.Length() > 0 )
                {
                if ( aUnicode )
                    {
                    appsArrayPtr.Append( ( TUint8* ) appInfo.iFullName.Ptr(),
                        appInfo.iFullName.Length() * 2 );
                    }
                else
                    {
                    appsArrayPtr.Append( appInfo.iFullName );
                    }
                }

            // Add length of caption
            appsArrayPtr.Append( appInfo.iCaption.Length() );
            // Add caption if there is one
            if ( appInfo.iCaption.Length() > 0 )
                {
                if ( aUnicode )
                    {
                    appsArrayPtr.Append( ( TUint8* ) appInfo.iCaption.Ptr(),
                        appInfo.iCaption.Length() * 2 );
                    }
                else
                    {
                    appsArrayPtr.Append( appInfo.iCaption );
                    }
                }

            // Add length of short caption
            appsArrayPtr.Append( appInfo.iShortCaption.Length() );
            // Add short caption if there is one
            if ( appInfo.iShortCaption.Length() > 0 )
                {
                if ( aUnicode )
                    {
                    appsArrayPtr.Append( ( TUint8* ) appInfo.iShortCaption.Ptr(),
                        appInfo.iCaption.Length() * 2 );
                    }
                else
                    {
                    appsArrayPtr.Append( appInfo.iShortCaption );
                    }
                }

            // Add app info to response buffer
            appListBuf->ExpandL( pos, appsArray->Length() );
            appListBuf->Write( pos, *appsArray, appsArray->Length() );

            pos += appsArray->Length();
            appsArrayPtr.Zero();
            }

        SendMessageL( EOk, appListBuf->Ptr( 0 ) );
        }

    CleanupStack::PopAndDestroy( 2 ); //appListBuf, appsArray

    HTI_LOG_FUNC_IN( "CHtiAppControl::HandleListInstalledAppsL" );
    }

void CHtiAppControl::HandleListAppsL( TBool aIncludeHidden,
                                            TBool aIncludeSystem,
                                            TBool aUnicode )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::HandleListAppsL" );

    RArray<RWsSession::TWindowGroupChainInfo>* wgChain = new ( ELeave )
                            RArray<RWsSession::TWindowGroupChainInfo>( 12 );
    CleanupDeletePushL( wgChain );
    CleanupClosePushL( *wgChain );
    User::LeaveIfError( iWs.WindowGroupList( 0, wgChain ) );
    TInt wgCount = wgChain->Count();

    HTI_LOG_FORMAT( "%d Window Groups in the chain", wgCount );

    TInt bufSize = 2 * KMaxFileName + 11; // caption & document + 11 constant bytes
    if ( aUnicode )
        {
        bufSize *= 2;
        }
    CBufFlat* appListBuf = CBufFlat::NewL( bufSize );
    CleanupStack::PushL( appListBuf );
    HBufC8* buf = HBufC8::NewLC( bufSize );
    TPtr8 bufPtr = buf->Des();

    TInt pos = 0;
    TInt numOfEntries = 0;

    for ( TInt i = 0; i < wgCount; i++ )
        {
        const RWsSession::TWindowGroupChainInfo& info = ( *wgChain )[i];
        CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( iWs, info.iId );

        HTI_LOG_FORMAT( "WG ID: %d", info.iId );
        HTI_LOG_DES( wgName->WindowGroupName() );

        // Info is returned only from root tasks and system and hidden tasks
        // are included only if requested.
        if ( info.iParentId <= 0 &&
             ( !wgName->IsSystem() || aIncludeSystem ) &&
             ( !wgName->Hidden() || aIncludeHidden ) )
            {
            // Add application UID
            TUint appUidUint = wgName->AppUid().iUid;
            bufPtr.Append( ( TUint8* ) &appUidUint, KTUintLength );

            // Add caption
            TPtrC caption = wgName->Caption();
            bufPtr.Append( caption.Length() );
            if ( caption.Length() > 0 )
                {
                if ( aUnicode )
                    {
                    bufPtr.Append(
                        ( TUint8* ) caption.Ptr(), caption.Length() * 2 );
                    }
                else
                    {
                    bufPtr.Append( caption );
                    }
                }

            // Add document name
            TPtrC document = wgName->DocName();
            bufPtr.Append( document.Length() );
            if ( document.Length() > 0 )
                {
                if ( aUnicode )
                    {
                    bufPtr.Append(
                        ( TUint8* ) document.Ptr(), document.Length() * 2 );
                    }
                else
                    {
                    bufPtr.Append( document );
                    }
                }

            // Add Hidden flag
            if ( wgName->Hidden() )
                bufPtr.Append( 1 );
            else
                bufPtr.Append( 0 );

            // Add System flag
            if ( wgName->IsSystem() )
                bufPtr.Append( 1 );
            else
                bufPtr.Append( 0 );

            // Add Ready flag
            if ( wgName->IsAppReady() )
                bufPtr.Append( 1 );
            else
                bufPtr.Append( 0 );

            // Add Busy flag
            if ( wgName->IsBusy() )
                bufPtr.Append( 1 );
            else
                bufPtr.Append( 0 );

            // Add Shutdown response flag
            if ( wgName->RespondsToShutdownEvent() )
                bufPtr.Append( 1 );
            else
                bufPtr.Append( 0 );

            // Add this task's info to response buffer
            appListBuf->ExpandL( pos, buf->Length() );
            appListBuf->Write( pos, *buf, buf->Length() );

            pos += buf->Length();
            bufPtr.Zero();
            numOfEntries++;
            } // if
        CleanupStack::PopAndDestroy(); // wgName
        } // for

    CleanupStack::PopAndDestroy(); // buf

    // Add number of entries to the beginning of the response
    TBuf8<2> entries;
    entries.Append( ( TUint8* ) ( &numOfEntries ), 2 );
    appListBuf->ExpandL( 0, 2 );
    appListBuf->Write( 0, entries, 2 );

    SendMessageL( EOk, appListBuf->Ptr( 0 ) );

    CleanupStack::PopAndDestroy( 3 ); // appListBuf, wgChain Close, wgChain delete

    HTI_LOG_FUNC_OUT( "CHtiAppControl::HandleListAppsL" );
    }

TBool CHtiAppControl::FindAppL( TApaAppInfo& aAppInfo,
                                    const TDesC& aAppFullName )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::FindAppL name" );
    TInt err = KErrNone;
    err = iAppServer.GetAllApps();
    if ( err != KErrNone )
        {
        SendErrorMsg( err, KErrDescrFailedFindApp );
        return EFalse;
        }

    TParse toFind;
    err = toFind.SetNoWild( aAppFullName, NULL, NULL );
    if ( err != KErrNone )
        {
        SendErrorMsg( err, KErrDescrFailedFindApp );
        return EFalse;
        }

    TBool isFullName = ( toFind.DrivePresent() && toFind.PathPresent() );

    while ( ( err = iAppServer.GetNextApp( aAppInfo) ) == KErrNone )
        {
        if ( isFullName )  // assume that full path has been given
            {
            if ( !aAppFullName.CompareF( aAppInfo.iFullName )  )
                {
                return ETrue;
                }
            }
        else // assume that only filename has been given
            {
            TParsePtrC current( aAppInfo.iFullName );

            // does the given filename contain extension
            if ( toFind.ExtPresent() )
                {
                // compare with filename + extension
                if ( !toFind.NameAndExt().CompareF( current.NameAndExt() ) )
                    {
                    return ETrue;
                    }
                }
            else
                {
                // compare with filename only
                if ( !toFind.Name().CompareF( current.Name() ) )
                    {
                    return ETrue;
                    }
                // Try to match the caption.
                // For Java MIDlets the full name is like C:\270194328.fakeapp
                // so we have to use caption to find MIDlets.
                if ( !toFind.Name().CompareF( aAppInfo.iCaption ) )
                    {
                    return ETrue;
                    }
                }
            }
        }

    if ( err == RApaLsSession::ENoMoreAppsInList )
        {
        SendMessageL( ENotFound );
        }
    else if ( err != KErrNone )
        {
        SendErrorMsg( err,KErrDescrFailedFindApp );
        }
    HTI_LOG_FUNC_OUT( "CHtiAppControl::FindAppL name" );
    return EFalse;
    }

TBool CHtiAppControl::FindAppL( TApaAppInfo &aAppInfo,
                                     const TInt32 aUid )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::FindAppL uid" );
    TInt err = iAppServer.GetAppInfo( aAppInfo, TUid::Uid( aUid ) );
    if ( err == KErrNone )
        {
        return ETrue;
        }
    else if ( err == KErrNotFound )
        {
        SendMessageL( ENotFound );
        }
    else
        {
        SendErrorMsg( err , KErrDescrFailedFindApp );
        }
    HTI_LOG_FUNC_OUT( "CHtiAppControl::FindAppL uid" );
    return EFalse;
    }

TApaTask CHtiAppControl::FindRunningRootApp( TUid aAppUid )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::FindRunningRootApp" );
    TApaTask task( iWs );
    task.SetWgId( 0 ); // initialize to empty task

    TInt wgId = 0; // on first call to FindByAppUid wgId must be zero
    CApaWindowGroupName::FindByAppUid( aAppUid, iWs, wgId );
    HTI_LOG_FORMAT( "FindByAppUid returned WG ID: %d", wgId );

    RArray<RWsSession::TWindowGroupChainInfo> wgs;
    TInt err = KErrNone;
    TInt wgCount = 0;
    if ( wgId != KErrNotFound )
        {
        // Get a list of Window Group Chain Infos
        err = iWs.WindowGroupList( 0, &wgs ); // get only priority 0 WGs
        wgCount = wgs.Count();
        HTI_LOG_FORMAT( "WindowGroupList returned %d WGs", wgCount );
        }
    while ( wgId != KErrNotFound && task.WgId() == 0 && err == KErrNone )
        {
        // App was found - check if it is root by looping through the
        // WG chain infos to find the one with out wgId
        TInt i = 0;
        while ( task.WgId() == 0 && i < wgCount )
            {
            const RWsSession::TWindowGroupChainInfo& info = wgs[i];
            if ( info.iId == wgId && info.iParentId <= 0 )
                {
                // This is the one and it is root (does not have parent)
                task.SetWgId( wgId );
                }
            i++;
            }
        if ( task.WgId() == 0 )
            {
            // This was not root - check if there's more instances of the app
            CApaWindowGroupName::FindByAppUid( aAppUid, iWs, wgId );
            HTI_LOG_FORMAT( "FindByAppUid returned WG ID: %d", wgId );
            }
        }

    wgs.Close();
    HTI_LOG_FORMAT( "Returning task with WG ID %d", task.WgId() );
    HTI_LOG_FUNC_OUT( "CHtiAppControl::FindRunningRootApp" );
    return task;
    }

TInt CHtiAppControl::OpenProcessL( RProcess& aProcess,
                                         const TDesC& aMatch )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::OpenProcessL" );
    TFullName processName;
    TInt err = KErrNone;
    TFindProcess finder( aMatch );

    err = finder.Next( processName );
    if ( err == KErrNone )
        {
        err = aProcess.Open( finder );
        }

    HTI_LOG_FUNC_OUT( "CHtiAppControl::OpenProcessL" );
    return err;
    }

template<class T> T CHtiAppControl::Parse32(
                        const TDesC8& a32int )
    {
    //manually construct TUint or TInt32
    return T( a32int[0] + ( a32int[1] << 8 ) +
                      ( a32int[2] << 16) + ( a32int[3] << 24) );
    }

void CHtiAppControl::SendMessageL( TAppCommand aResponseCode,
                                         const TDesC8& aMsg )
    {
    HTI_LOG_FORMAT( "SendMessage %d", aResponseCode );
    HTI_LOG_FORMAT( "Message len %d", aMsg.Length() );
    HBufC8* sendMsg = HBufC8::NewL( 1 + aMsg.Length() );
    CleanupStack::PushL( sendMsg );
    sendMsg->Des().Append( aResponseCode );
    sendMsg->Des().Append( aMsg );

    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
                            sendMsg,
                            KAppServiceUid ) );
    CleanupStack::Pop();
    }

inline TInt CHtiAppControl::SendErrorMsg( TInt anError,
                                                const TDesC8& aMsg )
    {
    return iDispatcher->DispatchOutgoingErrorMessage( anError,
                                               aMsg,
                                               KAppServiceUid );
    }

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
SwiUI::TPolicy CHtiAppControl::ConvertToPolicy( const TInt8 aValue )
    {
    if ( aValue == 0 ) return SwiUI::EPolicyNotAllowed;

    return  SwiUI::EPolicyAllowed;
    }
#else
Usif::TSifPolicy CHtiAppControl::ConvertToSifPolicy( const TInt8 aValue )
    {
    if ( aValue == 0 ) return Usif::ENotAllowed;

    return  Usif::EAllowed;    
    }
#endif

TBool CHtiAppControl::ValidateInstallParams( const TDesC8& aParams, TBool aIsUnicode )
    {
    HTI_LOG_FORMAT( "ValidateInstallParams => length: %d", aParams.Length() );
    if ( aParams.Length() > 0 )
        {
        TInt offset = 0;
        TInt length = aParams[offset]; // inst package path length;
        if ( aIsUnicode ) length *= 2;
        offset++;
        if ( aParams.Length() < offset + length )
            {
            HTI_LOG_TEXT( "ValidateInstallParams: Failed, data missing in inst pkg path" );
            return EFalse;
            }

        offset += length; // skip over inst package path
        offset += 10;     // skip over the following one byte params

        if ( aParams.Length() < offset )
            {
            HTI_LOG_TEXT( "ValidateInstallParams: Failed, data missing in one byte params" );
            return EFalse;
            }

        length = aParams[offset]; // login username length;
        if ( aIsUnicode ) length *= 2;
        offset++;
        if ( aParams.Length() < offset + length )
            {
            HTI_LOG_TEXT( "ValidateInstallParams: Failed, data missing in username" );
            return EFalse;
            }

        offset += length; // skip over login username

        length = aParams[offset]; // password length;
        if ( aIsUnicode ) length *= 2;
        offset++;
        if ( aParams.Length() < offset + length )
            {
            HTI_LOG_TEXT( "ValidateInstallParams: Failed, data missing in password" );
            return EFalse;
            }

        offset += length; // skip over password
        offset += 4;      // the last one byte params

        if ( aParams.Length() < offset || aParams.Length() > offset + 2)
            {
            HTI_LOG_TEXT( "ValidateInstallParams: Failed, final length incorrect" );
            return EFalse;
            }

        return ETrue;
        }

    return EFalse;
    }

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
TInt CHtiAppControl::GetPackageUidL( const TDesC& aPackageName,
                                          TInt aMimeIndex )
    {
    HTI_LOG_FUNC_IN( "CHtiAppControl::GetPackageUidL" );

    if ( aMimeIndex >= 0 && aMimeIndex < 2 ) // SIS
        {
        Swi::RSisRegistrySession regSession;
        User::LeaveIfError( regSession.Connect() );
        CleanupClosePushL( regSession );

        RArray<TUid> uids;
        CleanupClosePushL( uids );
        regSession.InstalledUidsL( uids );
        TInt count = uids.Count();
        HTI_LOG_FORMAT( "Found %d SISx installations", count );
        for ( TInt i = 0; i < count; i++ )
            {
            Swi::RSisRegistryEntry entry;
            CleanupClosePushL( entry );
            User::LeaveIfError( entry.Open( regSession, uids[i] ) );
            if ( !entry.IsInRomL() && entry.IsPresentL() )
                {
                if ( aPackageName.Compare( *( entry.PackageNameL() ) ) == 0 )
                    {
                    HTI_LOG_TEXT( "Matching SIS package found" );
                    TInt uid = entry.UidL().iUid;
                    CleanupStack::PopAndDestroy( 3 ); // entry, uids, regSession
                    return uid;
                    }
                }
            // Check augmentations of this entry
            entry.AugmentationsL( iAugmentations );
            TInt augCount = iAugmentations.Count();
            for ( TInt j = 0; j < augCount; j++ )
                {
                Swi::RSisRegistryEntry augmentation;
                CleanupClosePushL( augmentation );
                augmentation.OpenL( regSession, *iAugmentations[j] );
                if ( aPackageName.Compare(
                        *( augmentation.PackageNameL() ) ) == 0 )
                    {
                    if ( !augmentation.IsInRomL() && augmentation.IsPresentL() )
                        {
                        HTI_LOG_TEXT( "Matching SIS augmentation found" );
                        TInt uid = augmentation.UidL().iUid;
                        Swi::CSisRegistryPackage* pkg = augmentation.PackageL();
                        iAugmentationIndex = pkg->Index();
                        delete pkg;
                        HTI_LOG_FORMAT( "Aug. index %d", iAugmentationIndex );
                        CleanupStack::PopAndDestroy( 4 ); // augmentation, entry, uids, regSession
                        return uid;
                        }
                    }
                CleanupStack::PopAndDestroy(); // augmentation
                } // for j
            iAugmentations.ResetAndDestroy();
            CleanupStack::PopAndDestroy(); // entry
            } // for i
        User::Leave( KErrNotFound );
        }

    else if ( aMimeIndex > 2 && aMimeIndex < 7 ) // Java
        {
            RArray<TUid> uids;
            CleanupClosePushL( uids );

            CJavaRegistry* javaRegistry = CJavaRegistry::NewLC();
            javaRegistry->GetRegistryEntryUidsL( uids );

            TInt uid = KErrNotFound;
            TInt count = uids.Count();
            HTI_LOG_FORMAT( "Found %d Java installations", count );
            for ( TInt i = 0; i < count; i++ )
                {
                CJavaRegistryEntry* entry = javaRegistry->RegistryEntryL( uids[i] );
                if ( entry )
                    {
                    CleanupStack::PushL( entry );
                    if ( entry->Type() >= EGeneralPackage &&
                            entry->Type() < EGeneralApplication )
                        {
                        // entry was a package (MIDlet suite)
                        CJavaRegistryPackageEntry* packageEntry =
                            ( CJavaRegistryPackageEntry* ) entry;
                        if ( aPackageName.Compare( packageEntry->Name() ) == 0 )
                            {
                            HTI_LOG_TEXT( "Matching Java installation found" );
                            uid = packageEntry->Uid().iUid;
                            CleanupStack::PopAndDestroy( entry );
                            break;
                            }
                        }
                    CleanupStack::PopAndDestroy( entry );
                    }
                }
            CleanupStack::PopAndDestroy( javaRegistry );
            CleanupStack::PopAndDestroy( &uids );
            User::LeaveIfError( uid );
            return uid;
        }

    else if ( aMimeIndex == 7 ) // Widget
        {
        RWidgetRegistryClientSession widgetRegistry;
        User::LeaveIfError( widgetRegistry.Connect() );

        RWidgetInfoArray widgets;
        TRAPD( err, widgetRegistry.InstalledWidgetsL( widgets ) );
        if ( err != KErrNone )
            {
            HTI_LOG_FORMAT( "Failed to get installed widgets %d", err );
            widgets.ResetAndDestroy();
            widgetRegistry.Disconnect();
            User::Leave( err );
            }

        TInt uid = KErrNotFound;
        TInt count = widgets.Count();
        HTI_LOG_FORMAT( "Found %d Widget installations", count );
        for ( TInt i = 0; i < count; i++ )
            {
            CWidgetInfo* widgetInfo = widgets[i];
            HTI_LOG_DES( *( widgetInfo->iBundleName ) );
            if ( aPackageName.Compare( *( widgetInfo->iBundleName ) ) == 0 )
                {
                HTI_LOG_TEXT( "Matching Widget installation found" );
                uid = widgetInfo->iUid.iUid;
                break;
                }
            }
        widgets.ResetAndDestroy();
        widgetRegistry.Disconnect();
        User::LeaveIfError( uid );
        return uid;
        }

    else  // invalid mime index
        {
        User::Leave( KErrArgument );
        }

    return KErrNone; // never returns from here
    }
#else
TInt CHtiAppControl::GetComponentIdFromUid(const TInt32 aUid)
    {
    TInt cid = KErrNotFound;
    Usif::RSoftwareComponentRegistry registry;
    User::LeaveIfError(registry.Connect());
    CleanupClosePushL(registry);
    RArray<TUid> uidList;
    CleanupClosePushL(uidList);
    RArray<Usif::TComponentId> componentIdList;
    CleanupClosePushL(componentIdList);
    registry.GetComponentIdsL(componentIdList);
    for(TInt i = 0; i < componentIdList.Count(); i++)
        {
        Usif::TComponentId compId = componentIdList[i];
        Usif::CComponentEntry *compEntry = Usif::CComponentEntry::NewLC();
        if(registry.GetComponentL(compId, *compEntry))
            {
            /*if(compEntry->IsRemovable() && 
                 compEntry->SoftwareType() == Usif::KSoftwareTypeNative)*/
            if(compEntry->IsRemovable())
                {
                _LIT(KCompUid, "CompUid");
                Usif::CPropertyEntry *property = 
                    registry.GetComponentPropertyL(compId, KCompUid);
                CleanupStack::PushL(property);
                Usif::CIntPropertyEntry* intProperty = 
                    dynamic_cast<Usif::CIntPropertyEntry*>(property);
                uidList.AppendL(TUid::Uid(intProperty->IntValue()));
                CleanupStack::PopAndDestroy(property);
                }
            else
                {
                uidList.AppendL(KNullUid);
                }
            }
        CleanupStack::PopAndDestroy( compEntry );
        }
    TUid tuid(TUid::Uid(aUid));
    if(tuid != KNullUid)
        {
        TInt index = uidList.Find(tuid);
        if(index >= 0 && index < componentIdList.Count())
            {
            cid = componentIdList[index];
            }
        }
        CleanupStack::PopAndDestroy( 3, &registry );// componentIdList, uidList, registry
        return cid;
    }

TInt CHtiAppControl::GetComponentIdFromPackageName(const TDesC& aPackageName)
    {
    TInt cid = KErrNotFound;
    Usif::RSoftwareComponentRegistry registry;
    User::LeaveIfError(registry.Connect());
    CleanupClosePushL(registry);
    RArray<Usif::TComponentId> componentIdList;
    CleanupClosePushL(componentIdList);
    registry.GetComponentIdsL(componentIdList);
    TInt count = componentIdList.Count();    
    for(TInt i = 0; i < count; i++)
        {
        Usif::TComponentId compId = componentIdList[i];
        Usif::CComponentEntry *compEntry = Usif::CComponentEntry::NewLC();
        if(registry.GetComponentL(compId, *compEntry))
            {
            if(compEntry->IsRemovable())
                {
                if ( aPackageName.Compare( compEntry->Name() ) == 0 )
                    {
                    cid = compId;
                    CleanupStack::PopAndDestroy( compEntry );
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( compEntry );
        }
        CleanupStack::PopAndDestroy( 2, &registry );// componentIdList, registry
        return cid;
    }

#endif
// End of File
