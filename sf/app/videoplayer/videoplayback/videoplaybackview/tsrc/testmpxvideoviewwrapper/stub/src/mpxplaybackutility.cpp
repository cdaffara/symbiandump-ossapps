/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Factory method to create playback utility object
*
*/

#include <bamdesca.h>
#include <s32mem.h>
#include <e32math.h>
#include <mpxcmn.h>
#include <mpxplaybackobserver.h>
#include <mpxcollectionplaylist.h>
#include <mpxmedia.h>
#include <mpxmessagemonitor.h>
#include <mpxtaskqueue.h>
#include <mpxuser.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectionpath.h>
#include <mpxvideoplaybackdefs.h>

#include"mpxplaybackutility.h"


// ============================== MEMBER FUNCTIONS =================================================

// -------------------------------------------------------------------------------------------------
// Creates the playback utility object if not already created
// -------------------------------------------------------------------------------------------------
//
MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL( const TMPXCategory /*aCategory*/,
                                                    const TUid& /*aModeId*/ )
{
    return CMPXPlaybackUtility::NewL();
}

// -------------------------------------------------------------------------------------------------
// Two phases constructor
// -------------------------------------------------------------------------------------------------
//
CMPXPlaybackUtility* CMPXPlaybackUtility::NewL()
{
    CMPXPlaybackUtility* p = new(ELeave)CMPXPlaybackUtility();
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop(p);
    return p;
}

// -------------------------------------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------------------------------------
//
CMPXPlaybackUtility::~CMPXPlaybackUtility()
{
}

// -------------------------------------------------------------------------------------------------
// C++ constructor
// Create a unique name out of thread ID and this pointer: no other instance of
// this object will have the same name; used to identify this object for
// recieving messages
// -------------------------------------------------------------------------------------------------
//
CMPXPlaybackUtility::CMPXPlaybackUtility()
{
}

// -------------------------------------------------------------------------------------------------
// 2nd construtor
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::ConstructL()
{
    iState = EPbStateInitialised;
}

// -------------------------------------------------------------------------------------------------
// Add a observer
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::AddObserverL( MMPXPlaybackObserver& /*aObs*/ )
{
}

// -------------------------------------------------------------------------------------------------
// Remove a observer
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::RemoveObserverL( MMPXPlaybackObserver& /* aObs */ )
{
}

// -------------------------------------------------------------------------------------------------
// Delete this
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::Close()
{
    delete this;
}

// -------------------------------------------------------------------------------------------------
// Issue player commands
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::CommandL( TMPXPlaybackCommand aCmd )
{
    iCommand = aCmd;
}

// -------------------------------------------------------------------------------------------------
// Issue player commands
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::CommandL( CMPXCommand& aCmd, MMPXPlaybackCallback* /* aCallback */ )
{
    TInt cmdType = aCmd.ValueTObjectL<TInt>( KMPXCommandGeneralId );

    if ( cmdType == KMPXCommandIdPlaybackGeneral )
    {
        iCommand = aCmd.ValueTObjectL<TInt>( KMPXCommandPlaybackGeneralType );
    }
    else
    {
        iCommand = aCmd.ValueTObjectL<TInt>( KMPXMediaVideoPlaybackCommand );
    }
}

// -------------------------------------------------------------------------------------------------
// Current state of player
// -------------------------------------------------------------------------------------------------
//
TMPXPlaybackState CMPXPlaybackUtility::StateL() const
{
    return iState;
}

// -------------------------------------------------------------------------------------------------
// Determine whether there is a song by the state of the engine: if there is,
// its OK to return MMPXMedia, else NULL is returned
// -------------------------------------------------------------------------------------------------
//
MMPXSource* CMPXPlaybackUtility::Source()
{
    return this;
}

// -------------------------------------------------------------------------------------------------
// Set playback property, EPropertyChanged event when complete
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SetL( TMPXPlaybackProperty aProperty, TInt aValue )
{
    iProperty = aProperty;
    iPropertyValue = aValue;
}

// -------------------------------------------------------------------------------------------------
// Send property request
// Result will be called back in HandleProperty
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::PropertyL(
    MMPXPlaybackCallback& /* aCallback */,
    TMPXPlaybackProperty /* aProperty */)
{
}

// -------------------------------------------------------------------------------------------------
// Path to the collection
// -------------------------------------------------------------------------------------------------
//
CMPXCollectionPlaylist* CMPXPlaybackUtility::PlaylistL()
{
    CMPXCollectionPlaylist *playList = CMPXCollectionPlaylist::NewL();

    return playList;
}

// -------------------------------------------------------------------------------------------------
// Request for media properties.
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::MediaL(
    const TArray<TMPXAttribute>& /* aAttrs */,
    MMPXPlaybackCallback& /* aCallback */,
    CMPXAttributeSpecs* /* aSpecs */)
{
}

// -------------------------------------------------------------------------------------------------
// Request for media properties.
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::MediaL(
    const TArray<TMPXAttribute>& /* aAttrs */, MMPXPlaybackCallback& /* aCallback */ )
{
}

// -------------------------------------------------------------------------------------------------
// set primary client
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SetPrimaryClientL()
{
}

// -------------------------------------------------------------------------------------------------
// set primary client
// -------------------------------------------------------------------------------------------------
//
void CMPXPlaybackUtility::CancelRequest()
{
}

// End of file

