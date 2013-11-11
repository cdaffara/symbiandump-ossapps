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

#ifndef NMFWAMESSAGESENDINGOPERATION_H_
#define NMFWAMESSAGESENDINGOPERATION_H_

#include <QPointer>
#include <nmmessagesendingoperation.h>
#include <nmcommon.h>
#include <CFSMailCommon.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class NmDataPluginInterface;
class CFSMailClient;

class NmFwaMessageSendingOperation : public NmMessageSendingOperation,
                                     public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaMessageSendingOperation(NmDataPluginInterface &pluginInterface,
                                 NmMessage *message,
                                 CFSMailClient &mailClient);
    
    const NmMessage *getMessage() const;

    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected slots:
    void handleCompletedSaveOperation(int error);
    
protected:
    void doRunAsyncOperation();
    void doCompleteOperation();
    void doCancelOperation();

private:
    ~NmFwaMessageSendingOperation();
    int saveMessageWithSubparts();
    int sendMessageL();

private:
    NmDataPluginInterface &mPluginInterface;
    QPointer<NmOperation> mSaveOperation;    // Not owned
    NmMessage *mMessage;                     // Owned
    CFSMailClient &mMailClient;
    TInt mRequestId;
    bool mSaved;
};

#endif /* NMFWAMESSAGESENDINGOPERATION_H_ */
