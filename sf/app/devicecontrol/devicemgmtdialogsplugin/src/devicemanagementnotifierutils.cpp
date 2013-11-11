/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/

#include "devicemanagementnotifierutils.h"
#include <hbdocumentloader.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <e32property.h>
#include <qdebug.h>
#include <devicedialogsymbian.h>
#include "pnputillogger.h"
#include "omacppinquerydialog.h"
#include "devicemanagementnotifierwidget_p.h"
 enum TSyncmlHbNotifierKeys 
		{

     EHbSOSNotifierKeyStatus = 11, // status set will complete the client subscribe
     EHbSOSNotifierKeyStatusReturn = 12, // Return the content of actual status value accepted from UI
     
     EHbDMSyncNotifierKeyStatus = 13,
     EHbDMSyncNotifierKeyStatusReturn = 14
		};


devicemanagementnotifierutils::devicemanagementnotifierutils(devicemanagementnotifierwidget* ptr,
        const QVariantMap &parameters)
    {
	qDebug("devicemanagementnotifierutils");
	LOGSTRING("devicemanagementnotifierutils");
	
	QVariantMap::const_iterator i = parameters.constBegin();
    int notifiertolaunch = 0;
	while (i != parameters.constEnd())
        {
            if (i.key().toAscii() == "syncmlfw")
                {

            notifiertolaunch = i.value().toInt();
               // profileidenabled = true;
                }           
            ++i;
            }
    if(notifiertolaunch == 1000001 )// Connecting dialog
        {
    connectDialog = new syncmlConnectNotifier(ptr);
    	 connectDialog->launchDialog(parameters);
    	 notifier = NULL;
        }
        
    else 
        {
    notifier = new syncmlnotifier(ptr);
       notifier->launchDialog(parameters);
       connectDialog = NULL;
        }
        }
devicemanagementnotifierutils::~devicemanagementnotifierutils()
    {
    qDebug("devicemanagementnotifierutils::~devicemanagementnotifierutils");
    if(notifier)
    delete notifier;
    if(connectDialog)
    delete connectDialog;
    }

syncmlnotifier::syncmlnotifier(devicemanagementnotifierwidget* ptr)
    {
    iPtr = ptr;
    mDefaultServerPkgZero = false;
    qDebug("devicemanagementnotifierutils syncmlnotifier");
    QTranslator *translator = new QTranslator();
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    bool fine = translator->load("deviceupdates_en.qm", path);
    if (fine)
        qApp->installTranslator(translator);

    QTranslator *commontranslator = new QTranslator();

    fine = commontranslator->load("common_" + lang, path);
    if (fine)
        qApp->installTranslator(commontranslator);

    msymnotifier = new syncmlnotifierprivate();
    
    }

void syncmlnotifier::launchDialog(const QVariantMap &parameters)
    {

    QVariantMap::const_iterator i = parameters.constBegin();
    qDebug("devicemanagementnotifierutils syncmlnotifier launchDialog");
    //i++;

    bool profileidenabled = false;
    bool uiserverinteraction = false;
    bool serverpushinformativeenabled = false;
    bool serverpushinteractiveenabled = false;
    bool scomonotifierenabled = false;
    
    int profileid = 0;
    int uimode = 0;

    QString serverpushinformmessage;
    QString serverpushconfirmmessage;
    QString appname = NULL;
    QString size = NULL;
    QString sizeformat = NULL;
    QString data = NULL;
    QString serverdispname;
    bool defaultprofile = false;
	QString scomonotifiertype;
    int timeout = 0;
    while (i != parameters.constEnd())
        {
        if (i.key().toAscii() == "profileid")
            {

            profileid = i.value().toInt();
            profileidenabled = true;
            }

        if (i.key().toAscii() == "uimode")
            {
            uiserverinteraction = true;
            uimode = i.value().toInt();
            }

        if (i.key().toAscii() == "serverpushinformative")
            {
			qDebug("server push informative enabled");
            serverpushinformativeenabled = true;
            serverpushinformmessage = i.value().toString();
            }

        if (i.key().toAscii() == "serverpushconfirmative")
            {
		    LOGSTRING("Server Push Confirmative");
            serverpushinteractiveenabled = true;
            serverpushconfirmmessage = i.value().toString();
            }

        if (i.key().toAscii() == "serverdisplayname")
            {
            serverdispname = i.value().toString();
            }
        
        if (i.key().toAscii() == "fotadefaultprofile")
            {
            defaultprofile = i.value().toBool();
            }
        
        if (i.key().toAscii()== "scomonotifier")
            {
            scomonotifierenabled = true;
            scomonotifiertype = i.value().toString();
            }
        if (i.key().toAscii()=="scomoappdata")
            {
            //getting data for download failed cases
            data = i.value().toString();
            QStringList list1 = data.split(",");
            appname = list1[0];
            size =list1[1];
            sizeformat = list1[2];
            }
        if(i.key().toAscii()=="scomoappname")
            {
            //getting app name for install and uninstall failed cases
            appname = i.value().toString();
            }
        if (i.key().toAscii() == "pkgzerotimeout")
            {
            timeout = i.value().toInt();
            }
        ++i;
        }

    if (profileidenabled && uiserverinteraction)
        {
        qDebug("server interaction packet 0 enabled ");
        qDebug()<<EHbSOSNotifierKeyStatus;
        msymnotifier->setnotifierorigin(EHbSOSNotifierKeyStatus);
        createserverinitnotifier(profileid, uimode, serverdispname,
                defaultprofile, timeout);
        }
    else if (serverpushinformativeenabled)
        {
        qDebug("server push informative enabled ");
        qDebug()<<EHbDMSyncNotifierKeyStatus;
        msymnotifier->setnotifierorigin(EHbDMSyncNotifierKeyStatus);
        createserveralertinformative(serverpushinformmessage);
        }
    else if (serverpushinteractiveenabled)
        {
        qDebug("server push informative enabled ");
        qDebug()<<EHbDMSyncNotifierKeyStatus;
         msymnotifier->setnotifierorigin(EHbDMSyncNotifierKeyStatus);
        createserveralertconfirmative(serverpushconfirmmessage);
        }
    else if (scomonotifierenabled)
        {
        createscomonotifier(scomonotifiertype,appname,size,sizeformat);
        }

    }

void syncmlnotifier::createscomonotifier(QString notifiertype,QString app_name,QString asize,QString format)
    {
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/resources/error_notes.docml", &ok);
    if (!ok)
        {
        return;
        }
    QString val;
    HbDialog* dialog = qobject_cast<HbDialog*> (loader.findWidget("dialog"));
    HbLabel* label;

    dialog->setTimeout(HbPopup::ContextMenuTimeout);

    if (notifiertype == "installfailed")
        {
        //install failed case
        label = qobject_cast<HbLabel*> (loader.findWidget("heading_text"));
        val = hbTrId("txt_device_update_title_installation_failed");
        label->setPlainText(val);
        val = hbTrId("txt_device_update_info_installation_of_1_kb_failed").arg(
                app_name);
        label = qobject_cast<HbLabel*> (loader.findWidget("content_text"));
        label->setPlainText(val);
        }
    else if (notifiertype == "downloadfailed")
        {
        //download failed
        label = qobject_cast<HbLabel*> (loader.findWidget("heading_text"));
        val = hbTrId("txt_device_update_title_downloading_failed");

        label->setPlainText(val);
        int sizfor = -1;
        format == QString("kb") ? sizfor = 0 : sizfor = 1;
        format == QString("mb") ? sizfor = 1 : sizfor = 2;
        switch (sizfor)
            {
            case 0:
                val
                        = hbTrId(
                                "txt_device_update_info_downloading_of_1_Kb_failed").arg(
                                app_name, asize);
                break;
            case 1:
                val
                        = hbTrId(
                                "txt_device_update_info_downloading_of_1_Mb_failed").arg(
                                app_name, asize);
                break;
            default:
                val
                        = hbTrId(
                                "txt_device_update_info_downloading_of_1_Gb_failed").arg(
                                app_name, asize);
                break;
            }
        label = qobject_cast<HbLabel*> (loader.findWidget("content_text"));
        label->setPlainText(val);
        }
    else
        {
        //uninstall failed
        label = qobject_cast<HbLabel*> (loader.findWidget("heading_text"));
        val = hbTrId("txt_device_update_title_Uninstallation_failed");
        label->setPlainText(val);
        val = hbTrId("txt_device_update_info_uninstalling_of_1_Mb_failed").arg(
                app_name, asize);

        label = qobject_cast<HbLabel*> (loader.findWidget("content_text"));
        label->setPlainText(val);
        }
    dialog->setTimeout(HbPopup::StandardTimeout);
    dialog->show();
    }
void syncmlnotifier::createserverinitnotifier(int& profileid, int& uimode,
        QString& servername, bool& defaultprofile, int& timeout)
    {
    
    int inittype = msymnotifier->serverinittype(uimode);
    qDebug("Notification type");
    qDebug()<<inittype;

	LOGSTRING2("Init value = %d", inittype);

    /*For testing purpose*/
    //inittype = ESANUserInteractive;

    if (inittype == ESANUserInteractive)
        {
        HbDocumentLoader loader;
        bool ok = false;
        loader.load(":/xml/resources/dialog.docml", &ok);
        if (!ok)
            {
            return;
            }
        HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget(
                "dialog"));

        //set heading content
        HbLabel *contentheading = qobject_cast<HbLabel *> (loader.findWidget(
                "qtl_dialog_pri_heading"));
        QString heading;        
        //set body content
        HbLabel *contentbody = qobject_cast<HbLabel *> (loader.findWidget(
                "qtl_dialog_pri5"));
        QString serverinitmessage;
        QString softkeyok;
        QString softkeyCancel;
        if(defaultprofile)
            {
            mDefaultServerPkgZero = true;	
            heading = hbTrId("txt_device_update_title_update_available");
            serverinitmessage = 
              hbTrId("txt_device_update_info_recommended_update_is_avail").arg(
                            servername);
            softkeyok = hbTrId("txt_device_update_button_update");
            softkeyCancel = hbTrId("txt_device_update_button_later");                 
            }
        else
            {
            mDefaultServerPkgZero = false;	
            heading = hbTrId("txt_device_update_title_service_recomended");
            serverinitmessage = 
              hbTrId("txt_device_update_info_1_recommends_a_service_for").arg(
                            servername);
            softkeyok = hbTrId("txt_common_button_ok");
            softkeyCancel = hbTrId("txt_common_button_cancel");                            
            }
        contentheading->setPlainText(heading);
        contentbody->setPlainText(serverinitmessage);

        //set softkeys
        HbAction *primaryAction = (HbAction *) dialog->actions().first();
        
        primaryAction->setText(softkeyok);

        HbAction *secondaryAction = (HbAction *) dialog->actions().at(1);
        
        secondaryAction->setText(softkeyCancel);

        //set dialog properties
        dialog->setTimeout(timeout);
        dialog->setDismissPolicy(HbPopup::NoDismiss);
        //load appropriate icon from svg file
        HbLabel* label1 = qobject_cast<HbLabel*> (loader.findWidget("icon"));
        HbIcon* icon1 = new HbIcon(":/devman_icon/resources/iconnotifier.svg");
        label1->setIcon(*icon1);
        //label1->setMode(QIcon::Normal);
        //label1->setState(QIcon::Off);
        label1->setToolTip("Mode=Normal, State=Off");

        QObject::connect(dialog, SIGNAL(finished(HbAction*)), 
                this, SLOT(noteTimedOut(HbAction*)));
        QObject::connect(primaryAction, SIGNAL(triggered()), this,
                SLOT(okSelected()));

        QObject::connect(secondaryAction, SIGNAL(triggered()), this,
                SLOT(cancelSelected()));

        if (dialog)
            dialog->show();
        }
    else if (inittype == ESANUserInformative)
        {
        HbDocumentLoader loader;
        bool ok = false;
        loader.load(":/xml/resources/dialoginformative.docml", &ok);
        if (!ok)
            {
            return;
            }

        //hbTrId("qtl_dialog_pri_heading");

        HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget(
                "dialog"));

        HbLabel *content = qobject_cast<HbLabel *> (loader.findWidget(
                "qtl_dialog_pri5"));

        QString serverinitmessage = hbTrId(
                "txt_deviceupdate_info_updating_device_from_server").arg(
                servername);

        //getserverspecifictexttodisplay(profileid, uimode, serverinitmessage);
        content->setPlainText(serverinitmessage);

        //get string here /*TODO*/ /*QTranslations*/

        dialog->setTimeout(3000);

        //msymnotifier->setstatus(0);

        if (dialog)
            dialog->show();

        QObject::connect(dialog, SIGNAL(aboutToClose()), this,
                SLOT(okSelected()));

        }
    else if (inittype == ESANSilent)
        {
        msymnotifier->setstatus(0);
        }

    }

void syncmlnotifier::createserveralertinformative(const QString &string)
    {
    qDebug("createserveralertinformative start");
		LOGSTRING("createserveralertinformative start");
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/resources/dialoginformative.docml", &ok);
    if (!ok)
        {
        return;
        }
    HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));

    HbLabel *content = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri5"));

    // No translations required
    content->setPlainText(string);

    dialog->setTimeout(3000);

    if (dialog)
        dialog->show();

    QObject::connect(dialog, SIGNAL(aboutToClose()), this, SLOT(infoNoteOkSelected()));

	qDebug("createserveralertinformative end");

    }

void syncmlnotifier::createserveralertconfirmative(const QString &string)
    {
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/resources/dialogserverpushconfirm.docml", &ok);
    if (!ok)
        {
        return;
        }
    HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));

    //set heading content
    HbLabel *contentheading = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri_heading"));
    QString heading = hbTrId("txt_device_update_title_server_message");
    contentheading->setPlainText(heading);

    //set body content
    HbLabel *contentbody = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri5"));

    contentbody->setPlainText(string);

    //set softkeys
    HbAction *primaryAction = (HbAction *) dialog->actions().first();
    QString softkeyok = hbTrId("txt_common_button_yes");
    primaryAction->setText(softkeyok);

    HbAction *secondaryAction = (HbAction *) dialog->actions().at(1);
    QString softkeyCancel = hbTrId("txt_common_button_no");
    secondaryAction->setText(softkeyCancel);

    dialog->setTimeout(HbPopup::NoTimeout);

    QObject::connect(primaryAction, SIGNAL(triggered()), this,
            SLOT(conirmNoteOkSelected()));

    QObject::connect(secondaryAction, SIGNAL(triggered()), this,
            SLOT(conirmNoteCancelSelected()));

    if (dialog)
        dialog->show();
    }

void syncmlnotifier::okSelected()
    {
    qDebug("ok selected");
    //msymnotifier->setstatus(0);

iPtr->dmDevdialogDismissed(devicemanagementnotifierwidget::EServerAlertNote,0);
    }
void syncmlnotifier::infoNoteOkSelected()
    {
    iPtr->dmDevdialogDismissed(devicemanagementnotifierwidget::EInformativeNote,0);
    }

void syncmlnotifier::conirmNoteOkSelected()
    {
    iPtr->dmDevdialogDismissed(devicemanagementnotifierwidget::EConfirmativeNote,0);
    }
void syncmlnotifier::conirmNoteCancelSelected()
    {
    iPtr->dmDevdialogDismissed(devicemanagementnotifierwidget::EConfirmativeNote,-3);
    }

void syncmlnotifier::cancelSelected()
    {
    qDebug("cancel selected");
    //msymnotifier->setstatus(-3);
    iPtr->dmDevdialogDismissed(devicemanagementnotifierwidget::EServerAlertNote,-3);
    
    }

void syncmlnotifier::noteTimedOut(HbAction* action)
    {
    qDebug("Pkg zero note timed out");
    if (action == NULL)
        {
        int timeout = -33; //KErrTimedOut
        msymnotifier->setstatus(timeout);
        if(mDefaultServerPkgZero) //For default server action is RSK
        iPtr->dmDevdialogDismissed(
                devicemanagementnotifierwidget::EServerAlertNote, -3);
        else //For Non default server action is LSK
        iPtr->dmDevdialogDismissed(
                devicemanagementnotifierwidget::EServerAlertNote, 0);	        	
        }
    }

syncmlnotifier::~syncmlnotifier()
    {
    qDebug("syncmlnotifier::~syncmlnotifier");
        qDebug("syncmlnotifier::~syncmlnotifier end");
    }

