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
#ifndef ASYNCWORKER_H
#define ASYNCWORKER_H

#include <e32base.h>
class CntSimUtility;

class AsyncWorker : public CActive
{
public:
    AsyncWorker(CntSimUtility* su);
    ~AsyncWorker();

private: // from CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    CntSimUtility*  simUtility;
    friend class CntSimUtility;
};

#endif
