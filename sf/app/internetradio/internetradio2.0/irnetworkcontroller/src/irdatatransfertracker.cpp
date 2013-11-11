/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Tracks data transmitted through a network connection.
*
*/


#include <httpstringconstants.h>
#include <mhttpdatasupplier.h>
#include <rhttpheaders.h>

#include "irdebug.h"
#include "irdatatransfertracker.h"

// The threshold in bytes after which notifications are sent to observers.
// 0.1  MB = 104857.6 bytes ( rounded )
const TInt KIRDataTransferTrackerThreshold = 104858;
// 0.01 MB = 1048.57 bytes ( rounded )
const TInt KIRDataTransferTrackerMiniThreshold = 1049;
// The overhead in bytes per one HTTP header.
const TInt KVRBytesPerHeaderOverhead = 2; // "\r\n"
// The overhead in bytes per one header field in a request.
const TInt KVRBytesPerHeaderFieldOverhead = 4; // ": \r\n"
// The overhead in bytes per one header sent.
const TInt KVRBytesPerHeaderSentOverhead = 4; // "  \r\n"
// The overhead in bytes per one header received.
const TInt KVRBytesPerHeaderReceivedOverhead = 3; // " \r\n"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRDataTransferTracker* CIRDataTransferTracker::NewL()
    {
    IRLOG_DEBUG( "CIRDataTransferTracker::NewL - Entering" );
    CIRDataTransferTracker* self = new ( ELeave ) CIRDataTransferTracker;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRDataTransferTracker::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIRDataTransferTracker::CIRDataTransferTracker()
    {
    IRLOG_DEBUG( "CIRDataTransferTracker::CIRDataTransferTracker - Entering" );
    bThresholdCrossed = EFalse;
    IRLOG_DEBUG( "CIRDataTransferTracker::CIRDataTransferTracker - Exiting" );
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::ConstructL() const
    {
    IRLOG_DEBUG( "CIRDataTransferTracker::ConstructL" );
    return;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIRDataTransferTracker::~CIRDataTransferTracker()
    {
        IRLOG_DEBUG( "CIRDataTransferTracker::~CIRDataTransferTracker - Entering" );
        iObserver = NULL;
        IRLOG_DEBUG( "CIRDataTransferTracker::~CIRDataTransferTracker - Exiting" );

    }

// ---------------------------------------------------------------------------
// From class MIRDataTransferTracker.
// Sets the observer.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::SetObserver( MIRDataTransferObserver* aObserver )
    {
    IRLOG_DEBUG( "CIRDataTransferTracker::SetObserver - Entering" );
    iObserver = aObserver;
    IRLOG_DEBUG( "CIRDataTransferTracker::SetObserver - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class MIRDataTransferTracker.
// Returns the current transferred data amounts.
// ---------------------------------------------------------------------------
//
const MIRDataTransferTracker::TIRDataTransferPckg& CIRDataTransferTracker::TransferredData() const
    {
    IRLOG_DEBUG( "CIRDataTransferTracker::TIRDataTransferPckg" );
    return iPckg;
    }

// ---------------------------------------------------------------------------
// From class MIRDataTransferTracker.
// Binds this to track the supplied HTTP session.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::BindL( RHTTPSession aSession, TIRTransferCategory aCategory )
    {
    IRLOG_DEBUG2( "CIRDataTransferTracker::BindL() - aCategory = %d", static_cast<TInt>( aCategory ) );

    aSession.ConnectionInfo().SetPropertyL(
        aSession.StringPool().StringF( HTTP::ESessionId, aSession.GetTable() ), 
            THTTPHdrVal( aCategory ) );
    aSession.FilterCollection().AddFilterL(
        *this, THTTPEvent::EAnyTransactionEvent, MHTTPFilter::EClientFilters, RStringF() );
    IRLOG_DEBUG( "CIRDataTransferTracker::BindL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class MIRDataTransferTracker.
// Handles raw data transfers.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::RawDataTransferredL( TInt aBytesSent, TInt aBytesReceived, 
                                                  TIRTransferCategory aCategory )
    {
    IRLOG_DEBUG( "CIRDataTransferTracker::RawDataTransferredL - Entering" );
    ProcessDataL( aBytesSent, aBytesReceived, aCategory  );
    IRLOG_DEBUG( "CIRDataTransferTracker::RawDataTransferredL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class MHTTPFilter.
// Invoked when HTTP transactions are sent or received.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::MHFRunL( const RHTTPTransaction aTransaction, const THTTPEvent& aEvent )
    {
    IRLOG_DEBUG2( "CIRDataTransferTracker::MHFRunL() - aEvent.iStatus = %d", aEvent.iStatus );

    RHTTPSession session = aTransaction.Session();

    THTTPHdrVal categoryVal;

    if ( session.ConnectionInfo().Property(
        session.StringPool().StringF( HTTP::ESessionId, session.GetTable() ), categoryVal ) )
        {
        TIRTransferCategory category = static_cast<TIRTransferCategory>( categoryVal.Int() );

        switch ( aEvent.iStatus )
            {
            case THTTPEvent::EGotResponseHeaders:
                ParseHeaderSizeL( aTransaction, category );
                break;
            case THTTPEvent::EGotResponseBodyData:
                ParseBodySizeL( aTransaction, category );
                break;
            default:
                break;
            }
        }
    IRLOG_DEBUG( "CIRDataTransferTracker::MHFRunL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Calculates the header field sizes in bytes from the specified headers.
// ---------------------------------------------------------------------------
//
TInt CIRDataTransferTracker::HeaderSize( const RHTTPTransaction aTransaction, 
                                         RHTTPHeaders aHeaders )
    {
    IRLOG_DEBUG( "CIRDataTransferTracker::HeaderSize - Entering" );
    TInt size = KVRBytesPerHeaderOverhead;

    RStringPool stringPool = aTransaction.Session().StringPool();

    THTTPHdrFieldIter it = aHeaders.Fields();
    it.First();

    while ( !it.AtEnd() )
        {
        RStringF key = stringPool.StringF( it() );

        TPtrC8 ptr;
        aHeaders.GetRawField( key, ptr );

        size += key.DesC().Size();
        size += ptr.Size();
        size += KVRBytesPerHeaderFieldOverhead;

        ++it;
        }
    IRLOG_DEBUG( "CIRDataTransferTracker::HeaderSize - Exiting" );
    return size;

    }

// ---------------------------------------------------------------------------
// Parses the header sizes ( both request and response ) of the supplied HTTP
// transaction, logging it under the specified category.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::ParseHeaderSizeL( const RHTTPTransaction aTransaction, 
                                                  TIRTransferCategory aCategory )
    {
    IRLOG_DEBUG2( "CIRDataTransferTracker::ParseHeaderSizeL() - aCategory = %d", static_cast<TInt>( aCategory ) );

    RHTTPSession session = aTransaction.Session();

    // Parse the request size first.

    RHTTPRequest request = aTransaction.Request();
    TInt sent = HeaderSize( aTransaction, request.GetHeaderCollection() );

    // Now that the actual header fields are parsed we have to manually extract the original request.
    // The format of the request is "%S %S %S\r\n", where the first string is the method ( GET or POST ),
    // the second is the full URI of the request, and the third the HTTP protocol version in use.

    RStringF http = session.StringPool().StringF( HTTP::EHttp11, session.GetTable() );

    sent += request.Method().DesC().Size();
    sent += request.URI().UriDes().Size();
    sent += http.DesC().Size();
    sent += KVRBytesPerHeaderSentOverhead;

    // Proceed to parse the response size.

    RHTTPResponse response = aTransaction.Response();
    TInt received = HeaderSize( aTransaction, response.GetHeaderCollection() );

    // Now that the actual header fields of the response are parsed, we have to manually extract the
    // original response. The format of the response is "%S %d %S\r\n", where the first string is the HTTP
    // protocol version, the first integer is the HTTP status code, and the second string is the
    // status text.

    TBuf8<KDefaultRealWidth> responseCode;
    responseCode.AppendNum( static_cast<TInt64>( response.StatusCode() ) );

    received += session.StringPool().StringF( HTTP::EHttp11, session.GetTable() ).DesC().Size();
    received += responseCode.Size(); // Contains the byte for the trailing space as is, so need not add it as overhead below.
    received += response.StatusText().DesC().Size();
    received += KVRBytesPerHeaderReceivedOverhead;

    ProcessDataL( sent, received, aCategory );
    IRLOG_DEBUG( "CIRDataTransferTracker::ParseHeaderSizeL - Exiting" );

    }

// ---------------------------------------------------------------------------
// Parses the body size of the supplied HTTP transaction, logging it under the specified category.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::ParseBodySizeL(const RHTTPTransaction aTransaction, 
                                            TIRTransferCategory aCategory )
    {
    IRLOG_DEBUG2( "CIRDataTransferTracker::ParseBodySizeL() - aCategory = %d", static_cast<TInt>( aCategory ) );

    ASSERT( aTransaction.Response().HasBody() );

    TPtrC8 ptr;
    aTransaction.Response().Body()->GetNextDataPart( ptr );

    ProcessDataL( 0, ptr.Size(), aCategory );
    IRLOG_DEBUG( "CIRDataTransferTracker::ParseBodySizeL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Processes raw data sent and/or received via the connection.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::ProcessDataL( TInt aBytesSent, TInt aBytesReceived, 
                                           TIRTransferCategory aCategory )
    {
    IRLOG_DEBUG4( "CIRDataTransferTracker::ProcessDataL() - aBytesSent = %d, aBytesReceived = %d, aCategory = %d",
        aBytesSent, aBytesReceived, static_cast<TInt>( aCategory ) );

    switch ( aCategory )
        {
        case MIRDataTransferTracker::EIRTransferCategoryIsds:
            iPckg.iBytesSentIsds += aBytesSent;
            iPckg.iBytesReceivedIsds += aBytesReceived;
            break;
        case MIRDataTransferTracker::EIRTransferCategoryAudio:
            iPckg.iBytesSentAudio += aBytesSent;
            iPckg.iBytesReceivedAudio += aBytesReceived;
            break;
        default:
            iPckg.iBytesSentUnknown += aBytesSent;
            iPckg.iBytesReceivedUnknown += aBytesReceived;
            break;
        }

    iPckg.iBytesSentTotal += aBytesSent;
    iPckg.iBytesReceivedTotal += aBytesReceived;

    iBytesSinceLastNotification += aBytesSent + aBytesReceived;

    DispatchNotificationL();
    IRLOG_DEBUG( "CIRDataTransferTracker::ProcessDataL() - Exiting" );
    }

// ---------------------------------------------------------------------------
// Dispatches the notification of the data transfer conditionally.
// ---------------------------------------------------------------------------
//
void CIRDataTransferTracker::DispatchNotificationL()
    {
    IRLOG_DEBUG2( "CIRDataTransferTracker::DispatchNotificationL() - iBytesSinceLastNotif = %d",
        iBytesSinceLastNotification );

    // Set flag when Byte Counter crosses first threshold 0.1 MB
    if ( bThresholdCrossed == EFalse && 
        ( iPckg.iBytesSentTotal+iPckg.iBytesReceivedTotal ) >= KIRDataTransferTrackerThreshold )
        bThresholdCrossed = ETrue;

    // Smooth updates to each 0.01 MB
    if ( /*bThresholdCrossed == EFalse &&*/ iBytesSinceLastNotification >= KIRDataTransferTrackerMiniThreshold)
        {
        if ( iObserver )
            {
            iObserver->HandleDataTransferEventL( iPckg );
            }
        iBytesSinceLastNotification = 0;
        }


    // Streaming updates to each 0.1 MB afterwards
    else if ( iBytesSinceLastNotification >= KIRDataTransferTrackerThreshold )
        {
        if ( iObserver )
            {
            iObserver->HandleDataTransferEventL( iPckg );
            }
        iBytesSinceLastNotification = 0;
        }
    IRLOG_DEBUG( "CIRDataTransferTracker::DispatchNotificationL() - Exiting" );

    }
