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
 * Description: Implementation of downloading progress note
 *
 */

#include <hblabel.h>
#include <hbaction.h>
#include <e32property.h>
#include <qapplication.h>
#include <apgtask.h>
#include <e32base.h>
#include <eikenv.h>
#include <dmindicatorconsts.h>
#include "appmgmtdownloadmgr.h"
#include "ApplicationManagementUtility.h"

using namespace NApplicationManagement;

// ------------------------------------------------------------------------------------------------
// AppMgmtProgDialog::AppMgmtProgDialog
// ------------------------------------------------------------------------------------------------ 
AppMgmtProgDialog::AppMgmtProgDialog(QString aAppData, Download &mdl,int &aUserCancelled)
:m_Dlg(0)
    {   
    m_Data=aAppData;
    iDl=&mdl;
    mUsrCancel = aUserCancelled;
    m_Indi = 0;
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtProgDialog::~AppMgmtProgDialog()
// ------------------------------------------------------------------------------------------------ 
AppMgmtProgDialog::~AppMgmtProgDialog()
    {
    if(m_Dlg)
        {
        delete m_Dlg;
        m_Dlg=NULL;
        }
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtProgDialog::sendServerToBackground()
// ------------------------------------------------------------------------------------------------ 
void AppMgmtProgDialog::sendServerToBackground()
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
// AppMgmtProgDialog::startDialog
// ------------------------------------------------------------------------------------------------ 
void AppMgmtProgDialog::startDialog(int aContentSize,int aDownloaded)
    {
    if (m_Dlg == NULL)
        {
        m_Dlg = new HbProgressDialog(HbProgressDialog::ProgressDialog);
        CApplicationManagementUtility::mCurrDlg = m_Dlg;
        QStringList strList = m_Data.split(",");
        m_Name = strList[0];
        m_SizeStr = strList[1];
        }
    m_Dlg->setMinimum(0);
    m_Dlg->setMaximum(aContentSize);

    m_Dlg->setAutoClose(true);
    m_Dlg->setProgressValue(aDownloaded);

    QString val = hbTrId("txt_device_update_title_downloading");
    m_Dlg->setHeadingWidget(new HbLabel(val));

    val = hbTrId("txt_deviceupdate_info_file_1_2").arg(m_Name);
    val.append("\n");
    val.append(m_SizeStr);

    m_Dlg->setText(val);
    val = hbTrId("txt_common_button_hide");
    HbAction* hide = new HbAction();
    hide->setText(val);
    m_Dlg->clearActions();
    m_Dlg->addAction(hide);
    val = hbTrId("txt_common_button_cancel");
    HbAction* cancel = new HbAction();
    cancel->setText(val);
    m_Dlg->addAction(cancel);
    QObject::connect(hide, SIGNAL(triggered()), this,
            SLOT(hideAMProgDialog()));
    QObject::connect(cancel, SIGNAL(triggered()), this, SLOT(cancelDialog()));
    m_Dlg->show();
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtProgDialog::closeAMProgDialog()
// ------------------------------------------------------------------------------------------------ 
void AppMgmtProgDialog::closeAMProgDialog()
    {    
    CApplicationManagementUtility::mCurrDlg=0;
    if(m_Dlg)
        {
        m_Dlg->close();
        }
    if(CApplicationManagementUtility::mHidden==0)
        {
        sendServerToBackground();
        }
    else
        {
        m_Indi->deactivate(KScomoProgressIndicatorType);   
        }
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtProgDialog::cancelDialog()
// ------------------------------------------------------------------------------------------------ 
void AppMgmtProgDialog::cancelDialog()
    {
    mUsrCancel = 1;
    iDl->cancel();
    if(m_Dlg)
        {
        m_Dlg->close();
        }
    sendServerToBackground();
    CApplicationManagementUtility::mCurrDlg=0;
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtProgDialog::hideAMProgDialog()
// ------------------------------------------------------------------------------------------------ 
void AppMgmtProgDialog::hideAMProgDialog()
    {
    sendServerToBackground();
    
    QString str = hbTrId("txt_device_update_dblist_product_code_val_download").arg(m_Name);
    CApplicationManagementUtility::mHidden=1;
    m_Indi = new HbIndicator();
    m_Indi->activate(KScomoProgressIndicatorType,str);
    }

// ------------------------------------------------------------------------------------------------
// AppMgmtProgDialog::updateProgress
// ------------------------------------------------------------------------------------------------ 
void AppMgmtProgDialog::updateProgress(int aProgress)
    {
    if(m_Dlg)
        {
        m_Dlg->setProgressValue(aProgress);       
        }
    }

