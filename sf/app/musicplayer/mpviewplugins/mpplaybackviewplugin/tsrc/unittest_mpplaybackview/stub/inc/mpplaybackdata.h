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
* Description: mpplaybackdata stub for testing mpplaybackview
*
*/

#ifndef MPPLAYBACKDATA_H
#define MPPLAYBACKDATA_H

//includes
#include <QObject>
#include <QMetaType>


class MpPlaybackData : public QObject
{
    Q_OBJECT

public:

const QString& uri() const;



    enum SimplifiedState {
        NotPlaying,
        Playing,
        Paused,
        Stopped
    };
    // Test utility functions
    static int getUpdateCounter();
    static void resetUpdateCounter();
    void triggerStateUpdate(SimplifiedState state);
    void triggerLabelsChanged(const QString &title, const QString &artist);
    void setPlaybackState(MpPlaybackData::SimplifiedState state);
    void setUri( QString uri );

    // Stub functions
    explicit MpPlaybackData();
    virtual ~MpPlaybackData();


signals:
    void playbackStateChanged();
    void playbackInfoChanged();

public:
    const QString& title() const;
    const QString& artist() const;
    MpPlaybackData::SimplifiedState playbackState() const;

public:
    QString                  mArtist;
    QString                  mTitle;
    SimplifiedState          mPlaybackState;
    QString                  mUri;

};

#endif  // MPPLAYBACKDATA_H
