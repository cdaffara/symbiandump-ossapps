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
 */

#ifndef C_TESTUITIMER_H
#define C_TESTUITIMER_H


#include <e32base.h>

#define KPeriod 10000 // period of timer

/**
 *  timer for test programs
 */
NONSHARABLE_CLASS( CTestUiTimer ): public CTimer
    {
public:

    static CTestUiTimer* NewL();
    virtual  ~CTestUiTimer();

    void RunL();
    void DoCancel();
    void ConstructL();
    void IssueRequest();
    void TimeoutOperation(CActive* aObject, TTimeIntervalSeconds aTimeoutInSeconds);

private:
    CTestUiTimer();

public:
    TTimeIntervalMicroSeconds32 period;

private: // data
    CActive* iObject;
    };

#endif // C_TESTUITIMER_H
