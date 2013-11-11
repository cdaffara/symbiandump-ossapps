/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Source subsystem.
*
*/


// INCLUDE FILES

#include <fbs.h>
#include <e32svr.h>
#include <badesca.h>
#include <mmfdatasourcesink.hrh>

#include "CVSMMFDataBuffer.h"
#include "CVSDataSourceImp.h"
#include "VSPanic.h"

// EXTERNAL FUNCTION PROTOTYPES

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// LOCAL CONSTANTS AND MACROS

static const TInt KQcifWidth = 176;
static const TInt KQcifHeight = 144;

 // 4:2:0 planar and 32 bytes extra
 static const TInt KSourceBufferSize =
    ( KQcifWidth * KQcifHeight ) + ( ( KQcifWidth * KQcifHeight ) / 2 ) + 12;

// Granularity for video frame format array
static const TInt KNoOfSupportedFormats = 4;

// How many sequential FillBufferL calls is supported
static const TInt KNoOfSupportedBuffers = 2;

// ============================ MEMBER FUNCTIONS ===============================

// ============================ CVSProviderSwitchAO ============================

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::NewL
// -----------------------------------------------------------------------------
//
CVSProviderSwitchAO* CVSProviderSwitchAO::NewL(
    CVSDataSourceImp* aDataSourceImp )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::NewL() >>"), RThread().Id().operator TUint()));
    CVSProviderSwitchAO* self =
        new ( ELeave ) CVSProviderSwitchAO( aDataSourceImp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::NewL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::ConstructL
// -----------------------------------------------------------------------------
//
void CVSProviderSwitchAO::ConstructL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::ConstructL() >>"), RThread().Id().operator TUint()));
    User::LeaveIfError( iSwitchCs.CreateLocal() );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::ConstructL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::CVSProviderSwitchAO
// -----------------------------------------------------------------------------
//
CVSProviderSwitchAO::CVSProviderSwitchAO( CVSDataSourceImp* aDataSourceImp )
: CActive( EPriorityStandard ), iDataSourceImp( aDataSourceImp )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::CVSProviderSwitchAO() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::CVSProviderSwitchAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::~CVSProviderSwitchAO
// -----------------------------------------------------------------------------
//
CVSProviderSwitchAO::~CVSProviderSwitchAO()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::~CVSProviderSwitchAO() >>"), RThread().Id().operator TUint()));
    iSwitchCs.Close();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::~CVSProviderSwitchAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::ThreadLogon
// -----------------------------------------------------------------------------
//
TInt CVSProviderSwitchAO::ThreadLogon( MAsyncEventHandler& aEventHandler )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::ThreadLogon() >>"), RThread().Id().operator TUint()));
    iEventHandler = &aEventHandler;
    RThread me;
    iThreadId = me.Id();
    CActiveScheduler::Add( this );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::ThreadLogon() <<"), RThread().Id().operator TUint()));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::ThreadLogoff
// -----------------------------------------------------------------------------
//
void CVSProviderSwitchAO::ThreadLogoff()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::ThreadLogoff() >>"), RThread().Id().operator TUint()));
    if( RThread().Id().operator TUint() == iThreadId )
        {
        Deque();
        iThreadId = 0;
        iNewProvider = NULL;
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::ThreadLogoff() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::InitiateProviderSwitch
// -----------------------------------------------------------------------------
//
void CVSProviderSwitchAO::InitiateProviderSwitch(
    CVSDataProvider* aNewProvider )
    {
    iSwitchCs.Wait();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::InitiateProviderSwitch() >>"), RThread().Id().operator TUint()));
    iNewProvider = aNewProvider;
    iStatus = KRequestPending;
    SetActive();
    Signal( KErrNone );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::InitiateProviderSwitch() <<"), RThread().Id().operator TUint()));
    iSwitchCs.Signal();
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::RunL
// -----------------------------------------------------------------------------
//
void CVSProviderSwitchAO::RunL()
    {
    iSwitchCs.Wait();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::RunL() >>"), RThread().Id().operator TUint()));
    if( iStatus.Int() == KErrNone )
        {
        if ( iDataSourceImp->SwitchPending() )
            {
            Panic( EVSPanicSwitchAlreadyPending );
            }
        TRAPD(
            leaveCode,
            iDataSourceImp->DoProviderSwitchL( iNewProvider, *iEventHandler ) );
        if ( leaveCode != KErrNone )
            {
            iDataSourceImp->ProviderSwitchError( leaveCode );
            }
        }
    else
        {
        iNewProvider = NULL;
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::RunL() <<"), RThread().Id().operator TUint()));
    iSwitchCs.Signal();
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::DoCancel
// -----------------------------------------------------------------------------
//
void CVSProviderSwitchAO::DoCancel()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::DoCancel() >>"), RThread().Id().operator TUint()));
    // Nothing to do here. This AO is always signalled when it's activated.
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::DoCancel() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSProviderSwitchAO::Signal
// -----------------------------------------------------------------------------
//
void CVSProviderSwitchAO::Signal( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::Signal() >>"), RThread().Id().operator TUint()));
    if( IsActive() )
        {
        TRequestStatus* pStatus = &iStatus;
        RThread me;
        if( me.Id() == iThreadId )
            {
            User::RequestComplete( pStatus, aError );
            }
        else
            {
            if( me.Open( iThreadId ) == KErrNone )
                {
                *pStatus = KRequestPending;
                me.RequestComplete( pStatus, aError );
                me.Close();
                }
            else
                {
                Panic( EVSPanicThreadOpenFailure );
                }
            }
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSProviderSwitchAO::Signal() <<"), RThread().Id().operator TUint()));
    }

// ============================ CVSDataSourceObserverAO ========================

// -----------------------------------------------------------------------------
// CVSDataSourceObserverAO::CVSDataSourceObserverAO
// -----------------------------------------------------------------------------
//
CVSDataSourceObserverAO::CVSDataSourceObserverAO()
: CActive( EPriorityStandard )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::CVSDataSourceObserverAO() >>"), RThread().Id().operator TUint()));
    RThread me;
    iThreadId = me.Id();
    CActiveScheduler::Add( this );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::CVSDataSourceObserverAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceObserverAO::~CVSDataSourceObserverA
// -----------------------------------------------------------------------------
//
CVSDataSourceObserverAO::~CVSDataSourceObserverAO()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::~CVSDataSourceObserverAO() >>"), RThread().Id().operator TUint()));
    Cancel();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::~CVSDataSourceObserverAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceObserverAO::vsProviderSwitchDone
// -----------------------------------------------------------------------------
//
void CVSDataSourceObserverAO::vsProviderSwitchDone(
    MVSDataSourceObserver& aObserver,
    CVSDataProvider* aOldProvider )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::vsProviderSwitchDone() >>"), RThread().Id().operator TUint()));
    iObserver = &aObserver;
    iOldProvider = aOldProvider;
    iStatus = KRequestPending;
    SetActive();
    Signal( KErrNone );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::vsProviderSwitchDone() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceObserverAO::RunL
// -----------------------------------------------------------------------------
//
void CVSDataSourceObserverAO::RunL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::RunL() >>"), RThread().Id().operator TUint()));
    if( iStatus.Int() == KErrNone )
        {
        iObserver->vsProviderSwitchDone( iOldProvider );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::RunL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceObserverAO::DoCancel
// -----------------------------------------------------------------------------
//
void CVSDataSourceObserverAO::DoCancel()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::DoCancel() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::DoCancel() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceObserverAO::Signal
// -----------------------------------------------------------------------------
//
void CVSDataSourceObserverAO::Signal( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::Signal() >>"), RThread().Id().operator TUint()));
    TRequestStatus* pStatus = &iStatus;
    RThread me;
    if( me.Id() == iThreadId )
        {
        User::RequestComplete( pStatus, aError );
        }
    else
        {
        if( me.Open( iThreadId ) == KErrNone )
            {
            *pStatus = KRequestPending;
            me.RequestComplete( pStatus, aError );
            me.Close();
            }
        else
            {
            Panic( EVSPanicThreadOpenFailure );
            }
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceObserverAO::Signal() <<"), RThread().Id().operator TUint()));
    }



// =============================== CVSDataSource ===============================

// -----------------------------------------------------------------------------
// CVSDataSource::NewSourceL
// -----------------------------------------------------------------------------
//
CVSDataSource* CVSDataSource::NewSourceL( MVSDataSourceObserver* aObserver )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSource::NewSourceL() >>"), RThread().Id().operator TUint()));
    if ( aObserver == NULL )
        {
        User::Leave( KErrArgument );
        }
    CVSDataSource* self = CVSDataSourceImp::NewSourceL();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSource::NewSourceL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSDataSource::CVSDataSource
// -----------------------------------------------------------------------------
//
CVSDataSource::CVSDataSource() : MVTVideoSource( KNullUid )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSource::CVSDataSource() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSource::CVSDataSource() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSource::ConstructL
// -----------------------------------------------------------------------------
//
void CVSDataSource::ConstructL( MVSDataSourceObserver* aObserver )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSource::ConstructL() >>"), RThread().Id().operator TUint()));
    iObserver = aObserver;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSource::ConstructL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::NewSourceL
// -----------------------------------------------------------------------------
//
CVSDataSourceImp* CVSDataSourceImp::NewSourceL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::NewSourceL() >>"), RThread().Id().operator TUint()));
    CVSDataSourceImp* self = new (ELeave) CVSDataSourceImp;
    CleanupStack::PushL( self );
    self->ConstructSourceL( KNullDesC8 );
    CleanupStack::Pop( self );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::NewSourceL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SwitchDataProviderL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::SwitchDataProviderL(
    CVSDataProvider* aNewProvider )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SwitchDataProviderL() >>"), RThread().Id().operator TUint()));
    if ( aNewProvider == NULL )
        {
        __IF_DEBUG(Print(_L("SwitchDataProviderL(): NULL argument, leaving.")));
        User::Leave( KErrArgument );
        }
    // Is new provider same as old, then no need to change provider
    if ( aNewProvider == &DataProvider() )
        {
        __IF_DEBUG(Print(_L("SwitchDataProviderL(): New provider same as old: 0x%x"), aNewProvider));
        Observer().vsProviderSwitchDone( aNewProvider );
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SwitchDataProvider() <<"), RThread().Id().operator TUint()));
        return;
        }
    // If Protocol is in the currently initializing itself then complete this switch
    // request when Protocol ready (from SourcePlayL). No need to protect
    // iProtoInitOngoing test with CS.
    if ( iProtoInitOngoing )
        {
        __IF_DEBUG(Print(_L("PROTOCOL INIT ONGOING, PENDING SWITCH FOR A WHILE")));
        iProtoInitCS.Wait();
        iProviderSwitchRequestDuringProtoInit = ETrue;
        iNewProvider = aNewProvider;
        iProtoInitCS.Signal();
        return;
        }
    // Switch provider
    if ( iThreadLogonCalled )
        {
        iProviderSwitchAO->InitiateProviderSwitch( aNewProvider );
        }
    else
        {
        CVSDataProvider* oldProvider = SwitchProvider( aNewProvider );
        Observer().vsProviderSwitchDone( oldProvider );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SwitchDataProviderL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::PauseSending
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::PauseSending()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::PauseSending() >>"), RThread().Id().operator TUint()));
    iPauseCs.Wait();
    iSendingPaused = ETrue;
    iPauseCs.Signal();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::PauseSending() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::ResumeSending
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::ResumeSending()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::ResumeSending() >>"), RThread().Id().operator TUint()));
    iPauseCs.Wait();
    iSendingPaused = EFalse;
    iPauseCs.Signal();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::ResumeSending() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::~CVSDataSourceImp
// -----------------------------------------------------------------------------
//
CVSDataSourceImp::~CVSDataSourceImp()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::~CVSDataSourceImp() >>"), RThread().Id().operator TUint()));
    delete iDataSourceObserverAO;
    delete iProviderSwitchAO;
    iProtoMimeTypes.Close();
    delete iSupportedVideoFormats;
    iSupportedVideoSizes.Close();
    iSupportedFrameRates.Close();
    // Assuming no need to use critical section here
    iFreeQueue.Close();
    iActiveQueue.Close();
    iAllocatedBuffers.ResetAndDestroy();
    iQueueCs.Close();
    iPauseCs.Close();
    iProtoInitCS.Close();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::~CVSDataSourceImp() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::FillBufferL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::FillBufferL(
    CMMFBuffer* aBuffer,
    MVTVideoSink* aConsumer,
    TMediaId aMediaId )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FillBufferL() >>"), RThread().Id().operator TUint()));

    if ( aBuffer == NULL || aConsumer == NULL )
        {
        User::Leave( KErrArgument );
        }

    if ( aBuffer->Type() != KUidMmfDataBuffer )
        {
        User::Leave( KErrArgument );
        }

    DataProvider().SetFillBufferParams( aConsumer, aMediaId );

    iPauseCs.Wait();
    const TBool sendingPaused( iSendingPaused );
    iPauseCs.Signal();

    if( iSwitchOngoing )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FillBufferL(): Switch ongoing"), RThread().Id().operator TUint()));
        // queues are locked by us -> it's safe to modify queues without
        // locking
        User::LeaveIfError(
            FreeBufferNoWait( static_cast<CVSMMFDataBuffer*>( aBuffer ) ) );
        }
    else if( iSwitchPending )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FillBufferL(): Switch pending"), RThread().Id().operator TUint()));
        FreeBufferL( static_cast<CVSMMFDataBuffer*>( aBuffer ) );
        TRAPD(
            err,
            DoProviderSwitchL(
                iProviderSwitchAO->NewDataProvider(),
                iProviderSwitchAO->EventHandler() ) );
        if( err != KErrNone )
            {
            __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FillBufferL(): Error in switch = %d"), RThread().Id().operator TUint(), err));
            // Not leaving from here (it was provider switch related error),
            // let VT take care of the rest
            ProviderSwitchError( err );
            }
        }
    else if( sendingPaused )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FillBufferL(): Sending Paused"), RThread().Id().operator TUint()));
        // queues are locked by us -> it's safe to modify queues without
        // locking
        User::LeaveIfError(
            FreeBufferNoWait( static_cast<CVSMMFDataBuffer*>( aBuffer ) ) );
        }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FillBufferL(): No switch -> normal FillBufferL()"), RThread().Id().operator TUint()));
        FreeBufferL( static_cast<CVSMMFDataBuffer*>( aBuffer ) );
        DataProvider().FillBufferL( aBuffer, aConsumer, aMediaId );
        }

    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FillBufferL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::BufferEmptiedL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::BufferEmptiedL( CMMFBuffer* /*aBuffer*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::BufferEmptiedL() >>"), RThread().Id().operator TUint()));
    User::Leave( KErrNotSupported );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::BufferEmptiedL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::CanCreateSourceBuffer
// -----------------------------------------------------------------------------
//
TBool CVSDataSourceImp::CanCreateSourceBuffer()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CanCreateSourceBuffer() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CanCreateSourceBuffer) <<"), RThread().Id().operator TUint()));
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::CreateSourceBufferL
// -----------------------------------------------------------------------------
//
CMMFBuffer* CVSDataSourceImp::CreateSourceBufferL(
    TMediaId /*aMediaId*/,
    TBool &aReference )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CreateSourceBufferL() >>"), RThread().Id().operator TUint()));
    if ( iRequestedBufferCount >= KNoOfSupportedBuffers )
        {
        User::Leave( KErrOverflow );
        }
    aReference = ETrue;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CreateSourceBufferL(): Returning buffer: %d [0x%08x]"), RThread().Id().operator TUint(), iRequestedBufferCount + 1, iAllocatedBuffers[ iRequestedBufferCount ]));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CreateSourceBufferL() <<"), RThread().Id().operator TUint()));
    return iAllocatedBuffers[ iRequestedBufferCount++ ];
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::CreateSourceBufferL
// -----------------------------------------------------------------------------
//
CMMFBuffer* CVSDataSourceImp::CreateSourceBufferL(
    TMediaId /*aMediaId*/,
    CMMFBuffer& /*aSinkBuffer*/,
    TBool &aReference )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CreateSourceBufferL() >>"), RThread().Id().operator TUint()));
    if ( iRequestedBufferCount >= KNoOfSupportedBuffers )
        {
        User::Leave( KErrOverflow );
        }
    aReference = ETrue;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CreateSourceBufferL(): Returning buffer: %d [0x%08x]"), RThread().Id().operator TUint(), iRequestedBufferCount + 1, iAllocatedBuffers[ iRequestedBufferCount ]));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CreateSourceBufferL() <<"), RThread().Id().operator TUint()));
    return iAllocatedBuffers[ iRequestedBufferCount++ ];
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SourceThreadLogon
// -----------------------------------------------------------------------------
//
TInt CVSDataSourceImp::SourceThreadLogon( MAsyncEventHandler& aEventHandler )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourceThreadLogon() >>"), RThread().Id().operator TUint()));
    __ASSERT_DEBUG( (&DataProvider()) != NULL,
        Panic( EVSPanicProviderNotSet ) );
    TInt err( KErrNone );
    if( !iFbsStarted )
        {
        // Caller does not start the FBS server when compiled on proxy mode
        // so we have to take care of that by ourself.
        err = FbsStartup();
        if( err == KErrNone )
            {
            err = RFbsSession::Connect();
            if( err == KErrNone )
                {
                iFbsStarted = ETrue;
                }
            }
        }
    if( !iThreadLogonCalled && ( err == KErrNone ) )
        {
        err = DataProvider().SourceThreadLogon( aEventHandler );
        if (err != KErrNone)
            {
            return err;
            }
        iProviderSwitchAO->ThreadLogon( aEventHandler );
        iThreadLogonCalled = ETrue;
        iProtoInitOngoing = ETrue;
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourceThreadLogon() <<"), RThread().Id().operator TUint()));
    return err;
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SourceThreadLogoff
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::SourceThreadLogoff()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourceThreadLogoff() >>"), RThread().Id().operator TUint()));

    iThreadLogonCalled = EFalse;

    if ( iProviderSwitchRequestDuringProtoInit )
        {
        __IF_DEBUG(Print(_L("PROTOCOL INIT FAILED, NOW COMPLETING THE SWITCH REQUEST WITH ERROR")));
        iProviderSwitchRequestDuringProtoInit = EFalse;
        ProviderSwitchError( KErrAbort );
        }

    if( &DataProvider() )
        {
        DataProvider().SourceThreadLogoff();
        }

    iSwitchOngoing = iSwitchPending = EFalse;

    iProtoInitOngoing = EFalse;

    // If provider switch ongoing complete it
    if ( iProviderSwitchAO->NewDataProvider() != NULL )
        {
        CVSDataProvider* oldProvider =
            SwitchProvider( iProviderSwitchAO->NewDataProvider() );
        Observer().vsProviderSwitchDone( oldProvider );
        }

    iProviderSwitchAO->ThreadLogoff();

    delete iFormat; iFormat = 0;

    iCallFlags = 0;

    // Clear iFreeQueue
    while( iFreeQueue.Count() )
        {
        iFreeQueue.Remove( 0 );
        }

    // Clear iActiveQueue
    while( iActiveQueue.Count() )
        {
        iActiveQueue.Remove( 0 );
        }

    // Set frame buffer to NULL for all buffers
    for( TInt i = 0; i < iRequestedBufferCount; i++ )
        {
        TInt err = KErrNone;
        // Does not leave when framebuffer is set to NULL
        // Both type of buffers needs to be nulled
        TRAP( err, iAllocatedBuffers[i]->SetFrameBufferL( ( MCameraBuffer* )NULL ));
		TRAP( err, iAllocatedBuffers[i]->SetFrameBufferL( ( MFrameBuffer* )NULL ));        }

    // All pre-created buffers are free again
    iRequestedBufferCount = 0;

    if( iFbsStarted )
        {
        RFbsSession::Disconnect();
        iFbsStarted = EFalse;
        }

    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourceThreadLogoff() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SourcePrimeL
// -----------------------------------------------------------------------------
//
TInt CVSDataSourceImp::SourcePrimeL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePrimeL() >>"), RThread().Id().operator TUint()));
    TRAPD( result, DataProvider().SourcePrimeL() );
    if( ( result == KErrNone ) || ( result == KRequestPending ) )
        {
        iDSState = CVSDataSourceImp::EPrimed;
        }
    else
        {
        User::Leave( result );
        }
    iProtoInitOngoing = EFalse;
    // If provider switch was requested when Proto was  initializing itself then
    // fill switch request now
    iProtoInitCS.Wait();
    if ( iProviderSwitchRequestDuringProtoInit )
        {
        __IF_DEBUG(Print(_L("PROTOCOL INIT READY, NOW COMPLETING THE SWITCH REQUEST")));
        iProviderSwitchRequestDuringProtoInit = EFalse;
        iProviderSwitchAO->InitiateProviderSwitch( iNewProvider );
        iNewProvider = NULL;
        }
    iProtoInitCS.Signal();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePrimeL() <<"), RThread().Id().operator TUint()));
    return result;
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SourcePlayL
// -----------------------------------------------------------------------------
//
TInt CVSDataSourceImp::SourcePlayL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePlayL() >>"), RThread().Id().operator TUint()));
    if( !iSwitchOngoing )
        {
        DataProvider().SourcePlayL();
        }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePlayL(): Switch ongoing -> updating state only, play will be called after switch"), RThread().Id().operator TUint()));
        }
    iDSState = CVSDataSourceImp::EPlaying;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePlayL() <<"), RThread().Id().operator TUint()));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SourcePauseL
// -----------------------------------------------------------------------------
//
TInt CVSDataSourceImp::SourcePauseL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePauseL() >>"), RThread().Id().operator TUint()));
    if( !iSwitchOngoing )
        {
        DataProvider().SourcePauseL();
        }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePlayL(): Switch ongoing -> updating state only"), RThread().Id().operator TUint()));
        }
    iDSState = CVSDataSourceImp::EPrimed;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePauseL() <<"), RThread().Id().operator TUint()));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SourceStopL
// -----------------------------------------------------------------------------
//
TInt CVSDataSourceImp::SourceStopL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourceStopL() >>"), RThread().Id().operator TUint()));
    if( !iSwitchOngoing )
        {
        DataProvider().SourceStopL();
        }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourcePlayL(): Switch ongoing -> updating state only"), RThread().Id().operator TUint()));
        }
    iDSState = CVSDataSourceImp::EStopped;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SourceStopL() <<"), RThread().Id().operator TUint()));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::ConstructSourceL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::ConstructSourceL( const TDesC8& /*aInitData*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::ConstructSourceL() >>"), RThread().Id().operator TUint()));
    iYuv420PlanarDesc.Zero();
    iYuv420PlanarDesc.Append( KVtVideoMIMETypeYUV420 );
    User::LeaveIfError( iProtoMimeTypes.Append( &iYuv420PlanarDesc ) );
    iProviderSwitchAO = CVSProviderSwitchAO::NewL( this );
    iDataSourceObserverAO = new (ELeave) CVSDataSourceObserverAO();
    iSupportedVideoFormats =
        new (ELeave) CDesC8ArrayFlat( KNoOfSupportedFormats );
    // Get formats supported by all providers
    CVSDataProvider::EnumerateVideoFrameFormatsL( iSupportedVideoFormats );
    // Get video frame sizes for specified format supported by all providers
    TInt i, j;
    i = j = 0;
    for ( i = 0; i < iSupportedVideoFormats->Count(); i++ )
        {
        TPtrC8 format = iSupportedVideoFormats->MdcaPoint( i );
        RArray<TSize> sizeArray;
        CleanupClosePushL( sizeArray );
        CVSDataProvider::EnumerateVideoFrameSizesL( sizeArray, format );
        for ( j = 0; j < sizeArray.Count(); j++ )
            {
            TVSFrameSize sizePerFormat;
            sizePerFormat.iFormat.Set( format );
            sizePerFormat.iSize = sizeArray[j];
            User::LeaveIfError( iSupportedVideoSizes.Append( sizePerFormat ) );
            }
        CleanupStack::PopAndDestroy(); // sizeArray
        }
    // Get video frame rates for specified frame format and size supported
    // by all providers
    for ( i = 0; i < iSupportedVideoSizes.Count(); i++ )
        {
        RArray<TReal32> rateArray;
        CleanupClosePushL( rateArray );
        CVSDataProvider::EnumerateVideoFrameRatesL(
            rateArray,
            iSupportedVideoSizes[i].iFormat,
            iSupportedVideoSizes[i].iSize );
        for ( j = 0; j < rateArray.Count(); j++ )
            {
            TVSFrameRate framePerSizeAndFormat;
            framePerSizeAndFormat.iFormat.Set(
                iSupportedVideoSizes[i].iFormat );
            framePerSizeAndFormat.iSize = iSupportedVideoSizes[i].iSize;
            framePerSizeAndFormat.iRate = rateArray[j];
            User::LeaveIfError( iSupportedFrameRates.Append(
                framePerSizeAndFormat ) );
            }
        CleanupStack::PopAndDestroy(); // rateArray
        }
    // Create queue criticalsection
    User::LeaveIfError( iQueueCs.CreateLocal() );
    // Create VTEng-Protocol guard
    User::LeaveIfError( iPauseCs.CreateLocal() );
    // Pre-create buffers
    for( i = 0; i < KNoOfSupportedBuffers; i++ )
        {
        User::LeaveIfError( iFreeQueue.Append( 0 ) );
        User::LeaveIfError( iActiveQueue.Append( 0 ) );
        CVSMMFDataBuffer* buffer = CVSMMFDataBuffer::NewL( KSourceBufferSize );
        CleanupStack::PushL( buffer );
        User::LeaveIfError( iAllocatedBuffers.Append( buffer ) );
        CleanupStack::Pop(); // buffer
        }
    for( i = 0; i < KNoOfSupportedBuffers; i++ )
        {
        iFreeQueue.Remove( 0 );
        iActiveQueue.Remove( 0 );
        }
    // Init critical sections
    User::LeaveIfError( iProtoInitCS.CreateLocal() );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::ConstructSourceL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::GetMultimediaTypesL
// -----------------------------------------------------------------------------
//
const RArray<TDesC8*>& CVSDataSourceImp::GetMultimediaTypesL() const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetMultimediaTypeL() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetMultimediaTypeL() <<"), RThread().Id().operator TUint()));
    return iProtoMimeTypes;
    }


// -----------------------------------------------------------------------------
// CVSDataSourceImp::SetFormatL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::SetFormatL( const TDesC8& aFormat )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SetFormatL() >>"), RThread().Id().operator TUint()));
    if ( &DataProvider() == NULL )
        {
        User::Leave( KErrBadHandle );
        }
    TBool supported = EFalse;
    for ( TInt i = 0; i < iSupportedVideoFormats->Count(); i++ )
        {
        TPtrC8 format = iSupportedVideoFormats->MdcaPoint( i );
        if ( aFormat == format )
            {
            supported = ETrue;
            break;
            }
        }
    if ( !supported )
        {
        User::Leave( KErrNotSupported );
        }
    DataProvider().SetFormatL( aFormat );
    delete iFormat; iFormat = 0;
    iFormat = aFormat.AllocL();
    iCallFlags |= ESetFormatCalled;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SetFormatL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SetFrameRateL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::SetFrameRateL( TReal32 aFrameRate )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SetFrameRateL() >>"), RThread().Id().operator TUint()));
    if ( &DataProvider() == NULL )
        {
        User::Leave( KErrBadHandle );
        }
    TBool supported = EFalse;
    for ( TInt i = 0; i < iSupportedFrameRates.Count(); i++ )
        {
        TVSFrameRate rate = iSupportedFrameRates[i];
        if ( rate.iFormat == iFormat->Des() && rate.iSize == iSize)
            {
            if ( aFrameRate == rate.iRate )
                {
                supported = ETrue;
                break;
                }
            }
        }
    if ( !supported )
        {
        User::Leave( KErrNotSupported );
        }
    DataProvider().SetFrameRateL( aFrameRate );
    iFrameRate = aFrameRate;
    iCallFlags |= ESetFrameRateCalled;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SetFrameRateL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::SetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::SetVideoFrameSizeL( const TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SetVideoFrameSizeL() >>"), RThread().Id().operator TUint()));
    if ( &DataProvider() == NULL )
        {
        User::Leave( KErrBadHandle );
        }
    TBool supported = EFalse;
    for ( TInt i = 0; i < iSupportedVideoSizes.Count(); i++ )
        {
        TVSFrameSize size = iSupportedVideoSizes[i];
        if ( size.iFormat == iFormat->Des() )
            {
            if ( aSize == size.iSize )
                {
                supported = ETrue;
                break;
                }
            }
        }
    if ( !supported )
        {
        User::Leave( KErrNotSupported );
        }
    DataProvider().SetVideoFrameSizeL( aSize );
    iSize = aSize;
    iCallFlags |= ESetSizeCalled;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::SetVideoFrameSizeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::GetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::GetVideoFrameSizeL( TSize& aSize ) const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetVideoFrameSizeL() >>"), RThread().Id().operator TUint()));
    if ( &DataProvider() == NULL )
        {
        User::Leave( KErrBadHandle );
        }
    DataProvider().GetVideoFrameSizeL( aSize );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetVideoFrameSizeL() <<"), RThread().Id().operator TUint()));
    }


// -----------------------------------------------------------------------------
// CVSDataSourceImp::DoProviderSwitchL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::DoProviderSwitchL(
    CVSDataProvider* aNewProvider,
    MAsyncEventHandler& aEventHandler )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::DoProviderSwitchL() >>"), RThread().Id().operator TUint()));

    iQueueCs.Wait();

    // If there are entries in iActiveQueue wait until they are freed by Protocol
    if ( iActiveQueue.Count() > 0 )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::DoProviderSwitchL(): activequeue count > 0, <<"), RThread().Id().operator TUint()));
        iSwitchPending = ETrue;
        iQueueCs.Signal();
        return;
        }
    else
        {
        iSwitchPending = EFalse;
        }

    iSwitchOngoing = ETrue;

    /*
     * THE FOLLOWING CALLS USE CActiveSchedulerWait WHICH MEANS SOME OTHER
     * OBJECT MAY RUN IN BETWEEN THE FOLLOWING CALLS I.E. THESE CALLS ARE NOT
     * SYNCHRONOUS.
     */

    User::LeaveIfError( aNewProvider->SourceThreadLogon( aEventHandler ) );

    // Is format set by the Protocol
    if ( iCallFlags & ESetFormatCalled )
        {
        aNewProvider->SetFormatL( *iFormat );
        }
    // Is size set by the Protocol
    if ( iCallFlags & ESetSizeCalled )
        {
        aNewProvider->SetVideoFrameSizeL( iSize );
        }
    // Is framerate set by the Protocol
    if ( iCallFlags & ESetFrameRateCalled )
        {
        aNewProvider->SetFrameRateL( iFrameRate );
        }
    // Stop old provider
    if ( &DataProvider() != NULL )
        {
        switch( iDSState )
            {
        case EPlaying:
            DataProvider().SourcePauseL();
            DataProvider().SourceStopL();
            break;

        case EPrimed:
            DataProvider().SourceStopL();
            break;

        default:
            break;
            };
        DataProvider().SourceThreadLogoff();
        }

    // Set new provider state according to current state
    switch ( iDSState )
        {
        case EStopped:
            break;

        case EPrimed:
        case EPlaying:
            aNewProvider->PrimeL();

            // check if SourcePlayL() was called during prime
            if( iDSState == EPlaying )
                {
                aNewProvider->SourcePlayL();
                aNewProvider->SwitchFrom( DataProvider() );
                }
            // or SourceStopL()
            else if( iDSState == EStopped )
                {
                aNewProvider->SourceStopL();
                }
            break;
        }
    /*
     * THE FOLLOWING CALLS ARE SUPPOSED TO BE SYNCHRONOUS IN THAT THEY DO NOT
     * LET ACTIVE SCHEDULER RUN SOME OTHER OBJECT.
     */

    // Complete switch
    CVSDataProvider* oldProvider = SwitchProvider( aNewProvider );

    // Set consumer and media id
    DataProvider().SetFillBufferParams(
        oldProvider->Consumer(),
        oldProvider->MediaId() );

    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::DoProviderSwitchL(): iFreeQueue.Count() = %d, iActiveQueue.Count() = %d"), RThread().Id().operator TUint(), iFreeQueue.Count(), iActiveQueue.Count() ));

    // If old provider has pending FillBufferL() calls -> call FillBufferL()
    // for new provider if it is in Playing state.
	if( iDSState == EPlaying )
		{
        for ( TInt i = 0; i < iFreeQueue.Count(); i++ )
            {
            iFreeQueue[ i ]->Reset();
            DataProvider().FillBufferL(
                iFreeQueue[i],
                DataProvider().Consumer(),
                DataProvider().MediaId() );
    		}
        }

    // Buffers that are being used by Protocol are set to be needing a reset
    for ( TInt i = 0; i < iActiveQueue.Count(); i++ )
        {
        iActiveQueue[ i ]->SetNeedsReset( ETrue );
        }

    iSwitchOngoing = EFalse;

    iQueueCs.Signal();

    iDataSourceObserverAO->vsProviderSwitchDone( Observer(), oldProvider );
    iProviderSwitchAO->NewDataProvider() = NULL;

    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::DoProviderSwitchL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::CVSDataSourceImp
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::ProviderSwitchError( TInt aError )
    {
    DataProvider().NotifyError( aError );
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::CVSDataSourceImp
// -----------------------------------------------------------------------------
//
CVSDataSourceImp::CVSDataSourceImp()
: iFreeQueue( KNoOfSupportedBuffers ), iActiveQueue( KNoOfSupportedBuffers )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CVSDataSourceImp() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::CVSDataSourceImp() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::GetBufferL
// -----------------------------------------------------------------------------
//
CVSMMFDataBuffer* CVSDataSourceImp::GetBufferL( TBool aRemove )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetBufferL() >>"), RThread().Id().operator TUint()));
    if( !iQueueCs.IsBlocked() )
        {
        iPauseCs.Wait();
        const TBool sendingPaused( iSendingPaused );
        iPauseCs.Signal();

        iQueueCs.Wait();

        // while switch is pending, always return NULL we don't want to let
        // buffer float around
        if( iSwitchPending || sendingPaused )
            {
            iQueueCs.Signal();
            __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetBufferL(): Switch pending or sending paused -> returning NULL"), RThread().Id().operator TUint()));
            return 0;
            }

        // If no buffers in free (FillBufferL is called) queue -> return NULL
        if( !iFreeQueue.Count() )
            {
            iQueueCs.Signal();
            __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetBufferL(): NO free buffers -> returning NULL"), RThread().Id().operator TUint()));
            return 0;
            }
        // Otherwise get first buffer
        CVSMMFDataBuffer* temp = iFreeQueue[ 0 ];
        // Move to active (BufferFilledL is called) queue if requested
        if( aRemove )
            {
            iFreeQueue.Remove( 0 );
            TInt err = iActiveQueue.Append( temp );
            if ( err != KErrNone )
                {
                iQueueCs.Signal();
                User::Leave( err );
                }
            }
        iQueueCs.Signal();
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetBufferL() returning [$%x]"), RThread().Id().operator TUint(), temp));
        return temp;
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::GetBufferL(): blocked, returning [$%08x]"), RThread().Id().operator TUint(), 0));
    return 0;
    }

// -----------------------------------------------------------------------------
// CVSDataSourceImp::FreeBufferL
// -----------------------------------------------------------------------------
//
void CVSDataSourceImp::FreeBufferL( CVSMMFDataBuffer* aBuffer )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FreeBufferL() >>"), RThread().Id().operator TUint()));
    iQueueCs.Wait();
    TInt err = FreeBufferNoWait( aBuffer );
    iQueueCs.Signal();
    User::LeaveIfError( err );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FreeBufferL() <<"), RThread().Id().operator TUint()));
    }


// -----------------------------------------------------------------------------
// CVSDataSourceImp::FreeBufferNoWait( CVSMMFDataBuffer* aBuffer )
// -----------------------------------------------------------------------------
//
TInt CVSDataSourceImp::FreeBufferNoWait( CVSMMFDataBuffer* aBuffer )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FreeBufferNoWait() >>"), RThread().Id().operator TUint()));

    // Append buffer into free (FillBufferL is called) queue
    TInt err( KErrNone );

    // Try finding buffer from active (BufferFilledL is called) queue
    TInt actIdx = iActiveQueue.Find( aBuffer );

    // If it is found, remove it from there
    if( actIdx != KErrNotFound )
        {
        iActiveQueue.Remove( actIdx );
        TRAP( err, aBuffer->ReleaseL() );
        if( err != KErrNone )
            {
            return err;
            }
        }

    if( iFreeQueue.Find( aBuffer ) == KErrNotFound )
        {
        err = iFreeQueue.Append( aBuffer );
        }

#ifdef _DEBUG
    TBuf<256> b;
    TInt i;
    b = _L("Active: ");
    for( i = 0; i < iActiveQueue.Count(); i++ )
        {
        if( i > 0 )
            {
            b.Append( _L(", ") );
            }
        TBuf<16> b1;
        b1.Format( _L("[$%08x]"), iActiveQueue[i] );
        b.Append( b1 );
        }
    RDebug::Print( b );

    b = _L("Free  : ");
    for( i = 0; i < iFreeQueue.Count(); i++ )
        {
        if( i > 0 )
            {
            b.Append( _L(", ") );
            }
        TBuf<16> b1;
        b1.Format( _L("[$%08x]"), iFreeQueue[i] );
        b.Append( b1 );
        }
    RDebug::Print( b );
#endif
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSDataSourceImp::FreeBufferNoWait() <<"), RThread().Id().operator TUint()));

    return err;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
