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

#ifndef MAINVIEW_H
#define MAINVIEW_H


#include <hbview.h>
#include "htienginewrapper.h"


class HbMainWindow;
class HbApplication;
class SettingsView;
class HbAction;
class HbTextEdit;
class HbTextItem;
class HbLabel;
class QGraphicsLinearLayout;
class QGraphicsGridLayout;


/**
 * Class that implements the main view of Screen Grabber
  */
class MainView : public HbView
{
    Q_OBJECT
    
public:
    
    /**
     * Constructor
	*/
    MainView(HbMainWindow &mainWindow, HtiEngineWrapper &engineWrapper);
    
    /**
     * Destructor
     */
	~MainView();

	/**
     * Initializes Engine Wrapper
     * @param app application class of Screen Grabber
     */
    void init(HbApplication &app);
    
    void enableSerialComm();
    void enableBTComm();
    void enableIPComm();
    

private slots:
	void htiStatusChanged(HtiEngineWrapper::HtiStatus newStatus);
	void commStatusChanged(QString& newStatus);
	void updatePluginInfo(QStringList& pluginList);
	void autostartStatusChanged(HtiEngineWrapper::AutoStartStatus newStatus);
	void consoleStatusChanged(bool enabled);
	void watchDogStatusChanged(bool enabled);
	void commDetailsChanged(QString& newStatus);
	
	void startHti();
	void stopHti();
	void enableComm();
	void setPriority();
	void enableAutoStart();
	void disableAutoStart();
	void enableWatchdog();
	void disableWatchdog();
	void enableConsole();
	void disableConsole();
	void showParamList ();

	void doSelectComm(HbAction*);
	void doEnableSerialComm(HbAction* );
	void doSelectIAP(HbAction* );
	void doSelectIpComm(HbAction* );
	void doListenOnPort(HbAction* );
	void doConnectRemoteHost(HbAction*);
	void doSelectBTComm(HbAction*);
	void doEnableByBTAddress(HbAction*);
	void doEnableByBTName(HbAction*);
	void doSetPriority(HbAction*);
	void doSelectCfgFile(HbAction*);
	void doModifyHtiCfgFile(HbAction*);
	void doModifyBtCfgFile(HbAction*);
	void doModifySerialCfgFile(HbAction*);
	void doModifyIPCfgFile(HbAction*);
	void doSetHtiCfgParameter(HbAction*);
	void doSetBtCfgParameter(HbAction*);
	void doSetSerialCfgParameter(HbAction*);
	void doSetIPCfgParameter(HbAction*);
private:
    
    /**
     * Creates menu
     */
    void createMenu(HbApplication &app);
    
    /**
     * Creates toolbar components
     */
    void createToolbar();
    
    void createTexts();
    
    void openListDialog(const QStringList& items, const int currentSelection, 
            const QString &titleText, QObject* receiver, const char* member);
    void openIPAddressDialog(const QString &titleText, QObject* receiver, const char* member);
	
public:
    
private:
	/* Main windo of Screen Grabber */
    HbMainWindow &mMainWindow;
    
    /* Hti Engine Wrapper */
    HtiEngineWrapper& mEngineWrapper;
    
    /* Actions that are used in toolbars and menus */
    HbAction* mActionExit;
    HbAction* mActionStartHti;
    HbAction* mActionStopHti;
    HbAction* mActionSelectComm;
    HbAction* mActionSetPriority;
    HbAction* mActionEnableAuto;
    HbAction* mActionDisableAuto;
    HbAction* mActionEnableWdog;
    HbAction* mActionDisableWdog;
    HbAction* mActionEnableConsole;
	HbAction* mActionDisableConsole;
	HbAction* mActionSetParameter;
    
    HbLabel* mHtiStatusTxt;
    HbLabel* mHtiVersionTxt;
    HbLabel* mHtiAutoStartStatusTxt;
    HbLabel* mCommunicationTxt;
    HbLabel* mCommDetailsTxt;
    
    QStringList mPluginList;
    
    QString mIapSelection;

};

#endif // MAINVIEW_H


