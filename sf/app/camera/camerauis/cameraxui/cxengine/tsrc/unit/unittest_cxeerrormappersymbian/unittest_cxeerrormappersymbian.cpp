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
#include <QTest>
#include "cxeerror.h"
#include "e32err.h"
#include "cxeerrormappingsymbian.h"
#include "unittest_cxeerrormappersymbian.h"

// Enums


UnitTestCxeErrorMapperSymbian::UnitTestCxeErrorMapperSymbian()
{
}

UnitTestCxeErrorMapperSymbian::~UnitTestCxeErrorMapperSymbian()
{
}

// Run before each individual test case
void UnitTestCxeErrorMapperSymbian::init()
{
}

// Run after each individual test case
void UnitTestCxeErrorMapperSymbian::cleanup()
{
}

void UnitTestCxeErrorMapperSymbian::testSymbianErr2CxeErrorId()
{
    int symbianError = -10000;
    CxeError::Id result;
    
    // case 0: check default value
    result = CxeErrorHandlingSymbian::map(symbianError);
    QVERIFY(result == CxeError::General);
    
    // case 0: check with unknown value
    result = CxeErrorHandlingSymbian::map(100);
    QVERIFY(result == CxeError::General);

    // case 1: try with valid error code
    // KrrNone should be mapped to CxeError::None.
    symbianError = KErrNone;
    result = CxeErrorHandlingSymbian::map(symbianError);
    QVERIFY(result == CxeError::None);
    
    symbianError = KErrNotReady;
    result = CxeErrorHandlingSymbian::map(symbianError);
    QVERIFY(result == CxeError::NotReady);
    
    // case 2: try with invalid error code
    symbianError = KErrTotalLossOfPrecision;
    result = CxeErrorHandlingSymbian::map(symbianError);
    QVERIFY(result == CxeError::General);

}


// main() function non-GUI testing
QTEST_APPLESS_MAIN(UnitTestCxeErrorMapperSymbian)
;
