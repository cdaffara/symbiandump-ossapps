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


#include <hbdocumentloader.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <e32property.h>
#include <qdebug.h>
#include <hblistview.h>
#include <hbpushbutton.h>
#include <e32math.h>
#include <QStandardItemModel>
#include <devicedialogsymbian.h>
#include "devicemanagementnotifierutils.h"
#include "fotadevicedialogs.h"
#include "pnputillogger.h"


// ---------------------------------------------------------------------------
// fotadevicedialogs::fotadevicedialogs
// Constructor for fota related device dialogs 
// ---------------------------------------------------------------------------
//

fotadevicedialogs::fotadevicedialogs(const QVariantMap &parameters)
//:	devicemanagementnotifierwidget(parameters)
    {
    qDebug("devicemanagementnotifierutils fotadevicedialogs");
    QTranslator *translator = new QTranslator();
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    bool fine = translator->load("deviceupdates_" + lang, path);
    if (fine)
        qApp->installTranslator(translator); 
    
    QTranslator *commontranslator = new QTranslator();
    fine = commontranslator->load("common_" + lang, path);/*finally required once localisation available*/
    if(fine)
        qApp->installTranslator(commontranslator);
    else
        qDebug("device dialog common translator loading failed");

    
    launchFotaDialog(parameters);
    }



// ---------------------------------------------------------------------------
// fotadevicedialogs::launchFotaDialog
// This function lauches is called to lauch the specific dialog based on the
// dialogID passed to it.
// ---------------------------------------------------------------------------
//
void fotadevicedialogs::launchFotaDialog(const QVariantMap &parameters)
    {

    QVariantMap::const_iterator i = parameters.constBegin();
    qDebug("devicemanagementnotifierutils fotadevicedialogs launchDialog");    
    TFwUpdNoteTypes aType = EFwUpdDeviceBusy;
    
    //QString temp = QString::fromUtf8(reinterpret_cast<const char*>(url.Ptr()), url.Length());
    //TDesC8 * tempStr = KKeyDialog;
    //const QString temp = QString::fromUtf8(reinterpret_cast<const char*>(tempStr.Ptr()), tempStr.Length());
    i = parameters.find(keydialog);
    if(i != parameters.end())
        aType = (TFwUpdNoteTypes)i.value().toInt();
    
    
    if(aType == EFwUpdRebootNote)
    	{
    		createfotainformativedialog(aType,parameters);
    	}
    	else if(aType == EFwUpdResumeDownload || aType == EFwUpdResumeUpdate)
        {
        createfotaconfirmationdialog(aType,parameters);
        //createfotainformativedialog();
        }
    else if(aType == EFwDLNeedMoreMemory || aType == EFwDLConnectionFailure || aType == EFwDLGeneralFailure || 
    	 aType == EFwUpdNotEnoughBattery || aType == EFwUpdDeviceBusy || aType == EFwUpdSuccess || aType == EFwUpdNotCompatible
    || aType == EFwDLNonResumableFailure)
        {
        //createfotamessagedialog();
        createfotamessagedialog(aType,parameters);
        }
    }


// ---------------------------------------------------------------------------
// fotadevicedialogs::createfotainformativedialog
// This function is called to show the informative dialog of FOTA
// ---------------------------------------------------------------------------
//
void fotadevicedialogs::createfotainformativedialog(TFwUpdNoteTypes aType,const QVariantMap &parameters)
    {
    qDebug("createserveralertinformative start");
		LOGSTRING("createserveralertinformative start");
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/resources/fotainformativedialog.docml", &ok);
    if (!ok)
        {
        return;
        }
    HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));

    HbLabel *content = qobject_cast<HbLabel *> (loader.findWidget(
            "lblContent"));

    // No translations required
    //content->setPlainText("Testing");

    dialog->setTimeout(3000);

    if (dialog)
        dialog->show();

    QObject::connect(dialog, SIGNAL(aboutToClose()), this, SLOT(okSelected()));

	qDebug("createserveralertinformative end");

    }



// ---------------------------------------------------------------------------
// fotadevicedialogs::createfotaconfirmationdialog
// This function is called to show the confirmation dialog of FOTA.
// ---------------------------------------------------------------------------
//
void fotadevicedialogs::createfotaconfirmationdialog(TFwUpdNoteTypes aType,const QVariantMap &parameters)
    {
    qDebug("createserveralertinformative start");
        LOGSTRING("createserveralertinformative start");
        
    QVariantMap::const_iterator i;
    HbDocumentLoader loader;
    bool ok = false;
    HbDialog *dialog = NULL;
    
    QString param1;
    QString param2;
    TInt size = 0;
    TBool postpone = 0;

    i = parameters.find(keyparam1);
    if(i != parameters.end())
        size = i.value().toInt();

    i = parameters.find(keyparam2);
    if(i != parameters.end())
        param1 = i.value().toString();
    i = parameters.find(keyparam3);
    if(i != parameters.end())
        param2 = i.value().toString();
    i = parameters.find(keyparam4);
    if(i != parameters.end())
        postpone = i.value().toBool();

    
    switch(aType)
        {
        case EFwUpdResumeDownload:
            {          
            loader.load(":/xml/resources/fotasoftkeysdialog.docml", &ok);
            if (!ok)
            {
            return;
            }
            dialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));
            
            HbLabel *headingString = qobject_cast<HbLabel *> (loader.findWidget(
                                "lblDialogHeading"));
            
            headingString->setPlainText(hbTrId("txt_device_update_title_device_update"));
            
            HbLabel *contentString = qobject_cast<HbLabel *> (loader.findWidget(
                                "lblDialogContent"));
            
            TReal sizeRounded = 0;
            TReal sizeKB = size / 1024;
            if(sizeKB > 1024)
                {
                TReal sizeMB = sizeKB / 1024;
                Math::Round(sizeRounded,sizeMB,2);
                QString str = hbTrId("txt_device_update_info_the_last_updte_incomp_dwnld_Mb")
                        //.arg(param1).arg(param2).arg(sizeMB);
                        .arg(param2).arg(param1).arg(sizeRounded);
                contentString->setPlainText(str);//HACK
                }
            else
                {
                Math::Round(sizeRounded,sizeKB,2);
                contentString->setPlainText(hbTrId("txt_device_update_info_the_last_update_incomplete_dwnld_kb")
                        .arg(param1).arg(param2).arg(sizeRounded));
                }
            //HbAction *primaryAction = dialog->primaryAction();
            HbAction *primaryAction = (HbAction *) dialog->actions().first();
            primaryAction->setText(hbTrId("txt_common_button_continue_dialog"));

            //HbAction *secondaryAction = dialog->secondaryAction();
            HbAction *secondaryAction = (HbAction *) dialog->actions().at(1);
            secondaryAction->setText(hbTrId("txt_device_update_button_resume_later"));
            if(!postpone)
                secondaryAction->setEnabled(postpone);

            dialog->setTimeout(HbPopup::NoTimeout);
            dialog->setDismissPolicy(HbPopup::NoDismiss);

            QObject::connect(primaryAction, SIGNAL(triggered()), this,
                    SLOT(fotaLSK()));

            QObject::connect(secondaryAction, SIGNAL(triggered()), this,
                    SLOT(fotaRSK()));
            break;
            }
        case EFwUpdResumeUpdate:
            {
            bool ok = false;
                loader.load(":/xml/resources/fotaresumeupdate.docml", &ok);
                if (!ok)
                    {
                    return;
                    }
                
                dialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));
                
                dialog->setTimeout(HbPopup::NoTimeout);
                dialog->setDismissPolicy(HbPopup::NoDismiss);
                
                HbLabel *headingString = qobject_cast<HbLabel *> (loader.findWidget(
                        "lblHeading"));
                headingString->setPlainText(hbTrId("txt_device_update_title_device_update"));
                            
                
                //HbAction *primaryAction = dialog->primaryAction();
                HbAction *primaryAction = (HbAction *) dialog->actions().first();
                primaryAction->setText(hbTrId("txt_common_button_continue_dialog"));
                
                //HbAction *secondaryAction = dialog->secondaryAction();
                HbAction *secondaryAction = (HbAction *) dialog->actions().at(1);
                secondaryAction->setText(hbTrId("txt_device_update_button_resume_later"));
                if(!postpone)
                    secondaryAction->setEnabled(postpone);
                
                HbLabel *contentSwDetails = qobject_cast<HbLabel *> (loader.findWidget(
                                                "label"));
                TReal sizeRounded = 0;
                TReal sizeKB = size / 1024;
                
                QString content;
                if(sizeKB > 1024)
                    {
                    TReal sizeMB = sizeKB / 1024;
                    Math::Round(sizeRounded,sizeMB,2);
                    contentSwDetails->setPlainText(hbTrId("txt_device_update_info_the_last_update_incomp_instal_Mb")
                            .arg(param1).arg(param2).arg(sizeRounded));
                    }
                else
                    {
                    Math::Round(sizeRounded,sizeKB,2);
                    contentSwDetails->setPlainText(hbTrId("txt_device_update_info_the_last_update_incomp_instal_kb")
                            .arg(param1).arg(param2).arg(sizeRounded));
                    }
                
                HbLabel *contentCharger = qobject_cast<HbLabel *> (loader.findWidget(
                                                                "label_3"));
                contentCharger->setPlainText(hbTrId("txt_device_update_info_it_is_recommended_to_connec"));
                
                HbLabel *contentInstall = qobject_cast<HbLabel *> (loader.findWidget(
                                                                "label_2"));
                contentInstall->setPlainText(hbTrId("txt_device_update_info_after_the_installation_the"));
                
                HbLabel *contentEmergency = qobject_cast<HbLabel *> (loader.findWidget(
                                                                "label_1"));
                contentEmergency->setPlainText(hbTrId("txt_device_update_info_during_the_installation_the"));

                dialog->setTimeout(HbPopup::NoTimeout);

                QObject::connect(primaryAction, SIGNAL(triggered()), this,
                        SLOT(fotaLSK()));

                QObject::connect(secondaryAction, SIGNAL(triggered()), this,
                        SLOT(fotaRSK()));
            }
            break;
        default:
            {
            break;
            }
        }

    if (dialog)
        dialog->show();

    qDebug("createserveralertinformative end");

    }


// ---------------------------------------------------------------------------
// fotadevicedialogs::createfotamessagedialog
// This function is called to show the message dialog of FOTA
// ---------------------------------------------------------------------------
//
void fotadevicedialogs::createfotamessagedialog(TFwUpdNoteTypes aType,const QVariantMap &parameters)
    {
    QVariantMap::const_iterator i;
    qDebug("createserveralertinformative start");
        LOGSTRING("createserveralertinformative start");
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/resources/fotasoftkeydialog.docml", &ok);
    if (!ok)
        {
        return;
        }
    HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));
    
    HbLabel *headingString = qobject_cast<HbLabel *> (loader.findWidget(
            "lblHeaind"));
    headingString->setPlainText(hbTrId("txt_device_update_title_device_update"));
    
    HbLabel *contentString = qobject_cast<HbLabel *> (loader.findWidget(
            "lblContent"));
            
    dialog->setTimeout(HbPopup::NoTimeout);
    dialog->setDismissPolicy(HbPopup::NoDismiss);
    
    //HbAction *primaryAction = dialog->primaryAction();
    HbAction *primaryAction = (HbAction *) dialog->actions().first();
    primaryAction->setText(hbTrId("txt_common_button_ok_single_dialog"));

    QObject::connect(primaryAction, SIGNAL(triggered()), this,
                SLOT(fotaLSK()));
                
                
    switch(aType)
        {
        	case EFwDLNeedMoreMemory:
            {
            QString content;
            TInt aMemNeeded = 0;
            i = parameters.find(keyparam1);
            if(i != parameters.end())
                aMemNeeded = i.value().toInt();
            TInt aMemNeededKB = aMemNeeded / 1024;
            if(aMemNeededKB <= 1024)
                {
                content = hbTrId("txt_de_info_l1_kb_free_memory_needed_for_update").arg(aMemNeededKB);
                }
            else
                {
                TInt aMemNeededMB = aMemNeededKB / 1024;
                content = hbTrId("txt_de_info_l1_Mb_free_memory_needed_for_update").arg(aMemNeededMB);
                }
            contentString->setPlainText(content);
            break;
            }
        case EFwDLConnectionFailure:
            {
            contentString->setPlainText(hbTrId("txt_device_update_info_connection_lost_while_do"));
            break;
            }
        case EFwDLGeneralFailure:
            {
            contentString->setPlainText(hbTrId("txt_device_update_info_an_error_occurred_during_do"));
            break;
            }
        case EFwDLNonResumableFailure:
            {
            contentString->setPlainText(hbTrId("txt_device_update_info_an_error_occurred_update_stop"));
            break;
            }
        case EFwUpdDeviceBusy:
            {
            contentString->setPlainText(hbTrId("txt_device_update_info_FS_warning_device_memory_is_b"));
            break;
            }
       case EFwUpdSuccess:
           {
           headingString->setPlainText(hbTrId("txt_device_update_title_phone_uptodate"));
           contentString->setPlainText(hbTrId("txt_device_update_info_your_phone_is_now_updated_w"));
           break;
           }
       case EFwUpdNotCompatible:
           {           
           contentString->setPlainText(hbTrId("txt_device_update_info_the_phone_was_not_updated"));
           break;
           }
       case EFwUpdNotEnoughBattery:
           {
           contentString->setPlainText(hbTrId("txt_device_update_info_you_will_be_notified_once_t"));
           break;
           }
       default:
           {
           break;
           }
        }

    if (dialog)
        dialog->show();

    qDebug("createserveralertinformative end");

    }


// ---------------------------------------------------------------------------
// fotadevicedialogs::fotaLSK
// This slot is called when LSK is pressed in device dialog.
// ---------------------------------------------------------------------------
//
void fotadevicedialogs::fotaLSK()
    {
    qDebug("ok selected");
    QVariantMap resultMap;
    resultMap.insert(returnkey, EHbLSK);
    emit deviceDialogData(resultMap);
    }



// ---------------------------------------------------------------------------
// CFotaDownloadNotifHandler::aboutToClose
// This slot is called when RSK is pressed in device dialog.
// ---------------------------------------------------------------------------
//
void fotadevicedialogs::fotaRSK()
    {
    qDebug("cancel selected");
    QVariantMap resultMap;
    resultMap.insert(returnkey, EHbRSK);
    emit deviceDialogData(resultMap);
    }



// Set parameters
bool fotadevicedialogs::setDeviceDialogParameters(
    const QVariantMap &parameters)
{
    
    return true;
}

// Get error
int fotadevicedialogs::deviceDialogError() const
{
    
    return 0;
}


// Close device dialog
// ---------------------------------------------------------------------------
// fotadevicedialogs::closeDeviceDialog
// This slot is called when device dialog is closed due to errors.
// ---------------------------------------------------------------------------
//

void fotadevicedialogs::closeDeviceDialog(bool byClient)
{
		qDebug("cancel selected");
    emit deviceDialogClosed();
}


// Return display widget
HbDialog *fotadevicedialogs::deviceDialogWidget() const
{
   
    return const_cast<fotadevicedialogs*>(this);
    //return const_cast<devicemanagementnotifierwidget*>(this);
}
