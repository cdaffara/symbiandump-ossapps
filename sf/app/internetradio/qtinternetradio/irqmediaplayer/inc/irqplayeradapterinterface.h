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
#ifndef IRQPLAYERADAPTERINTERFACE_H_
#define IRQPLAYERADAPTERINTERFACE_H_

#include <QObject>
#include "irqenums.h"

class IRQMetaData;

/*
 * IRQPlayerAdapterInterface is the interface to use MMF adapter or PHONON adapter
 */
class IRQPlayerAdapterInterface : public QObject
{
    Q_OBJECT

public:

    /**
     *  Play specific url via certain access point id
     */
    virtual void playStation(const QString &aUrl, int aApId) = 0;

    /**
     *  Stop Playback
     */
    virtual void stop() = 0;

    /**
     *  Set volume to player
     */
    virtual void setVolume(int aVolume) = 0;

    /**
     *  Get current volume from player
     */
    virtual int getVolume() = 0;

    /**
     *  Get audio player instance
     */
    virtual void* getPlayerInstance() = 0;

    /**
     *  Turns on the stereo effect
     */
    virtual void enableStereoEffect() = 0;
    
    /**
     *  Turns off the stereo effect
     */
    virtual void disableStereoEffect() = 0;
    
signals:

    /**
     *  Connection is established
     */
    void connectionEstablished(int aBitrate);

    /**
     *  Send errors out
     */
    void errorOccured(IRQError aError);

    /**
     *  Send buffering progress out
     */
    void percentageBuffered(int aPercentage);

    /**
     *  Send meta data out
     */
    void metaDataReceived(IRQMetaData& aIRmetaData);

    /**
     *  Gets the preset volume
     */
    void volumeExpected(int& aVolume);

public:

    enum PlayState
    {
        EStopped     = 0,                    // Initial state; playback stopped
        EOpenning    = 1,                    // Try to open the url through MMF
        EConnecting  = 2,                    // Try to connect to the url
        EBuffering   = 3,                    // Buffer data
        EPlaying     = 4                     // Play the audio stream
    };

    PlayState             iPlayState;        // A simple state machine
};

#endif /* IRQPLAYERADAPTERINTERFACE_H_ */
