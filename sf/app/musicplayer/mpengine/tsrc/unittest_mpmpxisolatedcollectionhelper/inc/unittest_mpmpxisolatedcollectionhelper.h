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
* Description: Unit test for CMpMpxIsolatedCollectionHelper
*
*/

#ifndef TESTMPMPXISOLATEDCOLLECTIONHELPER_H
#define TESTMPMPXISOLATEDCOLLECTIONHELPER_H

#include <QtTest/QtTest>

/* in our unit test usually this goes on the CPP file, but in this case we need it here to get
 the observer interface, and also the mpxcollectionopenutility stub before the tested class so 
 we get the stub instead of the normal mpxcollectionopenutility.*/
#include "stub/inc/mpxcollectionopenutility.h"
// Do this so we can access all member variables.
#define private public
#include "mpmpxisolatedcollectionhelper.h"
#undef private

class CMpMpxIsolatedCollectionHelper;
class CMPXMedia;

class TestCMpMpxIsolatedCollectionHelper : public QObject,
                                                  MMpMpxIsolatedCollectionHelperObserver
{
    Q_OBJECT

public:

    TestCMpMpxIsolatedCollectionHelper();
    virtual ~TestCMpMpxIsolatedCollectionHelper();

private:
    
    void HandleIsolatedOpenL( const CMPXMedia& aEntries, TInt aError );
    void HandleIsolatedOpenRestorePathL( const CMPXCollectionPath& aPath, TInt aError );
    
public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testConstructor();
    void testOpenCollection();
    void testHandleOpen();

private:

    CMpMpxIsolatedCollectionHelper  *mTest;
    const CMPXMedia                 *iEntriesFromoOpenCallback; //not own
    TInt                             iErrorFromoOpenCallback;
    TInt                             iOpenCallbackCounter;
    TInt                             iOpenRestoreCallbackCounter;   

};

#endif  // TESTMPMPXISOLATEDCOLLECTIONHELPER_H




