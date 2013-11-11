/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConVideoParser header file
*
*/


#ifndef SCONVIDEOPARSER_H
#define SCONVIDEOPARSER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <videoplayer.h>
#include <TNEVideoClipInfo.h>
#include "cscontimeout.h"

// FORWARD DECLARATIONS
class CVideoOperationObserver;
class CVideoPlayerUtility;

// CLASS DECLARATION

//============================================================
// Class CSConVideoParser declaration
//============================================================  
NONSHARABLE_CLASS( CSConVideoParser ) : public CBase,
                                        public MVideoPlayerUtilityObserver,
                                        public MTNEVideoClipInfoObserver,
                                        public MTNEVideoClipThumbObserver,
                                        private MTimeOutObserver
{
public: // Constructors and destructor

    ~CSConVideoParser();
    static CSConVideoParser* NewLC();
    
    /**
    * Opens file
    * @return none
    */
    void OpenFileL( const RFs& aFs, const TDesC& aFilename );
    
    /**
    * Get video thumbnail
    * @return video thumbnail in jpeg format,
    *  return KNullDesC8 if thumbain could not found
    */
    const TDesC8& Thumbnail() const;
    
    /**
    * Get video frame rate
    * @return video frame rate frames/sec
    */
    TReal32 VideoFrameRateL() const;
    
    /**
    * Get video frame size
    * @return none
    */
    void VideoFrameSizeL(TSize& aSize) const;
    
    /**
    * Get video format
    * @return video format mimetype (example: video/mp4v-es) 
    */
    const TDesC8& VideoFormatMimeTypeL() const;
    
    /**
    * Get videostream bitrate
    * @return video bitrate bits/sec
    */
    TInt VideoBitRateL() const;
    
    /**
    * Get audiostream bitrate
    * @return audio bitrate bits/sec
    */
    TInt AudioBitRateL() const;
    
    /**
    * Get video duration
    * @return video duration in milliseconds.
    */
    TInt DurationL() const;
    
    /**
    * Get videostream bitrate
    * @return videostream mimetype
    */
    const TDesC& VideoMimeTypeL();
    
    /**
    * Get audiostream mimetype
    * @return audiostream mimetype
    */
    const TDesC& AudioMimeTypeL();
    
protected:  // from base classes
    /** @see MVideoPlayerUtilityObserver::MvpuoOpenComplete */
    void MvpuoOpenComplete(TInt aError);
    
    /** @see MVideoPlayerUtilityObserver::MvpuoPrepareComplete */
    void MvpuoPrepareComplete(TInt aError);
    
    /** @see MVideoPlayerUtilityObserver::MvpuoFrameReady */
    void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
    
    /** @see MVideoPlayerUtilityObserver::MvpuoPlayComplete */
    void MvpuoPlayComplete(TInt aError);
    
    /** @see MVideoPlayerUtilityObserver::MvpuoEvent */
    void MvpuoEvent(const TMMFEvent& aEvent);
    
    /** @see MTNEVideoClipInfoObserver::NotifyVideoClipInfoReady */
    void NotifyVideoClipInfoReady(CTNEVideoClipInfo& aInfo, TInt aError);
    
    void NotifyVideoClipThumbCompleted(CTNEVideoClipInfo& aInfo, 
            TInt aError, 
            CFbsBitmap* aThumb);
    
    /** @see MTimeOutObserver::TimeOut */
    void TimeOut();
    
private:
    
    CSConVideoParser();
    void ConstructL();
    
private: // data
    CVideoPlayerUtility* iVideoUtil;
    CTNEVideoClipInfo*   iVideoClip;
    
    RWsSession          iWsSession;
    CWsScreenDevice*    iScreen;
    RWindow*            iWindow;
    RWindowGroup        iRootWindow;
    
    TRequestStatus*     iCallerStatus;
    CActiveSchedulerWait iWait;
    CSconTimeOut*       iTimeOut;
    
    HBufC8* iThumbnail;
    HBufC*  iVideoMimeType;
    HBufC*  iAudioMimeType;
    
    TBool iVideoUtilReady;
    TBool iVideoClipReady;
    TInt iVideoUtilErr;
    TInt iVideoClipErr;
    TBool iAsyncStopCalled;
};

#endif // SCONVIDEOPARSER_H

