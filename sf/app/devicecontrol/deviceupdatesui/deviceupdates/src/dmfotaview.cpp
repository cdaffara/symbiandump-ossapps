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
* Description:  Methods for DmFotaView.
*
*/

#include <hbmainwindow.h>
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbdialog.h>
#include <sysutil.h>
#include <hbscrollarea.h>
#include <etel.h>
#include <eikenv.h>
#include <featmgr.h>
#include <etelmm.h>
#include <apgtask.h>
#include <coemain.h>
#include <hbnotificationdialog.h>
#include <centralrepository.h>
#include <sysversioninfo.h>
#include "nsmldmsyncprivatecrkeys.h"
#include "dmadvancedview.h"
#include "dmfotaview.h"
#include <xqconversions.h>
#include "deviceupdatemoniter.h"
#include "DeviceUpdateDebug.h"
// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// DMFotaView::DMFotaView
// -----------------------------------------------------------------------------
//
DMFotaView::DMFotaView(HbMainWindow *mainWindow):
    mMainWindow(mainWindow),profilesView(NULL),Connected(EFalse)
{
    qDebug("DMFotaView::DMFotaView >>");
    fotaPortraitView=0;
    fotaLandscapeView=0;
    mainDmInfo=0;
    qDebug("DMFotaView::DMFotaView <<");
}

// -----------------------------------------------------------------------------
// DMFotaView::~DMFotaView
// 
// -----------------------------------------------------------------------------
//
DMFotaView::~DMFotaView()
{
    qDebug("DMFotaView::~DMFotaView >>");
    if(mainDmInfo)
        delete mainDmInfo;
    
    if (iFotaEngine.Handle())
        iFotaEngine.Close();

    if (iMoniter)
        {
        iMoniter->Cancel();
        delete iMoniter;
        }
    qDebug("DMFotaView::~DMFotaView <<");
}

// -----------------------------------------------------------------------------
// DMFotaView::addFotaView
// Creates and adds the Fota Portrait and Landscape View
// -----------------------------------------------------------------------------
//
bool DMFotaView::addFotaView()
    {
    qDebug("DMFotaView::addFotaView >>");
    connect( mMainWindow, SIGNAL( orientationChanged(Qt::Orientation) ), this, SLOT( readSection(Qt::Orientation) ) );
        
    bool ok = false;
    QString val,val2;
    loader.load(":/xml/devman.docml", &ok);
    ok = false;
    loader2.load(":/xml/devman.docml", &ok);
    // Exit if the file format is invalid
    Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
    
    ok=false;
    loader.load(":/xml/devman.docml","Portrait", &ok);
    ok=false;
    loader2.load(":/xml/devman.docml","Landscape", &ok);
   
    // Load the view by name from the xml file
    fotaPortraitView = qobject_cast<HbView*>(loader.findWidget("p:view"));
    fotaLandscapeView = qobject_cast<HbView*>(loader2.findWidget("l:view"));


    HbAction *help = qobject_cast<HbAction*> (loader.findObject("help")); 
    help->setText(QString("txt_common_menu_help"));
    QObject::connect(help, SIGNAL(triggered()), this, SLOT(OnHelp()));
    HbAction *exit = qobject_cast<HbAction*> (loader.findObject("exit"));
    exit->setText(QString("txt_common_menu_exit"));
    QObject::connect(exit, SIGNAL(triggered()), this, SLOT(OnExit()));

    help = qobject_cast<HbAction*>(loader2.findObject("help"));
    help->setText(QString("txt_common_menu_help"));
    QObject::connect(help, SIGNAL(triggered()), this, SLOT(OnHelp()));
    exit = qobject_cast<HbAction*>( loader2.findObject("exit"));
    exit->setText(QString("txt_common_menu_exit"));
    QObject::connect(exit, SIGNAL(triggered()), this, SLOT(OnExit()));
    
    mSoftKeyBackAction = new HbAction(Hb::BackNaviAction ,this);
    mSoftKeyBackAction->setText("Back");
    fotaPortraitView->setNavigationAction(mSoftKeyBackAction);
	fotaLandscapeView->setNavigationAction(mSoftKeyBackAction);
    connect(mSoftKeyBackAction, SIGNAL(triggered()), this, SLOT(backtoMainWindow()));
        
    
    i=0;
    QString str;
    //Setting title text
    label = qobject_cast<HbLabel*> (loader.findWidget("p:title"));
    label2 = qobject_cast<HbLabel*> (loader2.findWidget("l:title"));
    val = hbTrId("txt_device_update_subhead_device_updates");
    label->setPlainText(val);
    label2->setPlainText(val);
    
    
    
    HbScrollArea* area = qobject_cast<HbScrollArea*> (loader.findWidget("scrollArea"));
    area->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOn);
    
    
    list1 << "label" << "label_1" << "label_2" << "label_3" << "label_4" << "label_5" << "label_6"<<"label_7"<<"label_8"<<"label_9"<<"label_10"<<"label_11";
    list2 << "label_13" << "label_14" << "label_15" << "label_16" << "label_17" << "label_18" << "label_19"<<"label_20"<<"label_21"<<"label_22"<<"label_23"<<"label_24";


    //Product Release
    TBuf< KSysUtilVersionTextLength > prodrelease;
    prodrelease.Zero();
    if(SysUtil::GetPRInformation(prodrelease)==KErrNone)
        {
        val = hbTrId("txt_device_update_dblist_product_release");
        str = QString::fromUtf16(prodrelease.Ptr(), prodrelease.Length());
        FormatList(val,str);
        }
    
    //Software version
    
    TBuf< KSysUtilVersionTextLength > swversion;
    TBuf< KSysUtilVersionTextLength > swversiondate;
    TBuf< KSysUtilVersionTextLength > typedesignator;
    TBuf< KSysUtilVersionTextLength > version;
    swversion.Zero();
    version.Zero();
    typedesignator.Zero();
    swversiondate.Zero();
    if( SysUtil::GetSWVersion(version)==KErrNone)
        {
        TInt len= version.Length();
        TInt pos1 = version.Find(KSmlEOL);
             if( pos1 != KErrNotFound && len > pos1 )
                {
                 TBuf<KSysUtilVersionTextLength> version1;
                 version1.Zero();
                 swversion.Append( version.Left(pos1));
                 version1.Append( version.Right( len-pos1-1 ));
                 len= version1.Length();
                 pos1 = version1.Find(KSmlEOL);
                 if( pos1 != KErrNotFound  && len > pos1 )
                     {
                     swversiondate.Append(version1.Left( pos1 ));
                     version.Zero();
                     version.Append( version1.Right( len-pos1-1 ));
                     len= version.Length();
                     pos1 = version.Find(KSmlEOL);
                     if( pos1 != KErrNotFound  && len > pos1 )
                         {
                         typedesignator.Append(version.Left(pos1));         
                         }
                     }
                }
            val = hbTrId("txt_device_update_dblist_software_version");
            str = QString::fromUtf16(swversion.Ptr(), swversion.Length());
            FormatList(val,str);
            }

    //Model
    TBuf<100> phoneName;
    TInt error = SysVersionInfo::GetVersionInfo(SysVersionInfo::EModelVersion, phoneName);
    if (error==KErrNone)
    {
    
    //phoneName.Copy( telid.iModel );
    //imei.Copy(telid.iSerialNumber);
    if( phoneName.Length()>0 )
        {
        val = hbTrId("txt_device_update_dblist_model");
        str = QString::fromUtf16(phoneName.Ptr(), phoneName.Length());
        FormatList(val,str);
        }
    }
    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    RTelServer::TPhoneInfo teleinfo;
    User::LeaveIfError( telServer.GetPhoneInfo( 0, teleinfo ) );
    RMobilePhone phone;
    User::LeaveIfError( phone.Open( telServer, teleinfo.iName ) );
    User::LeaveIfError(phone.Initialise()); 
    TUint32 teleidentityCaps;
    phone.GetIdentityCaps( teleidentityCaps );
    RMobilePhone::TMobilePhoneIdentityV1 telid;
    TRequestStatus status;
    phone.GetPhoneId( status, telid );
    User::WaitForRequest( status );
    TBuf <50> imei;
    if (status==KErrNone)
    {
    TBuf<100> phoneName;
    imei.Copy(telid.iSerialNumber);
    }
    phone.Close();
    telServer.Close();

    //type
    if(typedesignator.Length()>0)
        {
        val = hbTrId("txt_device_update_dblist_type");
        str = QString::fromUtf16(typedesignator.Ptr(), typedesignator.Length());
        FormatList(val,str);
        }
      
    //Product Code
       /* TInt runtimesupport(0);
        CRepository* cenrep = NULL;
        TRAPD( error, cenrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );  
        if(error)
            {
            runtimesupport=1;
            }
        if ( cenrep )
                {
                cenrep->Get( KNsmlDmRuntimeVerSupport, runtimesupport );
                delete cenrep; cenrep = NULL;
                }
          if(runtimesupport)
          { */
         TBuf<KSysUtilVersionTextLength> productcode;
         productcode.Zero();
         error = SysVersionInfo::GetVersionInfo(SysVersionInfo::EProductCode, productcode);
         if(error ==KErrNone )
         {            
             if( productcode.Length()>0 )
             {        
             val = hbTrId("txt_device_update_dblist_product_code");
             str = QString::fromUtf16(productcode.Ptr(), productcode.Length());  
             FormatList(val,str);
             }                    
        }

    //IMEI   
    if(imei.Length()>0)
        {
        val = hbTrId("txt_device_update_dblist_imei");
        str = QString::fromUtf16(imei.Ptr(), imei.Length()); 
        FormatList(val,str);
        }

   
    iFotaState = FotaEngineL().GetState(-1);
    
    TBuf8<80> Name;
    TBuf8<80> Version;
    TInt Size;
    
    if(iFotaState == RFotaEngineSession::EDownloadComplete || iFotaState == RFotaEngineSession::EStartingUpdate
            || iFotaState == RFotaEngineSession::EStartingDownload || iFotaState == RFotaEngineSession::EDownloadProgressing)
        {
        label = qobject_cast<HbLabel*> (loader.findWidget("p:updatelabel"));
        label2 = qobject_cast<HbLabel*> (loader2.findWidget("l:updatelabel"));
        FotaEngineL().GetCurrentFirmwareDetailsL(Name, Version, Size);
        
        const QString
        name =
                QString::fromUtf8(
                        reinterpret_cast<const char*> (Name.Ptr()),
                                Name.Length());
        const QString
        ver =
                QString::fromUtf8(
                        reinterpret_cast<const char*> (Version.Ptr()),
                                Version.Length());
        
        TInt sizeKB = Size / 1024;
        if(sizeKB < 1024)
            {
            val = hbTrId("txt_device_update_setlabel_the_last_update_1_2_kb").arg(name)
                    .arg(ver).arg(sizeKB);
            }
        else
            {
            TInt sizeMB = sizeKB / 1024;
            val = hbTrId("txt_device_update_setlabel_the_last_update_1_2_mb").arg(name)
                    .arg(ver).arg(sizeMB);
            }
        label->setPlainText(val);
        label2->setPlainText(val);
		
		val = hbTrId("txt_device_update_button_resume_update");
    
        updateButton = qobject_cast<HbPushButton*>(loader.findWidget("p:update"));
        updateButton->setText(val);
        QObject::connect(updateButton, SIGNAL(clicked()), this, SLOT(ResumeUpdate()));
        updateButtonLandscape = qobject_cast<HbPushButton*>(loader2.findWidget("l:update"));
        updateButtonLandscape->setText(val);
		QObject::connect(updateButtonLandscape, SIGNAL(clicked()), this, SLOT(ResumeUpdate()));
		Connected = ETrue;
        }
    else
        {
        //Setting help text for update button
        label = qobject_cast<HbLabel*> (loader.findWidget("p:updatelabel"));
        label2 = qobject_cast<HbLabel*> (loader2.findWidget("l:updatelabel"));
        val = hbTrId("txt_device_update_setlabel_to_update_your_device_s");
        label->setPlainText(val);
        label2->setPlainText(val);
        
        val = hbTrId("txt_device_update_button_update");
        updateButton = qobject_cast<HbPushButton*>(loader.findWidget("p:update"));
        updateButton->setText(val);
        QObject::connect(updateButton, SIGNAL(clicked()), this, SLOT(CheckforUpdate()));
        updateButtonLandscape = qobject_cast<HbPushButton*>(loader2.findWidget("l:update"));
        updateButtonLandscape->setText(val);
        QObject::connect(updateButtonLandscape, SIGNAL(clicked()), this, SLOT(CheckforUpdate()));
        Connected = ETrue;
        }
    
    val = hbTrId("txt_device_update_button_advanced");
    advancedButton = qobject_cast<HbPushButton*>(loader.findWidget("p:advanced")); 
    advancedButton->setText(val);
    QObject::connect(advancedButton, SIGNAL(clicked()), this, SLOT(AdvancedDeviceManager()));
    advancedButtonLandscape = qobject_cast<HbPushButton*>(loader2.findWidget("l:advanced"));
    advancedButtonLandscape->setText(val);
    QObject::connect(advancedButtonLandscape, SIGNAL(clicked()), this, SLOT(AdvancedDeviceManager()));
    fotaSupportEnabled();
    mMainWindow->addView(fotaPortraitView);
    mMainWindow->addView(fotaLandscapeView);
    
    if(mMainWindow->orientation()==Qt::Vertical)
            mMainWindow->setCurrentView(fotaPortraitView);
        else
            mMainWindow->setCurrentView(fotaLandscapeView);
    
    iMoniter = CDeviceUpdateMoniter::NewL(this);
    //iMoniter->StartMoniter();
    emit applicationReady();
    qDebug("DMFotaView::addFotaView <<");
    return ETrue;
    }

// -----------------------------------------------------------------------------
// DMFotaView::CheckforUpdate
// Checks for updates from the default server
// -----------------------------------------------------------------------------
//
void DMFotaView::CheckforUpdate()
 {
    qDebug("omadm DeviceManagerUi::CheckforUpdate >>");
    
    TInt profileId=NULL;
    if(mainDmInfo==0)
        {
        if(!profilesView)
            {
            bool launchview = false;
            AdvancedDeviceManager(launchview);
           /* mainDmInfo = new DmInfo();
            mainDmInfo->refreshProfileList();*/
            }
       // else 
        mainDmInfo = profilesView->dminfo;      
        }
    if((profileId = mainDmInfo->DefaultFotaProfileIdL())==KErrNotFound)
        {

        HbNotificationDialog* note = new HbNotificationDialog();
        QString val = hbTrId("txt_device_update_info_no_server_configured_to_get");
        note->setTitle(val);
        note->setTitleTextWrapping(Hb::TextWordWrap);
        note->setDismissPolicy(HbPopup::TapAnywhere);
        note->setTimeout(HbPopup::StandardTimeout);
        note->setAttribute(Qt::WA_DeleteOnClose, true);
        note->open();
        }
    else
        {
        mainDmInfo->sync(profileId);
        //Connecting note to be displayed
        //Buttons to be disabled
        displayNoteAndDisableButtons();
        
        }
    qDebug("omadm DeviceManagerUi::CheckforUpdate >>");
 }


void DMFotaView::ResumeUpdate()
    {
    qDebug("omadm DeviceManagerUi::ResumeUpdate >>");
    
    
    if(iFotaState == RFotaEngineSession::EDownloadComplete || iFotaState == RFotaEngineSession::EStartingUpdate)
        {
        qDebug("Calling resuming of update");
        FotaEngineL().Update(-1,(TSmlProfileId)1, _L8(""), _L8(""));
        }
    else if(iFotaState == RFotaEngineSession::EStartingDownload || iFotaState == RFotaEngineSession::EDownloadProgressing)
        {
        qDebug("Calling resuming of download");
        FotaEngineL().TryResumeDownload(EFalse);        
        }
    
    qDebug("omadm DeviceManagerUi::ResumeUpdate <<");
    }

// -----------------------------------------------------------------------------
// DMFotaView::displayNoteAndDisableButtons
// Displays the connecting note and disable all buttons
// -----------------------------------------------------------------------------
//
void DMFotaView::displayNoteAndDisableButtons()
    {
    qDebug("DMFotaView::displayNoteAndDisableButtons >>");
    updateButton->setEnabled(false);
    advancedButton->setEnabled(false);
    updateButtonLandscape->setEnabled(false);
    advancedButtonLandscape->setEnabled(false);
    HbNotificationDialog* note = new HbNotificationDialog();
    //QString val = hbTrId("txt_device_update_dpophead_device_update");
    note->setTitle(hbTrId("txt_device_update_dpophead_device_update"));
    note->setTitleTextWrapping(Hb::TextWordWrap);
    note->setText(hbTrId("txt_device_update_info_connecting"));
    HbIcon icon1;
    icon1.setIconName(":/icons/qgn_prop_sml_http.svg");
    note->setIcon(icon1);
    note->setAttribute(Qt::WA_DeleteOnClose, true);
    note->open();
    qDebug("DMFotaView::displayNoteAndDisableButtons <<");
    }

// -----------------------------------------------------------------------------
// DMFotaView::enableButtons
// Enables checkupdate and Advancedsetting buttons
// -----------------------------------------------------------------------------
//
void DMFotaView::enableButtons()
    {
    qDebug("DMFotaView::enableButtons >>");
    TInt value (0);
    RProperty::Get(TUid::Uid(KOmaDMAppUid), KFotaServerActive, value);
    if (!value)
        {
        fotaSupportEnabled();
        advancedButtonLandscape->setEnabled(true);
        advancedButton->setEnabled(true);
        }
    qDebug("DMFotaView::enableButtons <<");
    }

// -----------------------------------------------------------------------------
// DMFotaView::AdvancedDeviceManager
// Displays the Profiles view
// -----------------------------------------------------------------------------
//
void DMFotaView::AdvancedDeviceManager(bool launchadvanceview)
 { 
    qDebug("omadm DeviceManagerUi::AdvancedDeviceManager");
    if(!profilesView)
        {
        qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 1");
        profilesView = new DmAdvancedView(mMainWindow,this,mainDmInfo);
        qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 2");
        bool loadingok = profilesView->displayItems();
        if(loadingok)
            {
            qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 3");
            mMainWindow->addView(profilesView);
            qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 4");
            profilesView->setBackBehavior();  
            qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 5");
            if(launchadvanceview)
            mMainWindow->setCurrentView(profilesView);
            qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 6");
            }
        else
            {
            qFatal("omadm Unable to read groupbox.docml");
        
            }
        }
    else
        {
        profilesView->setBackBehavior();                
        mMainWindow->setCurrentView(profilesView);   
        }
    qDebug("DMFotaView::AdvancedDeviceManager <<");
 }

// -----------------------------------------------------------------------------
// DMFotaView::FormatList
// Adds data to the details list
// -----------------------------------------------------------------------------
//
void DMFotaView::FormatList(QString val,QString str)
    {
    qDebug("DMFotaView::FormatList >>");
    label = qobject_cast<HbLabel*> (loader.findWidget(list1[i]));
    label2 = qobject_cast<HbLabel*> (loader2.findWidget(list2[i++]));         
    label3 = qobject_cast<HbLabel*> (loader.findWidget(list1[i]));
    label4 = qobject_cast<HbLabel*> (loader2.findWidget(list2[i++]));
    label->setPlainText(val);
    label2->setPlainText(val);
    label3->setPlainText(str);
    label4->setPlainText(str);
    label->setVisible(ETrue);
    label2->setVisible(ETrue);
    label3->setVisible(ETrue);
    label4->setVisible(ETrue);
    qDebug("DMFotaView::FormatList <<");
    }


inline RFotaEngineSession & DMFotaView::FotaEngineL()
    {
    qDebug("DMFotaView::FotaEngineL >>");
    if (!iFotaEngine.Handle())
        iFotaEngine.OpenL();
    qDebug("DMFotaView::FotaEngineL <<");
    return iFotaEngine;
    }
// -----------------------------------------------------------------------------
// DMFotaView::backtoMainWindow
// Enables switching between views
// -----------------------------------------------------------------------------
//
void DMFotaView::backtoMainWindow()
    {
    qDebug("DMFotaView::backtoMainWindow >>");
    QString appName = qApp->applicationName();
    QString cpAppTitle("Control panel");
    mMainWindow->removeView(fotaPortraitView);
    mMainWindow->removeView(fotaLandscapeView);
    fotaPortraitView->deleteLater();
    fotaLandscapeView->deleteLater();
    fotaPortraitView = NULL;
    fotaLandscapeView = NULL;
    if (appName == cpAppTitle)
        {
        TApaTaskList taskList(CEikonEnv::Static()->WsSession());
        TApaTask task = taskList.FindApp(KControlPanelAppUid);
        if (task.Exists())
            {
            task.BringToForeground();
            }
        }
    qApp->quit();
    qDebug("DMFotaView::backtoMainWindow <<");
    }

// -----------------------------------------------------------------------------
// DMFotaView::OnHelp
// Displays Help contents
// -----------------------------------------------------------------------------
//
void DMFotaView::OnHelp()
{   
}

// -----------------------------------------------------------------------------
// DMFotaView::OnExit
// -----------------------------------------------------------------------------
//
void DMFotaView::OnExit()
{
    qDebug("DMFotaView::OnExit >>");
    qApp->quit();
    qDebug("DMFotaView::OnExit <<");
}

void DMFotaView::readSection( Qt::Orientation orientation )
{
    qDebug("DMFotaView::readSection >>");
    if(mMainWindow->currentView() == fotaPortraitView || mMainWindow->currentView() == fotaLandscapeView)
        {
        if( orientation == Qt::Vertical ) {
        // reading "portrait" section
        mMainWindow->setCurrentView(fotaPortraitView);
        }
     else {
        // reading "landscape" section
        mMainWindow->setCurrentView(fotaLandscapeView);
         }
        }
    else if(mMainWindow->currentView()==profilesView)
        {
        profilesView->reLayout(orientation);
        }
    qDebug("DMFotaView::readSection <<");

}   


void DMFotaView::UpdateDMUI(TBool aVal)
    {
    qDebug("DMFotaView::UpdateDMUI >>");
    qDebug("DMFotaView::UpdateDMUI >>");
	    QString val;
    if (aVal == 1)
        {
        qDebug("DMFotaView::aVal == 1 >>");
        if(Connected)
            {
            qDebug("DMFotaView::Connected >>");
            QObject::disconnect(updateButton,0,0,0);
            QObject::disconnect(updateButtonLandscape,0,0,0);
            Connected = EFalse;
            }
    
        iFotaState = FotaEngineL().GetState(-1);

        if (iFotaState == RFotaEngineSession::EDownloadComplete || iFotaState
                == RFotaEngineSession::EStartingUpdate || iFotaState
                == RFotaEngineSession::EStartingDownload || iFotaState
                == RFotaEngineSession::EDownloadProgressing)
            {
            qDebug("CDeviceUpdateMoniter::Resume >>");
            QString value = hbTrId("txt_device_update_button_resume_update");
            //updateButton = qobject_cast<HbPushButton*>(loader.findWidget("p:update");
            updateButton->setText(value);
            QObject::connect(updateButton, SIGNAL(clicked()), this,SLOT(ResumeUpdate()));
            //updateButtonLandscape = qobject_cast<HbPushButton*>(loader2.findWidget("l:update");
            updateButtonLandscape->setText(value);
		QObject::connect(updateButtonLandscape, SIGNAL(clicked()), this, SLOT(ResumeUpdate()));
		Connected = ETrue;

		TBuf8<80> Name;
		TBuf8<80> Version;
		TInt Size;

		FotaEngineL().GetCurrentFirmwareDetailsL(Name, Version, Size);
        
        	const QString
	        name =
        	        QString::fromUtf8(
	                        reinterpret_cast<const char*> (Name.Ptr()),
                                Name.Length());
	        const QString
	        ver =
        		QString::fromUtf8(
                        reinterpret_cast<const char*> (Version.Ptr()),
                                Version.Length());
        
        	TInt sizeKB = Size / 1024;
	        if(sizeKB < 1024)
	            {
	            val = hbTrId("txt_device_update_setlabel_the_last_update_1_2_kb").arg(name)
	                    .arg(ver).arg(sizeKB);
	            }
	        else
	            {
	            TInt sizeMB = sizeKB / 1024;
	            val = hbTrId("txt_device_update_setlabel_the_last_update_1_2_mb").arg(name)
        	            .arg(ver).arg(sizeMB);
	            }
	        
	        label->setPlainText(val);
	        label2->setPlainText(val);
	    }
        else
	    {
        qDebug("DMFotaView::Update >>");
        val = hbTrId("txt_device_update_setlabel_to_update_your_device_s");
	    label->setPlainText(val);
	    label2->setPlainText(val);
	    val = hbTrId("txt_device_update_button_update");
	    updateButton = qobject_cast<HbPushButton*>(loader.findWidget("p:update"));
	    updateButton->setText(val);
	    QObject::connect(updateButton, SIGNAL(clicked()), this, SLOT(CheckforUpdate()));
	    updateButtonLandscape = qobject_cast<HbPushButton*>(loader2.findWidget("l:update"));
	    updateButtonLandscape->setText(val);
	    QObject::connect(updateButtonLandscape, SIGNAL(clicked()), this, SLOT(CheckforUpdate()));
	    Connected = ETrue;
	    }
	}
    fotaSupportEnabled();
    if(fotaValue == 1 && aVal == 0)
    {
    	updateButton->setEnabled(aVal);
      updateButtonLandscape->setEnabled(aVal);
    }
    advancedButton->setEnabled(aVal);
    advancedButtonLandscape->setEnabled(aVal);
    qDebug("DMFotaView::UpdateDMUI <<");
    qDebug("DMFotaView::UpdateDMUI <<");
}

void DMFotaView::fotaSupportEnabled()
    { 
    fotaValue = 1;	
    CRepository* centrep( NULL);
    TUid uidValue = {0x101F9A08}; // KCRFotaAdapterEnabled
    TInt err = KErrNone;
    TRAP(err,centrep = CRepository::NewL( uidValue)); 
    if(centrep && err == KErrNone )
    {  
        centrep->Get( 1 , fotaValue ); // KCRFotaAdapterEnabled     
        delete centrep;
    }
    if(fotaValue == 0 || fotaValue == 1)
        {
        updateButton->setEnabled(fotaValue);
        updateButtonLandscape->setEnabled(fotaValue);
        }
    }
    

