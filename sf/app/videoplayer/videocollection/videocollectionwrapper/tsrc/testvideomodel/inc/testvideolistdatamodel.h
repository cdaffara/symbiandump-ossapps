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
* Description:  Testclas for testing VideoListDataModel class methods*
*/


#ifndef __TESTVIDEOLISTDATAMODEL_H__
#define __TESTVIDEOLISTDATAMODEL_H__


// INCLUDES
#include <QtTest/QtTest>


class VideoCollectionClient;
class VideoListDataModel;
class MediaObjectFactory;

class TestVideoListDataModel : public QObject
{
    Q_OBJECT
    
 
signals:
    /**
     * when testing dataChanged -signal emitting and
     * videoDataChangedSlot -slot
     */
    void testVideoDataChangedSignal(int);
       
    
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
     * Tests for the c++ constructor. Mainly to check that the parent is set
     * correctly.
     */
    void testConstructor();
    
    /**
     * Tests for possible failure initializations
     */
    void testInitializeFails();
    
    /**
     * Tests for possible succesfully initiliazation
     *
     */
    void testInitialize();
    
    /**
     * Tests to make sure model returns correct row count
     * in all cases
     *
     */
    void testRowCount();
    
    /**
     * tests if correct media id will be returned at certain positions
     *
     */
    void testMediaIdAtIndex();
    
    /**
     * verifies indexOfId
     */
    void testIndexOfId();
	
    /**
     * tests if correct file path for media will be returned at certain positions
     *
     */
    void testMediaFilePathForId();
	
    /**
     * Tests to make sure model returns correct data from itemData -method
     *
     */
    void testItemData();
    
    
    /**
     * Tests to make sure model returns correct data from Data -method
     *
     */
    void testData();
    
    /**
     * Tests to make sure model returns correct data from ColumnCount
     *
     */
    void testColumnCount();
       
    /**
     * Tests to make sure model returns correct data from index
     *
     */
    void testIndex();
   
    /**
     * Tests to make sure model returns correct data from parent
     *
     */
    void testParent();
    
    /**
     * Tests to make sure detail rows are created ok in all cases
     *
     */
    void testPrepareDetails(); 

    /**
     * Tests video count string is created ok in all cases
     *
     */
    void testPrepareVideoCountString();

    /**
     * Tests to make sure size strings are created ok in all cases
     *
     */
    void testPrepareSizeString();
      
    /**
     * Tests to make sure length strings are created ok in all cases
     *
     */
    void testPrepareLengthString();
        
    /**
     * Tests to make removeorws behaves correctly in all posiible cases
     *
     */
    void testRemoveRows();
    
    /**
     * tests deleteStartingFailsSlot
     */
    void testDeleteStartingFailsSlot();
    
    /**
     * verifies belongsToAlbum
     */
    void testBelongsToAlbum();
    
    /**
     * verifies setAlbumInUse
     */
    void testSetAlbumInUse();
    
    /**
     * verifies removeItemsFromAlbum;
     */
    void testRemoveItemsFromAlbum();
    
    /**
     * tests reportAsyncStatus
     */
    void testreportAsyncStatus();


private:
    
    /**
     * creates all objects used in tests,
     * returns false if some creation fails.
     * 
     * @return bool
     *
     */
    bool initializeTestObjects();
    
    /**
     * cleanups class data
     */
    void cleanupTestObjects();
    
    /**
     * videocollection client stub -object
     */
    VideoCollectionClient *mCollectionStub;
    
    /**
     * Test object for wich intialize is not called in 
     * init -method
     */
    VideoListDataModel *mTestObjectInitialized;
    
    /**
     * Test object for wich intialize is called in 
     * init -method
     */
    VideoListDataModel *mTestObjectNotInitialized;

    /**
     * CMPXMedia -object factory.
     */
    MediaObjectFactory *mMediaFactory;
};



#endif  // __TESTVIDEOLISTDATAMODEL_H__

// End of file
    


