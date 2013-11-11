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
* Description: 
*
*/




// INCLUDE FILES
#include "vcxmyvideosdrivemonitorevent.h"

#ifdef _DEBUG
_LIT(KIptvDmeMediaRemovedDes, "EMediaRemoved");
_LIT(KIptvDmeMediaInsertedDes, "EMediaInserted");
_LIT(KIptvDmeDriveLockedDes, "EDriveLocked");
_LIT(KIptvDmeDriveUnlockedDes, "EDriveUnlocked");
_LIT(KIptvDmeDriveMountedDes, "EDriveMounted");
_LIT(KIptvDmeDriveDismountedDes, "EDriveDismounted");
_LIT(KIptvDmeDriveVolumeNameChangedDes, "EDriveVolumeNameChanged");
_LIT(KIptvDmeUnknownDes, "Unknown");
#endif

// ============================ MEMBER FUNCTIONS ===============================

TVcxMyVideosDriveMonitorEvent::TVcxMyVideosDriveMonitorEvent()
    : iEvent( ENone )
    , iDrive( -1 )
    {
    }

// -----------------------------------------------------------------------------
// TVcxMyVideosDriveMonitorEvent::EventDes()
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
const TDesC& TVcxMyVideosDriveMonitorEvent::EventDes( TVcxMyVideosDriveMonitorEvent::TEvent aEvent )
    {
    switch ( aEvent )
        {
        case EMediaRemoved:
            return KIptvDmeMediaRemovedDes;
            
        case EMediaInserted:
            return KIptvDmeMediaInsertedDes;

        case EDriveLocked:
            return KIptvDmeDriveLockedDes;

        case EDriveUnlocked:
            return KIptvDmeDriveUnlockedDes;

        case EDriveMounted:
            return KIptvDmeDriveMountedDes;

        case EDriveDismounted:
            return KIptvDmeDriveDismountedDes;
        
        case EDriveVolumeNameChanged:
            return KIptvDmeDriveVolumeNameChangedDes;

        default:
            return KIptvDmeUnknownDes;
        }
    }
    
#else

const TDesC& TVcxMyVideosDriveMonitorEvent::EventDes( TVcxMyVideosDriveMonitorEvent::TEvent /*aEvent*/ )
    {
    return KNullDesC;
    }

#endif

