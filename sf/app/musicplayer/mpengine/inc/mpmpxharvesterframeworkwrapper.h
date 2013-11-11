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
* Description: Wrapper for mpx harvester framework utilities.
*
*/

#ifndef MPMPXHARVESTERFRAMEWORKWRAPPER_H
#define MPMPXHARVESTERFRAMEWORKWRAPPER_H

#include <QObject>
#include "mpmpxcommondefs.h"
#include "mpcommondefs.h"

class MpMpxHarvesterFrameworkWrapperPrivate;

class MpMpxHarvesterFrameworkWrapper : public QObject
{
    Q_OBJECT

    friend class MpMpxHarvesterFrameworkWrapperPrivate;

public:

    explicit MpMpxHarvesterFrameworkWrapper( quint32 clientSecureId=MpCommon::KMusicPlayerUid, QObject *parent=0 );
    virtual ~MpMpxHarvesterFrameworkWrapper();

    void scan();
    void cancelScan();
    void checkForSystemEvents();

signals:

    void scanStarted();
    void scanEnded( int count, int error );
    void scanCountChanged( int count );

    void diskEvent( MpxDiskEvents event );
    void usbEvent( MpxUsbEvents event );

private:

    Q_DISABLE_COPY( MpMpxHarvesterFrameworkWrapper )
    MpMpxHarvesterFrameworkWrapperPrivate *d_ptr;

};

#endif // MPMPXHARVESTERFRAMEWORKWRAPPER_H
