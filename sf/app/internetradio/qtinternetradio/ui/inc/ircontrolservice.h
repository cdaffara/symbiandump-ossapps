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

#ifndef IRCONTROLSERVICE_H
#define IRCONTROLSERVICE_H

// System includes
#include <QObject>
#include <xqserviceprovider.h>

// User includes
#include "irservicedef.h"

// Forward declarations
class IRApplication;

class IrControlService : public XQServiceProvider
{
    Q_OBJECT

public:
    explicit IrControlService(IRApplication *aIrApp, QObject *aParent = 0);
    virtual ~IrControlService();

public slots:
    int handleCmd(int aCmdId);  // service interface, called via Qt Highway

private:
    IrServiceResult::Type handleLaunchNowPlayingViewCmd();
    IrServiceResult::Type handleLaunchNormallyCmd();
    IrServiceResult::Type handlePlayCmd();
    IrServiceResult::Type handleStopCmd();
    IrServiceResult::Type handleCancelCmd();

private:
    IRApplication    *mIrApp;
    
    Q_DISABLE_COPY(IrControlService)

};

#endif // IRCONTROLSERVICE_H
