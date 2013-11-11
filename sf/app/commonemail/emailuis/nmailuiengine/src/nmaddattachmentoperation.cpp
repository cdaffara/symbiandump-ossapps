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

#include "nmuiengineheaders.h"

/*!
  Constructor
  */
NmAddAttachmentsOperation::NmAddAttachmentsOperation()
{
    NM_FUNCTION;
}

/*!
  Destructor
 */
NmAddAttachmentsOperation::~NmAddAttachmentsOperation()
{
    NM_FUNCTION;
}

/*!
  \brief Slot, complete
  The performer of the asynchronous function call should use this slot when
  the operation is completed, this will emit the operationPartCompleted signal * 
  \param result Result from operation
 */
void NmAddAttachmentsOperation::completeOperationPart(const QString &fileName, const NmId &msgPartId, int result)
{
    NM_FUNCTION;
    
    emit operationPartCompleted(fileName, msgPartId, result);
}


