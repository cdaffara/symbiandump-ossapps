/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */


#include <hbdevicedialog.h>
#include <apgtask.h>
#include <eikenv.h>
#include <driveinfo.h>
#include <hbapplication.h>
#include <hbmessagebox.h>
#include "amstorage.h"
#include "appmgmtnotifier.h"
#include "appmgmtdownloadmgr.h"
#include "ApplicationManagementUtility.h"
#include "debug.h"

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::AppMgmtNotifier
// ------------------------------------------------------------------------------------------------ 
AppMgmtNotifier::AppMgmtNotifier(QString aAppName)
    {
    m_appname = aAppName;
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::~AppMgmtNotifier
// ------------------------------------------------------------------------------------------------ 
AppMgmtNotifier::~AppMgmtNotifier()
    {
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::getAvailableDrives
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::getAvailableDrives()
    {
    RFs iFs;
    int err;
    qt_symbian_throwIfError(iFs.Connect());
    TDriveList driveList;
    TInt driveCount = 0;
    TBuf<20> buf;
    int i = 0;
    QString val;
    QStringList list;
    TDriveInfo info;
    TVolumeInfo volumeInfo;
    qt_symbian_throwIfError(DriveInfo::GetUserVisibleDrives(iFs, driveList,
            driveCount));
    TUint driveStatus = 0;
    for (TInt index(0); index < KMaxDrives; index++)
        {
        if (driveList[index])
            {
            TRealFormat realFormat;
            qt_symbian_throwIfError(DriveInfo::GetDriveStatus(iFs, index,
                    driveStatus));
            TFileName path;
            TDriveUnit driveUnit(index);
            if (!(driveStatus & DriveInfo::EDriveRemote))
                {
                TBuf<64> freeSpace;
                TReal free;
                TInt64 freeKBytes;
                realFormat.iWidth = 6;
                iFs.Drive(info, index);
                if(iFs.Volume(volumeInfo, index)==KErrNone)
                    {
                freeKBytes = 0;
                freeKBytes = volumeInfo.iFree / 1024;
                RDEBUG_2("free space: (%d)", volumeInfo.iFree  );
                free = volumeInfo.iFree / 1024;
                freeSpace.Num(freeKBytes);
                TInt sizeUnit = -1;

                //converting the drive freespace
                int len = freeSpace.Length();
                if (len >= 7)
                    {
                    TReal nul = (1024 * 1024);
                    TReal temp = free / nul;
                    freeSpace.Num(temp, realFormat);
                    sizeUnit = 1;
                    }
                else
                    {
                    TReal temp = free / 1024;
                    freeSpace.Num(temp, realFormat);
                    sizeUnit = 0;
                    }
                //Getting the drive name in QString
                path.Zero();
                path.Append(driveUnit.Name());
                val = QString::fromUtf16(path.Ptr(), path.Length());
                m_Drilist.Append(path);
                //Getting the drive free space in QString
                path.Zero();
                path.Append(freeSpace);
                QString str1 = QString::fromUtf16(path.Ptr(), path.Length());
                QString driv;
                if (sizeUnit == -1)
                    {
                    driv
                            = hbTrId(
                                    "txt_deviceupdate_setlabel_install_to_val_1_2_kb").arg(
                                    val, str1);
                    }
                else if (sizeUnit == 0)
                    {
                    driv
                            = hbTrId(
                                    "txt_deviceupdate_setlabel_install_to_val_1_2_Mb").arg(
                                    val, str1);
                    }
                else
                    {
                    driv
                            = hbTrId(
                                    "txt_deviceupdate_setlabel_install_to_val_1_2_Gb").arg(
                                    val, str1);
                    }
                    m_DriveBox->insertItem(i++, driv);
                    }
                }
            }
        }
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::showInstallDialog
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::showInstallDialog(CDeploymentComponent *aCompo, TRequestStatus &s)
    {
    bringServerToForeground();
    iStat = &s;
    iComp = aCompo;
    HbDocumentLoader loader;
    QString val;
    bool ok = false;
    HbLabel* label;
    int err;    
    iComp->SetDriveSelectionStatus(true);
    CDeliveryComponentStorage* iStorage ;
    QT_TRAP_THROWING(iStorage= CDeliveryComponentStorage::NewL());
    TRAP( err, QT_TRYCATCH_LEAVING(iStorage->UpdateL(*iComp)));
    delete iStorage;
    loader.load(":/xml/sample.docml", &ok);
    if (!ok)
        {
        return;
        }
    Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
    m_Dialog = qobject_cast<HbDialog*> (loader.findWidget("dialog"));

    //set title for the dialog
    label = qobject_cast<HbLabel*> (loader.findWidget(
            "qtl_dialog_pri_heading"));

    val = hbTrId("txt_device_update_title_install");
    label->setPlainText(val);

    TDeploymentComponentName name = aCompo->ComponentName();
    TBuf<KDeploymentComponentNameMaxLength> nameBuf;
    nameBuf.Copy(name);
    m_appname = QString::fromUtf16(nameBuf.Ptr(), nameBuf.Length());
    //set the app name
    label = qobject_cast<HbLabel*> (loader.findWidget("label"));
    val = hbTrId("txt_deviceupdate_info_file_1_2").arg(m_appname);
    label->setPlainText(val);

    //size
    label = qobject_cast<HbLabel*> (loader.findWidget("label_1"));
	label->setPlainText("");
    //icon
    label = qobject_cast<HbLabel*> (loader.findWidget("icon"));
    label->setIcon(HbIcon(":/icon/qgn_prop_sml_http.svg"));

    //"install to" label
    label = qobject_cast<HbLabel*> (loader.findWidget("qtl_dialog_pri5"));
    val = hbTrId("txt_device_update_setlabel_install_to");
    label->setPlainText(val);

    //getting the available free space in all drives and adding to combobox

    m_DriveBox = qobject_cast<HbComboBox*> (loader.findWidget("combobox"));
    
    TRAP(err,getAvailableDrives());
    if(err==KErrNone)
        {
    connect(m_DriveBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(defaultDriveChanged(int )));
        }
    else
        m_DriveBox->setVisible(false);
    //setting the actions
    m_OkButton = qobject_cast<HbAction*> (loader.findObject(
            "qtl_dialog_softkey_2_left"));
    m_Cancel = qobject_cast<HbAction*> (loader.findObject(
            "qtl_dialog_softkey_2_right"));
    val = hbTrId("txt_common_button_ok");
    m_OkButton->setText(val);
    val = hbTrId("txt_common_button_cancel");
    m_Cancel->setText(val);
    m_Dialog->setTimeout(20000);
    CApplicationManagementUtility::mCurrDlg = m_Dialog;
    m_Dialog->open(this, SLOT(dlgSlot(HbAction*)));
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::defaultDriveChanged
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::defaultDriveChanged(int i)
    {
    TAMInstallOptions opts = iComp->InstallOpts();
    TChar drive = m_Drilist[i*2];
    drive.LowerCase();
    int err;
    opts.iOptions.iDrive = drive;
    TPtrC8 buf;
    TAMInstallOptionsPckg optsb(opts);
    buf.Set(optsb);
    TRAP( err, QT_TRYCATCH_LEAVING(iComp->SetInstallOptsL(buf)));
    CDeliveryComponentStorage* iStorage;
    TRAP( err, QT_TRYCATCH_LEAVING(iStorage = CDeliveryComponentStorage::NewL()));
    TRAP( err, QT_TRYCATCH_LEAVING(iStorage->UpdateL(*iComp)));
    delete iStorage;
    iStorage = NULL;
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::dlgSlot
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::dlgSlot(HbAction* action)
    {
    if(action == m_Cancel)
        {
        sendServerToBackground();
        TRequestStatus* status(iStat);
        User::RequestComplete(status, KStatusUserCancelled);
        }
    else
        {
        TRequestStatus* status(iStat);
        User::RequestComplete(status, KErrCompletion );      
        }
    CApplicationManagementUtility::mCurrDlg = 0;
    m_OkButton=0;
    m_Cancel = 0;
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::showInstallFailedNote
//launching a device dialog to display notefor "install failed" case
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::showInstallFailedNote()
    {
    HbDeviceDialog deviceDialog;
    QVariantMap parameters;
    //_LIT(KScomoNotifier, "scomonotifier");
    parameters.insertMulti(QString("scomonotifier"), QVariant(QString("installfailed")));
    QString data = m_appname;
    parameters.insertMulti(QString("scomoappname"), QVariant(data));
    const char *deviceDialogType = "com.nokia.hb.devicemanagementdialog/1.0";
    bool ret = deviceDialog.show(QString(deviceDialogType), parameters);
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::sendServerToBackground
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::sendServerToBackground()
    {
    CCoeEnv* coe = CCoeEnv::Static();
    TApaTaskList taskList(coe->WsSession());
    TApaTask task=taskList.FindApp(TUid::Uid(KAppMgmtServerUid));  
    if(task.Exists())
        {
        task.SendToBackground();
        }
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::bringServerToForeground
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::bringServerToForeground()
    {
    CCoeEnv* coe = CCoeEnv::Static();
    TApaTaskList taskList(coe->WsSession());
    TApaTask task = taskList.FindApp(TUid::Uid(KAppMgmtServerUid));
    if (task.Exists())
        {
        task.BringToForeground();
        }
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::operationSuccessSlot
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::successSlot(HbAction* action)
    {
    RDEBUG("successslot");
    if(action == m_OkButton || action == NULL)
        {
        sendServerToBackground();
        RDEBUG("slot entered");
        TRequestStatus* status(iStat);
        User::RequestComplete(status, KErrCompletion);
        }
    m_OkButton = 0;
    RDEBUG("successslot end");
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::showInstallSuccessNote
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::showInstallSuccessNote()
    {
    if (CApplicationManagementUtility::mHidden == 1)
        {
        //Display Installation complete dialog
        HbDeviceNotificationDialog notificationDialog;
        QString text =
                hbTrId("txt_device_update_title_installation_complete");
        notificationDialog.setTitle(text);
        text
                = hbTrId("txt_device_update_dblist_product_code_val_installa").arg(
                        m_appname);
        notificationDialog.setText(text);
        notificationDialog.show();
        }
    else
        {
        bringServerToForeground();
        HbDocumentLoader loader;
        bool ok = false;
        loader.load(":/xml/error_notes.docml", &ok);
        QString val;
        // Exit if the file format is invalid
        Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
        m_Dialog = qobject_cast<HbDialog*> (loader.findWidget(
                "dialog"));
        HbLabel* label = qobject_cast<HbLabel*> (loader.findWidget(
                "heading_text"));
        val = hbTrId("txt_device_update_title_installation_complete");
        label->setPlainText(val);
        
        val = hbTrId("txt_deviceupdate_info_file_1_2").arg(m_appname);
        label = qobject_cast<HbLabel*> (loader.findWidget("content_text"));
        label->setPlainText(val);
        m_OkButton = qobject_cast<HbAction*> (loader.findObject(
                "qtl_dialog_softkey_2_left"));
        val = hbTrId("txt_common_button_ok");
        m_OkButton->setText(val);
        
        m_Dialog->setTimeout(HbPopup::ContextMenuTimeout);
        //m_Dialog->setAttribute(Qt::WA_DeleteOnClose);
        m_Dialog->open(this, SLOT(successSlot(HbAction*)));
	    CDialogWait* wait = CDialogWait::NewL();
        registerStatus(wait->iStatus);
	    RDEBUG_2("AppMgmtNotifier::Start before StartWait (%d)", wait->iStatus.Int());
        wait->StartWait();
	    delete wait;
        }
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::dialogUnSlot
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::dialogUnSlot(HbAction* retAction)
    {
    sendServerToBackground();
    if (retAction == m_Cancel)
        {      
        TRequestStatus* status(iStat);
        User::RequestComplete(status, KStatusUserCancelled);    
        }
    else
        {
        TRequestStatus* status(iStat);
        User::RequestComplete(status, KErrCompletion);
        }
    CApplicationManagementUtility::mCurrDlg = 0;
    m_OkButton = 0;
    m_Cancel = 0;
    }


// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::showUninstallDialog
//uninstall beginning confirmation note
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::showUninstallDialog(const CDeploymentComponent &aCompo,
        TRequestStatus &s)
    {
    iStat = &s;
    bringServerToForeground();
    
    HbDocumentLoader loader;
    bool ok;
    loader.load(":/xml/downloaddialog.docml", &ok);

    Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
    m_Dialog = qobject_cast<HbDialog*> (loader.findWidget("dialog"));

    HbLabel* label = qobject_cast<HbLabel*> (loader.findWidget(
            "qtl_dialog_pri_heading"));
    QString val = hbTrId("txt_device_update_title_uninstalling_application");
    label->setPlainText(val);

    label = qobject_cast<HbLabel*> (loader.findWidget("icon"));
    label->setIcon(HbIcon(":/icons/qgn_prop_sml_http.svg"));

    label = qobject_cast<HbLabel*> (loader.findWidget("qtl_dialog_pri5"));
    TDeploymentComponentName name = aCompo.ComponentName();
    TBuf<255> ne;
    ne.Copy(name);

    QString str = QString::fromUtf16(ne.Ptr(), ne.Length());
    val = hbTrId("txt_deviceupdate_info_application_1").arg(str);
    label->setPlainText(val);

    label = qobject_cast<HbLabel*> (loader.findWidget("qtl_dialog_pri5_1"));
    label->setPlainText("");
    
    m_OkButton = qobject_cast<HbAction*> (loader.findObject(
            "qtl_dialog_softkey_2_left"));
    m_Cancel = qobject_cast<HbAction*> (loader.findObject(
            "qtl_dialog_softkey_2_right"));
    val = hbTrId("txt_common_button_ok");
    m_OkButton->setText(val);
    val = hbTrId("txt_common_button_cancel");
    m_Cancel->setText(val);
    CApplicationManagementUtility::mCurrDlg = m_Dialog;
    m_Dialog->setTimeout(10000);
    m_Dialog->open(this, SLOT(dialogUnSlot(HbAction*)));
    }


// ------------------------------------------------------------------------------------------------
//AppMgmtNotifier::showUnInstallFailedNote
//Displaying uninstall failed note
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::showUnInstallFailedNote()
    {
    HbDeviceDialog deviceDialog;
    QVariantMap parameters;
    //_LIT(KScomoNotifier, "scomonotifier");
    parameters.insertMulti(QString("scomonotifier"), QVariant(QString(
            "uninstallfailed")));
    QString data = m_appname;
    parameters.insertMulti(QString("scomoappname"), QVariant(data));
    const char *deviceDialogType = "com.nokia.hb.devicemanagementdialog/1.0";
    bool ret = deviceDialog.show(QString(deviceDialogType), parameters);
    }


// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::showUnInstallSuccessNote
//uninstall success notification
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::showUnInstallSuccessNote()
    {
    if (CApplicationManagementUtility::mHidden == 1)
        {
        //Display Installation complete dialog
        HbDeviceNotificationDialog notificationDialog;
        QString text = hbTrId(
                "txt_device_update_title_uninstallation_complete");
        notificationDialog.setTitle(text);
        text
                = hbTrId(
                        "txt_device_update_dblist_product_code_val_uninstalla_comple").arg(
                        m_appname);
        notificationDialog.setText(text);
        notificationDialog.show();
        }
    else
        {
        bringServerToForeground();

        HbDocumentLoader loader;
        bool ok = false;
        loader.load(":/xml/error_notes.docml", &ok);
        QString val;
        // Exit if the file format is invalid
        Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
        m_Dialog = qobject_cast<HbDialog*> (loader.findWidget(
                "dialog"));
        QString blank("");
        HbLabel* label = qobject_cast<HbLabel*> (loader.findWidget(
                "heading_text"));
        val = hbTrId("txt_device_update_title_uninstallation_complete");
        label->setPlainText(val);
        val
                = hbTrId(
                        "txt_device_update_dblist_product_code_val_uninstalla_comple").arg(
                        m_appname);
        label = qobject_cast<HbLabel*> (loader.findWidget("content_text"));
        label->setPlainText(val);
        m_OkButton = qobject_cast<HbAction*> (loader.findObject(
                "qtl_dialog_softkey_2_left"));
        val = hbTrId("txt_common_button_ok");
        m_OkButton->setText(val);
        
        m_Dialog->setTimeout(HbPopup::StandardTimeout);
        m_Dialog->open(this, SLOT(successSlot(HbAction*)));
        CDialogWait* wait = CDialogWait::NewL();
        registerStatus(wait->iStatus);
        RDEBUG_2("AppMgmtNotifier::Start before StartWait (%d)", wait->iStatus.Int());
        wait->StartWait();
        delete wait;
        }
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::showDownloadFailedNote
// ------------------------------------------------------------------------------------------------ 
bool AppMgmtNotifier::showDownloadFailedNote(QString aNotifierdata)
    {
    HbDeviceDialog deviceDialog;
    QVariantMap parameters;
    //_LIT(KScomoNotifier, "scomonotifier");
    parameters.insertMulti(QString("scomonotifier"), QVariant(QString(
            "downloadfailed")));
    parameters.insertMulti(QString("scomoappdata"),QVariant(aNotifierdata));
    const char *deviceDialogType = "com.nokia.hb.devicemanagementdialog/1.0";
    bool stat = deviceDialog.show(QString(deviceDialogType), parameters); 
    return stat;
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtNotifier::showDownloadSuccessNote
// ------------------------------------------------------------------------------------------------ 
void AppMgmtNotifier::showDownloadSuccessNote()
    {
    if (CApplicationManagementUtility::mHidden == 1)
        {
        HbDeviceNotificationDialog notificationDialog;
        QString tr = hbTrId("txt_device_update_title_download_complete");
        notificationDialog.setTitle(tr);
        tr
                = hbTrId(
                        "txt_device_update_dblist_product_code_val_download_comple").arg(
                        m_appname);
        notificationDialog.setText(tr);
        notificationDialog.show();
        }
    else
        {
        bringServerToForeground();
        HbDocumentLoader loader;
        bool ok = false;
        loader.load(":/xml/error_notes.docml", &ok);
        QString val;
        // Exit if the file format is invalid
        Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
        m_Dialog = qobject_cast<HbDialog*> (loader.findWidget(
                "dialog"));
        HbLabel* label = qobject_cast<HbLabel*> (loader.findWidget(
                "heading_text"));
        val = hbTrId("txt_device_update_title_download_complete");
        label->setPlainText(val);

        val = hbTrId("txt_device_update_dblist_product_code_val_download_comple").arg(m_appname);
        label = qobject_cast<HbLabel*> (loader.findWidget("content_text"));
        label->setPlainText(val);
        m_OkButton = qobject_cast<HbAction*> (loader.findObject(
                "qtl_dialog_softkey_2_left"));
        val = hbTrId("txt_common_button_ok");
        m_OkButton->setText(val);
        
        m_Dialog->setTimeout(HbPopup::ContextMenuTimeout);

        m_Dialog->open(this, SLOT(successSlot(HbAction*)));
        CDialogWait* wait = CDialogWait::NewL();
        registerStatus(wait->iStatus);
        RDEBUG_2("AppMgmtNotifier::Start before StartWait (%d)", wait->iStatus.Int());
        wait->StartWait();
        delete wait;
        }
    }
void AppMgmtNotifier::registerStatus(TRequestStatus &stat)
    {
    RDEBUG("registering request status");
    iStat = &stat;
    stat = KRequestPending;
    RDEBUG_2("stat = %d",stat.Int() );
    RDEBUG("registering request status end");
    }
