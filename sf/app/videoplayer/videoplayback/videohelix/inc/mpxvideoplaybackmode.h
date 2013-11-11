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
* Description:   This class plays files with the Helix Engine
*
*/


// Version : %version: 21 %


#ifndef _CMPXVIDEOPLAYBACKMODE_H_
#define _CMPXVIDEOPLAYBACKMODE_H_

//
//  INCLUDES
//
#include <e32def.h>
#include <e32base.h>

#include "mpxhelixplaybackplugindefs.h"
#ifdef USE_S60_DOWNLOAD_MANAGER
#include "mpxvideodlmgrif.h"
#endif
#include "mpxvideo_debug.h"

//
//  FORWARD DECLARATIONS
//
class CMPXVideoPlaybackController;
class CMPXVideoDlMgrIf;
class CMPXVideoPosterFrameSetter;


//
//  CLASS DECLARATION
//

/*
 *  CMpxVideoPlaybackMode class
 */
NONSHARABLE_CLASS( CMPXVideoPlaybackMode ) : public CBase
{
    public:
        //
        //  Constructors and destructor
        //

        /*
         *  Destructor
         *  Destroy the object and release all memory objects
         */
        ~CMPXVideoPlaybackMode();

        inline virtual TInt GetMode();
        virtual void ConnectToDownloadL( CMPXCommand& aCmd );
        virtual void HandleOpenComplete();
        virtual void GetPdlStatusL( CMPXCommand& aCmd );
        virtual void UpdateSeekPosition( TInt64& aPosition );
        inline virtual TBool IsDownloadPaused();
        virtual TInt HandlePause();
        virtual void HandleBackground();
        virtual TBool IsNetworkMode2GL();
        virtual TBool CanPlayNow();
        virtual void OpenFileL( const TDesC& aMediaFile );
        virtual void OpenFileL( const RFile& aMediaFile );
        virtual void HandleSetPosterFrame();
        virtual void HandleFrameReady(TInt aError);
        virtual TBool SendErrorToView( TInt aError );
        virtual void HandlePauseToPlayTransitionL();

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        virtual void OpenFile64L( const RFile64& aMediaFile );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    protected:
        /*
         *  C++ default constructor
         */
        CMPXVideoPlaybackMode();

        /*
         *  the second phase constructor ConstructL to safely construct things
         *  that can leave
         */
        virtual void ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );

    protected:
        //
        //  Data
        //
        CMPXVideoPlaybackController*        iVideoPlaybackCtlr;   // not owned
        CMPXVideoPosterFrameSetter*         iPosterFrameSetter;
};

/*******************************************************/
/*         SUB CLASS DECLARATIONS                      */
/*******************************************************/

////////////////////////////////////////////////////////
///     Declaration of CMPXLocalPlaybackMode
////////////////////////////////////////////////////////
class CMPXLocalPlaybackMode : public CMPXVideoPlaybackMode
{
    public:
        static CMPXVideoPlaybackMode* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXLocalPlaybackMode();

        // Methods where video plabkack behavior varies for local media
        inline virtual TInt GetMode();
        virtual void HandleSetPosterFrame();
        virtual void HandleFrameReady(TInt aError);
};


////////////////////////////////////////////////////////
///     Declaration of CMPXStreamingPlaybackMode
////////////////////////////////////////////////////////
class CMPXStreamingPlaybackMode : public CMPXVideoPlaybackMode
{
    public:
        static CMPXVideoPlaybackMode* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXStreamingPlaybackMode();

        // Methods where video plabkack behavior varies for streaming media
        inline virtual TInt GetMode();
        virtual TBool CanPlayNow();
        virtual void HandleOpenComplete();
        void OpenFileL( const TDesC& aMediaFile );
        virtual TInt HandlePause();
};

////////////////////////////////////////////////////////////
///     Declaration of CMPXLiveStreamingPlaybackMode
///
///     live streaming is a specialized streaming case
////////////////////////////////////////////////////////////
class CMPXLiveStreamingPlaybackMode : public CMPXStreamingPlaybackMode
{
    public:
        static CMPXVideoPlaybackMode* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXLiveStreamingPlaybackMode();

        // Methods where video plabkack behavior varies for live streaming media
        inline virtual TInt GetMode();
        virtual TInt HandlePause();
        virtual TBool SendErrorToView( TInt aError );
        virtual void HandlePauseToPlayTransitionL();
};

////////////////////////////////////////////////////////////////////////////////
//
//     Declaration of CMPXProgressiveDLPlaybackMode
//
//     Progressive Download is a specialized local playback case
//
////////////////////////////////////////////////////////////////////////////////
class CMPXProgressiveDLPlaybackMode : public CMPXLocalPlaybackMode
{
    public:
        static CMPXVideoPlaybackMode* NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );
        virtual ~CMPXProgressiveDLPlaybackMode();

#ifdef USE_S60_DOWNLOAD_MANAGER

        inline virtual TInt GetMode();
        void ConnectToDownloadL( CMPXCommand& aCmd );
        void HandleOpenComplete();
        void GetPdlStatusL( CMPXCommand& aCmd );
        void UpdateSeekPosition( TInt64& aPosition );
        inline TBool IsDownloadPaused();
        void OpenFileL( const RFile& aMediaFile );

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        void OpenFile64L( const RFile64& aMediaFile );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

#endif // USE_S60_DOWNLOAD_MANAGER

    private:
        void ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );

#ifdef USE_S60_DOWNLOAD_MANAGER
    private:
        CMPXVideoDlMgrIf*  iDlMgrIf;   // owned
#endif // USE_S60_DOWNLOAD_MANAGER

};

#ifdef USE_S60_DOWNLOAD_MANAGER

inline
TInt CMPXProgressiveDLPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXProgressiveDLPlaybackMode::GetMode()"));
    return EMPXVideoProgressiveDownload;
}

inline
TBool CMPXProgressiveDLPlaybackMode::IsDownloadPaused()
{
    return iDlMgrIf->IsDownloadPaused();
}

#endif // USE_S60_DOWNLOAD_MANAGER

// INLINE METHODS

inline
TInt CMPXVideoPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::GetMode()"));
    // sub-classes to return their respective value
    return EMPXNumberOfModes;
}

inline
TInt CMPXLocalPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXLocalPlaybackMode::GetMode()"));
    return EMPXVideoLocal;
}

inline
TInt CMPXStreamingPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXStreamingPlaybackMode::GetMode()"));
    return EMPXVideoStreaming;
}

inline
TInt CMPXLiveStreamingPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXLiveStreamingPlaybackMode::GetMode()"));
    return EMPXVideoLiveStreaming;
}

inline
TBool CMPXVideoPlaybackMode::IsDownloadPaused()
{
    return EFalse;
}


#endif  //_CMPXVIDEOPLAYBACKMODE_H_
