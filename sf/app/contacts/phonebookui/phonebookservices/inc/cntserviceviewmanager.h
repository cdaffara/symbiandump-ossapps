/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CNTSERVICEVIEWMANAGER_H
#define CNTSERVICEVIEWMANAGER_H

#include "cntdefaultviewmanager.h"
#include <cntservicescontact.h>

#include <QObject>
#include <hbmainwindow.h>

class CntBaseView;
class CntServiceHandler;
class CntAbstractServiceProvider;

class CntServiceViewManager : public CntDefaultViewManager
{
    Q_OBJECT

public:
    CntServiceViewManager(HbMainWindow *mainWindow, CntAbstractServiceProvider& aProvider );
    ~CntServiceViewManager();
protected slots:
    virtual void closeApp();
signals:
    void applicationClosed();
};

#endif // CNTSERVICEVIEWMANAGER_H
