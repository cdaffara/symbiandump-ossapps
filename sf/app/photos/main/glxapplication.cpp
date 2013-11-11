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
* Description:   ?Description
*
*/

//include
#include <qsymbianevent.h>
#include <oommonitorplugin.h>
#include <hbinstance.h>
#include <w32std.h>

//userinclude
#include "glxapplication.h"
#include "mglxcache.h"

GlxApplication::GlxApplication( int &argc, char *argv[ ], Hb::ApplicationFlags flags )
    : HbApplication( argc, argv, flags )
{
}

GlxApplication::GlxApplication( QApplication::QS60MainApplicationFactory factory, int &argc, char *argv[], Hb::ApplicationFlags flags  )
    : HbApplication( factory, argc, argv, flags )
{
}

bool GlxApplication::symbianEventFilter(const QSymbianEvent *aQSEvent)
{
    if ( aQSEvent->type( ) == QSymbianEvent::WindowServerEvent ) {
        const TWsEvent *aEvent = aQSEvent->windowServerEvent();
        if ( aEvent && aEvent->Type() == KAppOomMonitor_FreeRam ) {
            cleanUpCache();
        }
    }
    return HbApplication::symbianEventFilter( aQSEvent );
}

void GlxApplication::initMattiAutomation()
{
    //For Matti Automation 
    //Monitor the first view ready signal to emit the application ready signal.
    HbMainWindow *window = hbInstance->allMainWindows().first();
    connect( window, SIGNAL( viewReady() ), this, SLOT( handleAppReady() ) );    
}

void GlxApplication::handleAppReady()
{
    emit applicationReady();
    HbMainWindow *window = hbInstance->allMainWindows().first();
    disconnect( window, SIGNAL( viewReady() ), this, SLOT( handleAppReady() ) ); 
}

void GlxApplication::cleanUpCache()
{
    qDebug("GlxApplication::cleanUpCache()");
    MGlxCache* cacheManager = MGlxCache::InstanceL();
    cacheManager->ReleaseRAML(true);
    cacheManager->Close();
}
 
