
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
* Description:   tester for methods in CVideoCollectionClient
* 
*/

#ifndef __TESTVIDEOCOLLECTIONCLIENT_H__
#define __TESTVIDEOCOLLECTIONCLIENT_H__


// INCLUDES
#include <QtTest/QtTest>

#include "stub/inc/mpxmedia.h"
#include "stub/inc/mpxmediaarray.h"
#include "stub/inc/mpxattribute.h"
#include "stub/inc/mpxcollectionpath.h"
#include "stub/inc/mpxcollectionutility.h"

#include <mpxcollectionframeworkdefs.h>


class VideoCollectionClient;
class StubSignalReceiver;

class TestVideoCollectionClient : public QObject
{
    Q_OBJECT

    // test functions for the test framework
private slots:

    
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
     * initialization while collectionutility exists allready
     *
     */
    void testInitializeCollUtilityExists();
    
    /**
     * initialization collection listener exists
     *
     */
    void testInitializeCollListenerExist();
    
    /**
     * initialization and collection utility creation fails
     *
     */
    void testInitializeUtilCreateFail();
    
    /**
     * initialization and collection utility creation fails
     *
     */
    void testInitializeNullReceiverCreateFail();
    
    /**
     * succeeded initialization test
     *
     */
    void testInitializeSucceed();
    
    /**
     * verifies collectionLevel -call
     *
     */
    void testCollectionLevel();
    
    /**
     * verifies getOpenStatus -call
     *
     */
    void testGetOpenStatus();
     
    /**
     * verifies setOpenStatus -call
     *
     */
    void testSetOpenStatus();
    
    /**
     * verifies startOpenCollection -call
     *
     */
    void testStartOpenCollection();
    
    /**
     * verifies startOpenCurrentState -call
     *
     */
    void testStartOpenCurrentState();
    
   
    /**
     * verifies deleteVideos -call
     *
     */
    void testDeleteVideos();
    

    /**
     * verifies openVideo -call
     *
     */
    void testOpenCategory();    
    
    /**
     * verifies openVideo -call
     *
     */
    void testOpenVideo();

    /**
     * verifies fetchMpxMediaByMpxId -call
     *
     */
    void testFetchMpxMediaByMpxId();
       
    /**
     * verifies getVideoDetails -call
     *
     */
    void testGetVideoDetails();
    
    /**
     * verifies removeAlbums
     */
    void testRemoveAlbums();
    
    /**
     * verifies addNewCollection -call.
     */
    void testAddNewCollection();
    
    /**
     * verifies addItemsInAlbum
     */
    void testAddItemsInAlbum();
    
    /**
     * verifies removeItemsFromAlbum
     */
    void testRemoveItemsFromAlbum();
    
    /**
     * verifies testRenameItem
     */
    void testRenameItem();

    /**
     * verifies back -call
     *
     */
    void testBack(); 
    
    
private:

    /**
     * testobject
     */
    VideoCollectionClient* mTestObject;
    
    /**
     * stubbed signal receiver
     */
    StubSignalReceiver* mSignalReceiver;

};




#endif  // __TESTVIDEOCOLLECTIONWRAPPER_H__

// End of file
    


