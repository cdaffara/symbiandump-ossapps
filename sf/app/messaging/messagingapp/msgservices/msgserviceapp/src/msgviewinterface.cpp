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
 * Description: Message view service interface used for interfacing between
 *              QT highway and other applications.
 *
 */

#include "msgservicewindow.h"
#include "msgviewinterface.h"
#include "msgserviceviewmanager.h"

//----------------------------------------------------------------------------
// MsgViewInterface::MsgViewInterface
// @see header
//----------------------------------------------------------------------------
MsgViewInterface::MsgViewInterface(MsgServiceViewManager* viewManager)
:XQServiceProvider( QLatin1String("messaging.com.nokia.symbian.IMessageView"), 
        viewManager),
 mViewManager(viewManager)
    {
    publishAll();    
    }

//----------------------------------------------------------------------------
// MsgViewInterface::~MsgViewInterface
// @see header
//----------------------------------------------------------------------------
MsgViewInterface::~MsgViewInterface()
    {
    }

//----------------------------------------------------------------------------
// MsgViewInterface::view
// @see header
//----------------------------------------------------------------------------
void MsgViewInterface::view(int msgId)
    {
    mViewManager->view(msgId);
    }
