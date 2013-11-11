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
#ifndef UNITTEST_CXEIMAGEDATAQUEUESYMBIAN_H
#define UNITTEST_CXEIMAGEDATAQUEUESYMBIAN_H

#include <QObject>

class CxeImageDataQueueUnit;

class UnitTestCxeImageDataQueueSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeImageDataQueueSymbian();
    ~UnitTestCxeImageDataQueueSymbian();

private slots:
    void init();
    void cleanup();

    void testSize();
    void testClear1();
    void testClear2();
    void testOperatorAtIndex();
    void testStartSave();

private:
    // Helper methods for test cases

private:
    // Using inherited class for test to isolate other engine classes out.
    CxeImageDataQueueUnit *mImageDataQueue;
};

#endif // UNITTEST_CXEIMAGEDATAQUEUESYMBIAN_H
