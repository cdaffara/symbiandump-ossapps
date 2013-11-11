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
* Description:  LiveComm Videotelephony's Local Video Player
*
*/
#ifndef LCVTLOCALVIDEOPLAYER_H
#define LCVTLOCALVIDEOPLAYER_H

#include <e32base.h>
#include "clcvtvideoplayerbase.h"


// FORWARD DECLARATIONS
class CLcVtSession;
class TVtCameraHandler;
class MLcWindow;
class MLcAudioControl;

class CLcVtLocalVideoPlayer : public CLcVtVideoPlayerBase
    {
public: // Constructors and destructor
    
    static CLcVtLocalVideoPlayer* NewL( 
        CLcVtSession* avtSession,
        MLcAudioControl& aLcAudioControl,
        TVtCameraHandler& aCameraHandler);
    
    ~CLcVtLocalVideoPlayer();
    
public: // from CLcVtVideoPlayerBase
    
    TLcVideoPlayerState LcVideoPlayerState() const;
    
    TBool LcIsPlayingL();
    
    void LcPlayL( TLcVideoPlayerSource aSource=ECameraSource );
    
    void LcPauseL();

    MLcCameraControl* LcCameraControl();
    
    MLcZoomControl* LcZoomControl();

    MLcBrightnessControl* LcBrightnessControl();
    
    void SetLcWindowRectL( TRect aRect );
    
    void SetLcWindowOrientationL( 
        TLcWindowOrientation aOrientation );

public:
    TBool IsOrientationChanged();
    
private: // Constructors
    
    CLcVtLocalVideoPlayer(
        CLcVtSession* avtSession,
        MLcAudioControl& aLcAudioControl,
        TVtCameraHandler& aCameraHandler);
    
    void ConstructL();
    
private: // Data
    
    TVtCameraHandler& iCameraHandler;
    TBool           iOrientationChanged;
    
    };


#endif //LCVTLOCALVIDEOPLAYER_H
