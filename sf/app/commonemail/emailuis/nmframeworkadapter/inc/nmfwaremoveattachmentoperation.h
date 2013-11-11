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

#ifndef NMFWAREMOVEATTACHMENTOPERATION_H_
#define NMFWAREMOVEATTACHMENTOPERATION_H_

#include <QObject>
#include <nmoperation.h>
#include <nmcommon.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class CFSMailClient;

class NmFwaRemoveAttachmentOperation : public NmOperation,
                                       public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaRemoveAttachmentOperation(const NmMessage &message,
                                   const NmId &attachmentPartId,
                                   CFSMailClient &mailClient);

public: // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);
    
protected:
    void doRunAsyncOperation();
    void doCancelOperation();
    
private:
	void doRunAsyncOperationL();
    ~NmFwaRemoveAttachmentOperation();
    
private:
    const NmMessage &mMessage;
    NmId mAttachmentPartId;
    CFSMailClient &mMailClient;
    TInt mRequestId;
};

#endif /* NMFWAREMOVEATTACHMENTOPERATION_H_ */
