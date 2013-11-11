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
* Description:
*
*/
#ifndef IRPLAYCONTROLLER_H
#define IRPLAYCONTROLLER_H

#include <QObject>
#include <hbglobal.h>

#include "irqenums.h"

class IRApplication;
class IRQMediaPlayer;
class IRQPreset;
class IRQMetaData; 
class IRQStatisticsReporter;
class HbMessageBox;
class IRDBWrapper;
class channelHistoryWrapper;
class urlInfoWrapper;

/**
 * This class is a wrapper for IRQMediaPlayer.
 */

class IRPlayController : public QObject
{
    Q_OBJECT

    
public slots:    
    // slots connected with own functions
    void cancelBuffering();
    
public:
    explicit IRPlayController(IRApplication* aApplication);
    ~IRPlayController();

#ifdef HS_WIDGET_ENABLED
    // MUST be called before prepare the network for playback
    // i.e., called before IRApplication::verifyNetworkConnectivity
    void setConnectingStationName(const QString &aStationName, bool aForceConnecting = false); 
    QString getConnectingStationName() const; 
	   
    // used when nowplaying view is the starting view, to reload preset to the player
    // because by default, the player will load the last played station preset into it
    // But when nowplaying view as starting view due to open pls with only one url, 
    // should reload the preset from pls file
    void reloadNowplayingPreset(IRQPreset *aPreset, bool aIsLogoAvailable, IRQConnectedFrom aConnectedFrom);
#endif    
    // Play a station
    void connectToChannel(IRQPreset *aPreset, IRQConnectedFrom aConnectedFrom);

    void emitStationLogoUpdated(bool aIsLogoAvailable);	
    bool isStationLogoAvailable() const;
    
    // Play control methods
    void resume();
    void stop(IRQTerminatedType aStopReason);
    int getVolume() const;
    void setVolume(int aVolume);

    enum EPlayState
    {
        //initial state, never ever played station
        EIdle = 0,
        //a station has been stopped
        EStopped,
#ifdef HS_WIDGET_ENABLED		
        //connecting network
        EConnecting,
#endif		
        //stream buffering
        EBuffering,		
        //playing
        EPlaying
    };
    EPlayState state() const;
    
    // Check the playing status
    bool isPlaying() const;

    // Check the stopped status
    bool isStopped() const;

    // Check the idle status
    bool isIdle() const;
    
    // Get the preset of now playing
    IRQPreset * getNowPlayingPreset() const;
    
    QString getNowPlayingUrl() const;
    int getNowPlayingBitRate() const;

    // Get the meta data
    IRQMetaData * getMetaData() const;
    
    IRQTerminatedType getStopReason() const;

signals:
    // emitted when meta data is available
    void metaDataAvailable(IRQMetaData*);

    // emitted when playing started
    void playingStarted();

    // emitted when playing stopped
    void playingStopped();

#ifdef HS_WIDGET_ENABLED
    // emitted when loading(connecting) started
    void connectingStarted(const QString &aStationName);
    
    // emitted when buffering started
    void bufferingStarted(const QString &aStationName);    
	    
    // emitted when connecting is cancelled
    void connectingCancelled(const QString &aStationName);

    // emitted when buffering is cancelled
    void bufferingCancelled(const QString &aStationName);
	
	void stationLogoUpdated(bool aIsDefaultLogo);
#endif	

private slots:
    // slots connected with IRQMediaPlayer
    void connectionEstablished(int aBitrate);
    void errorOccured(IRQError aError);
    void updateProgress(int aProgress);
    void fetchVolume(int &aVolume);
    void handleMetaDataReceived(IRQMetaData& aIRmetaData);

    // slots connected with own functions  
    void handleError();

private:
#ifdef SUBTITLE_STR_BY_LOCID
    void popupNote(const QString &aNote = hbTrId("txt_irad_info_failed_to_connect"));
#else
    void popupNote(const QString &aNote = hbTrId("Connecting failed"));
#endif
    void connectSignalSlot();
    
    QString getFirstTryUrl(IRQPreset *aPreset);

    // Play next URL if the previous failed to connect.
    bool playNextUrl();

    void doPlay(const QString aUrl);
	
	void startSession();
	void endSession(IRQTerminatedType aStopReason);
	
	void saveSong2DB(const IRQMetaData& aMetaData, const IRQPreset& aPreset);
    void saveStation2DB(IRQPreset *aPreset);
	
private:
    IRApplication *iApplication;
    IRQMediaPlayer *iMediaPlayer;
    
    IRQStatisticsReporter *iStatisticsReporter;
    bool iGetServerResult;
	
#ifdef HS_WIDGET_ENABLED    
    QString   iConnectingStationName; // used for home screen widget
#endif	

    IRQPreset *iNowPlayingPreset;
    IRQPreset *iNowPlayingPresetBackup;
    
    QString iLastPlayedUrl;
    QString iLastPlayedUrlBackup; 
        
    IRQConnectedFrom iConnectedFrom;
    IRQConnectedFrom iConnectedFromBackup;
            
    bool  iStationLogoAvailable;
    bool  iStationLogoAvailableBackup;            
    
    // reference of IRQMediaPlayer meta data        
    IRQMetaData *iMetaData;    

    EPlayState iPlayState;
    
    // If resuming the last played url fails, reset the player to init state
    // i.e., call connectToChannel() once again.
    bool iResuming;

    // variants for play next URL
    // available bitrates in the preset
    QList<int> iAvailableBitrate;

    // the bitrate is now tried.
    int iTryingBitrate;

    // the URLs of a specific bitrate
    QList<QString> *iUrlArray;

    // the bitrate from real station feedback.
    int iRealBitrate;

    // To handle error async
    IRQError iLastError;
    
    IRQTerminatedType iStopReason;
    
    HbMessageBox *iErrorNote;
    
    IRDBWrapper             *iDbWrapper;
    channelHistoryWrapper   *iHistoryWrapper;
    urlInfoWrapper          *iUrlInfoWrapper;        
};

#endif  //IRPLAYCONTROLLER_H
