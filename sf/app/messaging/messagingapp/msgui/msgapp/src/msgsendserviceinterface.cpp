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
 * Description: Message Application Launcher interface used for interfacing between
 *              QT highway and other applications
 *
 */

#include "msgsendserviceinterface.h"
#include "msgviewmanager.h"
//INCLUDES

MsgSendServiceInterface::MsgSendServiceInterface(QObject* parent, MsgViewManager* manager)
:XQServiceProvider( QLatin1String("com.nokia.services.hbserviceprovider.imessage.send"),parent),
mViewManager(manager)
    {
    publishAll();    
    }

MsgSendServiceInterface::~MsgSendServiceInterface()
    {
    }

void MsgSendServiceInterface::send(QVariant data)
    {   
    mViewManager->setServiceRequest(true);
    mViewManager->send(data);    
    }
