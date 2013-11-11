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
* Description: mpplaybackdata stub for testing mpmediawallview
*
*/

#ifndef MPPLAYBACKDATA_H
#define MPPLAYBACKDATA_H

//includes
#include <QObject>
#include <QMetaType>
#include <hbicon.h>

class MpPlaybackData : public QObject
{
    Q_OBJECT

public:

    enum SimplifiedState {
        NotPlaying,
        Playing,
        Paused,
        Stopped
    };


    explicit MpPlaybackData();
    virtual ~MpPlaybackData();

signals:
    void playbackStateChanged();
    void playbackInfoChanged();

public:

    int id();
    void setPlaybackState( const SimplifiedState state );
    MpPlaybackData::SimplifiedState playbackState() const;

public:

    MpPlaybackData::SimplifiedState mPlaybackState;
};

#endif  // MPPLAYBACKDATA_H
