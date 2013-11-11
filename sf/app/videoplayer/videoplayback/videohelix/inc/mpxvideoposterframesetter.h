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
* Description:   Poster Frame Setter
 *
*/

// Version : %version:  ou1cpsw#2 %



#ifndef _MPXVIDEOPOSTERFRAMESETTER_H
#define _MPXVIDEOPOSTERFRAMESETTER_H

//  INCLUDES
#include <e32base.h>
#include <thumbnailmanager.h>
#include <thumbnailobjectsource.h>
#include <thumbnailmanagerobserver.h>


// FORWARD DECLARATIONS
class CMPXVideoPlaybackController;
class CThumbnailManager;

// CLASS DECLARATION
NONSHARABLE_CLASS(CMPXVideoPosterFrameSetter) : public CBase,
                                                public MThumbnailManagerObserver
{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMPXVideoPosterFrameSetter* NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );
        
        /**
        * Destructor.
        */
        virtual ~CMPXVideoPosterFrameSetter();

    public: 
        
        void RequestPosterFrame();
        
        void HandlePosterFrameReady(TInt aError);
        
        // from MThumbnailManagerObserver
        void ThumbnailPreviewReady( MThumbnailData& aThumbnail,
                                    TThumbnailRequestId aId );
        
        // from MThumbnailManagerObserver
        void ThumbnailReady( TInt aError, 
                             MThumbnailData& aThumbnail,
                             TThumbnailRequestId aId );

        
    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPosterFrameSetter( CMPXVideoPlaybackController* aVideoPlaybackCtlr );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();        

    private:    // Data        

        CMPXVideoPlaybackController*    iVideoPlaybackCtlr;
        TThumbnailRequestId             iRequestId;
        CThumbnailManager*              iTNManager;
};

#endif      // _MPXVIDEOPOSTERFRAMESETTER_H
            
// End of File
