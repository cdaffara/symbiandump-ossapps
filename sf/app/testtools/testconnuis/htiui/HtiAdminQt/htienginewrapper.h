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

#ifndef HTIENGINEWRAPPER_H
#define HTIENGINEWRAPPER_H

#include <e32cmn.h>
#include <qobject.h>
#include <qstringlist.h>

class CHtiAdminEngine;
class QString;




/**
 * class that is used for communicating between Symbian and Qt code.
 */
class HtiEngineWrapper : public QObject 
	{
	Q_OBJECT
public:
	enum AutoStartStatus{
		AutoStartDisabled,
		AutoStartEnabled,
		AutoStartUnknown		
	};
	
	enum HtiStatus{
		Running,
		Stopped,
		Panic,
		Error
		};
	
	/**
	 * Constructor
	 */
	HtiEngineWrapper();
	
	/**
	 * Destructor
	 */
	~HtiEngineWrapper();
	
	
	bool init();
	bool stopHti();
	bool startHti();
	bool enableBtByAddress(QString& address);
	bool enableBtByName(QString& address);
	bool ipListen(QString& port, QString& iap);
	bool ipConnect(QString& host, QString& port, QString& iap);
	bool enableSerial(QString& commPort);
	bool btSearch();
	bool enableOtherComm(QString& pluginName);
	bool setPriorityBackground();
	bool setPriorityForeground();
	bool setPriorityHigh();
	bool setPriorityAbsoluteHigh();
	bool watchDogEnable(bool enable);
	bool autoStartEnable(bool enable);
	bool consoleEnable(bool enable);
	bool errorDialogsEnable(bool enable);
	bool setHtiCfgParam(QString& paramName, QString& paramValue);
	bool getHtiCfgParam(QString& paramName, QString& paramValue);
	bool setBtCfgParam(QString& paramName, QString& paramValue);
	bool getBtCfgParam(QString& paramName, QString& paramValue);
	bool setSerialCfgParam(QString& paramName, QString& paramValue);
	bool getSerialCfgParam(QString& paramName, QString& paramValue);
	bool setIPCfgParam(QString& paramName, QString& paramValue);
	bool getIPCfgParam(QString& paramName, QString& paramValue);
	void getVersionIfo(QString& version);
	bool updateStatuses();
	bool listCommPlugins();
	bool listIAPs(QStringList& list);
	bool getSelectedComm(QString& commName);
	
	//Called from engine side
	void updateStatus(HtiStatus status);
	void updateSelectedComm(const TDesC& aText);
	void updateAutostartStatus(AutoStartStatus status);
	void updateConsoleStatus(bool bEnabled);
	void updateWatchDogStatus(bool bEnabled);
	void updateCommDetails(const TDesC& aText);
	
		

signals:
	void statusChanged(HtiEngineWrapper::HtiStatus status);	
	void commSet(QString& statusText);
	void commDetails(QString& statusText);
	void commPluginsRetrieved(QStringList& pluginNames);
	void autostartSet(HtiEngineWrapper::AutoStartStatus status);
	void consoleSet(bool enabled);
	void watchDogSet(bool enabled);
	
private:
		CHtiAdminEngine* iEngine;
		QString iSelectedCommText;
		QString iCommDetailsText;
		QStringList iPluginNames;
		
	};

#endif //HTIENGINEWRAPPER_H
