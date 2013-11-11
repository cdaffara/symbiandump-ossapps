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
* Description:  This class observes the accessory framework.
 *
*/

// Version : %version: 9 %

//
//  INCLUDE FILES
//
#include <e32svr.h>
#include <AccPolSubblockNameArray.h>

#include "mpxvideoaccessoryobserver.h"
#include "mpxvideoplaybackcontroller.h"
#include "mpxvideo_debug.h"

//
// ============================ MEMBER FUNCTIONS ===================================================
//

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::NewL
//   Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoAccessoryObserver*
CMPXVideoAccessoryObserver::NewL( CMPXVideoPlaybackController* aController )
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::NewL()"));

    CMPXVideoAccessoryObserver* self = new (ELeave) CMPXVideoAccessoryObserver( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::ConstructL
//   Symbian 2nd phase constructor can leave
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoAccessoryObserver::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::ConstructL()"));

#ifdef __ACCESSORY_FW
    User::LeaveIfError( iServer.Connect() );
    User::LeaveIfError( iConnection.CreateSubSession( iServer ) );

    //
    //  Get initial accessory status synchronously
    //
    iConnection.GetAccessoryConnectionStatus( iGenericIdArray );
#endif

    //
    //  This will set check if a TV-Out accessory has been connected
    //  and initialize the member variable
    //
    UpdateTvOutStatusL();

    StartMonitoringL();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::CMPXVideoAccessoryObserver
// -------------------------------------------------------------------------------------------------
//
CMPXVideoAccessoryObserver::CMPXVideoAccessoryObserver( CMPXVideoPlaybackController* aController )
    : CActive( CActive::EPriorityStandard )
    , iController( aController )
    , iTvOutConnected( EFalse )
{
    CActiveScheduler::Add( this );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::~CMPXVideoAccessoryObserver
// Destructor, close and release the handles.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoAccessoryObserver::~CMPXVideoAccessoryObserver()
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::~CMPXVideoAccessoryObserver()"));

    Cancel();

#ifdef __ACCESSORY_FW
    iConnection.CloseSubSession();
    iServer.Disconnect();
#endif
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::StartMonitoringL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoAccessoryObserver::StartMonitoringL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::StartMonitoringL()"));

    if ( ! IsActive() )
    {
#ifdef __ACCESSORY_FW
        iConnection.NotifyAccessoryConnectionStatusChanged( iStatus, iGenericIdArray );
#endif

        SetActive();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::UpdateTvOutStatusL
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoAccessoryObserver::UpdateTvOutStatusL()
{
    MPX_DEBUG(_L("CMPXVideoAccessoryObserver::UpdateTvOutStatusL()"));

    TBool statusChanged = EFalse;

    //
    //  Initialize TV-Out value to EFalse before checking if
    //  accessories support TV-Out
    //
    TBool tvOutConnected = EFalse;

#ifdef __ACCESSORY_FW
    CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
    CleanupStack::PushL( nameArray );

    //
    //  Get the number of currently connected accessories
    //  Loop through each connected accessory looking for
    //  the desired capabilities
    //
    const TInt count = iGenericIdArray.Count();

    for ( TInt index = 0 ; index < count ; index++ )
    {
        TAccPolGenericID genId = iGenericIdArray.GetGenericIDL( index );
        
        //
        //  Get all supported capabilities for this connected accessory.
        //
        iConnection.GetSubblockNameArrayL( genId, *nameArray );

        //
        //  Check if this connected accessory supports TV-Out
        //
        if ( nameArray->HasName( KAccVideoOut ) )
        {
            MPX_DEBUG(_L("    TV-Out Capabilities Exist"));

            tvOutConnected = ETrue;                         
        }
    }

    CleanupStack::PopAndDestroy( nameArray );
#endif

    //
    //  Check if the TV-Out status has changed
    //
    if ( iTvOutConnected != tvOutConnected )
    {
        iTvOutConnected = tvOutConnected;
        statusChanged   = ETrue;
    }

    MPX_DEBUG(_L("CMPXVideoAccessoryObserver::UpdateTvOutStatusL  iTvOutConnected = %d"), iTvOutConnected);

    MPX_DEBUG(_L("CMPXVideoAccessoryObserver::UpdateTvOutStatusL() ret = %d"), statusChanged);

    return statusChanged;
}

// -----------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::RunL
// -----------------------------------------------------------------------------
//
void CMPXVideoAccessoryObserver::RunL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::RunL()"),
                   _L("iStatus = %d"), iStatus.Int());

#ifdef __ACCESSORY_FW
    if ( KErrNone == iStatus.Int() )
    {
        //
        //  Check for TV-Out status change
        //  If a changed, issue notice to the observer
        //
        if ( UpdateTvOutStatusL() )
        {
            iController->HandleTvOutEventL( iTvOutConnected );
        }
    }
#endif

    //
    //  Renew request for accessory events
    //
    StartMonitoringL();
}

// -----------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::DoCancel
// -----------------------------------------------------------------------------
//
void CMPXVideoAccessoryObserver::DoCancel()
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::DoCancel()"));

#ifdef __ACCESSORY_FW
    iConnection.CancelNotifyAccessoryConnectionStatusChanged();
#endif
}

// -----------------------------------------------------------------------------
//   CMPXVideoAccessoryObserver::RunError
// -----------------------------------------------------------------------------
//
TInt CMPXVideoAccessoryObserver::RunError( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoAccessoryObserver::RunError()"),
                   _L("aError = %d"), aError);

#ifdef __ACCESSORY_FW
    //
    //  Error occurred, issue new request for accessory status
    //
    iConnection.NotifyAccessoryConnectionStatusChanged( iStatus, iGenericIdArray );
#endif

    return KErrNone;
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

//  End of File
