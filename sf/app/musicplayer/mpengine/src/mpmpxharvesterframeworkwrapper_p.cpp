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
* Description: Wrapper for mpx harvester framework utilities - private implementation.
*
*/

#include <QStringList>

#include <s32strm.h>
#include <s32mem.h>
#include <apgcli.h>
#include <apacmdln.h>

#include <mpxcollectionutility.h>
#include <mpxharvesterutility.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxlog.h>

#include "mpmpxharvesterframeworkwrapper.h"
#include "mpmpxharvesterframeworkwrapper_p.h"
#include "mptrace.h"

/*!
    \class MpMpxHarvesterFrameworkWrapperPrivate
    \brief Wrapper for mpx harvester framework utilities - private implementation.

    This is a private implementation of the mpx harvester framework wrapper utilties interface.
*/

const int NoPreviousBroadCastMsg = -1;

/*!
 \internal
 */
MpMpxHarvesterFrameworkWrapperPrivate::MpMpxHarvesterFrameworkWrapperPrivate( MpMpxHarvesterFrameworkWrapper *wrapper )
    : q_ptr(wrapper),
      iCollectionUtility(0),
      iHarvesterUtility(0),
      iNumItemsAdded(0),
      iScanning( EFalse ),
      iPreviousBroadCastMsg( NoPreviousBroadCastMsg )
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxHarvesterFrameworkWrapperPrivate::~MpMpxHarvesterFrameworkWrapperPrivate()
{
    TX_ENTRY
    if ( iCollectionUtility ) {
        iCollectionUtility->Collection().CancelRequest();
        iCollectionUtility->Close();
    }

    if ( iHarvesterUtility ) {
        iHarvesterUtility->Close();
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::init( quint32 clientSecureId )
{
    iHostUid = TUid::Uid( clientSecureId );
    TRAPD( err, DoInitL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::scan()
{
    TRAPD( err, DoScanL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::cancelScan()
{
    TRAPD( err, DoCancelScanL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::checkForSystemEvents()
{
    TRAPD( err, DoCheckForSystemEventsL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::HandleOpenL(
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool aComplete,
    TInt aError )
{
    Q_UNUSED( aIndex );
    Q_UNUSED( aComplete );
    Q_UNUSED( aError );
    Q_UNUSED( aEntries );
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::HandleOpenL(
    const CMPXCollectionPlaylist& aPlaylist,
    TInt aError )
{
    Q_UNUSED( aPlaylist );
    Q_UNUSED( aError );
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
{
    Q_UNUSED( aMedia );
    Q_UNUSED( aError );
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::HandleCollectionMessage(
    CMPXMessage* aMsg,
    TInt aErr )
{
    TX_ENTRY_ARGS( "aErr=" << aErr );
    if ( aErr == KErrNone && aMsg ) {
        TRAP_IGNORE( DoHandleCollectionMessageL(*aMsg) );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::DoInitL()
{
    TX_ENTRY
    iCollectionUtility = MMPXCollectionUtility::NewL( this, iHostUid );
    iHarvesterUtility = CMPXHarvesterFactory::NewL();
    iHarvesterUtility->CheckForSystemEventsL();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::DoScanL()
{
    TX_ENTRY
    iNumItemsAdded = 0;
    iHarvesterUtility->ScanL();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::DoCancelScanL()
{
    TX_ENTRY
    // If harvester crashes, restart it.
    TRAPD( err, iHarvesterUtility->CancelScanL() );
    if ( err != KErrNone ) {
        iHarvesterUtility->Close();
        iHarvesterUtility = NULL;
        iHarvesterUtility = CMPXHarvesterFactory::NewL();
        iHarvesterUtility->CheckForSystemEventsL();
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxHarvesterFrameworkWrapperPrivate::DoHandleCollectionMessageL(
    const CMPXMessage& aMsg )
{
    TX_ENTRY
    TMPXMessageId id( aMsg.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) {
        TInt event( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        TX_LOG_ARGS( "event=" << event << ", type=" << type << ", data=" << data );

        if ( event == TMPXCollectionMessage::EBroadcastEvent &&
            BroadcastEventFilter( type ) ) {
            switch ( type ) {
                case EMcMsgFormatStart:
                    emit q_ptr->diskEvent(DiskFormatStarted);
                    break;
                case EMcMsgFormatEnd:
                    emit q_ptr->diskEvent(DiskFormatEnded);
                    break;
                case EMcMsgDiskRemoved:
                    emit q_ptr->diskEvent(DiskRemoved);
                    break;
                case EMcMsgDiskInserted:
                    emit q_ptr->diskEvent(DiskInserted);
                    break;
                case EMcMsgUSBMassStorageStart:
                    emit q_ptr->usbEvent(UsbMassStorageStarted);
                    break;
                case EMcMsgUSBMassStorageEnd:
                    emit q_ptr->usbEvent(UsbMassStorageEnded);
                    break;
                case EMcMsgUSBMTPStart:
                    emit q_ptr->usbEvent(UsbMtpStarted);
                    break;
                case EMcMsgUSBMTPEnd:
                    emit q_ptr->usbEvent(UsbMtpEnded);
                    break;
                case EMcMsgUSBMTPNotActive:
                    emit q_ptr->usbEvent(UsbMtpNotActive);
                    break;
                case EMcMsgRefreshStart:
                    iScanning = ETrue;
                    emit q_ptr->scanStarted();
                    break;
                case EMcMsgRefreshEnd:
                    iScanning = EFalse;
                    emit q_ptr->scanEnded( iNumItemsAdded, data );
                    break;
                default:
                    break;
            }
        }
    }
    else if( KMPXMessageIdItemChanged == id && iScanning ) {
        if ( aMsg.IsSupported( KMPXMessageArrayContents ) ) {
            // Loop through messages for arrays
            const CMPXMessageArray* messageArray =
                        aMsg.Value<CMPXMessageArray>( KMPXMessageArrayContents );
            User::LeaveIfNull( const_cast<CMPXMessageArray*>( messageArray ) );

            for ( TInt i=0; i<messageArray->Count(); ++i ) {
                  HandleCollectionMessage( messageArray->AtL( i ), KErrNone );
            }
            emit q_ptr->scanCountChanged( iNumItemsAdded );
        }
        else {
            // Single item
            TMPXChangeEventType changeType( aMsg.ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
            TMPXGeneralCategory cat( aMsg.ValueTObjectL<TMPXGeneralCategory>( KMPXMessageMediaGeneralCategory ) );
            if( changeType == EMPXItemInserted &&
                ( cat == EMPXSong || cat == EMPXPlaylist || cat == EMPXPodcast ) ) {
                iNumItemsAdded++;
            }
        }
    }
    TX_EXIT
}

/*!
 \internal
 Returns false if same event has been received more than once
 */
TBool MpMpxHarvesterFrameworkWrapperPrivate::BroadcastEventFilter( TInt aMsg )
{
    TX_ENTRY_ARGS( "New msg=" << aMsg << ", Prev. msg=" << iPreviousBroadCastMsg );
    if ( aMsg != iPreviousBroadCastMsg ) {
        iPreviousBroadCastMsg = aMsg;
        return true;
    }

    return false;    
}

/*!
 \internal
 Request Harvester to check if there are any system events active.
 */
void MpMpxHarvesterFrameworkWrapperPrivate::DoCheckForSystemEventsL()
{
    TX_ENTRY
    iPreviousBroadCastMsg = NoPreviousBroadCastMsg;    
    iHarvesterUtility->CheckForSystemEventsL();
    TX_EXIT
}

