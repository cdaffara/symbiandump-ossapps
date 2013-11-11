/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Drive monitor event object*
*/





#ifndef VCXMYVIDEOSDRIVEMONITOREVENT_H
#define VCXMYVIDEOSDRIVEMONITOREVENT_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Used for sending events from CVcxMyVideosDriveMonitor to client.
*/
NONSHARABLE_CLASS(TVcxMyVideosDriveMonitorEvent)
    {
    public:
    
        /**
        * Events, used in iEvent member variable.
        */
        enum TEvent
            {
            /**
            * Value used at construction.
            */                
            ENone,
            
            /**
            * Media was removed (typically memory card).
            */                
            EMediaRemoved,

            /**
            * Media was inserted (typically memory card).
            */
            EMediaInserted,
            
            /**
            * Drive was locked.
            */
            EDriveLocked,
            
            /**
            * Drive was unlocked.
            */
            EDriveUnlocked,

            /**
            * Drive was mounted.
            */            
            EDriveMounted,

            /**
            * Drive was dismounted.
            */            
            EDriveDismounted,
                        
            /**
            * Drive volume name was changed.
            */
            EDriveVolumeNameChanged
            };
            
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        TVcxMyVideosDriveMonitorEvent();

        /**
        * Returns event as a descriptor. Used for debugging purposes.
        *
        * @param aEvent Event as enum.
        * @return       Event as a descriptor.
        */
        static const TDesC& EventDes( TEvent aEvent );

    public:
    
        /**
        * Identifies the event, see TEvent for values.
        */
        TInt  iEvent;
        
        /**
        * The drive which caused the event (a = 0, b = 1, ... etc).
        */
        TInt iDrive;

    };

#endif      // VCXMYVIDEOSDRIVEMONITOREVENT_H   
            
// End of File
