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


#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbinputdialog.h>
#include <qgraphicslinearlayout.h>
#include <hbselectiondialog.h>
#include <hbmessagebox.h>



#include "htienginewrapper.h"
#include "mainview.h"
#include "hbtoolbar.h"

// ---------------------------------------------------------------------------

MainView::MainView(HbMainWindow &mainWindow, HtiEngineWrapper& engineWrapper):
    mMainWindow(mainWindow),
    mEngineWrapper(engineWrapper)
{
}

// ---------------------------------------------------------------------------

MainView::~MainView()
{

}

// ---------------------------------------------------------------------------

void MainView::init(HbApplication &app)
{
    this->setTitle("Hti Admin");
    createToolbar();
    createMenu(app);
    createTexts();
    
    mEngineWrapper.updateStatuses();
    
    QString version;
    mEngineWrapper.getVersionIfo(version);
    mEngineWrapper.listCommPlugins();
    
    mHtiVersionTxt->setPlainText("Hti Version: " + version);
}

// ---------------------------------------------------------------------------
void MainView::createMenu(HbApplication& app)
{
    
    HbMenu* menu = new HbMenu();
    
    if (menu != NULL) {
        
        mActionExit = menu->addAction("Exit");
        connect(mActionExit, SIGNAL(triggered()), &app, SLOT( quit() ) );

        connect(mActionStartHti, SIGNAL(triggered()), this, SLOT( startHti() ) );
        connect(mActionStopHti, SIGNAL(triggered()), this, SLOT( stopHti() ) );
        
        mActionSelectComm = menu->addAction("Select Communication");
        connect(mActionSelectComm, SIGNAL(triggered()), this, SLOT( enableComm() ) );
        
        mActionSetPriority = menu->addAction("Set Priority");
        connect(mActionSetPriority, SIGNAL(triggered()), this, SLOT( setPriority() ) );
        
        mActionEnableAuto = menu->addAction("Enable Auto Start");
        connect(mActionEnableAuto, SIGNAL(triggered()), this, SLOT( enableAutoStart() ) );
        
        mActionDisableAuto = menu->addAction("Disable Auto Start");
        connect(mActionDisableAuto, SIGNAL(triggered()), this, SLOT( disableAutoStart() ) );
        
        mActionEnableWdog = menu->addAction("Enable Watchdog");
        connect(mActionEnableWdog, SIGNAL(triggered()), this, SLOT( enableWatchdog() ) );
                
        mActionDisableWdog = menu->addAction("Disable Watchdog");
        connect(mActionDisableWdog, SIGNAL(triggered()), this, SLOT( disableWatchdog() ) );
        
        mActionEnableConsole = menu->addAction("Enable Console");
        connect(mActionEnableConsole, SIGNAL(triggered()), this, SLOT( enableConsole() ) );
                        
		mActionDisableConsole = menu->addAction("Disable Console");
		connect(mActionDisableConsole, SIGNAL(triggered()), this, SLOT( disableConsole() ) );
		
		mActionSetParameter = menu->addAction("Set Parameter");
		connect(mActionSetParameter, SIGNAL(triggered()), this, SLOT( showParamList() ) );
        
        this->setMenu(menu);
    }
    
    
    connect(&mEngineWrapper, SIGNAL(statusChanged(HtiEngineWrapper::HtiStatus)), this, SLOT(htiStatusChanged(HtiEngineWrapper::HtiStatus)));
    connect(&mEngineWrapper, SIGNAL(commSet(QString&)), this, SLOT(commStatusChanged(QString&)));
    connect(&mEngineWrapper, SIGNAL(commDetails(QString&)), this, SLOT(commDetailsChanged(QString&)));
    connect(&mEngineWrapper, SIGNAL(autostartSet(HtiEngineWrapper::AutoStartStatus)), this, SLOT(autostartStatusChanged(HtiEngineWrapper::AutoStartStatus)));
    connect(&mEngineWrapper, SIGNAL(consoleSet(bool)), this, SLOT(consoleStatusChanged(bool)));
    connect(&mEngineWrapper, SIGNAL(watchDogSet(bool)), this, SLOT(watchDogStatusChanged(bool)));
    connect(&mEngineWrapper, SIGNAL(commPluginsRetrieved(QStringList&)), this, SLOT(updatePluginInfo(QStringList&)));
    
    
}

// ---------------------------------------------------------------------------
void MainView::createTexts()
{
	QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
	
	mCommDetailsTxt = new HbLabel(this);
	mHtiStatusTxt = new HbLabel(this);
	mHtiVersionTxt = new HbLabel(this);
	
	mHtiAutoStartStatusTxt = new HbLabel(this);
	mCommunicationTxt = new HbLabel(this);
	
	mHtiStatusTxt->setPlainText("Hti Status:");
	mHtiVersionTxt->setPlainText("Hti Version:");
	mHtiAutoStartStatusTxt->setPlainText("Auto Start:");
	mCommunicationTxt->setPlainText("Communication ");
		
	layout->addItem(mHtiStatusTxt);
	layout->addItem(mHtiVersionTxt);
	layout->addItem(mHtiAutoStartStatusTxt);
	layout->addItem(mCommunicationTxt);
	layout->addItem(mCommDetailsTxt);

	layout->setMaximumHeight(300);
	this->setLayout(layout);
}

// ---------------------------------------------------------------------------
void MainView::htiStatusChanged(HtiEngineWrapper::HtiStatus newStatus)
{
	switch(newStatus)
			{
			case HtiEngineWrapper::Running:
				mHtiStatusTxt->setPlainText("Hti Status: Running" );
				break;
			case HtiEngineWrapper::Stopped:
				mHtiStatusTxt->setPlainText("Hti Status: Stopped" );
				break;
			case HtiEngineWrapper::Panic:
				mHtiStatusTxt->setPlainText("Hti Status: Panic" );
				break;
			default:
				mHtiStatusTxt->setPlainText("Hti Status: Error" );
			}
	
	// Update menu items status
	bool isRunning = (newStatus == HtiEngineWrapper::Running);
    mActionStartHti->setEnabled(!isRunning);
    mActionStopHti->setEnabled(isRunning);
    mActionSelectComm->setEnabled(!isRunning);
    mActionSetPriority->setEnabled(!isRunning);
    mActionEnableAuto->setEnabled(!isRunning);
    mActionDisableAuto->setEnabled(!isRunning);
    mActionEnableWdog->setEnabled(!isRunning);
    mActionDisableWdog->setEnabled(!isRunning);
    mActionEnableConsole->setEnabled(!isRunning);
    mActionDisableConsole->setEnabled(!isRunning);
    mActionSetParameter->setEnabled(!isRunning);
}

// ---------------------------------------------------------------------------
void MainView::commStatusChanged(QString& newStatus)
{
	if (mCommunicationTxt) {
		mCommunicationTxt->setPlainText("Communication: " + newStatus);
	}
}

// ---------------------------------------------------------------------------
void MainView::commDetailsChanged(QString& newStatus)
{
	if (mCommDetailsTxt) {
		mCommDetailsTxt->setPlainText(newStatus);
	}
}

// ---------------------------------------------------------------------------
void MainView::updatePluginInfo(QStringList& pluginList)
{
	mPluginList = pluginList;
}

// ---------------------------------------------------------------------------
void MainView::autostartStatusChanged(HtiEngineWrapper::AutoStartStatus newStatus)
{
	switch(newStatus)
		{
		case HtiEngineWrapper::AutoStartDisabled:
			mHtiAutoStartStatusTxt->setPlainText("Auto Start: Disabled" );
			break;
		case HtiEngineWrapper::AutoStartEnabled:
			mHtiAutoStartStatusTxt->setPlainText("Auto Start: Enabled" );
			break;
		default:
			mHtiAutoStartStatusTxt->setPlainText("Auto Start: Unknown" );
		}
	
	bool isAutoStartEnabled = (newStatus == HtiEngineWrapper::AutoStartEnabled);
    mActionEnableAuto->setVisible(!isAutoStartEnabled);
    mActionDisableAuto->setVisible(isAutoStartEnabled);
}

// ---------------------------------------------------------------------------
void MainView::consoleStatusChanged(bool enabled)
{
    mActionEnableConsole->setVisible(!enabled);
    mActionDisableConsole->setVisible(enabled);
}

// ---------------------------------------------------------------------------
void MainView::watchDogStatusChanged(bool enabled)
{
    mActionEnableWdog->setVisible(!enabled);
    mActionDisableWdog->setVisible(enabled);
}


// ---------------------------------------------------------------------------

void MainView::createToolbar()
{
	mActionStartHti = toolBar()->addAction("Start Hti");
	mActionStopHti = toolBar()->addAction("Stop Hti");
}

// ---------------------------------------------------------------------------

void MainView::startHti()
{
	mEngineWrapper.startHti();
}

// ---------------------------------------------------------------------------

void MainView::stopHti()
{
	mEngineWrapper.stopHti();
}


// ---------------------------------------------------------------------------
void MainView::enableComm()
{
    //Get current selection
	QString currentComm;
	mEngineWrapper.getSelectedComm(currentComm);
	int curSelection = mPluginList.indexOf(currentComm, 0);
	openListDialog(mPluginList, curSelection, QString("Select Comm"), this, SLOT(doSelectComm(HbAction*)));
}

void MainView::doSelectComm(HbAction* action)
{
    HbSelectionDialog *dlg = static_cast<HbSelectionDialog*>(sender());
    if(dlg && dlg->selectedModelIndexes().count() &&
            (!action || action == dlg->actions().at(0)))
    {
        int selectionIndex = dlg->selectedModelIndexes().at(0).row();

        if (mPluginList[selectionIndex] == QString("Bt serial comm"))
        {
            enableBTComm();
        }
        else if (mPluginList[selectionIndex] == QString("IP comm"))
        {
            enableIPComm();
        }
        else if (mPluginList[selectionIndex] == QString("SERIAL comm"))
        {
            enableSerialComm();
        }
        else
        {
            //All other comm plugins
            mEngineWrapper.enableOtherComm(mPluginList[selectionIndex]);
        }   
    }    
}
// ---------------------------------------------------------------------------
void MainView::enableSerialComm()
{
    QString heading = QString("Set Comm Port number");
    HbInputDialog::getInteger(heading, this, SLOT(doEnableSerialComm(HbAction*)), 0, scene());
}

void MainView::doEnableSerialComm(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString strPortNumber = dlg->value().toString();
        mEngineWrapper.enableSerial(strPortNumber);
    }
}
// ---------------------------------------------------------------------------
void MainView::enableIPComm()
{
    // Get IAPs list
    QStringList iapsList;
    mEngineWrapper.listIAPs(iapsList); 
    if(iapsList.count() == 0)
    {
        HbMessageBox::warning(QString("No IAPs for selection!"));
        return;
    }
    
    // Get current selection
    QString curIapName;
    QString param = "IAPName";
    mEngineWrapper.getIPCfgParam(param, curIapName);
    int curSelection = iapsList.indexOf(curIapName, 0);
    
    openListDialog(iapsList, curSelection, QString("Select IAP:"), this, SLOT(doSelectIAP(HbAction*)));
}

void MainView::doSelectIAP(HbAction* action)
{
    HbSelectionDialog *dlg = static_cast<HbSelectionDialog*>(sender());
    if(dlg && dlg->selectedModelIndexes().count() &&
            (!action || action == dlg->actions().at(0)))
    {
        int selectionIndex = dlg->selectedModelIndexes().at(0).row();
        mIapSelection = dlg->stringItems()[selectionIndex];
        
        QStringList items;
        items <<"Listen" <<"Connect";
        openListDialog(items, 0, QString("Select IP Comm"), this, SLOT(doSelectIpComm(HbAction*)));
    }  
}

void MainView::doSelectIpComm(HbAction* action)
{
    HbSelectionDialog *dlg = static_cast<HbSelectionDialog*>(sender());
    if(dlg && dlg->selectedModelIndexes().count() &&
            (!action || action == dlg->actions().at(0)))
    {
        int selectionIndex = dlg->selectedModelIndexes().at(0).row();
        if(selectionIndex == 0) //Listen
        {
            HbInputDialog::getInteger(QString("Local port"), this, SLOT(doListenOnPort(HbAction*)), 0, scene());
        }
        else //Connect
        {
            openIPAddressDialog(QString("Remote Host"), this, SLOT(doConnectRemoteHost(HbAction*)));
        }
    }
}

void MainView::doListenOnPort(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString port = dlg->value().toString();
        mEngineWrapper.ipListen(port, mIapSelection);
    }
}

void MainView::doConnectRemoteHost(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString host = dlg->value(0).toString();
        QString port = dlg->value(1).toString();
        mEngineWrapper.ipConnect(host, port, mIapSelection);
    }
}

// ---------------------------------------------------------------------------
void MainView::enableBTComm()
{
	QStringList items;
	items <<"BT address" <<"BT name" <<"Search when starting" ;
	openListDialog(items, 0, QString("Bluetooth Comm"), this, SLOT(doSelectBTComm(HbAction*)));
}

void MainView::doSelectBTComm(HbAction* action)
{
    HbSelectionDialog *dlg = static_cast<HbSelectionDialog*>(sender());
    if(dlg && dlg->selectedModelIndexes().count() &&
            (!action || action == dlg->actions().at(0)))
    {
        int selectionIndex = dlg->selectedModelIndexes().at(0).row();
        if(selectionIndex == 0)
        {
            HbInputDialog::getText(QString("BT address"), this, SLOT(doEnableByBTAddress(HbAction*)));
        }
        else if(selectionIndex == 1)
        {
            HbInputDialog::getText(QString("BT name"), this, SLOT(doEnableByBTName(HbAction*)));
        }
        else if(selectionIndex == 2)
        {
            mEngineWrapper.btSearch();
        }
    }
}

void MainView::doEnableByBTAddress(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString address = dlg->value().toString();
        mEngineWrapper.enableBtByAddress(address);
    }
}
void MainView::doEnableByBTName(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString name = dlg->value().toString();
        mEngineWrapper.enableBtByName(name);
    }
}

// ---------------------------------------------------------------------------
void MainView::setPriority()
{
    // Get current priority
    bool ok = false;
    QString curPriority;
    QString param = "Priority";
    mEngineWrapper.getHtiCfgParam(param, curPriority);
    int curSelection = curPriority.toInt(&ok);
    if(ok){
        curSelection--;
    }
    else{
    curSelection = 2;
    }
    
	QStringList items;
	items <<"Backgroung" <<"Foregound" <<"High" << "Absolute High";
	openListDialog(items, curSelection, QString("Select Hti Priority"), 
	        this, SLOT(doSetPriority(HbAction*)));
}

void MainView::doSetPriority(HbAction* action)
{
    HbSelectionDialog *dlg = static_cast<HbSelectionDialog*>(sender());
    if(dlg && dlg->selectedModelIndexes().count() &&
            (!action || action == dlg->actions().at(0)))
    {
        int selectionIndex = dlg->selectedModelIndexes().at(0).row();
        if(selectionIndex == 0)
        {
            mEngineWrapper.setPriorityBackground();
        }
        else if(selectionIndex == 1)
        {
            mEngineWrapper.setPriorityForeground();
        }
        else if(selectionIndex == 2)
        {
            mEngineWrapper.setPriorityHigh();
        }
        else
        {
            mEngineWrapper.setPriorityAbsoluteHigh();
        }
    }
}
// ---------------------------------------------------------------------------
void MainView::enableAutoStart()
{
	mEngineWrapper.autoStartEnable(true);
}

// ---------------------------------------------------------------------------
void MainView::disableAutoStart()
{
	mEngineWrapper.autoStartEnable(false);
}

// ---------------------------------------------------------------------------
void MainView::enableWatchdog()
{
	mEngineWrapper.watchDogEnable(true);
}

// ---------------------------------------------------------------------------
void MainView::disableWatchdog()
{
	mEngineWrapper.watchDogEnable(false);
}

// ---------------------------------------------------------------------------
void MainView::enableConsole()
{
	mEngineWrapper.consoleEnable(true);
}

// ---------------------------------------------------------------------------
void MainView::disableConsole()
{
	mEngineWrapper.consoleEnable(false);
}

// ---------------------------------------------------------------------------
void MainView::showParamList()
{
    QStringList items;
    items << "Hti.cfg" << "HtiBtComm.cfg" << "HtiSerialComm.cfg" << "HtiIPComm.cfg";
    openListDialog(items, 0, QString("Select cfg file to modify"), 
            this, SLOT(doSelectCfgFile(HbAction*)));
}

void MainView::doSelectCfgFile(HbAction* action)
{
    HbSelectionDialog *dlg = static_cast<HbSelectionDialog*>(sender());
    if(dlg && dlg->selectedModelIndexes().count() &&
            (!action || action == dlg->actions().at(0)))
    {
        int selectionIndex = dlg->selectedModelIndexes().at(0).row();
        QStringList items;
        if(selectionIndex == 0)
        {
            items <<"CommPlugin" <<"MaxMsgSize" <<"MaxQueueSize" <<"MaxHeapSize"<<"Priority"
                    <<"ShowConsole"<<"MaxWaitTime"<<"StartUpDelay"<<"EnableHtiWatchDog"
                    <<"EnableHtiAutoStart"<<"ShowErrorDialogs"<<"ReconnectDelay";
            openListDialog(items, 0, QString("Hti.cfg"), this, SLOT(doModifyHtiCfgFile(HbAction*)));
        }
        else if(selectionIndex == 1)
        {
            items <<"BtDeviceName" <<"BtDeviceName";
            openListDialog(items, 0, QString("HtiBtComm.cfg"), this, SLOT(doModifyBtCfgFile(HbAction*)));
        }
        else if(selectionIndex == 2)
        {
            items <<"CommPort" <<"DataRate"<<"Parity"<<"DataBits"<<"StopBits"<<"SendDelay"<<"Handshake";
            openListDialog(items, 0, QString("HtiSerialComm.cfg"), this, SLOT(doModifySerialCfgFile(HbAction*)));
        }
        else
        {
            items <<"IAPName"<<"LocalPort"<<"RemoteHost"<<"RemotePort"<<"ConnectTimeout";
            openListDialog(items, 0, QString("HtiIPComm.cfg"), this, SLOT(doModifyIPCfgFile(HbAction*)));
        }
    }
}

void MainView::doModifyHtiCfgFile(HbAction* action)
{
    HbSelectionDialog *dlgSelection = static_cast<HbSelectionDialog*>(sender());
    if(dlgSelection && dlgSelection->selectedModelIndexes().count() &&
            (!action || action == dlgSelection->actions().at(0)))
    {
        int selectionIndex = dlgSelection->selectedModelIndexes().at(0).row();
        HbInputDialog* dlgInput = new HbInputDialog();
        dlgInput->setAttribute(Qt::WA_DeleteOnClose);
        dlgInput->setPromptText(dlgSelection->stringItems()[selectionIndex]);
        dlgInput->setInputMode(HbInputDialog::TextInput);
        
        HbLabel *title = new HbLabel(dlgInput);
        title->setPlainText(QString("Set parameter value"));
        dlgInput->setHeadingWidget(title);
        
        dlgInput->open(this, SLOT(doSetHtiCfgParameter(HbAction*)));
    }
}

void MainView::doSetHtiCfgParameter(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString parameter = dlg->promptText();
        QString value = dlg->value().toString();
        mEngineWrapper.setHtiCfgParam(parameter, value);
    }
}

void MainView::doModifyBtCfgFile(HbAction* action)
{
    HbSelectionDialog *dlgSelection = static_cast<HbSelectionDialog*>(sender());
    if(dlgSelection && dlgSelection->selectedModelIndexes().count() &&
            (!action || action == dlgSelection->actions().at(0)))
    {
        int selectionIndex = dlgSelection->selectedModelIndexes().at(0).row();
        HbInputDialog* dlgInput = new HbInputDialog();
        dlgInput->setAttribute(Qt::WA_DeleteOnClose);
        dlgInput->setPromptText(dlgSelection->stringItems()[selectionIndex]);
        dlgInput->setInputMode(HbInputDialog::TextInput);
        
        HbLabel *title = new HbLabel(dlgInput);
        title->setPlainText(QString("Set parameter value"));
        dlgInput->setHeadingWidget(title);
        
        dlgInput->open(this, SLOT(doSetBtCfgParameter(HbAction*)));
    }
}

void MainView::doSetBtCfgParameter(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString parameter = dlg->promptText();
        QString value = dlg->value().toString();
        mEngineWrapper.setBtCfgParam(parameter, value);
    }
}

void MainView::doModifySerialCfgFile(HbAction* action)
{
    HbSelectionDialog *dlgSelection = static_cast<HbSelectionDialog*>(sender());
    if(dlgSelection && dlgSelection->selectedModelIndexes().count() &&
            (!action || action == dlgSelection->actions().at(0)))
    {
        int selectionIndex = dlgSelection->selectedModelIndexes().at(0).row();
        HbInputDialog* dlgInput = new HbInputDialog();
        dlgInput->setAttribute(Qt::WA_DeleteOnClose);
        dlgInput->setPromptText(dlgSelection->stringItems()[selectionIndex]);
        dlgInput->setInputMode(HbInputDialog::TextInput);
        
        HbLabel *title = new HbLabel(dlgInput);
        title->setPlainText(QString("Set parameter value"));
        dlgInput->setHeadingWidget(title);
        
        dlgInput->open(this, SLOT(doSetSerialCfgParameter(HbAction*)));
    }
}

void MainView::doSetSerialCfgParameter(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString parameter = dlg->promptText();
        QString value = dlg->value().toString();
        mEngineWrapper.setSerialCfgParam(parameter, value);
    }
}

void MainView::doModifyIPCfgFile(HbAction* action)
{
    HbSelectionDialog *dlgSelection = static_cast<HbSelectionDialog*>(sender());
    if(dlgSelection && dlgSelection->selectedModelIndexes().count() &&
            (!action || action == dlgSelection->actions().at(0)))
    {
        int selectionIndex = dlgSelection->selectedModelIndexes().at(0).row();
        HbInputDialog* dlgInput = new HbInputDialog();
        dlgInput->setAttribute(Qt::WA_DeleteOnClose);
        dlgInput->setPromptText(dlgSelection->stringItems()[selectionIndex]);
        dlgInput->setInputMode(HbInputDialog::TextInput);
        
        HbLabel *title = new HbLabel(dlgInput);
        title->setPlainText(QString("Set parameter value"));
        dlgInput->setHeadingWidget(title);
        
        dlgInput->open(this, SLOT(doSetIPCfgParameter(HbAction*)));
    }
}

void MainView::doSetIPCfgParameter(HbAction* action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (action == dlg->actions().at(0))
    {
        QString parameter = dlg->promptText();
        QString value = dlg->value().toString();
        mEngineWrapper.setIPCfgParam(parameter, value);
    }
}

void MainView::openListDialog(const QStringList& items, const int currentSelection,
        const QString &titleText, QObject* receiver, const char* member)
{
    // Create a list and some simple content for it
    HbSelectionDialog *dlg = new HbSelectionDialog();
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    // Set items to be popup's content
    dlg->setStringItems(items);
    dlg->setSelectionMode(HbAbstractItemView::SingleSelection);
    QList<QVariant> current;
    current.append(QVariant(currentSelection));
    dlg->setSelectedItems(current);
    
    HbLabel *title = new HbLabel(dlg);
    title->setPlainText(titleText);
    dlg->setHeadingWidget(title);

    // Launch popup and handle the user response:
    dlg->open(receiver, member);
}

void MainView::openIPAddressDialog(const QString &titleText, QObject* receiver, const char* member)
{
    HbInputDialog* dlg = new HbInputDialog();
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setAdditionalRowVisible(true);
    dlg->setPromptText(QString("IP Address:"), 0);
    dlg->setInputMode(HbInputDialog::IpInput, 0);
    dlg->setPromptText(QString("Port:"), 1);
    dlg->setInputMode(HbInputDialog::IntInput, 1);
    
    HbLabel *title = new HbLabel(dlg);
    title->setPlainText(titleText);
    dlg->setHeadingWidget(title);
    
    dlg->open(receiver, member);
}
