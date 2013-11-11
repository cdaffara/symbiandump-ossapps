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
#ifndef UNITTEST_CXESTILLIMAGESYMBIAN_H
#define UNITTEST_CXESTILLIMAGESYMBIAN_H

#include <QObject>

#include "cxestillimagesymbian.h"

class UnitTestCxeStillImageSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeStillImageSymbian();
    ~UnitTestCxeStillImageSymbian();

private slots:
    void init();
    void cleanup();

    void testdataItem();
    void testfilename();
    void testsnapshot();
    void testsaved();
    void testsetSnapshot();
    void testsetFilename();
    void testsetSaved();
    void testsetDataItem();

private:
    // Helper methods for test cases

private:
    CxeStillImageSymbian *mCxeStillImageSymbian;
   
};

#endif // UNITTEST_CXESTILLIMAGESYMBIAN_H
