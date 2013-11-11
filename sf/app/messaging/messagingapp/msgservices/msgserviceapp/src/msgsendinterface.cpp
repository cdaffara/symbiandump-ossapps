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
 * Description: Message send service interface used for interfacing between
 *              QT highway and other applications.
 *
 */

#include "msgsendinterface.h"
#include "msgserviceviewmanager.h"

//----------------------------------------------------------------------------
// MsgSendInterface::MsgSendInterface
// @see header
//----------------------------------------------------------------------------
MsgSendInterface::MsgSendInterface(MsgServiceViewManager* viewManager)
:XQServiceProvider(QLatin1String("messaging.com.nokia.symbian.IMessageSend"),
        viewManager),
mViewManager(viewManager)
    {
    publishAll();    
    }

//----------------------------------------------------------------------------
// MsgSendInterface::~MsgSendInterface
// @see header
//----------------------------------------------------------------------------
MsgSendInterface::~MsgSendInterface()
    {
    }

//----------------------------------------------------------------------------
// MsgSendInterface::send
// @see header
//----------------------------------------------------------------------------
void MsgSendInterface::send(const QString phoneNumber, 
                            const qint32 contactId, 
                            const QString displayName)
    {   
    mViewManager->send(phoneNumber,contactId,displayName);    
    }

//----------------------------------------------------------------------------
// MsgSendInterface::send
// @see header
//----------------------------------------------------------------------------
void MsgSendInterface::send(const QString phoneNumber, 
                            const QString alias, 
                            const QString bodyText)
    {   
    mViewManager->send(phoneNumber, alias, bodyText);    
    }

//----------------------------------------------------------------------------
// MsgSendInterface::send
// @see header
//----------------------------------------------------------------------------
void MsgSendInterface::send(const QVariantMap addressList, 
              const QString bodyText)
      {
      mViewManager->send(addressList,bodyText);    
      }
//EOF

