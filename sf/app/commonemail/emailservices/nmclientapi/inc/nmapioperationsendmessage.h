/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *      Send message operation header.
 */

#ifndef NMAPIOPERATIONSENDMESSAGE_H_
#define NMAPIOPERATIONSENDMESSAGE_H_

#include <QObject>
#include <nmapioperation.h>
#include <nmapimessage.h>
#include <MFSMailRequestObserver.h>

class CFSMailBox;
namespace EmailClientApi
{

class NmApiMessage;

class NmApiOperationSendMessage : public NmApiOperation, public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmApiOperationSendMessage(const NmApiMessage &message, CFSMailBox &mailBox, QObject *parent = 0);
    virtual ~NmApiOperationSendMessage();
    
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

private slots:
    void start();

public slots:
    void cancel();
    
private:
    NmApiMessage mMessage;
    CFSMailBox &mMailBox;
    TInt mRequestId;
};
}

#endif /* NMAPIOPERATIONSENDMESSAGE_H_ */
