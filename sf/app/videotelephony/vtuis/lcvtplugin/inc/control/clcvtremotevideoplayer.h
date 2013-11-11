/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  LiveComm Videotelephony's Remte Video Player
*
*/
#ifndef LCVTREMOTEVIDEOPLAYER_H
#define LCVTREMOTEVIDEOPLAYER_H

#include <e32base.h>
#include "clcvtvideoplayerbase.h"

class CLcVtRemoteVideoPlayer : public CLcVtVideoPlayerBase
    {
public: // Constructors and destructor
    
    static CLcVtRemoteVideoPlayer* NewL( 
            CLcVtSession* avtSession,
            MLcAudioControl& aLcAudioControl );

    
    ~CLcVtRemoteVideoPlayer();
 
    
public: // from CLcVtVideoPlayerBase
    
    TLcVideoPlayerState LcVideoPlayerState() const;
    
    TBool LcIsPlayingL();    

    void SetLcWindowRectL( TRect aRect );
    
    
private: // Constructors
    
    CLcVtRemoteVideoPlayer( 
            CLcVtSession* avtSession,
            MLcAudioControl& aLcAudioControl );
    
    void ConstructL();
    
private: // Data    
    };

#endif //LCVTREMOTEVIDEOPLAYER_H
