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



#ifndef __TESTVIDEODELETEWORKER_H__
#define __TESTVIDEODELETEWORKER_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoCollectionClient;
class VideoDeleteWorker;


class TestVideoDeleteWorker : public QObject
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
     * verifies requestDelete
     */
    void testRequestDelete();
    
    /**
     * verifies removeFromRequest
     */
    void testRemoveFromRequest();
    
    /**
     * verifies isDeleting
     */
    void testIsDeleting();
    
    /**
     * verifies updateStatus, clearStatus 
     * and getLastStatus
     */
    void testUpdateStatus();
    
    /**
     * verifies continueSlot
     */
    void testContinueSlot();
    
private:
  
    /**
     * object under test
     */  
    VideoDeleteWorker *mTestObject;
    
    /**
     * dummy collectionclient.
     */
    VideoCollectionClient *mDummyCollection;

};


#endif  // __TESTVIDEODELETEWORKER_H__

// End of file
    


