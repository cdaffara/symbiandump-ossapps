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
* Description:  MPX Video base playback view
*
*/

// Version : %version:  6 %



// This file defines the API for .dll

#ifndef __MPXVIDEOVIEWWRAPPER_H__
#define __MPXVIDEOVIEWWRAPPER_H__

//  Include Files

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf


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

    private:
        CMPXVideoViewWrapper( VideoBasePlaybackView* aView );
        void ConstructL();

        void SetFileDetails(TBool aDefault);

    public:
        void HandleCommandL( TInt aCommand );

        TBool IsLive();

        TBool IsPlaylist();

        TBool IsMultiItemPlaylist();

        void RequestMediaL();

        void CreateGeneralPlaybackCommandL( int aCmd );

        void ActivateClosePlayerActiveObject();

        void IssueVideoAppForegroundCmdL( TBool aForeground );

        void UpdateVideoRectDone();

        void SurfacedAttached( TBool aAttached );

        void SetDefaultAspectRatio( int aspectRatio );

    public: // data

        VideoBasePlaybackView*        iView;
        VideoPlaybackViewFileDetails* iFileDetails;
        TBool                         iMediaRequested;
        TBool                         iClosePlayerAO;
        TBool                         iForeground;
        TBool                         iAttatched;
        TInt                          iAspectRatio;
};

#endif  // __MPXVIDEOVIEWWRAPPER_H__

// EOF
