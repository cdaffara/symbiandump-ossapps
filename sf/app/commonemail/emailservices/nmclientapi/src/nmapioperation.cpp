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

#include "nmapiheaders.h"

namespace EmailClientApi {

NmApiOperation::NmApiOperation(QObject *parent, NmApiOperationStatus status, NmApiOperationType operationType)
    :QObject(parent),
     mStatus(status),
     mOperationType(operationType)
{
    NM_FUNCTION;
    QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
}

NmApiOperation::~NmApiOperation()
{
    NM_FUNCTION;
}

/*!
 \fn status 
 \return Operation status.
 */
NmApiOperationStatus NmApiOperation::status() const 
{
    NM_FUNCTION;
    return mStatus;
}
 
/*!
 \fn operation 
 \return Operation type.
 */
NmApiOperationType NmApiOperation::operation() const
{
    NM_FUNCTION;
    return mOperationType;
}

}
