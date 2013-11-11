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
* Description: Music Player engine stub for mpplaybackview.
*
*/

#ifndef MPENGINE_H
#define MPENGINE_H

#include <QObject>
#include <QStringList>


class MpPlaybackData;

class MpEngine : public QObject
{
    Q_OBJECT

  friend class MpEngineFactory;

public:
    enum EngineMode{
        StandAlone,
        Fetch,
        Embedded,
        MediaBrowsing
    };
    
private:
    explicit MpEngine();

public:

    virtual ~MpEngine();

    MpPlaybackData *playbackData();
    void retrieveSongDetails( int index = -1 );
   
    // Equalizer related
    void applyPreset( int preset );
    void disableEqualizer();
    int activePreset();
    QStringList presetNames();

public slots:
    // Playback related
    void stop();
    void setShuffle(bool mode);
    void setRepeat( bool mode);
    void startSeekBackward();
    void startSeekForward();
    void stopSeeking();
    void skipBackward();
    void skipForward();
    void playPause();
    void setPosition( int position );

signals:

    // Equalizer related
    void libraryUpdated();
    void corruptedStop();

private:
    Q_DISABLE_COPY( MpEngine )

public:
    int mActivePreset;
    int mApplyPresetCount;
    int mDisableEquqlizerCount;
    int mRetrieveSongDetailsCount;
    int mStopCount;
    int mStartSeekBackwardCount;
    int mStartSeekForwardCount;
    int mStopSeekingCount;
    int mSkipBackwardCount;
    int mSkipForwardCount;

};

#endif // MPENGINE_H
