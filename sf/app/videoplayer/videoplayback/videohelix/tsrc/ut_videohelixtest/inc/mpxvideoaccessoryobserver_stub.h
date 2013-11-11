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
* Description:   This class observes the accessory framework.
 *
*/

// Version : %version: 5 %



#ifndef MPXVIDEOACCESSORYOBSERVER_H
#define MPXVIDEOACCESSORYOBSERVER_H

//
//  INCLUDES
//
#include <e32std.h>
#include <e32base.h>

#include "mpxvideo_debug.h"

//
//  FORWARD DECLARATIONS
//
class CMPXVideoPlaybackController;

//
//  CLASS DECLARATION
//

NONSHARABLE_CLASS( CMPXVideoAccessoryObserver )
{
    public:
        /*
         *  Two phase constructor
         *  @param aObs  Reference to an event observer
         */
        static CMPXVideoAccessoryObserver* NewL( CMPXVideoPlaybackController* aController );

        /*
         *  Destructor
         */
        virtual ~CMPXVideoAccessoryObserver();

    public:

        /*
         *  Returns the status of the TV-Out cable connection
         *  @return the cable connection status
         *     ETrue    cable is connected.
         *     EFalse   cable is not connected.
         */
        TBool IsTvOutConnected();

        /*
         *  Returns if the playback is allowed when the TV-Out cable is connected
         *  @return  playback is allowed
         *     ETrue    playback is allowed.
         *     EFalse   playback is not allowed.
         */
        TBool IsTvOutPlaybackAllowed();

        void UpdateTvOutStatusL( TBool aTvOutConnected );
        void SetTvOutConnected( TBool aConnected );
        void SetTvOutPlaybackAllowed( TBool aAllowed );

    private:

        /*
         *  C++ default constructor.
         *  @param aObs  Reference to an event observer
         */
        CMPXVideoAccessoryObserver( CMPXVideoPlaybackController* aController );

        /*
         *  By default Symbian 2nd phase constructor is private.
         *  Create and connect to the driver and Central Repository.
         */
        void ConstructL();

    private:

        CMPXVideoPlaybackController*        iController;
        TBool                               iTvOutConnected;
        TBool                               iTvOutPlaybackAllowed;
};

#endif  //  MPXVIDEOACCESSORYOBSERVER_H

// End of File
