/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

// System includes
#include <xqserviceutil.h>

// User includes
#include "radiocontrolservice.h"
#include "radiouiengine.h"
#include "radioservicedef.h"

// Constants
const int SERVICE_POWEROFF_DELAY = 5000;

/*!
 *
 */
RadioControlService::RadioControlService( RadioUiEngine& engine ) :
    XQServiceProvider( RADIO_SERVICE +"."+ RADIO_CONTROL_SERVICE, &engine ),
    mUiEngine( engine )
{
    publishAll();
}

/*!
 *
 */
RadioControlService::~RadioControlService()
{
}

/*!
 * Public slot
 */
void RadioControlService::command( int commandId )
{
    //TODO: Uncomment when vendor id can be read from the client
//    if ( requestInfo().clientVendorId() != NOKIA_VENDORID ) {
//        return;
//    }

    switch ( commandId )
    {
        case RadioServiceCommand::PowerOn:
            mUiEngine.setPowerOn();
            break;

        case RadioServiceCommand::PowerOff:
            mUiEngine.setPowerOff( SERVICE_POWEROFF_DELAY );
            break;

        case RadioServiceCommand::Previous:
            mUiEngine.skipStation( StationSkip::Previous, 0, TuneReason::SkipFromWidget );
            break;

        case RadioServiceCommand::Next:
            mUiEngine.skipStation( StationSkip::Next, 0, TuneReason::SkipFromWidget );
            break;

        case RadioServiceCommand::PreviousFavorite:
             mUiEngine.skipStation( StationSkip::PreviousFavorite, 0, TuneReason::SkipFromWidget );
             break;

        case RadioServiceCommand::NextFavorite:
             mUiEngine.skipStation( StationSkip::NextFavorite, 0, TuneReason::SkipFromWidget );
             break;

        case RadioServiceCommand::SeekUp:
            mUiEngine.seekStation( Seek::Up );
            break;

        case RadioServiceCommand::SeekDown:
            mUiEngine.seekStation( Seek::Down );
            break;

        case RadioServiceCommand::Foreground:
            XQServiceUtil::toBackground( false );
            break;

        case RadioServiceCommand::Background:
            XQServiceUtil::toBackground( true );
            break;

        case RadioServiceCommand::Mute:
            mUiEngine.setMute( true );
            break;

        case RadioServiceCommand::UnMute:
            mUiEngine.setMute( false );
            break;

        default:
            break;
    }
}
