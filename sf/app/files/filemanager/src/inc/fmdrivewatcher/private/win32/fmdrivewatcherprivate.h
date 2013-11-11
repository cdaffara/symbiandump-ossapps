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
*     The header file of drive event watcher private
*/

#ifndef FMDRIVEWATCHERPRIVATE_H
#define FMDRIVEWATCHERPRIVATE_H

#include <QObject>

class FmDriveWatcher;

class FmDriveWatcherPrivate : public QObject
{
Q_OBJECT
public:
    FmDriveWatcherPrivate( FmDriveWatcher *driveWatcher );
    virtual ~FmDriveWatcherPrivate();

    void startWatch();
    void cancelWatch();

    void onDriveAdded();
    void onDriveRemoved();

signals:
    void driveAddedOrChanged();

private:
    FmDriveWatcher *q;
};

#endif
