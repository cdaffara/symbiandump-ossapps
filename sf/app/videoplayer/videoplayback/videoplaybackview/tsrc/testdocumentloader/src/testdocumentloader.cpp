/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   tester for methods in TestVideoPlaybackDocumentLoader
*
*/

// Version : %version:  7 %


#include <qdebug>
#include <qobject>

#include <hbmainwindow.h>
#include <hbapplication.h>

#include "mpxvideo_debug.h"
#include "videoplaybackcontrolscontroller.h"
#include "testdocumentloader.h"

#define private public
#include "videoplaybackdocumentloader.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestDocumentLoader::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestDocumentLoader tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testdocumentloader.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestDocumentLoader::init()
{
    MPX_ENTER_EXIT(_L("TestDocumentLoader::init()"));

    mController = new VideoPlaybackControlsController();
    mLoader = new VideoPlaybackDocumentLoader( mController );
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestDocumentLoader::cleanup()
{
    MPX_ENTER_EXIT(_L("TestDocumentLoader::cleanup()"));

    if ( mLoader )
    {
        delete mLoader;
        mLoader = NULL;
    }

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }
}

// ---------------------------------------------------------------------------
// testCreateObject
// ---------------------------------------------------------------------------
//
void TestDocumentLoader::testCreateObject()
{
    MPX_ENTER_EXIT(_L("TestDocumentLoader::testCreateObject()"));

    init();

    QObject *object = mLoader->createObject( "", "controlBarLayout" );
    QVERIFY( object->objectName() == "controlBarLayout" );

    object = mLoader->createObject( "", "fileDetailsLayout" );
    QVERIFY( object->objectName() == "fileDetailsLayout" );

    object = mLoader->createObject( "", "detailsPlaybackWindow" );
    QVERIFY( object->objectName() == "detailsPlaybackWindow" );

    cleanup();
}

// End of file
