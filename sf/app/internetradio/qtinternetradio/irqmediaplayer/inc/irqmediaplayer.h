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
#ifndef IRQMEDIAPLAYER_H_
#define IRQMEDIAPLAYER_H_

#include <QObject>
#include "irqenums.h"
#include "irqmediaplayerexporter.h"

class IRQPlayerAdapterInterface;
class IRQMetaData;

/**
 * This class provides the interface to IR Media Player component
 */

class IRQMEDIAPLAYER_DLL_EXPORT IRQMediaPlayer : public QObject
{
    Q_OBJECT

public:

    /**
     *  Constructor
     */
    IRQMediaPlayer();

    /**
     *  Destructor
     */
    ~IRQMediaPlayer();

    /**
     *  Returns the current volume of playback, it's a percentage
     *  @return int
     */
    int  getVolume();

public slots:

    /**
     *  Plays a radio station
     */
    void playStation(const QString &aUrl, int aApId);

    /**
     *  Stops playback
     */
    void stop();

    /**
     *  Sets the volume to player, it's a percentage
     */
    void setVolume(int aVolume);

    /**
     *  Turns on the stereo effect
     */
    void enableStereoEffect();

    /**
     *  Turns off the stereo effect
     */
    void disableStereoEffect();

signals:

    /**
     *  Notifies the successful connection with a radio station
     */
    void connectionEstablished(int aBitrate);

    /**
     *  Notifies the error
     */
    void errorOccured(IRQError aError);

    /**
     *  Notifies the buffering status
     */
    void percentageBuffered(int aPercentage);

    /**
     *  Notifies the meta data
     */
    void metaDataReceived(IRQMetaData& aIRMetaData);

    /**
     *  Gets the preset volume, it's a percentage
     */
    void volumeExpected(int& aVolume);

private:

    /**
     *  Player adpater interface
     */
    IRQPlayerAdapterInterface* iPlayer;
};

#endif /* IRQMEDIAPLAYER_H_ */
