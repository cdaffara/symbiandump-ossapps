/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef UNITTEST_CXEIMAGEDATAITEMSYMBIAN_H
#define UNITTEST_CXEIMAGEDATAITEMSYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeImageDataItemSymbian;
class CxeStillImageSymbian;

class UnitTestCxeImageDataItemSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeImageDataItemSymbian();
    ~UnitTestCxeImageDataItemSymbian();

private slots:
    
    // for every test case
    void init();
    
    // for every test case
    void cleanup();

    // before any test cases
    void initTestCase();
    // after last test case
    void cleanupTestCase();
        
    void testSave();
    void testPath();
    void testState();
    void testId();
    void testIsLocationEnabled();

private:
    QString generateImageFileName();
    QString dummyPath(const QString &drive);
    QString path();

private:
    CxeImageDataItemSymbian *mImageDataItem; // This will be recreated for each test
    QString mFilename;
    int mImageCounter;
};

#endif // UNITTEST_CXEIMAGEDATAITEMSYMBIAN_H
