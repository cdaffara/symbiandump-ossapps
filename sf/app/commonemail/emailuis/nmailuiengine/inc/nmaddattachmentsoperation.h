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

#ifndef NMADDATTACHMENTSOPERATION_H_
#define NMADDATTACHMENTSOPERATION_H_

#include <QObject>
#include "nmcommon.h"
#include "nmoperation.h"

class NMUIENGINE_EXPORT NmAddAttachmentsOperation: public NmOperation
{
    Q_OBJECT
public:
    NmAddAttachmentsOperation();
    
signals:
    // The observer of the asynchronous operation can connect to this signal.
    void operationPartCompleted(const QString &fileName, const NmId &msgPartId, int result);
    
public slots:
    void completeOperationPart(const QString &fileName, const NmId &msgPartId, int result);

protected:
    virtual ~NmAddAttachmentsOperation();
};

#endif /* NMADDATTACHMENTSOPERATION_H_ */
