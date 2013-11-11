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
*     The source file of drive watcher
*/

#include "fmdrivewatcher.h"

#include "fmdrivewatcherprivate.h"

FmDriveWatcher::FmDriveWatcher( QObject *parent ) : QObject( parent )
{
    d = new FmDriveWatcherPrivate( this );
    connect( d, SIGNAL(driveAddedOrChanged()), this, SLOT( onDriveAddedOrChanged() ));
}

FmDriveWatcher::~FmDriveWatcher()
{
    delete d;
}


void FmDriveWatcher::startWatch()
{
    d->startWatch();
}
void FmDriveWatcher::cancelWatch()
{
    d->cancelWatch();
}

void FmDriveWatcher::onDriveAddedOrChanged()
{
    emit driveAddedOrChanged();
}
