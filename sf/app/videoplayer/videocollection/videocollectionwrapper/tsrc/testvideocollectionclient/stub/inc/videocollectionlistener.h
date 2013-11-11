/**
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
* Description: stub videocollectionListener for testing CVideoCollectionClient
* 
*/

#ifndef __VIDEOCOLLECTIONLISTENER_H
#define __VIDEOCOLLECTIONLISTENER_H

// INCLUDES
#include <QObject>
#include <mpxcollectionobserver.h>
#include <mpxmessage2.h>


// FORWARD DECLARATIONS
class VideoCollectionClient;
class VideoDataSignalReceiver;

// CLASS DECLARATION
class VideoCollectionListener : public QObject, public MMPXCollectionObserver
{
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
   
    /**
     * disable copy-constructor and assignment operator
     */
    Q_DISABLE_COPY(VideoCollectionListener) 
 
public: 
    
    /**
     * Constructor
     */
    VideoCollectionListener(VideoCollectionClient &collectionClient, VideoDataSignalReceiver &signalReceiver);

    /**
     * Destructor.
     */
    virtual ~VideoCollectionListener();
    
    /**
     * 
     */
    void setRequestNewMediaArray(bool /*request*/)
    {
        // NOP at the moment
    }
    
    /**
     * NOP.
     */
    void HandleCollectionMediaL(const CMPXMedia& /*aMedia*/, 
                                           TInt /*aError*/){};
    
    /**
     * NOP.
     */
    void HandleCollectionMessageL(const CMPXMessage& /*aMsg*/) {};

    /**
     * NOP.
     */
    void HandleCollectionMessage(CMPXMessage* /*aMsg*/, TInt /*aErr*/) {}; 

    /**
     * NOP.
     */
    void HandleOpenL(const CMPXMedia& /*aEntries*/,
                                 TInt /*aIndex*/,TBool /*aComplete*/,TInt /*aError*/){};
     
    /**
     * NOP.
     */
    void HandleOpenL(const CMPXCollectionPlaylist& /*aPlaylist*/ ,TInt /*aError*/) {};  

    /**
     * NOP.
     */
    void HandleCommandComplete(CMPXCommand* /*aCommandResult*/, 
                                           TInt /*aError*/) {};
    
    
    
};

#endif
