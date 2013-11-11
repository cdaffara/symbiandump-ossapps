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



#ifndef __TESTVIDEOLISTDATA_H__
#define __TESTVIDEOLISTDATA_H__


// INCLUDES
#include <QtTest/QtTest>

class MediaObjectFactory;
class VideoDataContainer;


class TestVideoDataContainer : public QObject
{
    Q_OBJECT
    
   
    // test functions for the test framework
private slots:
    /**
     * called before anything else when test starts
     */
    void initTestCase();

    /**
     * will be called before each testfunction is executed.
     *
     */
    void init(); 
    
    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();
    
    /**
     * will be called after testing ends
     *
     */
    void cleanupTestCase();
    
    /**
     * Verifies clear
     */
    void testClear();

    /**
     * Verifies remove
     */
    void testRemove();
      
    /**
     * Verifies append
     */  
    void testAppend();

    /**
     * Verifies fromIndex
     */  
    void testFromIndex();
      
    /**
     * Verifies indexOfId
     */  
    void testIndexOfId();

    /**
     * Verifies idFromIndex
     */  
    void testIdFromIndex();

    /**
     * Verifies count
     */ 
    void testCount();
    
    /**
     * verifies markItemRemoved
     */
    void testMarkItemRemoved();
    
    /**
     * verifies clearRemoved
     */
    void testClearRemoved();
    
    /**
     * verifies restoreRemovedItems
     */
    void testRestoreRemovedItems();
    
    /**
     * verifies getRemovedMedia
     */
    void testGetRemovedMedia();

private:
  
    /**
     * object under test
     */  
    VideoDataContainer *mTestObject;
    
    /**
     * CMPXMedia -object factory.
     */
    MediaObjectFactory *mMediaFactory;

};


#endif  // __TESTVIDEOLISTDATA_H__

// End of file
    


