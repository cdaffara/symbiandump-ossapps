/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCntParserServerSession implementation.
*
*/


#include "cntparserserversession.h"
#include <cntdb.h>
#include "cntparsercollector.h"
#include "cntparserinfolog.h"
#include "irmcconsts.h"

#include "debug.h"

//------------------------------------------------------------
// CCntParserServerSession::CCntParserServerSession()
//------------------------------------------------------------
CCntParserServerSession::CCntParserServerSession()
    {
    }
//------------------------------------------------------------
// CCntParserServerSession::NewL(CCntParserServer* aServer)
//------------------------------------------------------------
CCntParserServerSession* CCntParserServerSession::NewL( CCntParserServer* aServer )
    {
    LOGGER_ENTERFN( "CntParserServerSession::NewL()" );

    CCntParserServerSession* self = new( ELeave ) CCntParserServerSession();
    CleanupStack::PushL( self );
    self->ConstructL( aServer );
    CleanupStack::Pop( self );
    LOGGER_LEAVEFN( "CntParserServerSession::NewL()" );
    return self;
    }
//------------------------------------------------------------
// CCntParserServerSession::ConstructL( CCntParserServer* aServer )
//------------------------------------------------------------
void CCntParserServerSession::ConstructL( CCntParserServer* aServer )
    {
    iContactsParserSrv = aServer;
    iContactsParserSrv->IncreaseSessionCountL();
    }
//------------------------------------------------------------
// CCntParserServerSession::~CCntParserServerSession()
//------------------------------------------------------------
CCntParserServerSession::~CCntParserServerSession()
    {
    LOGGER_ENTERFN( "CntParserServerSession::~CCntParserServerSession()" );
    if ( iContactsParserSrv )
        {
        iContactsParserSrv->DecreaseSessionCount();
        }
    LOGGER_LEAVEFN( "CntParserServerSession::~CCntParserServerSession()" );
    }
//------------------------------------------------------------
// CCntParserServerSession::ServiceL( const RMessage2& aMessage )
//------------------------------------------------------------
void CCntParserServerSession::ServiceL( const RMessage2& aMessage )
    {
    LOGGER_WRITE( "CntParserServerSession: ServiceL" );

    TInt err( KErrNone );
    TInt ret( KErrNone );

    TRAP( err, ret = DispatchMessageL( aMessage ) );

    if ( err != KErrNone )
        {
        LOGGER_WRITE_1( "CntParserServerSession: ServiceL: Error trapped, complete with ERR code %d", err );

        iPhoneBookRequest = EFalse; //If requested, then clear request flag
        iContactsParserSrv->SetRequests( 0 );   //Reset requests count in server to allow new requests to start..
        aMessage.Complete( err ); //If trap error, complete with it
        return;
        }   

    if ( ret == KErrNone )
        {
        LOGGER_WRITE( "CntParserServerSession: ServiceL: No complete" );
        return;
        }

    if ( ret == KPerformComplete )
        {
        LOGGER_WRITE( "CntParserServerSession: ServiceL: Normal complete" );

        aMessage.Complete( KErrNone );
        return;
        }

    if ( ret == KPBNotUpdated )
        {
        LOGGER_WRITE( "CntParserServerSession: ServiceL: Complete (PB not updated)" );

        iPhoneBookRequest = EFalse;
        aMessage.Complete( KPBNotUpdated );
        return;
        }
    LOGGER_WRITE( "CntParserServerSession: ServiceL: Error, should never come here" );
    aMessage.Complete( KErrGeneral );
    }
//------------------------------------------------------------
// CCntParserServerSession::CompletePhoneBookRequest( TInt aErr )
//------------------------------------------------------------
void CCntParserServerSession::CompletePhoneBookRequest( TInt aErr )
    {
    LOGGER_WRITE( "CntParserServerSession: CompletePhoneBookRequest" );

    if ( iPhoneBookRequest ) //If this session has requested phonebook, then the completion should be done here, as it was not done in ServiceL
        {
        LOGGER_WRITE( "CntParserServerSession: CompletePhoneBookRequest: collector or cancel" );
        iPhoneBookRequest = EFalse;
        iPhoneBookRequestMessage.Complete( aErr );
        }
    else
        {
        LOGGER_WRITE( "CntParserServerSession: CompletePhoneBookRequest: No complete for this session" );
        }
    }
//------------------------------------------------------------
// CCntParserServerSession::DispatchMessageL( const RMessage2 &aMessage )
//------------------------------------------------------------
TInt CCntParserServerSession::DispatchMessageL( const RMessage2 &aMessage )
    {
    LOGGER_WRITE( "CntParserServerSession: DispatchMessageL" );

    TInt res( KPerformComplete );

    switch( aMessage.Function() )
        {
        case ECntParserServerCreateIrMCL2:  //Create pb.vcf file
            {
            res = ETrue;

            if ( iPhoneBookRequest == EFalse )
                {
                iPhoneBookRequestMessage = aMessage;
                iPhoneBookRequest = ETrue;

                TInt sid=aMessage.SecureId();
                TInt MaxNumOfContacts = aMessage.Int0();
                TBool Forced = aMessage.Int1();
                TBuf<8> sidLit;
                sidLit.AppendNumFixedWidth( sid, EHex, 8);
                TBuf<255> path;
                path.Insert( 0, KPhoneBookFilename );

                res = iContactsParserSrv->CreateIrMCL2PhoneBookL( path, (TInt) MaxNumOfContacts, Forced );
                }
            break;
            }
        case ECntParserServerCntParsed: //How many contacts already parsed
            {
            TPckgBuf<TInt> p((TInt) iContactsParserSrv->ContactsParsed());
            aMessage.WriteL( 0, p, 0 );
            break;
            }
        case ECntParserServerTotalNumOfContacts: //How many contacts total?
            {
            TPckgBuf<TInt> p((TInt) iContactsParserSrv->TotalNumOfContacts());
            aMessage.WriteL( 0, p, 0 );
            break;
            }
        case ECntParserServerDisconnect:    //Disconnect session from server
            {
            iContactsParserSrv->Disconnect();
            break;
            }
        case ECntParserServerCancel:    //Cancel phone book request
            {
            if (iPhoneBookRequest)
                {
                iContactsParserSrv->CancelPhoneBookRequest();
                CompletePhoneBookRequest( KErrCancel );
                }
            break;
            }
        case ECntParserServerCntSaved: //How many contacts total?
            {
            TPckgBuf<TInt> p((TInt) iContactsParserSrv->ContactsSaved());
            aMessage.WriteL( 0, p, 0 );
            break;
            }
        case ECntParserServerCancelAll: //Cancel all requests, not just current?
            {
            iContactsParserSrv->CancelAllPhoneBookRequests();
            break;
            }
        // requests we don't understand
        // so panic the client here, this function also completes the message
        default:
            {
            PanicClient( EBadRequest );
            break;
            }
        }
    return res;
    }
//------------------------------------------------------------
// CCntParserServerSession::PanicClient(TInt aPanic) const
//------------------------------------------------------------
void CCntParserServerSession::PanicClient(TInt aPanic) const
    {
    User::Panic( KTxtServer,aPanic );
    }
    
// end of file
