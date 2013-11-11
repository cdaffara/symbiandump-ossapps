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
* Description: Embedded playback helper.
*
*/

#include <mpxplaybackutility.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionplugin.hrh>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>

#include "mpmpxembeddedplaybackhelper.h"
#include "mptrace.h"


/*!
    \class CMpMpxEmbeddedPlaybackHelper
    \brief Helper class to play isolated media, 

    This class is used by mpmpxplaybackframeworkwrapper_p to play isolated content specified by an uri,
    when MusicPlayer is launched in embedded mode.
*/

/*!
 \internal
 Two-phased constructor.
 */
CMpMpxEmbeddedPlaybackHelper* CMpMpxEmbeddedPlaybackHelper::NewL( 
    TUid aHostUid,
    MMPXPlaybackUtility* aPlaybackUtility,
    MMpMpxEmbeddedPlaybackHelperObserver* aObserver )
{
    CMpMpxEmbeddedPlaybackHelper* self =
            new ( ELeave ) CMpMpxEmbeddedPlaybackHelper( aHostUid, aPlaybackUtility, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

/*!
 \internal
 Destructor
 */
CMpMpxEmbeddedPlaybackHelper::~CMpMpxEmbeddedPlaybackHelper()
{
    TX_ENTRY
    if ( iCollectionUtility ) {
        iCollectionUtility->Collection().CancelRequest();
        iCollectionUtility->Close();
    }
    if ( iCollectionUiHelper ) {
        iCollectionUiHelper->Close();
    }
    TX_EXIT
}

/*!
 \internal
  */
void CMpMpxEmbeddedPlaybackHelper::playL( const TDesC& aFilename )
{
    TX_LOG
    iCollectionUiHelper->OpenL( iHostUid, aFilename, this, EMPXCollectionPluginMusic );
}

/*!
 \internal
 */
void CMpMpxEmbeddedPlaybackHelper::HandleOpenL( 
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool aComplete,
    TInt aError )
{
    Q_UNUSED( aEntries );
    Q_UNUSED( aIndex );
    Q_UNUSED( aComplete );
    Q_UNUSED( aError );
}

/*!
 \internal
 */
void CMpMpxEmbeddedPlaybackHelper::HandleOpenL( 
    const CMPXCollectionPlaylist& aPlaylist,
    TInt aError )
{
    TX_ENTRY_ARGS( "aError=" << aError );
    if ( aError == KErrNone ) {
        iPlaybackUtility->InitL( aPlaylist, ETrue );
    }
    else {
        iObserver->HandleEmbeddedPlaybackError( aError );
    }
    TX_EXIT
}

/*!
 \internal
 */
void CMpMpxEmbeddedPlaybackHelper::HandleCollectionMessage( 
    CMPXMessage* aMsg,
    TInt aErr )
{
    TX_ENTRY_ARGS( "aErr=" << aErr );
    TInt error = aErr;
    if ( error == KErrNone && aMsg ) {
        TRAP( error, DoHandleCollectionMessageL( *aMsg ) );
    }

    if ( error != KErrNone ) {
        iObserver->HandleEmbeddedPlaybackError( error );
    }
    TX_EXIT
}

/*!
 \internal
 */
void CMpMpxEmbeddedPlaybackHelper::HandleCollectionMediaL( 
    const CMPXMedia& aMedia,
    TInt aError )
{
    Q_UNUSED( aMedia );
    Q_UNUSED( aError );
}

/*!
 \internal
 */
void CMpMpxEmbeddedPlaybackHelper::HandleEmbeddedOpenL( 
    TInt aErr,
    TMPXGeneralCategory aCategory )
{
    TX_ENTRY_ARGS( "aErr=" << aErr );
    Q_UNUSED( aCategory );
    if ( aErr != KErrNone ) {
        iObserver->HandleEmbeddedPlaybackError( aErr );
    }
}

/*!
 \internal
 c++ Contructor
 */
CMpMpxEmbeddedPlaybackHelper::CMpMpxEmbeddedPlaybackHelper(
        TUid aHostUid,
        MMPXPlaybackUtility* aPlaybackUtility,
        MMpMpxEmbeddedPlaybackHelperObserver* aObserver )
    : iPlaybackUtility( aPlaybackUtility ),
      iObserver( aObserver ),
      iHostUid( aHostUid )
{
}

/*!
 \internal
 Leaving constructor
 */
void CMpMpxEmbeddedPlaybackHelper::ConstructL()
{
    iCollectionUtility = MMPXCollectionUtility::NewL( this, iHostUid );
    iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL( iHostUid );
}

/*!
 \internal
 */
void CMpMpxEmbeddedPlaybackHelper::DoHandleCollectionMessageL( 
    const CMPXMessage& aMsg )
{
    TX_ENTRY
    TMPXMessageId id( aMsg.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) {
        TInt event( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        TX_LOG_ARGS( "event=" << event << ", type=" << type << ", data=" << data );

         if ( event == TMPXCollectionMessage::EPathChanged &&
              type == EMcPathChangedByOpen &&
              data == EMcItemOpened ) {
            // This will result in HandleOpenL with CMPXCollectionPlaylist
            iCollectionUtility->Collection().OpenL();
        }
    }
    TX_EXIT
}

