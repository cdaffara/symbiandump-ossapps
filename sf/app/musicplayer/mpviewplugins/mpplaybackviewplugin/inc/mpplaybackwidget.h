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
* Description: Playback widget for Music Player playback view.
*
*/

#ifndef MPPLAYBACKWIDGET_H
#define MPPLAYBACKWIDGET_H

#include <hbwidget.h>

class MpPlaybackData;
class HbStackedLayout;
class QString;
class HbTextItem;
class HbProgressSlider;
class HbLabel;
class MpPlaybackDocumentLoader;
class MpAlbumCoverWidget;
class MpLightMonitor;

class MpPlaybackWidget : public HbWidget
{
    Q_OBJECT

public:

    MpPlaybackWidget( MpPlaybackData *data, QGraphicsItem *parent=0 );
    virtual ~MpPlaybackWidget();
    void repeatChanged( bool value );

signals:

    void setPlaybackPosition( int value );
    void signalPlaybackInfoChanged();

public slots:

    void playbackInfoChanged();
    void durationChanged();
    void positionChanged();
    void albumArtChanged();
    void handleLcdLightStatus( bool lightOn );
    void handleCollectionPlaylistOpened();
  
private slots:

    void handleSliderPressed();
    void handleSliderReleased();
    void handleSliderMoved(int value);

private:

    QString formatDuration( int seconds );
    void connectPlaybackDataSignals( bool connectSignal );
	void updatePlaybackInfo();

private:

    MpPlaybackData           *mPlaybackData;        // Not own
    MpPlaybackDocumentLoader *mDocumentLoader;      // Own
    MpLightMonitor           *mLightMonitor;        // Own

    HbStackedLayout          *mLayout;              // Not own
    HbLabel                  *mSongTitle;           // Not own
    HbLabel                  *mArtistName;          // Not own
    HbLabel                  *mAlbumName;           // Not own
    MpAlbumCoverWidget       *mAlbumArt;            // Not own
    HbProgressSlider         *mProgressBar;         // Not own
    HbLabel                  *mRealAudioIndicator;  // Not own
    HbLabel                  *mRepeatIndicator;     // Not own
    bool                     mProgreesBarDragging;
    int                      mDuration;

    Q_DISABLE_COPY(MpPlaybackWidget)
};

#endif  // MPPLAYBACKWIDGET_H
