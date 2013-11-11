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
* 
* Description:
*     The source file of drive event watcher private
*/

#include "fmdrivewatcherprivate.h"

#include "fmdrivewatcher.h"
#include "fmcommon.h"

FmDriveWatcherPrivate::FmDriveWatcherPrivate( FmDriveWatcher *driveWatcher ) :
    q( driveWatcher )
{
    fs.Connect();
    mDriveEvent = CFmDriveEvent::NewL( fs, this );
}

FmDriveWatcherPrivate::~FmDriveWatcherPrivate()
{
    fs.Close();
    delete mDriveEvent;
}


void FmDriveWatcherPrivate::startWatch()
{
    mDriveEvent->Setup();
}

void FmDriveWatcherPrivate::cancelWatch()
{
    mDriveEvent->Cancel();
}

void FmDriveWatcherPrivate::OnDriveAddedOrChangedL()
{
    FM_LOG( QString( "FmDriveWatcherPrivate::OnDriveAddedOrChangedL start" ) );
    emit driveAddedOrChanged();
    FM_LOG( QString( "FmDriveWatcherPrivate::OnDriveAddedOrChangedL end" ) );
}
