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
 *     Zhiqiang Yang <zhiqiang.yang@nokia.com>
 * 
 * Description:
 *     The source file of File Manager main window
 */

#include "fmmainwindow.h"
#include "fmcommon.h"

#include <hbaction.h>
#include <QTimer>

/*!
    \fn void applicationReady()
    This signal is emitted when application is launched and ready to use
*/


/*!
    constructor
*/
FmMainWindow::FmMainWindow() : mViewManager( 0 ), mFirstViewLoaded( false )
{
    // delayedLoading is not used currently but reserved for future
    // connect(this, SIGNAL(viewReady()), this, SLOT(delayedLoading()));
    init();
}

FmMainWindow::~FmMainWindow()
{
   //save file manager activity to activity manager.
    mViewManager->saveActivity();   
    FmViewManager::RemoveViewManager();
}

/*
    handle orientation change event. not used yet as views can handle such event by themself
*/
void FmMainWindow::onOrientationChanged( Qt::Orientation orientation )
{
	Q_UNUSED( orientation );
}

/*
    init main window
*/
void FmMainWindow::init()
{
    FM_LOG("FmMainWindow::init start");

    mViewManager = FmViewManager::CreateViewManager( this );
    mViewManager->createDriverView();
    connect(this, SIGNAL(aboutToChangeView(HbView *, HbView *)), 
            mViewManager, SLOT(onAboutToChangeView(HbView *, HbView *)));
    connect( this, SIGNAL( orientationChanged( Qt::Orientation ) ),
             this, SLOT( onOrientationChanged( Qt::Orientation ) ) );
    

    connect(&mShowTimer, SIGNAL(timeout()), this, SLOT(onApplicationReady()));
    mShowTimer.start();

    FM_LOG("FmMainWindow::init end");
}


/*
    delayedLoading is not used currently but reserved for future
*/
void FmMainWindow::delayedLoading()
{
    FM_LOG("FmMainWindow::delayedLoading start");
    if( mFirstViewLoaded ) {
        return;
    }
    init();
    mFirstViewLoaded = true;
    FM_LOG("FmMainWindow::delayedLoading end");
}

/*
    Slot to receive timer event after application started and emit applicationReady signal
*/
void FmMainWindow::onApplicationReady()
{
    mShowTimer.stop();
    emit applicationReady();
}
