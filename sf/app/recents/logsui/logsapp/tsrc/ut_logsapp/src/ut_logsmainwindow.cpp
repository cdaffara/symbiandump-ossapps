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

//USER
#include "ut_logsmainwindow.h"
#include "logsmainwindow.h"
#include "qthighway_stub_helper.h"
#include "hbstubs_helper.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hbmainwindow.h>
#include <QKeyEvent>
#include <xqkeycapture.h>

void UT_LogsMainWindow::initTestCase()
{

}

void UT_LogsMainWindow::cleanupTestCase()
{

}


void UT_LogsMainWindow::init()
{
    mMainWindow = new LogsMainWindow;
}

void UT_LogsMainWindow::cleanup()
{
    delete mMainWindow;
    mMainWindow = 0;
}

void UT_LogsMainWindow::testKeyPressEvent()
{
    QSignalSpy spy( mMainWindow, SIGNAL(callKeyPressed()) );
    // Not handled
    QKeyEvent event( QEvent::KeyPress, Qt::Key_Hangup, Qt::NoModifier);
    mMainWindow->keyPressEvent( &event );
    QVERIFY( spy.count() == 0 );
    
    // Handled
    QKeyEvent event2( QEvent::KeyPress, Qt::Key_Yes, Qt::NoModifier);
    mMainWindow->keyPressEvent( &event2 );
    QVERIFY( spy.count() == 1 );
    
    // Handled
    spy.clear();
    QKeyEvent event3( QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    mMainWindow->keyPressEvent( &event3 );
    QVERIFY( spy.count() == 1 );
}

void UT_LogsMainWindow::testSendAppToBackground()
{
    QtHighwayStubHelper::reset();
    mMainWindow->sendAppToBackground();
    QVERIFY( QtHighwayStubHelper::utilToBackground() );  
}

void UT_LogsMainWindow::testBringAppToForeground()
{
    QtHighwayStubHelper::reset();
    HbStubHelper::reset();
    mMainWindow->bringAppToForeground();
    QVERIFY( HbStubHelper::isWidgetRaised() ); 
    
    // Subsequent call raise tries to raise regardless of current state
    HbStubHelper::reset();
    mMainWindow->bringAppToForeground();
    QVERIFY( HbStubHelper::isWidgetRaised() ); 
}
 
void UT_LogsMainWindow::testEventFilter()
{    
    QSignalSpy spy( mMainWindow, SIGNAL(localeChanged()) );
    QVERIFY( !mMainWindow->mKeyCapture->mKeys.contains(Qt::Key_Yes) );

    //Event we are not interested in
    QEvent event(QEvent::Show);
    QVERIFY( !mMainWindow->eventFilter(this, &event) );
    QVERIFY( !mMainWindow->mLocaleChanged );
    QVERIFY( !mMainWindow->mKeyCapture->mKeys.contains(Qt::Key_Yes) );
   
    //Coming foreground, locale not changed
    QEvent eventFg(QEvent::ApplicationActivate);
    QVERIFY( !mMainWindow->eventFilter(this, &eventFg) );
    QVERIFY( !mMainWindow->mLocaleChanged );
    QVERIFY( mMainWindow->mKeyCapture->mKeys.contains(Qt::Key_Yes) );
    
    //LocaleChange event on FG
    mMainWindow->mForeground = true;
    QEvent eventLocale(QEvent::LocaleChange);
    QVERIFY( !mMainWindow->eventFilter(this, &eventLocale) );
    QVERIFY( !mMainWindow->mLocaleChanged );
    QVERIFY( spy.count() == 1 );
    QVERIFY( mMainWindow->mKeyCapture->mKeys.contains(Qt::Key_Yes) );
    spy.clear();
    
    //Going background partially, surface check is not reliable at the moment so 
    // don't care about that, just handle as full foreground loosing
    HbStubHelper::createWindowSurface();
    QEvent eventBg(QEvent::ApplicationDeactivate);
    QVERIFY( !mMainWindow->eventFilter(this, &eventBg) );
    QVERIFY( !mMainWindow->mLocaleChanged );
    QVERIFY( !mMainWindow->mKeyCapture->mKeys.contains(Qt::Key_Yes) );
    
    //Going background fully
    HbStubHelper::reset();
    QVERIFY( !mMainWindow->eventFilter(this, &eventBg) );
    QVERIFY( !mMainWindow->mLocaleChanged );
    QVERIFY( !mMainWindow->mKeyCapture->mKeys.contains(Qt::Key_Yes) );
        
    //LocaleChange event on BG
    mMainWindow->mForeground = false;
    QVERIFY( !mMainWindow->eventFilter(this, &eventLocale) );
    QVERIFY( mMainWindow->mLocaleChanged );
    QVERIFY( !mMainWindow->mKeyCapture->mKeys.contains(Qt::Key_Yes) );
    QVERIFY( spy.count() == 0 );
}

void UT_LogsMainWindow::testObscuredCalled()
{
    QSignalSpy bgSpy( mMainWindow, SIGNAL(appLostForeground()) );
    mMainWindow->mForeground = true;
    mMainWindow->obscuredCalled();
    QVERIFY( !mMainWindow->isForeground() );
    QVERIFY( bgSpy.count() == 1 );
}

void UT_LogsMainWindow::testRevealedCalled()
{
    QSignalSpy localeSpy( mMainWindow, SIGNAL(localeChanged()) );
    QSignalSpy fgSpy( mMainWindow, SIGNAL(appGainedForeground()) );
    
    mMainWindow->mForeground = false;
    mMainWindow->revealedCalled();
    QVERIFY( mMainWindow->isForeground() );
    QVERIFY( !mMainWindow->mLocaleChanged );
    QVERIFY( localeSpy.count() == 0 );
    QVERIFY( fgSpy.count() == 1 );
    
    //Coming foreground after locale change event
    localeSpy.clear();
    fgSpy.clear();
    mMainWindow->mLocaleChanged = true;
    mMainWindow->revealedCalled();
    QVERIFY( mMainWindow->isForeground() );
    QVERIFY( !mMainWindow->mLocaleChanged );
    QVERIFY( localeSpy.count() == 1 );
    QVERIFY( fgSpy.count() == 1 );
}
