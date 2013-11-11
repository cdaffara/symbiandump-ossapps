/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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

#ifndef CPLAUNCHERSERVICE_H
#define CPLAUNCHERSERVICE_H

#include <xqserviceprovider.h>

class HbMainWindow;

class CpLauncherService : public XQServiceProvider
{
    Q_OBJECT
public:
    explicit CpLauncherService(HbMainWindow *mainWindow = 0);
    virtual ~CpLauncherService();
    bool complete();
public slots:
    bool launchSettingView(const QString &pluginFile,const QVariant &hint);
private slots:
    void setReturnValue(const QVariant &returnValue);
    void handleClientDisconnected();
private:
    HbMainWindow *mMainWindow;
    int mAsyncRequestIndex;
    QVariant mReturnValue;
};

#endif //CPLAUNCHERSERVICE_H

//End of File
