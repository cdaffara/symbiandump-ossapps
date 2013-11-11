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
* Description:  This class is the stub for the accessory observer.
 *
*/

// Version : %version: 5 %

//
//  INCLUDE FILES
//
#include "stifutilities.h"
#include "mpxvideoaccessoryobserver_stub.h"
#include "mpxvideoplaybackcontroller.h"


//
// ============================ MEMBER FUNCTIONS ===================================================
//


CMPXVideoAccessoryObserver*
CMPXVideoAccessoryObserver::NewL( CMPXVideoPlaybackController* aController )
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::NewL()"));

    CMPXVideoAccessoryObserver* self = new (ELeave) CMPXVideoAccessoryObserver( aController );

    StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

    if ( utility->iAccObserver )
    {
        utility->iAccObserver->iController = aController;
    }
    else
    {
        utility->iAccObserver = new (ELeave) CMPXVideoAccessoryObserver( aController );
        utility->iAccObserver->ConstructL();
    }

    return utility->iAccObserver;
}


void CMPXVideoAccessoryObserver::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::CMPXVideoAccessoryObserver
// -------------------------------------------------------------------------------------------------
//
CMPXVideoAccessoryObserver::CMPXVideoAccessoryObserver( CMPXVideoPlaybackController* aController )
    : iController( aController )
    , iTvOutConnected( EFalse )
    , iTvOutPlaybackAllowed( ETrue )
{
}

CMPXVideoAccessoryObserver::~CMPXVideoAccessoryObserver()
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::~CMPXVideoAccessoryObserver()"));
}

void CMPXVideoAccessoryObserver::SetTvOutConnected( TBool aConnected )
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::SetTvOutConnected()"),
                   _L("aConnected = %d"), aConnected);

    iTvOutConnected = aConnected;

    if ( ! iTvOutConnected )
    {
        iTvOutPlaybackAllowed = ETrue;
    }
}

void CMPXVideoAccessoryObserver::SetTvOutPlaybackAllowed( TBool aAllowed )
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::SetTvOutPlaybackAllowed()"),
                   _L("aAllowed = %d"), aAllowed);

    iTvOutPlaybackAllowed = aAllowed;
}

void CMPXVideoAccessoryObserver::UpdateTvOutStatusL( TBool aTvOutConnected )
{
    MPX_DEBUG(_L("CMPXVideoAccessoryObserver::UpdateTvOutStatusL()"));

    //
    //  Check if the TV-Out status has changed
    //
    if ( iTvOutConnected != aTvOutConnected )
    {
        iTvOutConnected = aTvOutConnected;

        iController->HandleTvOutEventL( iTvOutConnected );
    }

    MPX_DEBUG(_L("    iTvOutConnected = %d"), iTvOutConnected);
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::IsTvOutConnected
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoAccessoryObserver::IsTvOutConnected()
{
    MPX_DEBUG(_L("CMPXVideoAccessoryObserver::IsTvOutConnected(%d)"), iTvOutConnected);

    return iTvOutConnected;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::IsTvOutPlaybackAllowed
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoAccessoryObserver::IsTvOutPlaybackAllowed()
{
    TBool playAllowed = ( ! iTvOutConnected || iTvOutPlaybackAllowed );
    MPX_DEBUG(_L("CMPXVideoAccessoryObserver::IsTvOutPlaybackAllowed(%d)"), playAllowed);

    return playAllowed;
}

//  End of File
