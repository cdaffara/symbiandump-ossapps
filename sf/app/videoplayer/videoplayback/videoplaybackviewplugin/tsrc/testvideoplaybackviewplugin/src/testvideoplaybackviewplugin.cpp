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
* Description:   tester for methods in VideoSortFilterProxyModel
* 
*/

#include <hbapplication.h>
#include <hbinstance.h>



#include "testvideoplaybackviewplugin.h"
#include "videoplaybackview.h"

#include "videoplaybackviewplugin.h"
#include "mpxhbvideocommondefs.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    
    TestVideoPlaybackViewPlugin tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideoplaybackviewplugin.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::init()
{    
    
    mVidPBPlugin = new VideoPlaybackViewPlugin();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::cleanup()
{
    delete mVidPBPlugin; 
    mVidPBPlugin = 0;
}

// ---------------------------------------------------------------------------
// Slot: test create view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testCreateView()
{
    qDebug("===> testCreateView()"); 
               
    init();	  

    QVERIFY(mVidPBPlugin != NULL );
	
    QVERIFY(mVidPBPlugin->mView == NULL );
	
    mVidPBPlugin->createView();
	  
    QVERIFY(mVidPBPlugin->mView != NULL );
}

// ---------------------------------------------------------------------------
// Slot: test activate view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testActivateView()
{
    QVERIFY(mVidPBPlugin->mView != NULL );			
    mVidPBPlugin->activateView();
		
    QVERIFY(mVidPBPlugin->mViewActivated == true  );
}

// ---------------------------------------------------------------------------
// Slot: test deactivate view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testDeactivateView()
{
    QVERIFY(mVidPBPlugin->mView != NULL );
    QVERIFY(mVidPBPlugin->mViewActivated == true  );
		
    mVidPBPlugin->deactivateView();
		
    QVERIFY(mVidPBPlugin->mViewActivated == false  );
}

// ---------------------------------------------------------------------------
// Slot: test destroy view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testDestroyView()
{
    QVERIFY(mVidPBPlugin->mView != NULL );
		
    mVidPBPlugin->destroyView();
		
    QVERIFY(mVidPBPlugin->mView == NULL );    
}

// ---------------------------------------------------------------------------
// Slot: test get view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testGetView()
{
    QVERIFY( mVidPBPlugin->getView() != NULL );	
    QVERIFY(mVidPBPlugin->mView == mVidPBPlugin->getView() );		    
}

// End of file
