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

#include "lcfmainwindow.h"
#include "logslogger.h"
#include <QKeyEvent>


// -----------------------------------------------------------------------------
// MainWindow::MainWindow
// -----------------------------------------------------------------------------
//
LcfMainWindow::LcfMainWindow() 
    : HbMainWindow()
{

}

// -----------------------------------------------------------------------------
// LcfMainWindow::~LcfMainWindow
// -----------------------------------------------------------------------------
//
LcfMainWindow::~LcfMainWindow()
{

}

// -----------------------------------------------------------------------------
// LcfMainWindow::keyPressEvent
// -----------------------------------------------------------------------------
//
void LcfMainWindow::keyPressEvent( QKeyEvent *event )
{
    LOGS_QDEBUG_2( "LcfMainWindow::keyPressEvent, key", event->key() );
    if ( event->key() == Qt::Key_Call || event->key() == Qt::Key_Yes ) {
        // Handling at window level seems to be only way to avoid other
        // applications to handle call key as well.
        emit callKeyPressed();
        event->accept();
        return;
    }
    HbMainWindow::keyPressEvent(event);
}

