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

#include "logsmainwindow.h"
#include "logslogger.h"
#include <QKeyEvent>
#include <QApplication>
#include <xqserviceutil.h>
#include <xqkeycapture.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMainWindow::LogsMainWindow() 
    : HbMainWindow(), mForeground(false), mLocaleChanged(false)
{
    qApp->installEventFilter(this);
    mKeyCapture = new XQKeyCapture;
    connect( this, SIGNAL(obscured()), this, SLOT(obscuredCalled()) );
    connect( this, SIGNAL(revealed()), this, SLOT(revealedCalled()) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMainWindow::~LogsMainWindow()
{
    stopKeyCapture();
    delete mKeyCapture;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMainWindow::sendAppToBackground()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMainWindow::sendAppToBackground" );
    XQServiceUtil::toBackground(true);
    LOGS_QDEBUG( "logs [UI] <- LogsMainWindow::sendAppToBackground" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMainWindow::bringAppToForeground()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMainWindow::bringAppToForeground" );
    show();
    raise();
    LOGS_QDEBUG( "logs [UI] <- LogsMainWindow::bringAppToForeground" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsMainWindow::isForeground() const
{
    return mForeground;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMainWindow::keyPressEvent( QKeyEvent *event )
{
    LOGS_QDEBUG_2( "LogsMainWindow::keyPressEvent, key", event->key() );
    if ( event->key() == Qt::Key_Call 
         || event->key() == Qt::Key_Yes
         || event->key() == Qt::Key_Enter ) {
        emit callKeyPressed();
        event->accept();
        return;
    }
    HbMainWindow::keyPressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsMainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ApplicationActivate) {
        LOGS_QDEBUG( "logs [UI] -> eventFilter(), QEvent::ApplicationActivate" );
        startKeyCapture();
    } else if (event->type() == QEvent::ApplicationDeactivate ) {
        LOGS_QDEBUG( "logs [UI] -> eventFilter(), QEvent::ApplicationDeactivate" );
        stopKeyCapture();
    } else if (event->type() == QEvent::LocaleChange) {
        if (mForeground) {
            emit localeChanged();
        } else {
            mLocaleChanged = true;
        }
    }
    
    return HbMainWindow::eventFilter(obj,event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMainWindow::obscuredCalled()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMainWindow::obscuredCalled" );
    mForeground = false;
    emit appLostForeground();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMainWindow::revealedCalled()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMainWindow::revealedCalled" );
    mForeground = true;
    if (mLocaleChanged) {
        LOGS_QDEBUG( "logs [UI] -> locale changed when we were on BG" );
        emit localeChanged();
        mLocaleChanged = false;
    }
    emit appGainedForeground();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMainWindow::startKeyCapture()
{
    mKeyCapture->captureKey(Qt::Key_Yes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMainWindow::stopKeyCapture()
{
    if ( mKeyCapture ){
        mKeyCapture->cancelCaptureKey(Qt::Key_Yes);
    }
}
