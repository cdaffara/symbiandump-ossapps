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
* Description:   tester for methods in VideoProxyModelGeneric
*
*/

#include "xqplugin.h"
#include "xqserviceutilstub.h"
#include "xqserviceproviderstub.h"
#include "xqserviceutilxtra.h"

#include <hbaction.h>
#include <qpointer.h>
#include <hbapplication.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionuiloader.h"
#include "testcollectionview.h"
#include "videocollectioncommon.h"
#include "videolistview.h"

#include "videolistviewdata.h"
#include "videocollectionuiloaderdata.h"
#include "videoservicebrowsedata.h"
#include "videolistviewdata.h"
#include "videocollectionviewutils.h"
#include "videocollectionviewutilsdata.h"

#define private public
#include "videocollectionviewplugin.h"
#include "videoservices.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    hbInstance->mWindowses.append(&window);

    TestCollectionView tv;

    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testcollectionview.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}



// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestCollectionView::initTestCase()
{
    // nop
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestCollectionView::cleanupTestCase()
{
    // nop
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestCollectionView::init()
{

    VideoCollectionUiLoaderData::reset();
    VideoListViewData::reset();
    VideoServiceBrowseData::reset();
    VideoListViewData::reset();
    
    mTestView = 0;
    mTestView = new VideoCollectionViewPlugin();
    mTestView->createView();
    
    HbMainWindow *window = hbInstance->allMainWindows().value(0);
    if (window)
    {
        HbView *view = qobject_cast<HbView*>(mTestView->getView());
        window->addView(view);
    }
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestCollectionView::cleanup()
{
    if(hbInstance->allMainWindows().count() > 0)
    {
        HbMainWindow *window = hbInstance->allMainWindows().at(0);
        QList<HbView*> views = window->views(); 
        int count = views.count();
        for (int i = 0; i < count; i++)
        {
            window->removeView(views.at(i));
        }
    }
    
    delete mTestView;
    mTestView = 0;
}

// ---------------------------------------------------------------------------
// testCreateView
// ---------------------------------------------------------------------------
//
void TestCollectionView::testCreateView()
{
    cleanup();
    VideoCollectionUiLoaderData::reset();
    VideoListViewData::reset();
    
    // finding view object from docml fails
    VideoCollectionUiLoaderData::mFindFailure = true;
	mTestView = new VideoCollectionViewPlugin();
    mTestView->createView();

    QVERIFY( mTestView->mView == 0 );
    QVERIFY( mTestView->mUiLoader != 0);

    delete mTestView;

    VideoCollectionUiLoaderData::mFindFailure = false;
	mTestView = new VideoCollectionViewPlugin();
    mTestView->createView();
    QVERIFY( mTestView->mView != 0 );
    QVERIFY( mTestView->mUiLoader != 0);
    QCOMPARE( VideoListViewData::mInitializeViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );
    
    // second create without view object (for coverity)
    delete mTestView->mView;
    mTestView->mView = 0;
    mTestView->createView();
    QVERIFY( mTestView->mView != 0 );
    QVERIFY( mTestView->mUiLoader != 0);
    QCOMPARE( VideoListViewData::mInitializeViewCount, 2 );
    QVERIFY( mTestView->mActivated == false );
    
    // make sure command signaling works
    connect( this, SIGNAL(commandSignal(int)), mTestView->mView, SIGNAL(command(int)));
    QSignalSpy commandSpy(mTestView, SIGNAL(command(int)));
    emit commandSignal(5);
    QCOMPARE( commandSpy.count(), 1 );
    QList<QVariant> arguments = commandSpy.takeFirst();
    QCOMPARE( arguments.at(0).toInt(), 5 );
    disconnect();

    // view exists
    VideoListView* current = mTestView->mView;
    mTestView->createView();
    QCOMPARE( mTestView->mView, current );    
    HbMainWindow *window = hbInstance->allMainWindows().value(0);
    if (window)
    {
        window->addView(current);
    }
}

// ---------------------------------------------------------------------------
// testActivateView
// ---------------------------------------------------------------------------
//
void TestCollectionView::testActivateView()
{
    XQServiceUtilXtra *serviceUtil = XQServiceUtilXtra::instance();
    VideoServices *videoServices = VideoServices::instance();
    
    // activate view two times
    mTestView->activateView();
    QVERIFY( mTestView->mActivated == true );
    QCOMPARE( VideoListViewData::mActivateViewCount, 1 );
    mTestView->activateView();
    QCOMPARE( VideoListViewData::mActivateViewCount, 1 );
    cleanup();
    
    // activate view as service:
    // -browse service
    // -captured category
    init();
    serviceUtil->setCurrentService(true);
    videoServices->setCurrentService(VideoServices::EBrowse);
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    VideoServiceBrowseData::mBrowseCategory = KVcxMvcCategoryIdCaptured;
    mTestView->activateView();
    QCOMPARE(VideoListViewData::mActivateViewCount, 1);
    QVERIFY(VideoListViewData::mActivatedItemId.iId1 == KVcxMvcCategoryIdCaptured);
    QVERIFY(VideoListViewData::mActivatedItemId.iId2 == KVcxMvcMediaTypeCategory);
    cleanup();
    
    // activate view as browse service:
    // -browse service
    // -downloads category
    init();
    serviceUtil->setCurrentService(true);
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    videoServices->setCurrentService(VideoServices::EBrowse);
    VideoServiceBrowseData::mBrowseCategory = KVcxMvcCategoryIdDownloads;
    mTestView->activateView();
    QCOMPARE(VideoListViewData::mActivateViewCount, 1);
    QVERIFY(VideoListViewData::mActivatedItemId.iId1 == KVcxMvcCategoryIdDownloads);
    QVERIFY(VideoListViewData::mActivatedItemId.iId2 == KVcxMvcMediaTypeCategory);
    cleanup();

    // activate view as browse service:
    // -uri fetch service
    init();
    serviceUtil->setCurrentService(true);
    VideoCollectionViewUtilsData::mIsServiceValue = true;
    VideoCollectionViewUtils::instance().setIsService();
    videoServices->setCurrentService(VideoServices::EUriFetcher);
    mTestView->activateView();
    QCOMPARE(VideoListViewData::mActivateViewCount, 1);
    QVERIFY(VideoListViewData::mActivatedItemId == TMPXItemId::InvalidId());
    cleanup();
    
    // final cleanup
    serviceUtil->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();
    VideoCollectionViewUtilsData::mIsServiceValue = false;
    VideoCollectionViewUtils::instance().setIsService();
}

// ---------------------------------------------------------------------------
// testDeactivateView
// ---------------------------------------------------------------------------
//
void TestCollectionView::testDeactivateView()
{
    mTestView->deactivateView();
    QCOMPARE( VideoListViewData::mDeactivateViewCount, 0 );
    QVERIFY( mTestView->mActivated == false );

    mTestView->activateView();

    mTestView->deactivateView();
    QCOMPARE( VideoListViewData::mDeactivateViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );

    mTestView->deactivateView();
    QCOMPARE( VideoListViewData::mDeactivateViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );
}

// ---------------------------------------------------------------------------
// testDestroyView
// ---------------------------------------------------------------------------
//
void TestCollectionView::testDestroyView()
{
    // remove view from mainwindow so that we don't get hanging pointers there, as
    // the test is creating and destroying view several times.
    HbMainWindow *window = hbInstance->allMainWindows().value(0);
    QVERIFY(window);
    window->removeView(mTestView->mView);
    
    mTestView->activateView();
    
    mTestView->destroyView();

    QCOMPARE( VideoListViewData::mDeactivateViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );
    QVERIFY( mTestView->mView == 0 );
    QVERIFY( mTestView->mUiLoader == 0);

    mTestView->createView();
    QVERIFY( mTestView->mView != 0 );
    QVERIFY( mTestView->mUiLoader != 0);
    
    mTestView->destroyView();
    QCOMPARE( VideoListViewData::mDeactivateViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );
    QVERIFY( mTestView->mView == 0 );
    QVERIFY( mTestView->mUiLoader == 0);
}

// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestCollectionView::testBack()
{
    mTestView->back();
    QCOMPARE( VideoListViewData::mBackCount, 0 );
    mTestView->activateView();
    mTestView->back();
    QCOMPARE( VideoListViewData::mBackCount, 1 );
}

// ---------------------------------------------------------------------------
// testOrientationChange
// ---------------------------------------------------------------------------
//
void TestCollectionView::testOrientationChange()
{
    // there's nothing to be tested for this.
    // slot is probably going to be removed. This is here just to have the
    // function decision coverage on correct level.
    mTestView->orientationChange(Qt::Horizontal);

}

// ---------------------------------------------------------------------------
// testTimerEvent
// ---------------------------------------------------------------------------
//
void TestCollectionView::testTimerEvent()
{
    QSignalSpy commandSpy(mTestView, SIGNAL(command(int)));
    QSignalSpy delayedSpy(mTestView, SIGNAL(doDelayeds()));
    
    QEvent event(QEvent::Timer);
    QApplication::sendEvent(mTestView, &event);
    QVERIFY(commandSpy.count() == 0);
    QVERIFY(delayedSpy.count() == 0);
    
    QTimerEvent timerEvent(mTestView->mTimerId);
    QApplication::sendEvent(mTestView, &timerEvent);
    QVERIFY(commandSpy.count() == 1);
    QVERIFY(delayedSpy.count() == 1);
}

// ---------------------------------------------------------------------------
// testGetView
// ---------------------------------------------------------------------------
//
void TestCollectionView::testGetView()
{
    QVERIFY( mTestView->getView() == mTestView->mView );
    mTestView->createView();
    QVERIFY( mTestView->getView() == mTestView->mView );
    mTestView->activateView();
    QVERIFY( mTestView->getView() == mTestView->mView );
    mTestView->deactivateView();
    QVERIFY( mTestView->getView() == mTestView->mView );
    mTestView->destroyView();
    QVERIFY( mTestView->getView() == 0 );    
    // need to create view to handle cleaning up correctly
    mTestView->createView();
    HbMainWindow *window = hbInstance->allMainWindows().value(0);
    window->removeView(mTestView->mView);
}

// End of file

