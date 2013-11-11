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
* Description: NMail application service interface used for displaying the
*              messages contained by the given mailbox. The service utilizes
*              thr Qt highway framework.
*
*/

#ifndef NMMAILBOXSERVICEINTERFACE_H_
#define NMMAILBOXSERVICEINTERFACE_H_

//  INCLUDES
#include <QObject>

#ifdef Q_OS_SYMBIAN
#include <xqserviceprovider.h>
#else
#include <QVariant>
#define NM_WINS_ENV
#endif


//  FORWARD DECLARATIONS
class HbView;
class NmApplication;
class NmId;
class NmUiEngine;

#ifndef NM_WINS_ENV
class NmMailboxServiceInterface : public XQServiceProvider
#else
class NmMailboxServiceInterface : public QObject
#endif
{
    Q_OBJECT

public:

    NmMailboxServiceInterface(QObject *parent,
                              NmUiEngine &uiEngine,
                              NmApplication *application);

    ~NmMailboxServiceInterface();


public slots:

    void displayInboxByMailboxId(QVariant data);


private:

    bool mailboxExistsById(const NmId &mailboxId) const;


private:

    NmUiEngine &mUiEngine; // Not owned
    NmApplication *mApplication; // Not owned
    int mAsyncReqId;
};

#endif /* NMMAILBOXSERVICEINTERFACE_H_ */
