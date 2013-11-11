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

#ifndef AMWAITPROGDIALOG_H_
#define AMWAITPROGDIALOG_H_

#include <QObject>
#include <hbprogressdialog.h>
#include <hbindicator.h>
#include <usif/sif/sif.h>
#include "ApplicationManagementUtility.h"

class MInstallerCallBack
        {
    public:
        virtual void InstallationCancelled()=0;
        };
class AMWaitProgDialog : public QObject
    {
    Q_OBJECT

public:
    AMWaitProgDialog(QString aAppName,MInstallerCallBack& aCallback );
    ~AMWaitProgDialog();
    
    void createWaitDialog(TInt aType);
    void closeAMWaitDialog();
    void startIndicator();
    void registerInstallRequest(TRequestStatus &aStat);
public slots:
    void hideAMWaitDialog();
    void cancelWaitDialog();

private:
    HbProgressDialog* mDlg;
    HbIndicator* mIndi;
    TRequestStatus* m_Stat;
    QString m_appname;
    TInt mType;
    int mIndicatorDeactive;
    MInstallerCallBack& m_callback;
    };

#endif 
