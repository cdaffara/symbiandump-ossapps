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
#ifndef NMAPIOPERATION_H_
#define NMAPIOPERATION_H_

#include <QObject>
#include <QVariant>
#include "nmapicommon.h"

#include <nmapidef.h>

namespace EmailClientApi 
{

class NMAPI_EXPORT NmApiOperation : public QObject
{ 
    Q_OBJECT 
    
protected:
    NmApiOperation(QObject *parent, NmApiOperationStatus status, NmApiOperationType operationType);
    
public:
    virtual ~NmApiOperation();

    NmApiOperationStatus status() const;
    NmApiOperationType operation() const;

private slots:
    virtual void start() = 0;

public slots:
    virtual void cancel() = 0; 

signals:    
    void operationComplete(int resultCode, QVariant data = QVariant());

protected:
    NmApiOperationStatus mStatus;
    NmApiOperationType mOperationType;
};

}
#endif /* NMAPIOPERATION_H_ */
