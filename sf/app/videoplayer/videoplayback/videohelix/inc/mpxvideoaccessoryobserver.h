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

// Version : %version: 7 %



#ifndef MPXVIDEOACCESSORYOBSERVER_H
#define MPXVIDEOACCESSORYOBSERVER_H

//
//  INCLUDES
//
#include <e32std.h>
#include <e32base.h>
#include <AccessoryServer.h>
#include <AccessoryConnection.h>

#include "mpxvideo_debug.h"

//
//  FORWARD DECLARATIONS
//
class CMPXVideoPlaybackController;

//
//  CLASS DECLARATION
//

NONSHARABLE_CLASS( CMPXVideoAccessoryObserver ) : public CActive
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

    private:

        /*
         *  From CActive
         *
         *  DoCancel() must not wait for event completion;
         *  this is handled by Cancel().
         */
        void DoCancel();

        /*
         *  From CActive
         *
         *  Handles an active object's request completion event.
         */
        void RunL();

        /*
         *  From CActive
         *
         *  If RunL leaves, then the active scheduler calls RunError()
         *  to handle the leave.
         */
        TInt RunError( TInt aError );

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

        /*
         *  Checks if the TV-Out status has changed by parsing
         *  the Generic ID Array searching for changes with accessories
         *  that support TV-Out
         *  @return   Status Changed
         *      ETrue   TV-Out Status has changed
         *      EFalse  TV-Out Status has not changed
         */
        TBool UpdateTvOutStatusL();

        /*
         *  Starts listening for Accessory events.
         *  @param None
         *  @return None
         */
        void StartMonitoringL();

    private:

        CMPXVideoPlaybackController*        iController;
        TBool                               iTvOutConnected;

#ifdef __ACCESSORY_FW
        RAccessoryServer                    iServer;
        RAccessoryConnection                iConnection;
        TAccPolGenericIDArray               iGenericIdArray;
#endif //__ACCESSORY_FW
};

#endif  //  MPXVIDEOACCESSORYOBSERVER_H

// End of File
