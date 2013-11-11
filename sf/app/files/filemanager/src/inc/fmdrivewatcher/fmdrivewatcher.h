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
*     The header file of drive watcher
*/

#ifndef FMDRIVEWATCHER_H
#define FMDRIVEWATCHER_H

#include <QObject>

class FmDriveWatcherPrivate;

class FmDriveWatcher : public QObject
{
Q_OBJECT
public:
    FmDriveWatcher( QObject *parent = 0 );
    virtual ~FmDriveWatcher();
    void startWatch();
    void cancelWatch();

signals:
    void driveAddedOrChanged();
private slots:
    void onDriveAddedOrChanged();
private:
    FmDriveWatcherPrivate *d;
};


#endif
