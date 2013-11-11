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
*     The source file of filemanager view manager
*
*/

#include "testviewmanager.h"
#include "fmoperationservice.h"
#include "fmoperationbase.h"

#include <hbview.h>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbactivitymanager.h>

#include <QFileInfo>

TestViewManager *TestViewManager::mTestViewManager = 0;

TestViewManager::TestViewManager( HbMainWindow* mainWindow ) : mOperationService( 0 )
{
    mMainWindow = mainWindow;
}

TestViewManager::~TestViewManager(void)
{
    delete mOperationService;
    delete mMainWindow;
}

TestViewManager *TestViewManager::CreateTestViewManager( HbMainWindow* mainWindow )
{
    if( !mTestViewManager ){
    mTestViewManager = new TestViewManager( mainWindow );
    }

    return mTestViewManager;
}

TestViewManager *TestViewManager::viewManager()
{
    return mTestViewManager;
}

FmOperationService *TestViewManager::operationService()
{
    if ( !mOperationService ) {
        mOperationService = new FmOperationService( this );
        mOperationService->setObjectName( "operationServicefortest" );
    }    
    return mOperationService;
}

void TestViewManager::RemoveViewManager()
{
    delete mTestViewManager;
    mTestViewManager = 0;
}

