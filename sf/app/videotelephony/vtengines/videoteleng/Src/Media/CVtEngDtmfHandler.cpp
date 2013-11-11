/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler for DTMF sending.
*
*/


// INCLUDE FILES
#include    "CVtEngDtmfHandler.h"
#include    "CVtEngMediaHandler.h"
#include    "VtEngPanic.h"
#include    "cvtlogger.h"

// LOCAL MACROS
#define DTMF_STATES_COUNT 3
const TInt KProtoDtmfCommandId = -1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::NewL
//
// -----------------------------------------------------------------------------
//
CVtEngDtmfHandler* CVtEngDtmfHandler::NewL( MVtH324ConfigCommand& aH324Config )
    {
    __VTPRINTENTER( "DTMF.New" )
    CVtEngDtmfHandler* self = new ( ELeave ) CVtEngDtmfHandler( aH324Config );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    __VTPRINTEXIT( "DTMF.New" )
	return self;
	}

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::~CVtEngDtmfHandler
//
// -----------------------------------------------------------------------------
//
CVtEngDtmfHandler::~CVtEngDtmfHandler()
    {
    __VTPRINTENTER( "DTMF.~" )
    if ( iDtmfStates )
    	{
        iDtmfStates->ResetAndDestroy();
    	}
    delete iDtmfStates;
    if( iProtoDtmf )
        {
        VTProtocolFactory::DeleteUserInputIndication(iProtoDtmf);
        }
    iProtoDtmf = NULL;
    __VTPRINTEXIT( "DTMF.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::HandleL
//
// -----------------------------------------------------------------------------
//
void CVtEngDtmfHandler::HandleL( CVtEngOperation& aOp )
    {
    __VTPRINTENTER( "DTMF.HandleL" )

    const TVtEngCommandId cmdId( aOp.Command() );
    switch ( cmdId )
        {
        case KVtEngStartDtmfTone:
            {
            __VTPRINT( DEBUG_MEDIA , "DTMF. start tone" )
            TChar tone;
            TVtEngOpParamUtil<TChar>::Set( tone, aOp );
            CVtEngDtmfState* state = (*iDtmfStates)[iCurrentState];
            if ( state )
                {
                TVtEngDtmfTone dtmfTone( tone );
                state->SendDtmfL( dtmfTone );
                }
            }
            break;
        case KVtEngStopDtmfTone:
            __VTPRINT( DEBUG_MEDIA , "DTMF. stop tone (no op)" )
            break;
        // Handling of different UIIs
        case KVtEngSetUIIDtmfSupport:
            SetUIISupport( EDTMFString );
            __VTPRINT( DEBUG_MEDIA , "DTMF Support enabled" )
            break;
        case KVtEngSetUIIBasicStringSupport:
            SetUIISupport( EBasicString );
            __VTPRINT( DEBUG_MEDIA , "Basic string Support enabled" )
            break;
        case KVtEngSetUIIIA5StringSupport:
            SetUIISupport( EIA5String );
            __VTPRINT( DEBUG_MEDIA , "IA5 String Support enabled" )
            break;
        case KVtEngSetUIIGeneralStringSupport:
            SetUIISupport( EGeneralString );
            __VTPRINT( DEBUG_MEDIA , "General String Support enabled" )
            break;
        default:
            __VTPRINT( DEBUG_MEDIA , "DTMF.Handle cmd not supported" )
            User::Leave( KErrNotSupported );
        }
    __VTPRINTEXIT( "DTMF.HandleL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::ActivateState
//
// -----------------------------------------------------------------------------
//
CVtEngDtmfState& CVtEngDtmfHandler::ActivateState( TVtEngDtmfState aNewState )
    {
    __VTPRINTENTER( "DTMF.ActivateState" )
    __VTPRINT2( DEBUG_MEDIA , " new state=%d", aNewState )
    __ASSERT_ALWAYS( aNewState >= EVtDtmfStateIdle &&
        aNewState <= EVtDtmfStateBuffered,
        Panic( EVtEngPanicInvalidDtmfState) );
    iCurrentState = aNewState;
    __VTPRINTEXIT( "DTMF.ActivateState" )
    return *(*iDtmfStates)[iCurrentState];
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::SendComplete
//
// -----------------------------------------------------------------------------
//
void CVtEngDtmfHandler::SendComplete( TInt aError )
    {
    __VTPRINTENTER( "DTMF.SendComplete" )
#ifdef VTDEBUG
    TRAPD( ignore, HandleSendCompleteL( aError ) );
    __VTPRINT2( DEBUG_MEDIA , "DTMF.SendComplete err=%d", ignore )
#else
    TRAP_IGNORE( HandleSendCompleteL( aError ) );
#endif // VTDEBUG
    __VTPRINTEXIT( "DTMF.SendComplete" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::ProtoDtmf
//
// -----------------------------------------------------------------------------
//
MVTUserInput& CVtEngDtmfHandler::ReserveProtoDtmfL(
    const TVtEngDtmfTone& aTone )
    {
    __VTPRINTENTER( "DTMF.ReserveProtoDtmfL" )
    if ( iProtoDtmf != NULL )
        {
        __VTPRINT( DEBUG_MEDIA , "DTMF.ReserveProtoDtmfL IN USE" )
        User::Leave( KErrInUse );
        }
    TUint8 val = (TUint8) aTone.iTone;

#ifdef VTDEBUG
    if ( CVtLogger::Flags() & CVtLogger::ELogMedia )
        {
        const TChar ch( val );
        _LIT( KVtDtmfBufferTrace, "VT: DTMF. tone value=" );
        TBuf<25> buffer( KVtDtmfBufferTrace() );
        buffer.Append( ch );
        RDebug::Print( buffer );
        }
#endif

    

    // NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // IT MUST BE CHECKED IS GIVEN LETTER SUPPORTED BY
    // USED ENCODING SCHEME. NOW WE PASS ONLY NUMBERS AND * AND #
    // AND THEY ARE SAFE TO USE BUT USING POWERFULL UIIs GIVE POSSIBILITY
    // TO SEND ALL DIGITS AND NUMBERS. IN THAT CASE SET OF CHARS MUST BE CHECKED
    // IN VTENGINE. (THAT IS THE REASON WHY THERE IS SEPARATE IF ELSEs)
    // NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    __VTPRINT2( DEBUG_MEDIA , "DTMF. Proto USERINPUT %d", iUIISupport )
    switch ( iUIISupport )
        {
        case EBasicString:
        case EIA5String:
        case EGeneralString:
            __VTPRINT( DEBUG_MEDIA , "DTMF. Proto USERINPUT alphanumeric" )
				iProtoDtmf = VTProtocolFactory::CreateUserInputIndication( EVtUiiAlphaNumeric, val );
            break;
        case EDTMFString:
            __VTPRINT( DEBUG_MEDIA , "DTMF. Proto USERINPUT iDTMFString" )
				iProtoDtmf = VTProtocolFactory::CreateUserInputIndication( EVtUiiDTMF, val);
            break;
        default:
            __VTPRINT( DEBUG_MEDIA , "DTMF. Proto USERINPUT unsupported" )
            break;
        }
    __VTPRINTEXIT( "DTMF.ReserveProtoDtmfL" )
    return *iProtoDtmf;
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::ReleaseProtoDtmf
//
// -----------------------------------------------------------------------------
//
void CVtEngDtmfHandler::ReleaseProtoDtmf()
    {
    __VTPRINTENTER( "DTMF.ReleaseProtoDtmf" )
    if( iProtoDtmf )
        {
        VTProtocolFactory::DeleteUserInputIndication(iProtoDtmf);
        }
    iProtoDtmf = NULL;
    __VTPRINTEXIT( "DTMF.ReleaseProtoDtmf" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::CVtEngDtmfHandler
//
// -----------------------------------------------------------------------------
//
CVtEngDtmfHandler::CVtEngDtmfHandler( MVtH324ConfigCommand& aH324Config ) :
    iCurrentState( EVtDtmfStateIdle ), iH324Config( aH324Config ),
    iUIISupport( EIA5String ), iIssuedDtmfCommandId(KProtoDtmfCommandId)
    {
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::ConstructL
//
// -----------------------------------------------------------------------------
//
void CVtEngDtmfHandler::ConstructL()
    {
    __VTPRINTENTER( "DTMF.ConstructL" )
    iDtmfStates = new ( ELeave )
        CArrayPtrFlat<CVtEngDtmfState>( DTMF_STATES_COUNT );

    for ( TInt state = EVtDtmfStateIdle;
          state <= EVtDtmfStateBuffered;
          state++ )
        {
        CreateStateL( state );
        }
    __VTPRINTEXIT( "DTMF.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::HandleSendCompleteL
//
// -----------------------------------------------------------------------------
//
void CVtEngDtmfHandler::HandleSendCompleteL( TInt aError )
    {
    __VTPRINTENTER( "DTMF.HandleSendCompleteL" )

    __VTPRINT2( DEBUG_MEDIA , " HandleSendCompleteL err=%d", aError )
    if ( iDtmfStates )
        {
        CVtEngDtmfState* state = (*iDtmfStates)[iCurrentState];
        if ( state )
            {
            __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
                "DTMF. complete on state %d", iCurrentState )
            state->DtmfSendCompleteL( aError );
            }
        }
    __VTPRINTEXIT( "DTMF.HandleSendCompleteL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::CreateState
//
// -----------------------------------------------------------------------------
//
void CVtEngDtmfHandler::CreateStateL( TInt aState )
    {
    __VTPRINTENTER( "DTMF.CreateStateL" )
    CVtEngDtmfState* dtmfState = NULL;
    switch ( aState )
        {
        case EVtDtmfStateIdle:
            dtmfState = CVtEngDtmfIdle::NewL( *this, iH324Config );
            break;
        case EVtDtmfStateSending:
            dtmfState = CVtEngDtmfSending::NewL( *this, iH324Config );
            break;
        case EVtDtmfStateBuffered:
            dtmfState = CVtEngDtmfBuffered::NewL( *this, iH324Config );
            break;
        default:
            Panic( EVtEngPanicInvalidDtmfState );
            break;
        }
    CleanupStack::PushL( dtmfState );
    iDtmfStates->AppendL( dtmfState );
    CleanupStack::Pop();
    __VTPRINTEXIT( "DTMF.CreateStateL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::GetUIISupport
// returns the current UII support
// -----------------------------------------------------------------------------
//
CVtEngDtmfHandler::TUIISupport CVtEngDtmfHandler::GetUIISupport() const
    {
    __VTPRINT2( DEBUG_MEDIA , "DTMF. GETUSERINPUT support=%d", iUIISupport )
    return iUIISupport;
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::SetUIISupport
// Sets the UII support mode
// -----------------------------------------------------------------------------
//
void CVtEngDtmfHandler::SetUIISupport( CVtEngDtmfHandler::TUIISupport aValue )
    {
    __VTPRINT2( DEBUG_MEDIA , "DTMF. SETUSERINPUT support=%d", aValue )
    iUIISupport = aValue;
    }

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::CheckCommandId
// Compares last issued DTMF command ID to parameter given command ID
// -----------------------------------------------------------------------------
//
TBool CVtEngDtmfHandler::CheckCommandId( TInt aCheckCommandId ) const
	{
	__VTPRINT3( DEBUG_MEDIA , "DTMF. CHKCMID aCheckCommandId=%d iIssuedDtmfCommandId=%d",\
		aCheckCommandId, iIssuedDtmfCommandId )
	
	if( iIssuedDtmfCommandId == aCheckCommandId )
		{
		return ETrue;
		}
	
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CVtEngDtmfHandler::StoreCommandId
// Stores given command ID
// -----------------------------------------------------------------------------
//
void CVtEngDtmfHandler::StoreCommandId( TInt aStoreCommandId )
	{
	__VTPRINT2( DEBUG_MEDIA , "DTMF. STORECOMMANDID aStoreCommandId=%d", aStoreCommandId )
	
	iIssuedDtmfCommandId = aStoreCommandId;			
	}
//  End of File
