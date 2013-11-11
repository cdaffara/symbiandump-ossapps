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
#ifndef UNITTEST_CXETESTUTILS_H
#define UNITTEST_CXETESTUTILS_H

#include <QObject>

class UnitTestCxeTestUtils : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeTestUtils();
    ~UnitTestCxeTestUtils();

private slots:

    void testWaitForSignal1();
    void testWaitForSignal2();
    void testWaitForSignal3();
    void testWaitForSignal4();

    void testWaitForState1();
    void testWaitForState2();
    void testWaitForState3();
};

#endif // UNITTEST_CXETESTUTILS_H
