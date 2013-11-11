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


#include <apgtask.h>
#include <hbaction.h>
#include <eikenv.h>
#include <hblabel.h>
#include <dmindicatorconsts.h>
#include "amwaitprogdialog.h"
#include "ApplicationManagementServer.h"

using namespace NApplicationManagement;

// ------------------------------------------------------------------------------------------------
// AMWaitProgDialog::AMWaitProgDialog()
// ------------------------------------------------------------------------------------------------
AMWaitProgDialog::AMWaitProgDialog(QString aAppName, MInstallerCallBack& aCallback)
:mDlg(0),m_callback(aCallback)
    {
    mIndi = 0;
    m_appname = aAppName;
    }
// ------------------------------------------------------------------------------------------------
// AMWaitProgDialog::~AMWaitProgDialog
// ------------------------------------------------------------------------------------------------
AMWaitProgDialog::~AMWaitProgDialog()
    {
    if(mDlg)
        {
        delete mDlg;
        mDlg=NULL;
        }
    if(mIndi)
        {
        delete mIndi;
        mIndi = NULL;
        }
    }
// ------------------------------------------------------------------------------------------------
// AMWaitProgDialog::createWaitDialog
// ------------------------------------------------------------------------------------------------
void AMWaitProgDialog::createWaitDialog(TInt aType)
    {
    mType = aType;
    if(mDlg==NULL)
        {
        mDlg = new HbProgressDialog(HbProgressDialog::WaitDialog);
        CApplicationManagementUtility::mCurrDlg=mDlg;
        }
    if (aType == EInstallWaitDlg)
        {
        //installing wait dialog
        mDlg->setHeadingWidget(new HbLabel(hbTrId(
                "txt_device_update_title_installing")));
        mDlg->setText(hbTrId("txt_deviceupdate_info_file_1_2").arg(m_appname));
        QString val = hbTrId("txt_common_button_hide");
        HbAction* hide = new HbAction(mDlg);
        hide->setText(val);
        mDlg->clearActions();
        mDlg->addAction(hide);
        val = hbTrId("txt_common_button_cancel");
        HbAction* cancel = new HbAction(mDlg);
        cancel->setText(val);
        mDlg->addAction(cancel);
        QObject::connect(hide, SIGNAL(triggered()), this,
                SLOT(hideAMWaitDialog()));
        QObject::connect(cancel, SIGNAL(triggered()), this,
                SLOT(cancelWaitDialog()));
        
        mDlg->show();
        }
    else
        {
        //uninstalling wait dialog
    mDlg->setHeadingWidget(new HbLabel(hbTrId(
                "txt_device_update_title_removing")));
    mDlg->setText(hbTrId("txt_deviceupdate_info_application_1").arg(m_appname));
        QString val = hbTrId("txt_common_button_hide");
        HbAction* hide = new HbAction();
        hide->setText(val);
        mDlg->clearActions();
        mDlg->addAction(hide);
        val = hbTrId("txt_common_button_cancel");
        QObject::connect(hide, SIGNAL(triggered()), this,
                SLOT(hideAMWaitDialog()));
        mDlg->setAttribute(Qt::WA_DeleteOnClose, true);
        mDlg->show();
        }
    }

// ------------------------------------------------------------------------------------------------
// AMWaitProgDialog::startIndicator()
// ------------------------------------------------------------------------------------------------
void AMWaitProgDialog::startIndicator()
        {
    CApplicationManagementUtility::mHidden = 1;
    mIndi = new HbIndicator();
    if (mType == EInstallWaitDlg)
        {
        QString str(hbTrId("txt_device_update_title_installing"));
        mIndi->activate(KScomoProgressIndicatorType, str);
        }
    else
        {
        QString str(hbTrId("txt_device_update_title_removing"));
        mIndi->activate(KScomoProgressIndicatorType, str);
        }
    }

// ------------------------------------------------------------------------------------------------
// AMWaitProgDialog::closeAMWaitDialog()
// ------------------------------------------------------------------------------------------------
void AMWaitProgDialog::closeAMWaitDialog()
    {
    CApplicationManagementUtility::mCurrDlg=0;
    if(mDlg)
        {
    mDlg->close();       
        }
    if(mIndi)
        {
        mIndi->deactivate(KScomoProgressIndicatorType);
        }
    }

// ------------------------------------------------------------------------------------------------
// AMWaitProgDialog::hideAMWaitDialog()
// ------------------------------------------------------------------------------------------------
void AMWaitProgDialog::hideAMWaitDialog()
    {
    CCoeEnv* coe = CCoeEnv::Static();
    TApaTaskList taskList(coe->WsSession());
        TApaTask task=taskList.FindApp(TUid::Uid(KAppMgmtServerUid));
        if(task.Exists())
            {
            task.SendToBackground();
            }
        startIndicator();
    }

// ------------------------------------------------------------------------------------------------
// AMWaitProgDialog::cancelDialog()
// ------------------------------------------------------------------------------------------------
void AMWaitProgDialog::cancelWaitDialog()
    {
    //mInstaller.CancelOperation();
    m_callback.InstallationCancelled();
    closeAMWaitDialog();
    CCoeEnv* coe = CCoeEnv::Static();
    TApaTaskList taskList(coe->WsSession());
    TApaTask task=taskList.FindApp(TUid::Uid(KAppMgmtServerUid));  
    if(task.Exists())
        {
        task.SendToBackground();
        }
    CApplicationManagementUtility::mCurrDlg=0;
    }

// ------------------------------------------------------------------------------------------------
// AMWaitProgDialog::registerInstallRequest
// ------------------------------------------------------------------------------------------------
void AMWaitProgDialog::registerInstallRequest(TRequestStatus &stat)
    {
    //mInstaller = aInstaller;
    m_Stat = &stat;
    }
