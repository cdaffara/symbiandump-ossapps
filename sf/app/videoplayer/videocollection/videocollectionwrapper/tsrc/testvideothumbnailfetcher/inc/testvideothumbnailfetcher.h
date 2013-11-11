/**
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
* Description:   Unit test for methods in VideoThumbnailFetcher
*
*/

#ifndef __TESTVIDEOTHUMBNAILFETCHER_H__
#define __TESTVIDEOTHUMBNAILFETCHER_H__

// INCLUDES
#include <QtTest/QtTest>

// FORWARD DECLARATIONS
class VideoThumbnailFetcherTester;

class TestVideoThumbnailFetcher : public QObject
{
    Q_OBJECT

    void init();
    
    void cleanup();
    
    // test functions for the test framework
private slots:

    void testConstructDestruct();    
    void testAddFetch();
    void testCancelFetches();
    void testFetchCount();
    void testPauseFetching();
    void testContinueFetching();
    void testEnableThumbnailCreation();
    void testStartThumbnailFetches();
    void testStartThumbnailCreation();
    void testThumbnailReadySlot();

private:
    VideoThumbnailFetcherTester *mTestObject;

};

#endif  // __TESTVIDEOTHUMBNAILFETCHER_H__

// End of file
