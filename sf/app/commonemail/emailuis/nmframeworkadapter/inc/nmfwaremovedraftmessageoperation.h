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

#ifndef NMFWAREMOVEDRAFTMESSAGEOPERATION_H_
#define NMFWAREMOVEDRAFTMESSAGEOPERATION_H_

#include <QPointer>
#include <nmoperation.h>
#include <nmcommon.h>
#include <CFSMailCommon.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class NmDataPluginInterface;
class CFSMailClient;

class NmFwaRemoveDraftMessageOperation : public NmOperation,
                                         public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaRemoveDraftMessageOperation(NmDataPluginInterface &pluginInterface,
                                 NmMessage *message,
                                 CFSMailClient &mailClient);
    
    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected:
    void doRunAsyncOperation();
    void doCompleteOperation();

private:
    ~NmFwaRemoveDraftMessageOperation();

    void removeMessageL();

private:
    NmDataPluginInterface &mPluginInterface;
    NmMessage *mMessage;    // Owned
    CFSMailClient &mMailClient;
    TInt mRequestId;
};

#endif /* NMFWAREMOVEDRAFTMESSAGEOPERATION_H_ */
