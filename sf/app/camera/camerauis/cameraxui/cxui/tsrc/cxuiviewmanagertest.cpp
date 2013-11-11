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
#include <QTest>
#include <QSignalSpy>
#include <hbmainwindow.h>
#include "cxutils.h"
#include "cxuiviewmanager.h"
#include "cxuifakeengine.h"
#include "cxuitest.h"
#include "cxuienums.h"


// Tests only that correct view is activated
void TestCxUi::testViewManager()
    {
    CX_DEBUG(("testViewManager"));
    //mEngineFake->initModeFake(Cxe::ImageMode);
    mEngineFake->initMode(Cxe::ImageMode);
    //QTest::qWait(5000);

    connect(this, SIGNAL(toPostcaptureView()), mViewManager, SLOT(changeToPostcaptureView()));
    connect(this, SIGNAL(toPrecaptureView()), mViewManager, SLOT(changeToPrecaptureView()));

    // Test if we are in still VF
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::StillPreCaptureView );
    
    emit toPostcaptureView();
    // Test if image review activated
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPostCaptureView );
    
    emit toPrecaptureView();
    // Test if still VF view active
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPreCaptureView );
    
    mEngineFake->initModeFake(Cxe::VideoMode);
    emit toPrecaptureView();
    // Test if video VF view active
    QVERIFY( mMainWindow->currentViewIndex() == CxUi::VideoPreCaptureView );
    
    emit toPostcaptureView();
    // Test if video review active
    QVERIFY( mMainWindow->currentViewIndex() == CxUi::VideoPostCaptureView );
    
    emit toPrecaptureView();
    // Test if video VF view active
    QVERIFY( mMainWindow->currentViewIndex() == CxUi::VideoPreCaptureView ); 
    
    // Go back to original state...
    mEngineFake->initModeFake(Cxe::ImageMode);
    mEngineFake->initMode(Cxe::ImageMode);
    emit toPrecaptureView();
    QTest::qWait(5000);
    }

// Tests capture key in still mode
void TestCxUi::testCaptureKeyInImageMode()
    {   
    // Test if we are in still VF
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::StillPreCaptureView );    
    CX_DEBUG(("testCaptureKey in still capture mode"));
    
    QSignalSpy deviceStateSpy(&mEngineFake->stillCaptureControl(), 
            SIGNAL(snapshotReady(int)));   
    
    sendCameraKeyEvent();
    QVERIFY( deviceStateSpy.isValid() );        
    QCOMPARE( deviceStateSpy.count(), 1 );
    if (deviceStateSpy.count() > 0) {
        // Expect zero error code
        QList<QVariant> initModeArguments = deviceStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).toInt() == 0 );
    }    
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPostCaptureView );
    
    CX_DEBUG(("Test to get back viewfinder with capture key"));
    sendCameraKeyEvent();
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::StillPreCaptureView );
    
    // Test capture key in wrong engine state?
    CX_DEBUG(("testCaptureKey in uninitialezed engine state, capture pending"));
    mEngineFake->fakeStillCaptureControl().setFakeState(CxeStillCaptureControl::Uninitialized);
    sendCameraKeyEvent();     
    
    CX_DEBUG(("Change engine in ready state"));    
    mEngineFake->fakeStillCaptureControl().setFakeState(CxeStillCaptureControl::Ready);
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPostCaptureView );
    
    QSignalSpy autofocusStateSpy(&mEngineFake->autoFocusControl(), 
                SIGNAL(stateChanged(CxeAutoFocusControl::State, CxeError::Id)));          
    QVERIFY( autofocusStateSpy.isValid() );  
    
    CX_DEBUG(("test halfkey press to go back"));
    sendAutofocusKeyEvent(QEvent::KeyPress); 
    //QTest::qWait(1000);
    sendAutofocusKeyEvent(QEvent::KeyRelease);    
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::StillPreCaptureView );
    
    QCOMPARE( autofocusStateSpy.count(), 3 );
    if (autofocusStateSpy.count() > 0) {
        QList<QVariant> initModeArguments = autofocusStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::InProgress );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        CX_DEBUG(("testAutoFocus_2a"));
        initModeArguments = autofocusStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Canceling );
        initModeArguments = autofocusStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Unknown );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );  
    }
    
    mEngineFake->fakeStillCaptureControl().setFakeState(CxeStillCaptureControl::Capturing);
    sendCameraKeyEvent();
    // Nothing should happen
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::StillPreCaptureView );  
    }

// Tests capture key in video mode
void TestCxUi::testCaptureKeyInVideoMode()
{ 

    /*connect(this, SIGNAL(toPrecaptureView()), mViewManager, SLOT(changeToPrecaptureView()));

    QSignalSpy deviceStateSpy(&mEngineFake->videoCaptureControl(), 
            SIGNAL(stateChanged(CxeVideoCaptureControl::State, int)));    
    QVERIFY( deviceStateSpy.isValid() ); 
    emit toPrecaptureView();
    QTest::qWait(500);
    // Test if we are in still VF
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::VideoPreCaptureView );

    // Generate camera key event to start recording
    QKeyEvent* captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, 
            Qt::Key_unknown, 
            0, 
            0xab, 
            0,
            0);    
    
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
                       QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(5000);
    
    // Generate camera key event to stop recording
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, 
            Qt::Key_unknown, 
            0, 
            0xab, 
            0,
            0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
                           QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(3000);


        QCOMPARE( deviceStateSpy.count(), 3 );
        
        QList<QVariant> initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Recording );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Stopping );
        initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Idle );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );  
    
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::VideoPostCaptureView );
    
    CX_DEBUG(("Test to get back viewfinder with capture key"));
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, Qt::Key_unknown, 0, 0xab, 0,0);    
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(1000); 
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::VideoPreCaptureView );*/
}

