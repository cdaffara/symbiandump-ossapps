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
* Description:   Seek or trickplay
 *
*/

// Version : %version: 3 %



#ifndef _MPXVIDEOSEEKER_H
#define _MPXVIDEOSEEKER_H

//  INCLUDES
#include <e32base.h>

#include "mpxhelixplaybackplugindefs.h"

// FORWARD DECLARATIONS
class CMPXVideoPlaybackController;

// CLASS DECLARATION
NONSHARABLE_CLASS(CMPXVideoSeeker) : public CBase
{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMPXVideoSeeker* NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );
        
        /**
        * Destructor.
        */
        virtual ~CMPXVideoSeeker();

    public: // New functions
        
        /**
        * Start seeking
        * @param aForward Seeking direction
        * @param aClipLength Clips length
        * @return void
        */
        void StartSeekingL( TBool aForward,
                            TMPXVideoPlaybackState aState,
                            TInt64 aPosition,
                            TInt64 aDuration );

        /**
        * Stops seeking
        * @return void
        */
        void StopSeekingL();
        
    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoSeeker( CMPXVideoPlaybackController* aVideoPlaybackCtlr );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Timer callback for stopping trick play
        * @return KErrNone
        */        
        static TInt StopTrickPlay( TAny* aPtr );

        /**
        * stop trick play after 5 sec
        * @return void
        */        
        void DoStopTrickPlay();        

        /**
        * convert to seeking from trick play
        * @return void
        */  
        void ConvertToSeeking();

        /**
        * Timer callback for updating position info
        * @return KErrNone
        */        
        static TInt UpdatePosition( TAny* aPtr );

        /**
        * Update position info
        * @return void
        */        
        void DoUpdatePosition();

    private:    // Data
        
        TBool                        iForward;
        TMPXVideoPlaybackState       iPreviousState;

        TInt                         iIncrements;
        TInt64                       iPosition;
        TInt64                       iDuration;

        CPeriodic*                   iSeekTimer;
        CPeriodic*                   iTrickPlayTimer;

        CMPXVideoPlaybackController* iVideoPlaybackCtlr;
};

#endif      // _MPXVIDEOSEEKER_H
            
// End of File
