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
 *      Draft message creation operation header.
 */

#ifndef NMAPIOPERATIONCREATEDRAFTMESSAGE_H_
#define NMAPIOPERATIONCREATEDRAFTMESSAGE_H_

#include <QObject>
#include <QVariant>
#include <MFSMailRequestObserver.h>
#include <nmapioperation.h>

class CFSMailBox;

using namespace EmailClientApi;

class NmApiOperationCreateDraftMessage : public NmApiOperation, 
                                         public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmApiOperationCreateDraftMessage(const QVariant *initData,
                                     CFSMailBox &mailBox,
                                     QObject *parent = 0);
    virtual ~NmApiOperationCreateDraftMessage();

    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

private slots:
    void start();

public slots:
    void cancel();
    
private:
    const QVariant mInitData;
    CFSMailBox &mMailBox;
    TInt mRequestId;

};


#endif /* NMAPIOPERATIONCREATEDRAFTMESSAGE_H_ */
