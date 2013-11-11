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
 *     Email message related operations
 */

#ifndef NMAPIMESSAGEMANAGER_P_H_
#define NMAPIMESSAGEMANAGER_P_H_

#include <QObject>
#include <QPointer>

#include <CFSMailCommon.h>
#include <nmapimessagemanager.h>

class NmApiEngine;
class CFSMailBox;
class CFSMailPlugin;

namespace EmailClientApi
{

class NmApiMessageManagerPrivate : public QObject
{
 Q_OBJECT  
public:
    NmApiMessageManagerPrivate(QObject *parent,quint64 mailboxId);
    virtual ~NmApiMessageManagerPrivate();
    
public slots: 
    QPointer<NmApiOperation> createDraftMessage(const QVariant *initData);
    QPointer<NmApiOperation> sendMessage(const NmApiMessage &message);
    QPointer<NmApiOperation> saveMessage(const NmApiMessage &message);

private:
    void initialise();
    
private:    
    TFSMailMsgId mMailBoxId;
    CFSMailBox *mMailBox;
    CFSMailPlugin *mMailPlugin; //not owned
    NmApiEngine *mEngine;
};

}

#endif /*NMAPIMESSAGEMANAGER_P_H_ */
