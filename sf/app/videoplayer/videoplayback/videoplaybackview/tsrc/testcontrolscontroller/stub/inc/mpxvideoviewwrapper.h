/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub MPX Video Wrapper
*
*/

// Version : %version:  5 %



// This file defines the API for .dll

#ifndef __MPXVIDEOVIEWWRAPPER_H__
#define __MPXVIDEOVIEWWRAPPER_H__

//  Include Files

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf
#include <mpxplaybackframeworkdefs.h>

#include "videoplaybackcontrol.hrh"

//  Constants

//  Forward Declarations
class VideoBasePlaybackView;
class VideoPlaybackViewFileDetails;


//  Class Definitions

class CMPXVideoViewWrapper : public CBase
{
    public:
        static CMPXVideoViewWrapper* NewL( VideoBasePlaybackView* aView );
        virtual ~CMPXVideoViewWrapper();

        CMPXVideoViewWrapper( VideoBasePlaybackView* aView );
        void ConstructL();

    public:
        void HandleCommandL( TInt aCommand );

        void SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue );

        void UpdateVideoRect( TInt aX, TInt aY, TInt aWidth, TInt aHeight, TBool transitionEffect );

        TBool IsResumingPlaybackAfterTermination();

        TInt VolumeSteps();

    public: // data

        VideoBasePlaybackView*         mView;
        TBool                            mMediaRequested;
        VideoPlaybackViewFileDetails*    mFileDetails;
        TBool                            mClosePlayerAO;
        TBool                            mForeground;
        int                              mProperty;
        int                              mCommandId;
        int                              mVolumeSteps;
};

#endif  // __MPXVIDEOVIEWWRAPPER_H__

// EOF
