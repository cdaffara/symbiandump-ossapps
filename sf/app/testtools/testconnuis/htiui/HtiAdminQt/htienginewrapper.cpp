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


#include "htienginewrapper.h"
#include "chtiadminengine.h"

#include <coecntrl.h>
#include <QString>
#include <qlocale.h>


#include <HtiVersion.h>



// ---------------------------------------------------------------------------

HtiEngineWrapper::HtiEngineWrapper():
	iEngine(0)
{
}
// ---------------------------------------------------------------------------	
HtiEngineWrapper::~HtiEngineWrapper()
{
	delete iEngine;
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::init()
{
    TRAPD(error, iEngine = CHtiAdminEngine::NewL(this));
    
    if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::startHti()
{
	TRAPD(error, iEngine->StartHtiL());
	
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::stopHti()
{
	TRAPD(error, iEngine->StopHtiL());
	
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::enableBtByAddress(QString& address)
{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedAddress(address.utf16());
	
	
	TRAPD(error, iEngine->EnableBtByAddressL(convertedAddress));
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::ipListen(QString& port, QString& iap)
{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedPort(port.utf16());
	TBuf<64> convertedIAP(iap.utf16());
	
	TRAPD(error, iEngine->IPListenL(convertedPort, convertedIAP));
		
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
	
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::ipConnect(QString& host, QString& port, QString& iap)
{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedHostAddress(host.utf16());
	TBuf<64> convertedPort(port.utf16());
	TBuf<64> convertedIAP(iap.utf16());
		
	TRAPD(error, iEngine->IPConnectL(convertedHostAddress, convertedPort, convertedIAP));
		
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
	
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::enableBtByName(QString& address)
	{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedAddress(address.utf16());
	
	
	TRAPD(error, iEngine->EnableBtByNameL(convertedAddress));
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::btSearch()
	{
	TRAPD(error, iEngine->BtSearchL());
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::enableSerial(QString& commPort)
{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedText(commPort.utf16());
		
	TRAPD(error, iEngine->EnableSerialL(convertedText));
		
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
	
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::enableOtherComm(QString& pluginName)
	{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedText(pluginName.utf16());
	
	TRAPD(error, iEngine->EnableOtherCommL(convertedText));
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::setPriorityBackground()
	{
	TRAPD(error, iEngine->SetPriorityBackgroundL());
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::setPriorityForeground()
	{
	TRAPD(error, iEngine->SetPriorityForegroundL());
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::setPriorityHigh()
	{
	TRAPD(error, iEngine->SetPriorityHighL());
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::setPriorityAbsoluteHigh()
	{
	TRAPD(error, iEngine->SetPriorityAbsoluteHighL());
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::watchDogEnable(bool enable)
	{
	TRAPD(error, iEngine->WatchDogEnableL(enable));
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	}


// ---------------------------------------------------------------------------
bool HtiEngineWrapper::autoStartEnable(bool enable)
	{
	TRAPD(error, iEngine->AutoStartEnableL(enable));
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::consoleEnable(bool enable)
	{
	TRAPD(error, iEngine->ConsoleEnableL(enable));
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::errorDialogsEnable(bool enable)
	{
	TRAPD(error, iEngine->ErrorDialogsEnableL(enable));
		
		if (error != KErrNone) {
			return false;
		}
		else {
			return true;
		}
	}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::setHtiCfgParam(QString& paramName, QString& paramValue)
{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedName(paramName.utf16());
	TBuf<64> convertedValue(paramValue.utf16());
		
	TRAPD(error, iEngine->SetHtiCfgParamL(convertedName, convertedValue));
		
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::getHtiCfgParam(QString& paramName, QString& paramValue)
{
    // A conversion between a TBuf and QString is needed here.
    TBuf<64> convertedName(paramName.utf16());
    TBuf<64> value;
        
    TRAPD(error, iEngine->GetHtiCfgParamL(convertedName, value));
        
    paramValue = QString((QChar*)value.Ptr(),value.Length());
    if (error != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::setBtCfgParam(QString& paramName, QString& paramValue)
{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedName(paramName.utf16());
	TBuf<64> convertedValue(paramValue.utf16());
		
	TRAPD(error, iEngine->SetBtCfgParamL(convertedName, convertedValue));
		
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::getBtCfgParam(QString& paramName, QString& paramValue)
{
    // A conversion between a TBuf and QString is needed here.
    TBuf<64> convertedName(paramName.utf16());
    TBuf<64> value;
        
    TRAPD(error, iEngine->GetBtCfgParamL(convertedName, value));
    paramValue = QString((QChar*)value.Ptr(),value.Length());    
    if (error != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::setSerialCfgParam(QString& paramName, QString& paramValue)
{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedName(paramName.utf16());
	TBuf<64> convertedValue(paramValue.utf16());
		
	TRAPD(error, iEngine->SetSerialCfgParamL(convertedName, convertedValue));
		
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::getSerialCfgParam(QString& paramName, QString& paramValue)
{
    // A conversion between a TBuf and QString is needed here.
    TBuf<64> convertedName(paramName.utf16());
    TBuf<64> value;
        
    TRAPD(error, iEngine->GetSerialCfgParamL(convertedName, value));
    paramValue = QString((QChar*)value.Ptr(),value.Length());
    
    if (error != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::setIPCfgParam(QString& paramName, QString& paramValue)
{
	// A conversion between a TBuf and QString is needed here.
	TBuf<64> convertedName(paramName.utf16());
	TBuf<64> convertedValue(paramValue.utf16());
		
	TRAPD(error, iEngine->SetIPCfgParamL(convertedName, convertedValue));
		
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::getIPCfgParam(QString& paramName, QString& paramValue)
{
    // A conversion between a TBuf and QString is needed here.
    TBuf<64> convertedName(paramName.utf16());
    TBuf<64> value;
        
    TRAPD(error, iEngine->GetIPCfgParamL(convertedName, value));
    paramValue = QString((QChar*)value.Ptr(),value.Length());    
    if (error != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------
void HtiEngineWrapper::getVersionIfo(QString& version)
{
	version= QString("%1.%2.%3 (%4 wk%5)")
				.arg(KHtiVersionMajor).arg(KHtiVersionMinor).arg(KHtiVersionBuild)
				.arg(KHtiVersionYear).arg(KHtiVersionWeek);
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::updateStatuses()
{
	TRAPD(error, iEngine->CheckStatusesL());
	
	if (error != KErrNone) {
		return false;
	}
	else {
		return true;
	}
}

// ---------------------------------------------------------------------------
void HtiEngineWrapper::updateStatus(HtiStatus status)
{
	emit statusChanged(status);
}

// ---------------------------------------------------------------------------
void HtiEngineWrapper::updateSelectedComm(const TDesC& aText)
{
	iSelectedCommText = QString((QChar*)aText.Ptr(),aText.Length());

	emit commSet(iSelectedCommText);
}

// ---------------------------------------------------------------------------
void HtiEngineWrapper::updateCommDetails(const TDesC& aText)
{
	iCommDetailsText = QString((QChar*)aText.Ptr(),aText.Length());

	emit commDetails(iCommDetailsText);
}

// ---------------------------------------------------------------------------
void HtiEngineWrapper::updateAutostartStatus(AutoStartStatus status)
{
	emit autostartSet(status);
}

void HtiEngineWrapper::updateConsoleStatus(bool bEnabled)
{
    emit consoleSet(bEnabled);
}

void HtiEngineWrapper::updateWatchDogStatus(bool bEnabled)
{
    emit watchDogSet(bEnabled);
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::listCommPlugins()
{
	CDesCArray* pluginsArray = NULL;
	
	TRAPD(error, pluginsArray = iEngine->SearchCommPluginsL());
		
	if (error != KErrNone) {
		return false;
	}
	else {
		for(TInt i = 0; i < pluginsArray->Count(); ++i)
			{
			TPtrC16 plugin (pluginsArray->MdcaPoint(i));
			int len = pluginsArray->MdcaPoint(i).Length();
			
			QString name ((QChar*)plugin.Ptr(), len);
			iPluginNames << name;
			}
		
		emit commPluginsRetrieved(iPluginNames);
		
		return true;
	}
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::listIAPs(QStringList& list)
{
    CDesCArray* arrayIAPs = NULL;
    
    TRAPD(error, arrayIAPs = iEngine->SearchIAPsL());
        
    if (error != KErrNone) {
        return false;
    }
    else {
        for(TInt i = 0; i < arrayIAPs->Count(); ++i)
            {
            TPtrC16 iap (arrayIAPs->MdcaPoint(i));
            int len = arrayIAPs->MdcaPoint(i).Length();
            
            QString name ((QChar*)iap.Ptr(), len);
            list << name;
            }
        
        return true;
    }
    return true;
}

// ---------------------------------------------------------------------------
bool HtiEngineWrapper::getSelectedComm(QString& commName)
{
    TBuf<64> pluginName;
    iEngine->GetSelectedComm(pluginName);
    commName = QString((QChar*)pluginName.Ptr(), pluginName.Length());
    return true;
}
