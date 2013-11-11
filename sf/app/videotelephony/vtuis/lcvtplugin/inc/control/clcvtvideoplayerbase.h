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
* Description:  LiveComm Videotelephony's Video Player Base Class
*
*/

#ifndef LCVTPLAYERS_H
#define LCVTPLAYERS_H

#include <e32base.h>

#include <lcvideoplayer.h>
#include <lcwindow.h>
#include <lcaudiocontrol.h>
#include <lcsourcefilecontrol.h>
#include <lcdestinationfilecontrol.h>
#include "clcvtsession.h"

    
class CLcVtVideoPlayerBase : public CBase, public MLcVideoPlayer, public MLcWindow
    {
    
public: // Constructors and destructor
    
    ~CLcVtVideoPlayerBase();
    
public: // from MLcVideoPlayer
    
    //TLcVideoPlayerState LcVideoPlayerState() const;
    
    //TBool LcIsPlayingL();
    
    void LcPlayL( TLcVideoPlayerSource aSource=ECameraSource );
    
    void LcPauseL();

    MLcWindow* LcWindow();

    MLcCameraControl* LcCameraControl();
    
    MLcSourceFileControl* LcSourceFileControl();
    
    MLcDestinationFileControl* LcDestinationFileControl();
    
    MLcAudioControl* LcAudioControl();

    MLcZoomControl* LcZoomControl();

    MLcBrightnessControl* LcBrightnessControl();


public: // from MLcWindow
    
    void EnableLcWindowL( TBool aEnable );
    
    TBool IsLcWindowEnabled();        
    
    void SetLcWindowRectL( TRect aRect );
    
    TRect LcWindowRect();
    
    void SetLcWindowOrientationL( 
        TLcWindowOrientation aOrientation );
    
    TLcWindowOrientation LcWindowOrientationL();
    
protected: // Constructors
    
    CLcVtVideoPlayerBase(
        CLcVtSession* avtSession,
        MLcAudioControl& aLcAudioControl );
    
protected: // Data        
    
    CLcVtSession* ivtSession; // Not owned
    
    MLcAudioControl& iLcAudioControl;

    TBool iIsWindowEnabled;
    
    TRect iWindowRect;
    
    MLcWindow::TLcWindowOrientation iWindowOrientation;
    };
   

#endif // LCVTPLAYERS_H

// end of file
