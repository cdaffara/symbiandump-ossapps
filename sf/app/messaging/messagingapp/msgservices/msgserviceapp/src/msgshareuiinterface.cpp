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
 * Description: Message share UI service interface used for interfacing between
 *              QT highway and other applications.
 *
 */

#include "msgshareuiinterface.h"
#include "msgserviceviewmanager.h"

//----------------------------------------------------------------------------
// MsgViewService::MsgViewService
// @see header
//----------------------------------------------------------------------------
MsgShareUiInterface::MsgShareUiInterface(MsgServiceViewManager* viewManager)
:XQServiceProvider( QLatin1String("messaging.com.nokia.symbian.IFileShare"),
        viewManager),
mViewManager(viewManager)
    {
    publishAll();    
    }

//----------------------------------------------------------------------------
// MsgShareUiInterface::~MsgShareUiInterface
// @see header
//----------------------------------------------------------------------------
MsgShareUiInterface::~MsgShareUiInterface()
    {
    }

//----------------------------------------------------------------------------
// MsgShareUiInterface::send
// @see header
//----------------------------------------------------------------------------
void MsgShareUiInterface::send(QVariant data)
    {   
    mViewManager->send(data);    
    }
