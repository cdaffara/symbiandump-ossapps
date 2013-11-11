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

#include "cntmainwindow.h"
#include "cntdefaultviewmanager.h"
#include "cntviewnavigator.h"
#include "cntabstractviewfactory.h"
#include "cntkeygrabber.h"
#include <cntdebug.h>
#include <cntabstractengine.h>


CntMainWindow::CntMainWindow(QWidget *parent, int defaultView)
    : HbMainWindow(parent),
    mViewManager( NULL ),
    mDefaultView( defaultView )
{
    CNT_ENTRY
    CntKeyGrabber *keyGrabber = new CntKeyGrabber(this, this);
    
    if (defaultView != noView)
    {
        CntViewNavigator* navigator = new CntViewNavigator(this);
        navigator->addException( favoritesMemberView, collectionView );
        navigator->addEffect( groupMemberView, groupActionsView );
        navigator->addEffect( groupActionsView, groupMemberView );
        navigator->addEffect( contactCardView, historyView );
        navigator->addEffect( historyView, contactCardView );
        navigator->addRoot( namesView );
        navigator->addRoot( collectionView );
        navigator->addRoot( groupMemberView );
        navigator->addRoot( favoritesMemberView );
                
        mViewManager = new CntDefaultViewManager( this );
        mViewManager->setViewNavigator( navigator );
        mViewManager->setViewFactory( new CntDefaultViewFactory( mViewManager->engine().extensionManager()) );
        
        //activate the view
        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, defaultView);
        mViewManager->changeView( viewParameters );
    }

    CNT_EXIT
}

CntMainWindow::~CntMainWindow()
{
    CNT_ENTRY

    delete mViewManager;
    mViewManager = NULL;
    
    CNT_EXIT
}

// end of file
