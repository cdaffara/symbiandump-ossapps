/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DMFotaView header.
*
*/

#ifndef DMFOTAVIEW_H_
#define DMFOTAVIEW_H_


#include <hbapplication.h>
#include <hbwidget.h>
#include <QGraphicsLinearLayout>
#include <hbgroupbox.h>
#include <hbview.h>
#include <sysutil.h>
#include <hblabel.h>
#include <hbdocumentloader.h>
#include <hbmenu.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include <QStandardItemModel>
#include <QLocale>
#include <centralrepository.h>
#include <QTranslator>
#include <nsmldmdef.h>
#include <fotaengine.h>
#include "dminforetrieval.h"
#include "deviceupdatemoniterobserver.h"
//_LIT( KSmlEOL,      "\n" );
const TUid KControlPanelAppUid ={0x20025fd9};

class HbDialog;
class HbPushButton;
class DmAdvancedView;
class CDeviceUpdateMoniter;

class DMFotaView : public HbView, public MDeviceMoniterObserver
    {
    Q_OBJECT
public:
    DMFotaView(HbMainWindow *mainWindow);
    virtual ~DMFotaView();
    bool addFotaView();
    //HbDialog* createDialog() const;
    void displayNoteAndDisableButtons();
    void enableButtons();
    
signals :
	/**
	 * applicationReady() - To emit the application ready signal for matti tool.
   */
   void applicationReady();
public slots :
    void OnHelp();
    void OnExit();
	void CheckforUpdate();
	void AdvancedDeviceManager(bool launchadvanceview = true);	
	void backtoMainWindow();
	void readSection( Qt::Orientation orientation );
	void ResumeUpdate();
public:
	void UpdateDMUI(TBool aVal);
private:
    void FormatList(QString val, QString str);
    RFotaEngineSession& FotaEngineL();
    void fotaSupportEnabled();
private:
	HbMainWindow* mMainWindow;
	HbView* fotaPortraitView;
	HbView* fotaLandscapeView;
	DmAdvancedView* profilesView;
	HbAction    *mSoftKeyBackAction;
    RSyncMLSession iSession;
    RSyncMLDevManProfile profile;
    RSyncMLDevManJob iSyncJob;
    QTranslator* mTranslator;
    HbLabel *label,*label2,*label3,*label4;
    QStringList list1,list2;
    TInt i;   
    DmInfo* mainDmInfo;
    HbDocumentLoader loader,loader2;
    HbPushButton *updateButton;
    TInt fotaValue;
    HbPushButton *advancedButton;
    HbPushButton *updateButtonLandscape;
    HbPushButton *advancedButtonLandscape;
    
    RFotaEngineSession iFotaEngine;
    RFotaEngineSession::TState iFotaState;
    
    CDeviceUpdateMoniter * iMoniter;
    TBool Connected;
    };
    

#endif
