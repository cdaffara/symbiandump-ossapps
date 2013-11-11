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

#include <QObject>
#include <QTimeLine>
#include <hbprogressdialog.h>
#include <hbindicator.h>
#include <download.h>

using namespace WRT;
class AppMgmtProgDialog : public QObject
    {
    Q_OBJECT

public:
    AppMgmtProgDialog(QString aAppData, Download &mdl,int &aUserCancelled);
    ~AppMgmtProgDialog();
    void startDialog(int aContentSize,int aDownloaded);
    void closeAMProgDialog();
    void updateProgress(int aProgress);
public slots:
    void hideAMProgDialog();
    void cancelDialog();
private:
    void sendServerToBackground();
private:
    int iContentSize;
    int mUsrCancel;
    Download* iDl;
    QString m_Data;
    HbProgressDialog* m_Dlg;
    QString m_Name;
    QString m_SizeStr;
    HbIndicator* m_Indi;
    };
