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
* Description:  Implementation of HtiAdmin main.
*
*/

#include <coecntrl.h>
#include <w32std.h>
#include <commdb.h>

#include <HtiCfg.h>
#include <HtiCommPluginInterface.h> // for KHTICommInterfaceUid


#include "htienginewrapper.h"
#include "chtiadminengine.h"


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

const TInt32 KHTIImplUidIsaComm   = {0x1020DEBD};
const TInt32 KHTIImplUidSerial    = {0x10210CCA};
const TInt32 KHTIImplUidBluetooth = {0x200212CC};
const TInt32 KHTIImplUidIPComm    = {0x200212CE};
const TInt32 KHTIImplUidUsbSerialComm = {0x200212D0};
const TInt32 KHTIImplUidTraceComm = {0x200212D8};

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
_LIT8( KIAPName,            "IAPName");


// ---------------------------------------------------------------------------

CHtiAdminEngine::CHtiAdminEngine()
{
    iIAPsArray = NULL;
}

// ---------------------------------------------------------------------------
CHtiAdminEngine* CHtiAdminEngine::NewLC(HtiEngineWrapper *aEngineWrapper)
    {
    CHtiAdminEngine* self = new (ELeave) CHtiAdminEngine();
    CleanupStack::PushL(self);
    self->ConstructL(aEngineWrapper);
    return self;
    }

// ---------------------------------------------------------------------------
CHtiAdminEngine* CHtiAdminEngine::NewL(HtiEngineWrapper *aEngineWrapper)
    {
    CHtiAdminEngine* self = CHtiAdminEngine::NewLC(aEngineWrapper);
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::ConstructL(HtiEngineWrapper *aEngineWrapper)
    {
    iEngineWrapper = aEngineWrapper;
    iHtiCfg = CHtiCfg::NewL();
    
    CheckStatusesL();
    StartTimer();
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::CheckStatusesL()
    {
    UpdateConsoleStatus();
    UpdateWatchDogStatus();
    UpdateAutoStartStatus(); 
    UpdateSelectedComm();
    UpdateStatusL();
    }


// ---------------------------------------------------------------------------
CHtiAdminEngine::~CHtiAdminEngine()
{
	KillTimer();	
	delete iHtiCfg;
	delete iPluginNameArray;	
	delete iPluginDataTypeArray;	
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::StartHtiL()
{

	RProcess prs;
	TInt err;
	TBool isRunning = EFalse;
	if ( OpenHtiProcess( prs ) )
		{
		if ( prs.ExitType() == EExitPending )
			{
			isRunning = ETrue;
			iEngineWrapper->updateStatus(HtiEngineWrapper::Running);
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
			iEngineWrapper->updateStatus(HtiEngineWrapper::Error);
			HTI_LOG_TEXT("create error");
			}
		}

}


// ---------------------------------------------------------------------------

void CHtiAdminEngine::StopHtiL()
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
			iEngineWrapper->updateStatus(HtiEngineWrapper::Stopped);
			}
		prs.Close();
		}

}

// ---------------------------------------------------------------------------

void CHtiAdminEngine::EnableBtByAddressL(const TDesC& aAddress)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath,  KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8( "BTSERIAL" ) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiBtCommCfg ) );
	iHtiCfg->RemoveParameterL( KBtDeviceName );
	iHtiCfg->RemoveParameterL( KBtDeviceAddress );
	
	TBuf8<KMaxParameterValueLength> value;
	value.Copy(aAddress);
	iHtiCfg->SetParameterL( KBtDeviceAddress, value);

	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiBtCommCfg);
	UpdateSelectedComm();

}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::EnableBtByNameL(const TDesC& aAddress)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8( "BTSERIAL" ) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiBtCommCfg ) );
	iHtiCfg->RemoveParameterL( KBtDeviceName );
	iHtiCfg->RemoveParameterL( KBtDeviceAddress );

	TBuf8<KMaxParameterValueLength> value;
	value.Copy(aAddress);
	iHtiCfg->SetParameterL( KBtDeviceName, value);
	
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiBtCommCfg );
	UpdateSelectedComm();
}


// ---------------------------------------------------------------------------
void CHtiAdminEngine::BtSearchL()
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

// ---------------------------------------------------------------------------
void CHtiAdminEngine::IPListenL(const TDesC& aPort, const TDesC& aIAP)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8("IPCOMM"));
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiIPCommCfg ) );
	
	TBuf8<KMaxParameterValueLength> port;
	port.Copy(aPort);
	iHtiCfg->SetParameterL( KLocalPort, port);
	
	TBuf8<KMaxParameterValueLength> iap;
	iap.Copy(aIAP);
	iHtiCfg->SetParameterL(KIAPName, iap);

	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiIPCommCfg );
	UpdateSelectedComm();

}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::IPConnectL(const TDesC& aHost, const TDesC& aPort, const TDesC& aIAP)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8( "IPCOMM" ) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiIPCommCfg ) );
	iHtiCfg->RemoveParameterL( KLocalPort );

	TBuf8<KMaxParameterValueLength> value;
	value.Copy(aHost);
	iHtiCfg->SetParameterL( KRemoteHost, value);
	
	TBuf8<KMaxParameterValueLength> port;
    port.Copy(aPort);
    iHtiCfg->SetParameterL(KRemotePort, port);

    TBuf8<KMaxParameterValueLength> iap;
    iap.Copy(aIAP);
    iHtiCfg->SetParameterL(KIAPName, iap);

    iHtiCfg->SaveCfgL(KCfgFilePath, KHtiIPCommCfg);
    UpdateSelectedComm();
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::EnableSerialL(const TDesC& aComPortNumber)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KCommPlugin, _L8( "SERIAL" ) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
	
	TBuf8<KMaxParameterValueLength> value;
	value.Copy(aComPortNumber);
	iHtiCfg->SetParameterL( KCommPortNumber, value );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiSerialCommCfg );
	UpdateSelectedComm();
}


// ---------------------------------------------------------------------------
void CHtiAdminEngine::EnableOtherCommL(const TDesC& aSelectedPluginName)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	
	TInt namePosition;
	iPluginNameArray->Find(aSelectedPluginName, namePosition);
	
	//Save correct data type for selected plugin
	iHtiCfg->SetParameterL( KCommPlugin, iPluginDataTypeArray->MdcaPoint(namePosition) );
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
	UpdateSelectedComm();
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetPriorityBackgroundL()
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KPriority, _L8("1"));
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetPriorityForegroundL()
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KPriority, _L8("2"));
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetPriorityHighL()
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KPriority, _L8("3"));
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );

}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetPriorityAbsoluteHighL()
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	iHtiCfg->SetParameterL( KPriority, _L8("4"));
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::AutoStartEnableL(TBool aEnableAutoStart)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
	if(aEnableAutoStart)
		{
		iHtiCfg->SetParameterL( KEnableHtiAutoStart, _L8("1"));
		}
	else
		{
		iHtiCfg->SetParameterL( KEnableHtiAutoStart, _L8("0"));
		}
	
	iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
	UpdateAutoStartStatus();
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::WatchDogEnableL(TBool aEnableWDog)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
    
	if(aEnableWDog)
		{
		iHtiCfg->SetParameterL( KEnableHtiWatchDog, _L8("1"));
		}
	else
		{
		iHtiCfg->SetParameterL( KEnableHtiWatchDog, _L8("0"));
		}
    
    iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
    UpdateWatchDogStatus();
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::ConsoleEnableL(TBool aEnableConsole)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
    
	if(aEnableConsole)
		{
		iHtiCfg->SetParameterL( KShowConsole, _L8("1"));
		}
	else
		{
		iHtiCfg->SetParameterL( KShowConsole, _L8("0"));
		}
    
    iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
    UpdateConsoleStatus();
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::ErrorDialogsEnableL(TBool aEnableErrorDialogs)
{
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
    
	if(aEnableErrorDialogs)
		{
		iHtiCfg->SetParameterL( KShowErrorDialogs, _L8("1"));
		}
	else
		{
		iHtiCfg->SetParameterL( KShowErrorDialogs, _L8("0"));
		}
    
    iHtiCfg->SaveCfgL( KCfgFilePath, KHtiCfg );
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetHtiCfgParamL(const TDesC& aParamName, const TDesC& aParamValue)
{
	SetCfgParamL(aParamName, aParamValue, KHtiCfg);
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetBtCfgParamL(const TDesC& aParamName, const TDesC& aParamValue)
{
	SetCfgParamL(aParamName, aParamValue, KHtiBtCommCfg);
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetSerialCfgParamL(const TDesC& aParamName, const TDesC& aParamValue)
{
	SetCfgParamL(aParamName, aParamValue, KHtiSerialCommCfg);
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetIPCfgParamL(const TDesC& aParamName, const TDesC& aParamValue)
{
	SetCfgParamL(aParamName, aParamValue, KHtiIPCommCfg);
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::SetCfgParamL(const TDesC& aParamName, const TDesC& aParamValue, const TDesC& aFileName)
{
	TBuf8<KMaxParameterNameLength> name;
	name.Copy(aParamName);
	
	TBuf8<KMaxParameterValueLength> value;
	value.Copy(aParamValue);
	
	TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, aFileName ) );
	
	iHtiCfg->SetParameterL( name, value );
	iHtiCfg->SaveCfgL( KCfgFilePath, aFileName);
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::GetHtiCfgParamL(const TDesC& aParamName, TDes& aParamValue)
    {
    GetCfgParamL(aParamName, aParamValue, KHtiCfg);
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::GetBtCfgParamL(const TDesC& aParamName, TDes& aParamValue)
    {
    GetCfgParamL(aParamName, aParamValue, KHtiBtCommCfg);
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::GetSerialCfgParamL(const TDesC& aParamName, TDes& aParamValue)
    {
    GetCfgParamL(aParamName, aParamValue, KHtiSerialCommCfg);
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::GetIPCfgParamL(const TDesC& aParamName, TDes& aParamValue)
    {
    GetCfgParamL(aParamName, aParamValue, KHtiIPCommCfg);
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::GetCfgParamL(const TDesC& aParamName, TDes& aParamValue, const TDesC& aFileName)
    {
    TBuf8<KMaxParameterNameLength> name;
    name.Copy(aParamName);
    
    TBuf<KMaxParameterValueLength> value;

    TRAP_IGNORE( iHtiCfg->LoadCfgL( KCfgFilePath, aFileName ) );
    aParamValue.Copy(iHtiCfg->GetParameterL(name));
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::GetSelectedComm(TDes& aPluginName)
    {
    TBuf8<KMaxParameterValueLength> commValue;
    TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
    if ( !err )
        {
        TRAP( err, commValue = iHtiCfg->GetParameterL( KCommPlugin ) );
        }
    // Use default if not found from config
    if ( commValue.Length() == 0 )
        {
        commValue.Copy(KCommDefaultImplementation  );        
        }
    TInt typePosition;
    iPluginDataTypeArray->Find(commValue, typePosition);
    aPluginName.Copy(iPluginNameArray->MdcaPoint(typePosition));
    }

// ---------------------------------------------------------------------------
TBool CHtiAdminEngine::OpenHtiProcess( RProcess& aPrs )
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
			iEngineWrapper->updateStatus(HtiEngineWrapper::Error);
			}
		}
	else if ( err == KErrNotFound )
		{
		iEngineWrapper->updateStatus(HtiEngineWrapper::Stopped);
		}
	else
		{
		iEngineWrapper->updateStatus(HtiEngineWrapper::Error);
		}

	HTI_LOG_TEXT( "OpenHtiProcess NOK" );

	return EFalse;
}

// ---------------------------------------------------------------------------
void CHtiAdminEngine::UpdateStatusL()
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
                iEngineWrapper->updateStatus(HtiEngineWrapper::Running);
                }
                break;

            case EExitKill:
            case EExitTerminate:
                {
                iEngineWrapper->updateStatus(HtiEngineWrapper::Stopped);
                }
                break;

            case EExitPanic:
                {
                iEngineWrapper->updateStatus(HtiEngineWrapper::Panic);
                }
                break;
            };
        prs.Close();
        }
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::KillHtiWatchDogL()
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

// ---------------------------------------------------------------------------
void CHtiAdminEngine::StartTimer()
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
// ---------------------------------------------------------------------------
void CHtiAdminEngine::KillTimer()
    {
    if ( iPeriodic )
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        iPeriodic = NULL;
        }
    }

// ---------------------------------------------------------------------------
TInt CHtiAdminEngine::TimerCallBackL( TAny* aPtr )
    {
    ( (CHtiAdminEngine*)aPtr )->UpdateStatusL();
    return ETrue;
    }


// ---------------------------------------------------------------------------
void CHtiAdminEngine::UpdateAutoStartStatus()
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
        iEngineWrapper->updateAutostartStatus( HtiEngineWrapper::AutoStartDisabled );
        }
    else if ( value == 1 )
        {
        iEngineWrapper->updateAutostartStatus( HtiEngineWrapper::AutoStartEnabled );
        }
    else
        {
        iEngineWrapper->updateAutostartStatus( HtiEngineWrapper::AutoStartUnknown );
        }
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::UpdateConsoleStatus()
    {
    HTI_LOG_TEXT( "UpdateConsoleStatus" );
    TInt value = KErrNotFound;
    TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
    if(!err)
        {
        TRAP( err, value = iHtiCfg->GetParameterIntL(KShowConsole) );
        }
    
    if ( value == KErrNotFound )
        {
        value = 0;
        }

    iEngineWrapper->updateConsoleStatus(value);
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::UpdateWatchDogStatus()
    {
    HTI_LOG_TEXT( "UpdateWatchDogStatus" );
    TInt value = KErrNotFound;
    TRAPD( err, iHtiCfg->LoadCfgL( KCfgFilePath, KHtiCfg ) );
    if(!err)
        {
        TRAP( err, value = iHtiCfg->GetParameterIntL(KEnableHtiWatchDog) );
        }
    
    if ( value == KErrNotFound )
        {
        value = 0;
        }

    iEngineWrapper->updateWatchDogStatus(value);    
    }

// ---------------------------------------------------------------------------
void CHtiAdminEngine::UpdateSelectedComm()
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
		iEngineWrapper->updateSelectedComm( commPlugin );

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
        iEngineWrapper->updateCommDetails( portBuf );
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
        iEngineWrapper->updateCommDetails(hostBuf);
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
        iEngineWrapper->updateCommDetails(hostBuf);
        }

    else
        {
        iEngineWrapper->updateCommDetails(KNullDesC);
        }
    }

// ---------------------------------------------------------------------------
void CleanupRArray( TAny* object )
    {
    ( ( RImplInfoPtrArray* ) object )->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
CDesCArray* CHtiAdminEngine::SearchCommPluginsL()
    {
    const TInt pluginNameArrayGranularity (15);
	iPluginNameArray = new (ELeave) CDesCArrayFlat(pluginNameArrayGranularity);
	iPluginDataTypeArray = new (ELeave) CDesC8ArrayFlat(pluginNameArrayGranularity);
			
	RImplInfoPtrArray implInfoArray;
	CleanupStack::PushL( TCleanupItem( CleanupRArray, &implInfoArray ) );
	
	REComSession::ListImplementationsL( KHTICommInterfaceUid, implInfoArray );

	for ( TInt i = 0; i < implInfoArray.Count(); ++i )
		{
		iPluginNameArray->AppendL(implInfoArray[i]->DisplayName());
		iPluginDataTypeArray->AppendL(implInfoArray[i]->DataType());
		}
    
	CleanupStack::PopAndDestroy(); //implInfoArray
		
	return iPluginNameArray;
    }

// ---------------------------------------------------------------------------
CDesCArray* CHtiAdminEngine::SearchIAPsL()
    {
    if(iIAPsArray)
        {
        return iIAPsArray;
        }
    
    TBuf<KMaxParameterValueLength> name;
    const TInt arrayGranularity (10);
    iIAPsArray = new (ELeave) CDesCArrayFlat(arrayGranularity);

    // Get IAP names from the database
    CCommsDatabase* db = CCommsDatabase::NewL(EDatabaseTypeIAP);;
    CleanupStack::PushL(db);
    db->ShowHiddenRecords();
    CCommsDbTableView* view = db->OpenTableLC(TPtrC(IAP));
    TInt res = view->GotoFirstRecord();

    while(res == KErrNone)
        {
        view->ReadTextL(TPtrC(COMMDB_NAME), name);
        iIAPsArray->InsertL(0, name);
        res = view->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy(view); //view
    CleanupStack::PopAndDestroy(db); //db   
    
    return iIAPsArray;
    }
