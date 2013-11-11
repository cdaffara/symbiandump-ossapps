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
* Description:  CSconOviAccountHandler implementation
*
*/


#include "sconoviaccounthandler.h"
#include <wsoviaccountmanager.h>
#include "debug.h"

// Constants
_LIT8( KUserId, "#UserID: " );
_LIT8( KFirstName, "\n#FirstName: " );
_LIT8( KLastName, "\n#LastName: " );
_LIT8( KEmail, "\n#Email: " );
_LIT8( KEmailVerified, "\n#EmailVerified: " );
_LIT8( KMobile, "\n#Mobile: " );
_LIT8( KMobileVerified, "\n#MobileVerified: " );



// -----------------------------------------------------------------------------
// CreateCSconOviAccountHandlerL()
// Entry to CSconOviAccountHandler
// -----------------------------------------------------------------------------
//
EXPORT_C CSconOviAccountHandler* CreateCSconOviAccountHandlerL()
    {
    TRACE_FUNC;
    return CSconOviAccountHandler::NewL();
    }

CSconOviAccountHandler* CSconOviAccountHandler::NewL()
    {
    TRACE_FUNC;
    CSconOviAccountHandler* self = new (ELeave) CSconOviAccountHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CSconOviAccountHandler::~CSconOviAccountHandler()
    {
    TRACE_FUNC;
    delete iOviAccountManager;
    }

void CSconOviAccountHandler::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    iOviAccountManager = CWSOviAccountManager::NewL();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// GetOviAccountDetailsL()
// Reads account info and prints details to aAccountInfoStream
// -----------------------------------------------------------------------------
//
void CSconOviAccountHandler::GetOviAccountDetailsL( RWriteStream& aAccountInfoStream )
    {
    TRACE_FUNC_ENTRY;
    
    // not owned
    CWSAccount* account = iOviAccountManager->Account();
    if ( !account )
        {
        // Account information not found.
        LOGGER_WRITE("Account information not found, leaving KErrNotFound");
        User::Leave( KErrNotFound );
        }
    
    // Write userId
    const TDesC8& userId = account->UserId();
    aAccountInfoStream.WriteL( KUserId );
    aAccountInfoStream.WriteL( userId );
    
    // Write first name
    HBufC8* temp = account->AccountItemL( KWSAccAttrAccountFirstName );
    if ( temp )
        {
        aAccountInfoStream.WriteL( KFirstName );
        aAccountInfoStream.WriteL( temp->Des() );
        delete temp;
        temp = NULL;
        }
    
    // Write last name
    temp = account->AccountItemL( KWSAccAttrAccountLastName );
    if ( temp )
        {
        aAccountInfoStream.WriteL( KLastName );
        aAccountInfoStream.WriteL( temp->Des() );
        delete temp;
        temp = NULL;
        }
    
    // Write email
    temp = account->AccountItemL( KWSAccAttrAccountEmail );
    if ( temp )
        {
        aAccountInfoStream.WriteL( KEmail );
        aAccountInfoStream.WriteL( temp->Des() );
        delete temp;
        temp = NULL;
        }
    
    // Write email verified (true/false)
    temp = account->AccountItemL( KWSAccAttrAccountEmailVerified );
    if ( temp )
        {
        aAccountInfoStream.WriteL( KEmailVerified );
        aAccountInfoStream.WriteL( temp->Des() );
        delete temp;
        temp = NULL;
        }
    
    // Write mobile number
    temp = account->AccountItemL( KWSAccAttrAccountMobile );
    if ( temp )
        {
        aAccountInfoStream.WriteL( KMobile );
        aAccountInfoStream.WriteL( temp->Des() );
        delete temp;
        temp = NULL;
        }
    
    // Write mobile verified
    temp = account->AccountItemL( KWSAccAttrAccountMobileVerified );
    if ( temp )
        {
        aAccountInfoStream.WriteL( KMobileVerified );
        aAccountInfoStream.WriteL( temp->Des() );
        delete temp;
        temp = NULL;
        }
        
    TRACE_FUNC_EXIT;
    }
