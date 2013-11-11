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
* Description: 
*     Main plugin class
*
*/

#include "ipssosaopluginheaders.h"

#ifdef _DEBUG
_LIT( KIpsSosAOSmtpLogicPanicLit, "AOsmtp");
#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSmtpAgent* CIpsSosAOSmtpAgent::NewL( 
        CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSosAOSmtpAgent* self = CIpsSosAOSmtpAgent::NewLC( 
            aSession );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSmtpAgent* CIpsSosAOSmtpAgent::NewLC( 
        CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSosAOSmtpAgent* self = new( ELeave ) CIpsSosAOSmtpAgent( 
            aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;     
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSmtpAgent::CIpsSosAOSmtpAgent( 
        CMsvSession& aSession ) 
    : CActive( EPriorityStandard ), iSession( aSession )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpAgent::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSmtpAgent::~CIpsSosAOSmtpAgent()
    {
    FUNC_LOG;
    Cancel();
    ReleaseInternalData( );
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpAgent::DoCancel()
    {
    FUNC_LOG;
    if ( iOperation && iOperation->IsActive() )
        {
        iOperation->Cancel();
        }
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpAgent::RunL()
    {
    FUNC_LOG;
    if ( iStatus != KErrNone )
        {
        iError = this->iStatus.Int();
        }
    else
        {
        ReleaseInternalData( );
        }
    
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpAgent::EmptyOutboxNowL( TMsvId aMessage )
    {
    FUNC_LOG;

    if ( iOperation && iOperation->IsActive() )
        {
        __ASSERT_DEBUG(EFalse, 
                User::Panic( KIpsSosAOSmtpLogicPanicLit, KErrServerBusy) );
        // leave if not debug mode
        User::Leave( KErrServerBusy );
        }
    CreateInternalDataL( );
    
    iOperation->StartSendL( aMessage );
    iStatus = KRequestPending;
    SetActive();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpAgent::CreateInternalDataL( )
    {
    FUNC_LOG;
    if ( !iOperation )
        {
		//<QMail>
        iOperation = CIpsPlgSmtpOperation::NewL(
            iSession, iStatus  );
        }
    
	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSmtpAgent::ReleaseInternalData( )
    {
    FUNC_LOG;
    if ( iOperation && iOperation->IsActive() )
        {
        iOperation->Cancel();
        }
        
    else if ( iOperation && !iOperation->IsActive() )
        {
        delete iOperation;
        iOperation = NULL;
        }
	//<QMail>

    //</QMail>
    }


//EOF

