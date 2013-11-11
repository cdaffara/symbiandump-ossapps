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
#include "asyncworker.h"
#include "cntsimutility.h"

AsyncWorker::AsyncWorker(CntSimUtility* su) :
    CActive(EPriorityStandard),
    simUtility(su)
{
    CActiveScheduler::Add(this);
}

AsyncWorker::~AsyncWorker()
{
    Cancel();
}

void AsyncWorker::RunL()
{
    simUtility->RequestCompleted(iStatus.Int());
}

void AsyncWorker::DoCancel()
{
}

TInt AsyncWorker::RunError(TInt aError)
{
    Q_UNUSED(aError)
    return KErrNone;
}
