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
* Description:  AppUi implementation, all functions here
*
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <AknQueryDialog.h>
#include <aknmessagequerydialog.h>
#include <w32std.h>
#include <eikmenup.h>
#include <commdb.h>

#include <HtiAdmin.rsg>
#include <HtiCommPluginInterface.h> // for KHTICommInterfaceUid
#include <HtiCfg.h>
#include <HtiVersion.h>

#include "HtiAdmin.pan"
#include "HtiAdminAppUi.h"
#include "HtiAdminAppView.h"
#include "HtiAdmin.hrh"


#ifdef __ENABLE_LOGGING__

#include <flogger.h>

_LIT(KLogFolder, "hti");
_LIT(KLogFile, "htiadmin.txt");
#define HTI_LOG_TEXT(a1) {_LIT(temp, a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#define HTI_LOG_FORMAT(a1,a2) {_LIT(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, (a2));}
#define HTI_LOG_DES(a1) {RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, (a1));}

#else // __ENABLE_LOGGING__

#define HTI_LOG_TEXT(a1)
#define HTI_LOG_FORMAT(a1,a2)
#define HTI_LOG_DES(a1)

#endif // __ENABLE_LOGGING__

// CONSTANTS
_LIT( KHtiWatchDogMatchPattern, "HtiWatchDog*" );
_LIT( KHtiFrameworkExe, "HtiFramework.exe" );
_LIT( KHtiMatchPattern, "HtiFramework*" );
_LIT( KHtiFrameworkCaption, "HTI status: " );
_LIT( KHtiAutoStartCaption, "AutoStart: " );
_LIT( KHtiVersionCaption, "Version: " );
_LIT( KHtiSelectedCommCaption, "Communication: " );
_LIT( KHtiAdminStartParameter, "admin" );
_LIT( KHtiAdminVersionFormat, "%u.%u.%u (%uwk%02u)" );
_LIT( KHtiAdminAboutVersionFormat, "Version %u.%u.%u - " );
const static TInt KTerminateReason = 1;


const TInt32 KHTIImplUidSerial    = {0x10210CCA};
const TInt32 KHTIImplUidBluetooth = {0x200212CC};
const TInt32 KHTIImplUidIPComm    = {0x200212CE};

// configuration file constants
_LIT( KCfgFilePath,         "\\");
_LIT( KHtiCfg,              "hti.cfg" );
_LIT( KHtiBtCommCfg,        "HTIBtComm.cfg" );
_LIT( KHtiSerialCommCfg ,   "HTISerialComm.cfg" );
_LIT( KHtiIPCommCfg,        "HTIIPComm.cfg" );
// hti.cfg
_LIT8( KCommPlugin,         "CommPlugin" );
_LIT8( KPriority,           "Priority" );
_LIT8( KShowConsole,        "ShowConsole" );
_LIT8( KEnableHtiWatchDog,  "EnableHtiWatchDog" );
_LIT8( KEnableHtiAutoStart, "EnableHtiAutoStart" );
_LIT8( KShowErrorDialogs,   "ShowErrorDialogs" );
// HtiBtComm.cfg
_LIT8( KBtDeviceAddress,    "BtDeviceAddress" );
_LIT8( KBtDeviceName,       "BtDeviceName" );
// HtiSerialComm.cfg
_LIT8( KCommPortNumber,     "CommPort" );
// HTIIPComm.cfg
_LIT8( KLocalPort,          "LocalPort" );
_LIT8( KRemoteHost,         "RemoteHost" );
_LIT8( KRemotePort,         "RemotePort" );
_LIT8( KIAPName,            "IAPName" );


// ConstructL is called by the application framework
void CHtiAdminAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    iAppView = CHtiAdminAppView::NewL( ClientRect() );
    AddToStackL( iAppView );

    iAppView->SetCaption( KHtiFrameworkCaption );
    iAppView->SetVersionCaption( KHtiVersionCaption );
    iAppView->SetAutoStartCaption( KHtiAutoStartCaption );
    iAppView->SetSelectedCommCaption( KHtiSelectedCommCaption );
    UpdateVersion();
    UpdateStatusL();
    StartTimer();
    iHtiCfg = CHtiCfg::NewL();
    UpdateAutoStartStatus(); // uses iHtiCfg
    UpdateSelectedComm(); // uses iHtiCfg
    }

CHtiAdminAppUi::CHtiAdminAppUi()
    {
    iHtiCfg = NULL;
    }

CHtiAdminAppUi::~CHtiAdminAppUi()
    {
    KillTimer();
    if ( iAppView )
        {
        RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }

    if ( iHtiCfg )
        delete iHtiCfg;
    }

void CHtiAdminAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane)
    {
    HTI_LOG_FORMAT( "DynInitMenuPaneL 0x%x", aResourceId );

    if ( aResourceId == R_HTIADMIN_MENU )
        {
        RProcess prs;
        if ( OpenHtiProcess( prs ) )
            {
            if ( prs.ExitType() == EExitPending )
                {
                aMenuPane->SetItemDimmed( EHtiAdminStart, ETrue );
                aMenuPane->SetItemDimmed( EHtiAdminSubOptions, ETrue );
                prs.Close();
                return;
                }
            }
        aMenuPane->SetItemDimmed( EHtiAdminStop, ETrue );
        prs.Close();
        }

    else if ( aResourceId == R_HTIADMIN_SUBMENU_OPTIONS )
        {
#if !defined(__ENABLE_LOGGING__)
        aMenuPane->SetItemDimmed( EHtiAdminSubLogs, ETrue );
#endif
        }
    else if ( aResourceId == R_HTIADMIN_SUBMENU_PRIORITY )
        {
        TInt priority = 3; // default value
        TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
        if ( !err )
            {
            TRAP( err, priority = iHtiCfg->GetParameterIntL( KPriority ) );
            }

        switch ( priority )
            {
            case 1:
                aMenuPane->SetItemButtonState( EHtiAdminPriorityBackground, EEikMenuItemSymbolOn );
                break;
            case 2:
                aMenuPane->SetItemButtonState( EHtiAdminPriorityForeground, EEikMenuItemSymbolOn );
                break;
            case 4:
                aMenuPane->SetItemButtonState( EHtiAdminPriorityAbsoluteHigh, EEikMenuItemSymbolOn );
                break;
            default:
                aMenuPane->SetItemButtonState( EHtiAdminPriorityHigh, EEikMenuItemSymbolOn );
                break;
            }
        }

    else if ( aResourceId == R_HTIADMIN_SUBMENU_AUTOSTART )
        {
#ifdef __WINS__
        TInt value = 0; // default value for emulator
#else
        TInt value = 1; // default value for hardware
#endif
        TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
        if ( !err )
            {
            TRAP( err, value = iHtiCfg->GetParameterIntL( KEnableHtiAutoStart ) );
            }

        if ( value )
            aMenuPane->SetItemButtonState( EHtiAdminAutoStartEnable, EEikMenuItemSymbolOn );
        else
            aMenuPane->SetItemButtonState( EHtiAdminAutoStartDisable, EEikMenuItemSymbolOn );
        }

    else if ( aResourceId == R_HTIADMIN_SUBMENU_WATCHDOG )
        {
        TInt value = 0; // default value
        TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
        if ( !err )
            {
            TRAP( err, value = iHtiCfg->GetParameterIntL( KEnableHtiWatchDog ) );
            }

        if ( value )
            aMenuPane->SetItemButtonState( EHtiAdminWatchDogEnable, EEikMenuItemSymbolOn );
        else
            aMenuPane->SetItemButtonState( EHtiAdminWatchDogDisable, EEikMenuItemSymbolOn );
        }

    else if ( aResourceId == R_HTIADMIN_SUBMENU_CONSOLE )
        {
        TInt value = 0; // default value
        TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
        if ( !err )
            {
            TRAP( err, value = iHtiCfg->GetParameterIntL( KShowConsole ) );
            }

        if ( value )
            aMenuPane->SetItemButtonState( EHtiAdminConsoleEnable, EEikMenuItemSymbolOn );
        else
            aMenuPane->SetItemButtonState( EHtiAdminConsoleDisable, EEikMenuItemSymbolOn );
        }

    else if ( aResourceId == R_HTIADMIN_SUBMENU_ERROR_DIALOGS )
        {
        TInt value = 1; // default value
        TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
        if ( !err )
            {
            TRAP( err, value = iHtiCfg->GetParameterIntL( KShowErrorDialogs ) );
            }

        if ( value )
            aMenuPane->SetItemButtonState( EHtiAdminErrorDialogsEnable, EEikMenuItemSymbolOn );
        else
            aMenuPane->SetItemButtonState( EHtiAdminErrorDialogsDisable, EEikMenuItemSymbolOn );
        }
    }


// Helper function to get data from a dialog
TInt ShowDialog(TInt aResourceId, const TDesC &aText, TDes8 &aValue)
    {
    TBuf<KMaxParameterValueLength> data;

    CAknTextQueryDialog* dlg;
    dlg = new (ELeave) CAknTextQueryDialog(data);
    CleanupStack::PushL(dlg);
    dlg->SetPromptL(aText);
    dlg->SetMaxLength(KMaxParameterValueLength);
    CleanupStack::Pop(); // dlg

    if ( dlg->ExecuteLD( aResourceId ) )
        {
        aValue.Copy(data);
        return KErrNone;
        }

    return KErrCancel;
    }

void CHtiAdminAppUi::ShowBTListQueryL()
	{
	TInt selectedOption( KErrNotFound );
	        	 
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(20);
	CleanupStack::PushL(array);
	
	array->AppendL(_L("By BT address"));
	array->AppendL(_L("By BT Name"));
	array->AppendL(_L("Search when starting"));
				 
	CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog( &selectedOption );
	dlg->PrepareLC(R_QUERY_DIALOG);
	dlg->SetItemTextArray(array);
	dlg->SetOwnershipType(ELbmDoesNotOwnItemArray);
	dlg->RunLD();
	
	CleanupStack::PopAndDestroy(array); 
	
	if(selectedOption == 0)
		{
		HandleBtByAddress();
		}
	else if(selectedOption == 1)
		{
		HandleBtByName();
		}
	else if (selectedOption == 2)
		{
		HandleBtSearch();
		}
	}

void CHtiAdminAppUi::ShowIPListQueryL()
	{
	TInt selectedOption( KErrNotFound );
	        	 
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(10);
	CleanupStack::PushL(array);
	
	array->AppendL(_L("Listen"));
	array->AppendL(_L("Connect"));
	
	CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog( &selectedOption );
	dlg->PrepareLC(R_QUERY_DIALOG);
	dlg->SetItemTextArray(array);
	dlg->SetOwnershipType(ELbmDoesNotOwnItemArray);
	dlg->RunLD();
	
	CleanupStack::PopAndDestroy(array); 
	
	if(selectedOption == 0)
		{
		HandleIPListen();
		}
	else if(selectedOption == 1)
		{
		HandleIPConnect();
		}
	}

void CleanupRArray( TAny* object )
    {
    ( ( RImplInfoPtrArray* ) object )->ResetAndDestroy();
    }

void CHtiAdminAppUi::ShowCommsListQueryL()
	{
	TInt selectedOption( KErrNotFound );
	        	 
	const TInt pluginNameArrayGranularity (15);
	CDesCArrayFlat* pluginNameArray = new (ELeave) CDesCArrayFlat(pluginNameArrayGranularity);
	CleanupStack::PushL(pluginNameArray);
		
	RImplInfoPtrArray implInfoArray;
	CleanupStack::PushL( TCleanupItem( CleanupRArray, &implInfoArray ) );
	
	REComSession::ListImplementationsL( KHTICommInterfaceUid, implInfoArray );

	for ( TInt i = 0; i < implInfoArray.Count(); ++i )
		{
		pluginNameArray->AppendL(implInfoArray[i]->DisplayName());
		}
	
	CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog( &selectedOption );
	
	dlg->PrepareLC(R_QUERY_DIALOG);
	dlg->SetHeaderTextL(_L("Select communication"));
	dlg->SetItemTextArray(pluginNameArray);
	dlg->SetOwnershipType(ELbmDoesNotOwnItemArray);
	
	if(dlg->RunLD())
		{
		TInt32 uid = implInfoArray[selectedOption]->ImplementationUid().iUid;
		switch(uid)
			{
			case KHTIImplUidSerial:  
				{
				TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
				iHtiCfg->SetParameterL( KCommPlugin, _L8( "SERIAL" ) );
				iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

				TBuf8<KMaxParameterValueLength> portNum;
				if ( !ShowDialog( R_NUMERIC_QUERY, _L("Com port number:"), portNum ) )
					{
					TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiSerialCommCfg ) );
					iHtiCfg->SetParameterL( KCommPortNumber, portNum );
					iHtiCfg->SaveCfgL( KCfgFilePath, KHtiSerialCommCfg );
					}
				UpdateSelectedComm();
				}
				break;
				
			case KHTIImplUidBluetooth:  
				{
				ShowBTListQueryL();				
				}
				break;
				
			case KHTIImplUidIPComm:  
				{
				ShowIPListQueryL();
				}
				break;
			default:
				{
				//Just save the conf info for other plugins
				TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
				iHtiCfg->SetParameterL( KCommPlugin, implInfoArray[selectedOption]->DataType() );
				iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
				UpdateSelectedComm();
				}
			}
		}
	CleanupStack::PopAndDestroy(2, pluginNameArray);  
	}

void CHtiAdminAppUi::HandleBtByAddress()
	{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath,  KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8( "BTSERIAL" ) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiBtCommCfg ) );
	iHtiCfg->RemoveParameterL( KBtDeviceName );
	iHtiCfg->RemoveParameterL( KBtDeviceAddress );

	TBuf8<KMaxParameterValueLength> address;
	if ( !ShowDialog( R_TEXT_QUERY, _L("BT address:"), address) )
		{
		iHtiCfg->SetParameterL( KBtDeviceAddress, address);
		}

	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiBtCommCfg);
	UpdateSelectedComm();
	}

void CHtiAdminAppUi::HandleBtByName()
	{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8( "BTSERIAL" ) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiBtCommCfg ) );
	iHtiCfg->RemoveParameterL( KBtDeviceName );
	iHtiCfg->RemoveParameterL( KBtDeviceAddress );

	TBuf8<KMaxParameterValueLength> name;
	if ( !ShowDialog( R_TEXT_QUERY, _L( "BT name:" ), name ) )
		{
		iHtiCfg->SetParameterL( KBtDeviceName, name );
		}

	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiBtCommCfg );
	UpdateSelectedComm();
	}

void CHtiAdminAppUi::HandleBtSearch()
	{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8( "BTSERIAL" ) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiBtCommCfg ) );
	iHtiCfg->RemoveParameterL( KBtDeviceName );
	iHtiCfg->RemoveParameterL( KBtDeviceAddress );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiBtCommCfg );
	UpdateSelectedComm();
	}

void CHtiAdminAppUi::HandleIPListen()
	{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8("IPCOMM"));
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiIPCommCfg ) );
	iHtiCfg->RemoveParameterL( KRemotePort );
	iHtiCfg->RemoveParameterL( KRemoteHost );
	
	TBuf8<KMaxParameterValueLength> localPort;
	if ( !ShowDialog( R_TEXT_QUERY, _L( "Local port:" ), localPort ) )
		{
		iHtiCfg->SetParameterL( KLocalPort, localPort );
		ShowCommsDBListQueryL();
		}
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiIPCommCfg );
	UpdateSelectedComm();
	}
	
void CHtiAdminAppUi::HandleIPConnect()
	{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8( "IPCOMM" ) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiIPCommCfg ) );
	iHtiCfg->RemoveParameterL( KLocalPort );
	TBuf8<KMaxParameterValueLength> remoteHost;
	TBuf8<KMaxParameterValueLength> remotePort;
	if ( !ShowDialog( R_TEXT_QUERY, _L("Remote host:"), remoteHost ) )
		{
		iHtiCfg->SetParameterL( KRemoteHost, remoteHost );
		if ( !ShowDialog( R_TEXT_QUERY, _L( "Remote port:" ), remotePort ) )
			{
			iHtiCfg->SetParameterL( KRemotePort, remotePort );
			ShowCommsDBListQueryL();
			}
		}
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiIPCommCfg );
	UpdateSelectedComm();
	}
	

// handle any menu commands
void CHtiAdminAppUi::HandleCommandL(TInt aCommand)
    {
    TFileName cfgFile;

    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EHtiAdminStart:
            {
            HTI_LOG_TEXT( "Start" );
            RProcess prs;
            TInt err;
            TBool isRunning = EFalse;
            if ( OpenHtiProcess( prs ) )
                {
                if ( prs.ExitType() == EExitPending )
                    {
                    isRunning = ETrue;
                    _LIT(KText, "Already running");
                    iAppView->SetStatus( KText );
                    }
                prs.Close();
                }

            if ( !isRunning )
                {
                HTI_LOG_TEXT( "create process" );
                err = prs.Create( KHtiFrameworkExe, KHtiAdminStartParameter );
                if ( err == KErrNone )
                    {
                    prs.Resume();
                    prs.Close();
                    UpdateStatusL();
                    HTI_LOG_TEXT("create done");
                    }
                else
                    {
                    TBuf<64> msg;
                    msg.Format( _L("Error in starting: %d"), err );
                    iAppView->SetStatus( msg );
                    HTI_LOG_TEXT("create error");
                    }
                }
            }
            break;

        case EHtiAdminStop:
            {
            HTI_LOG_TEXT("Stop");

            // kill the watchdog to prevent it from restarting HTI again
            KillHtiWatchDogL();

            RProcess prs;
            if ( OpenHtiProcess( prs ) )
                {
                if ( prs.ExitType() == EExitPending )
                    {
                    HTI_LOG_TEXT("try to kill");
                    prs.Kill( KTerminateReason );
                    UpdateStatusL();
                    }
                else
                    {
                    _LIT(KText, "Already stopped");
                    iAppView->SetStatus( KText );
                    }
                prs.Close();
                }
            }
            break;

        case EHtiAdminAbout:
            {
            HTI_LOG_TEXT("About");
            TDateSuffix suffix( KHtiVersionDay - 1 );
            TMonthName month( ( TMonth ) ( KHtiVersionMonth - 1 ) );
            TBuf<64> msgPart1;
            msgPart1.Format( KHtiAdminAboutVersionFormat,
                    KHtiVersionMajor, KHtiVersionMinor, KHtiVersionBuild );
            msgPart1.AppendNum( KHtiVersionDay );
            msgPart1.Append( suffix );
            msgPart1.Append( ' ' );
            msgPart1.Append( month );
            msgPart1.Append( ' ' );
            msgPart1.AppendNum( KHtiVersionYear );
            msgPart1.Append( '.' );
            msgPart1.Append( ' ' );
            HBufC* msgPart2 = iEikonEnv->AllocReadResourceLC( R_HTIADMIN_ABOUT_TEXT );
            HBufC* fullMsg = HBufC::NewLC( msgPart1.Length() + msgPart2->Length() );
            fullMsg->Des().Append( msgPart1 );
            fullMsg->Des().Append( *msgPart2 );
            CAknMessageQueryDialog* dialog = new ( ELeave ) CAknMessageQueryDialog;
            dialog->PrepareLC( R_HTIADMIN_ABOUT_DIALOG );
            dialog->SetMessageTextL( *fullMsg );
            dialog->RunLD();
            CleanupStack::PopAndDestroy( 2 ); // fullMsg, msgPart2
            }
            break;

        case EHtiAdminSubOptions:
            break;

        case EHtiAdminSubLogs:
            break;

        case EHtiAdminCreateLogFolder:
#ifdef __ENABLE_LOGGING__
            {
            TInt err = KErrNone;
            RFs fs;
            err = fs.Connect();
            if ( err == KErrNone )
                {
                err = fs.MkDirAll(_L("c:\\logs\\hti\\"));
                }
            fs.Close();
            }
#endif
            break;

        case EHtiAdminRemoveLogFolder:
#ifdef __ENABLE_LOGGING__
            {
            TInt err = KErrNone;
            RFs fs;
            err = fs.Connect();
            if ( err == KErrNone )
                {
                CFileMan *fileman = CFileMan::NewL( fs );
                fileman->RmDir( _L( "c:\\logs\\hti\\" ) );
                delete fileman;
                }
            fs.Close();
            }
#endif
            break;

        case EHtiAdminClearLogs:
#ifdef __ENABLE_LOGGING__
            {
            TInt err = KErrNone;
            RFs fs;
            err = fs.Connect();
            if ( err == KErrNone )
                {
                CFileMan *fileman = CFileMan::NewL( fs );
                fileman->Delete( _L( "c:\\logs\\hti\\*" ) );
                fileman->Delete( _L( "c:\\HTI_*.log" ) );
                delete fileman;
                }
            fs.Close();
            }
#endif
            break;

        case EHtiAdminSubComms:
        	{
        	ShowCommsListQueryL();
        	}
            break;

        case EHtiAdminSubPriority:
            break;

        case EHtiAdminPriorityBackground:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KPriority, _L8("1"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminPriorityForeground:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KPriority, _L8("2"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminPriorityHigh:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KPriority, _L8("3"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminPriorityAbsoluteHigh:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KPriority, _L8("4"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminSubAutoStart:
            break;

        case EHtiAdminAutoStartEnable:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KEnableHtiAutoStart, _L8("1"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            UpdateAutoStartStatus();
            break;

        case EHtiAdminAutoStartDisable:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KEnableHtiAutoStart, _L8("0"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            UpdateAutoStartStatus();
            break;

        case EHtiAdminSubWatchDog:
            break;

        case EHtiAdminWatchDogEnable:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KEnableHtiWatchDog, _L8("1"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminWatchDogDisable:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KEnableHtiWatchDog, _L8("0"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminSubConsole:
            break;

        case EHtiAdminConsoleEnable:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KShowConsole, _L8("1"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminConsoleDisable:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KShowConsole, _L8("0"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminErrorDialogsEnable:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KShowErrorDialogs, _L8("1"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminErrorDialogsDisable:
            TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
            iHtiCfg->SetParameterL( KShowErrorDialogs, _L8("0"));
            iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
            break;

        case EHtiAdminSubSetParam:
            break;

        case EHtiAdminHtiCfg:
            cfgFile = KHtiCfg;

        case EHtiAdminHtiBtCommCfg:
            if ( !cfgFile.Length() )
                cfgFile = KHtiBtCommCfg;

        case EHtiAdminHtiIPCommCfg:
            if ( !cfgFile.Length() )
                cfgFile = KHtiIPCommCfg;

        case EHtiAdminHtiSerialCommCfg:
            {
            if ( !cfgFile.Length() )
                cfgFile = KHtiSerialCommCfg;

            TBuf8<KMaxParameterNameLength> parameterName;
            if ( !ShowDialog( R_TEXT_QUERY, _L("Parameter name:"), parameterName) )
                {
                TBuf8<KMaxParameterValueLength> parameterValue;
                if ( !ShowDialog( R_TEXT_QUERY, _L("Parameter value:"), parameterValue ) )
                    {
                    TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, cfgFile ) );
                    iHtiCfg->SetParameterL( parameterName, parameterValue );
                    iHtiCfg->SaveCfgL( KCfgFilePath, cfgFile);
                    }
                }
            }
            break;

        default:
        	{
			CAknErrorNote* dialog = new(ELeave)CAknErrorNote(ETrue);			 
			dialog->ExecuteLD(_L("Unknown Command Recieved"));
			}
        	break;
        }
    }

void CHtiAdminAppUi::HandleForegroundEventL( TBool aForeground )
    {
    CAknAppUi::HandleForegroundEventL( aForeground );

    if ( aForeground )
        {
        UpdateStatusL();
        UpdateAutoStartStatus();
        StartTimer();
        }
    else
        {
        KillTimer();
        }
    }

TBool CHtiAdminAppUi::OpenHtiProcess( RProcess& aPrs )
    {
    HTI_LOG_TEXT( "OpenHtiProcess" );

    TFullName processName;
    TInt err;

    TFindProcess finder( KHtiMatchPattern );
    err = finder.Next( processName );
    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "try to open..." );
        err = aPrs.Open( finder );
        HTI_LOG_TEXT( "opened" );

        if ( err == KErrNone )
            {
            HTI_LOG_TEXT( "OpenHtiProcess OK" );
            return ETrue;
            }
        else
            {
            HTI_LOG_TEXT( "Failed to open process" );
            TBuf<64> msg;
            msg.Format( _L( "Error: %d" ), err );
            iAppView->SetStatus( msg );
            }
        }
    else if ( err == KErrNotFound )
        {
        HTI_LOG_TEXT( "Not found" );
        _LIT( KNotFound, "Stopped" );
        iAppView->SetStatus( KNotFound );
        }
    else
        {
        TBuf<64> msg;
        msg.Format( _L( "Error: %d" ), err );
        iAppView->SetStatus( msg );
        }

    HTI_LOG_TEXT( "OpenHtiProcess NOK" );

    return EFalse;
    }


void CHtiAdminAppUi::UpdateVersion()
    {
    HTI_LOG_TEXT( "UpdateVersion" );
    TBuf<128> tmp;
    tmp.Format( KHtiAdminVersionFormat, KHtiVersionMajor, KHtiVersionMinor,
        KHtiVersionBuild, KHtiVersionYear, KHtiVersionWeek );
     iAppView->SetVersion( tmp );
    }


void CHtiAdminAppUi::UpdateStatusL()
    {
    HTI_LOG_TEXT( "UpdateStatusL" );
    RProcess prs;
    if ( OpenHtiProcess( prs ) )
        {
        HTI_LOG_TEXT( "Update display" );
        TExitType exT = prs.ExitType();

        switch ( exT )
            {
            case EExitPending:
                {
                _LIT( KTxt, "Running" );
                iAppView->SetStatus( KTxt );
                }
                break;

            case EExitKill:
            case EExitTerminate:
                {
                _LIT( KTxt, "Stopped" );
                iAppView->SetStatus( KTxt );
                }
                break;

            case EExitPanic:
                {
                _LIT( KTxt, "Panic" );
                iAppView->SetStatus( KTxt );
                }
                break;
            };
        prs.Close();
        }
    }

void CHtiAdminAppUi::UpdateAutoStartStatus()
    {
    HTI_LOG_TEXT( "UpdateAutoStartStatus" );
    TInt value = KErrNotFound;
    TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
    if ( !err )
        {
        TRAP( err, value = iHtiCfg->GetParameterIntL(KEnableHtiAutoStart) );
        }

    if ( value == KErrNotFound )
        {
        // Use default values if not found from config.
#ifdef __WINS__
        value = 0;
#else
        value = 1;
#endif
        }

    if ( value == 0 )
        {
        _LIT( KTxt, "Disabled" );
        iAppView->SetAutoStartStatus( KTxt );
        }
    else if ( value == 1 )
        {
        _LIT( KTxt, "Enabled" );
        iAppView->SetAutoStartStatus( KTxt );
        }
    else
        {
        _LIT( KTxt, "Unknown" );
        iAppView->SetAutoStartStatus( KTxt );
        }
    }

void CHtiAdminAppUi::UpdateSelectedComm()
    {
    HTI_LOG_TEXT( "UpdateSelectedComm" );
    TBuf<64> commPlugin;
    TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
    if ( !err )
        {
        TBuf8<KMaxParameterValueLength> commValue;
        TRAP( err, commValue = iHtiCfg->GetParameterL( KCommPlugin ) );
        if ( err == KErrNone )
            {
            commPlugin.Copy( commValue );
            }
        }
    // Use default if not found from config
    if ( commPlugin.Length() == 0 )
        {
        commPlugin.Copy(KCommDefaultImplementation  );        
        }
    iAppView->SetSelectedComm( commPlugin );

    if ( commPlugin.Compare( _L( "SERIAL" ) ) == 0 )
        {
        TInt portNum = 0;
        TRAP( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiSerialCommCfg ) );
        if ( err == KErrNone )
            {
            TRAP( err, portNum = iHtiCfg->GetParameterIntL( KCommPortNumber ) );
            }
        TBuf<16> portBuf;
        portBuf.Copy( _L( "Port: " ) );
        portBuf.AppendNum( portNum );
        iAppView->SetCommDetails( portBuf );
        }

    else if ( commPlugin.Compare( _L( "BTSERIAL" ) ) == 0 )
        {
        TBuf<64> hostBuf;
        TRAP( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiBtCommCfg ) );
        if ( err == KErrNone )
            {
            TBuf8<KMaxParameterValueLength> hostValue;
            TRAP( err, hostValue = iHtiCfg->GetParameterL( KBtDeviceAddress ) );
            if ( err != KErrNone )
                {
                TRAP( err, hostValue = iHtiCfg->GetParameterL( KBtDeviceName ) );
                }
            if ( err == KErrNone )
                {
                hostBuf.Copy( hostValue );
                }
            }
        if ( hostBuf.Length() > 0 )
            {
            hostBuf.Insert( 0, _L( "Host: " ) );
            }
        iAppView->SetCommDetails( hostBuf );
        }

    else if ( commPlugin.Compare( _L( "IPCOMM" ) ) == 0 )
        {
        TBuf<128> hostBuf;
        TRAP( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiIPCommCfg ) );
        if ( err == KErrNone )
            {
            TInt localPort = 0;
            TRAP( err, localPort = iHtiCfg->GetParameterIntL( KLocalPort ) );
            if ( err == KErrNone && localPort > 0 )
                {
                hostBuf.Copy( _L( "Listen: " ) );
                hostBuf.AppendNum( localPort );
                }
            else
                {
                TBuf8<KMaxParameterValueLength> remoteHost;
                TInt remotePort = 0;
                TRAP( err, remoteHost = iHtiCfg->GetParameterL( KRemoteHost ) );
                if ( err == KErrNone )
                    {
                    TRAP( err, remotePort = iHtiCfg->GetParameterIntL(
                            KRemotePort ) );
                    }
                if ( err == KErrNone )
                    {
                    hostBuf.Copy( remoteHost );
                    hostBuf.Insert( 0, _L("Connect: " ) );
                    hostBuf.Append( ':' );
                    hostBuf.AppendNum( remotePort );
                    }
                else
                    {
                    hostBuf.Copy( _L( "Invalid config!" ) );
                    }
                }
            }
        iAppView->SetCommDetails( hostBuf );
        }

    else
        {
        iAppView->SetCommDetails( KNullDesC );
        }
    }

void CHtiAdminAppUi::KillHtiWatchDogL()
    {
    TFullName processName;
    TFindProcess finder( KHtiWatchDogMatchPattern );
    TInt err = finder.Next( processName );
    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "HTI watchdog process found. Trying to open and kill it..." );
        RProcess prs;
        User::LeaveIfError( prs.Open( finder ) );
        prs.Kill( KTerminateReason );
        prs.Close();
        HTI_LOG_TEXT( "HTI watchdog killed" );
        }
    }

void CHtiAdminAppUi::StartTimer()
    {
    // start timer to watch the status
    if ( !iPeriodic )
        {
        iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle );
        iPeriodic->Start( 1 * 1000 * 1000,
                          10 * 1000 * 1000,
                          TCallBack( TimerCallBackL, this ) );
        }
    }

void CHtiAdminAppUi::KillTimer()
    {
    if ( iPeriodic )
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        iPeriodic = NULL;
        }
    }

TInt CHtiAdminAppUi::TimerCallBackL( TAny* aPtr )
    {
    ( (CHtiAdminAppUi*)aPtr )->UpdateStatusL();
    return ETrue;
    }

void CHtiAdminAppUi::ShowCommsDBListQueryL()
    {
    TInt selectedOption( KErrNotFound );
    TBuf<KMaxParameterValueLength> name;
                 
    CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(16);
    CleanupStack::PushL(array);

    // Get IAP names from the database
    CCommsDatabase* db = CCommsDatabase::NewL(EDatabaseTypeIAP);;
    CleanupStack::PushL(db);
    db->ShowHiddenRecords();
    CCommsDbTableView* view = db->OpenTableLC(TPtrC(IAP));
    TInt res = view->GotoFirstRecord();

    while(res == KErrNone)
        {
        view->ReadTextL(TPtrC(COMMDB_NAME), name);
        array->InsertL(0, name);
        res = view->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy(view); //view
    CleanupStack::PopAndDestroy(db); //db    

    // Show IAP selection dialog
    CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog(&selectedOption);
    dlg->PrepareLC(R_SELECT_IAP_DIALOG);
    dlg->SetItemTextArray(array);
    dlg->SetOwnershipType(ELbmDoesNotOwnItemArray);
    dlg->RunLD();

    if(selectedOption != KErrNotFound)
        {
        // Write selected IAP name to config file
        name = array->MdcaPoint(selectedOption); 
        TBuf8<KMaxParameterValueLength> iapName;
        iapName.Copy(name);
        iHtiCfg->SetParameterL(KIAPName, iapName);
        }
    CleanupStack::PopAndDestroy(array); 
    }
// End of file
