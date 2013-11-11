
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
* Description:   tester for methods in CVideoCollectionListener
* 
*/

#ifndef __TESTVIDEOCOLLECTIONLISTENER_H__
#define __TESTVIDEOCOLLECTIONLISTENER_H__


// INCLUDES
#include <QtTest/QtTest>
#include "stub/inc/videocollectionclient.h"

class StubSignalReceiver;
class StubCollection;
class VideoCollectionListener;
class VideoCollectionClient;
class MediaObjectFactory;

class TestVideoCollectionListener : public QObject
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
     * verifies HandleCollectionMediaL
     */
    void testHandleCollectionMediaLFunc();
    
    /**
     * verifies HandleOpenL with param
     * - const CMPXMedia& 
     * - TInt 
     * - TBool
     * - TInt 
     */
    void testHandleOpenLMediaFunc();
    
    /**
     * verifies HandleOpenL with param
     * - const CMPXCollectionPlaylist&
     * - TInt 
     */
    void testHandleOpenLPlaylistFunc();
     
    /**
     * verifies HandleCommandComplete with invalid
     * params
     */
    void testHandleCommandCompleteInvalid();
    
    /**
     * verifies HandleCommandComplete (and handleGetVideoDetailsResp)  for
     * KVcxCommandMyVideosGetMediaFullDetailsByMpxId command
     */
    void testHandleCommandCompleteGetDetails();
    
    /**
     * verifies HandleCommandComplete  for
     * KVcxCommandMyVideosRemoveAlbums command
     */
    void testHandleCommandCompleteRemoveAlbums();
    
    /**
     * verifies HandleCollectionMessage 
     * error case, empty message
     */
    void testCollectionMessageError();
    
    /**
     * verifies HandleCollectionMessage during 
     * collection openening phase
     */
    void testCollectionMessageNotOpened();
       
    /**
     * verifies HandleCollectionMessage when
     * collection has opened and
     * receiving message id == KVcxCommandIdMyVideos
     */
    void testCollectionMessageMyVidCommandId();
    
    /**
     * verifies handleMyVideosMessageArray method
     * collection has opened and
     * receiving message id == KVcxCommandIdMyVideos and
     * KVcxMediaMyVideosCommandId == KVcxMessageMyVideosMessageArray
     */
    void testHandleMyVideosMessageArray();
    
    /**
     * verifies handleMyVideosMPXMessage  method
     * collection has opened and
     * receiving message id == KVcxCommandIdMyVideos and
     * KVcxMediaMyVideosCommandId != KVcxMessageMyVideosMessageArray
     */
    void testHandleMyVideosMPXMessage();
    
    /**
     * verifies handleMyVideosMPXMessage method
     * collection has opened and
     * receiving message id == KVcxCommandIdMyVideos and
     * KVcxMediaMyVideosCommandId == KVcxMessageMyVideosDeleteResp
     */
    void testMyVideosDeleteMsgArray();
    
    /**
     * verifies handleMPXMessage method
     * collection has opened and
     * receiving message id != KVcxCommandIdMyVideos 
     */
    void testHandleMPXMessage();
    
private:
    
    /**
     * testobject
     */
    VideoCollectionListener *mTestObject;
    
    /**
     * stub collection client 
     */
    VideoCollectionClient *mStubCollectionClient; 
    
    /**
     * stub collection
     */
    StubCollection *mStubCollection;
    
    /**
     * media object factory
     */
    MediaObjectFactory *mMediaFactory;
    
    /**
     * stubbed signal receiver    
     */
    StubSignalReceiver *mSignalReceiver;


};




#endif  // __TESTVIDEOCOLLECTIONWRAPPER_H__

// End of file
    


