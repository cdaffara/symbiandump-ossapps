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
* Description: 
*     Contains mailbox specified always online logic
*
*/

// INCLUDE FILES
#include "ipssosaopluginheaders.h"

// Literals used when trying to match a mailbox 
// to a received EMN message
_LIT(KIpsSosAOMailatPattern, "*mailat:*@*");
// Same as KImapPattern, but includes also username
_LIT(KIpsSosAOImapUserPattern, "*imap://*@*"); 
// Include username and auth
_LIT(KIpsSosAOPopUserPattern, "*pop://*;*@*");    
// Same as KPopUserPattern, but does not include auth
_LIT(KIpsSosAOPopUserNoAuthPattern, "*pop://*@*");  
// Literals used when trying to parse username, server
_LIT(KIpsSosAOForwardSlashes, "//");
_LIT(KIpsSosAOAtSign, "@");
_LIT(KIpsSosAOSemicolon, ";");


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOEMNResolver::CIpsSosAOEMNResolver()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOEMNResolver::~CIpsSosAOEMNResolver()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOEMNResolver::ConstructL()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOEMNResolver* CIpsSosAOEMNResolver::NewL()
    {
    FUNC_LOG;
    CIpsSosAOEMNResolver* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOEMNResolver* CIpsSosAOEMNResolver::NewLC()
    {
    FUNC_LOG;
    CIpsSosAOEMNResolver* self = 
        new ( ELeave ) CIpsSosAOEMNResolver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TInt CIpsSosAOEMNResolver::FindEMNMailbox( 
    const TDesC& aMailboxURI, 
    const RPointerArray<CIpsSosAOMBoxLogic>& aMailboxLogics )
    {
    FUNC_LOG;
    TInt retIndex = KErrNotFound;
    if ( aMailboxLogics.Count() == 0 )
        {
        return KErrNotFound;
        }
    
    TBuf<KIpsSosAOTextBufferSize> emailAddress;
    TBuf8<KIpsSosAOTextBufferSize> username;
    TBuf<KIpsSosAOTextBufferSize> incomingServer;
    
    const TInt count = aMailboxLogics.Count();

    for ( TInt loop = 0; (loop < count) && (retIndex == KErrNotFound); loop++)
        {
        TBool isEmn = aMailboxLogics[loop]->IsEmnOn() ||
        !aMailboxLogics[loop]->FirstEMNReceived();
        
        // We are only interested of those mailboxes which have EMN on.
        if ( isEmn )
            {
            // Get needed settings
            aMailboxLogics[loop]->GetEmailAddress( emailAddress );
            aMailboxLogics[loop]->GetAgent().GetServerAddress( incomingServer );
            aMailboxLogics[loop]->GetAgent().GetUsername( username );
            TBool isImap = aMailboxLogics[loop]->GetAgent().IsTypeImap4();
            
            
            // <emn mailbox="mailat:usernamemuimaumail.somehost.com"/>
            if ( aMailboxURI.Match( KIpsSosAOMailatPattern ) == 0 && 
                 aMailboxURI.Find( emailAddress ) > 0 )
                {
                retIndex = loop;
                }
            // <emn mailbox="imap://usernamemiumaumail.somehost.com"/>
            else if ( aMailboxURI.Match( 
                    KIpsSosAOImapUserPattern ) == 0 && isImap )
                {
                if ( HandleEmnImapUserURI( aMailboxURI, username, incomingServer ) )
                    {
                    retIndex = loop;
                    }
                }
            // <emn mailbox="pop://userxyz;auth=3598302miumaumail.somehost.com"/>
            else if ( aMailboxURI.Match( 
                    KIpsSosAOPopUserPattern ) == 0 && !isImap )
                {
                if ( HandleEmnPopUserURI( aMailboxURI, username, incomingServer ) )
                    {
                    retIndex = loop;
                    }
                }
            // <emn mailbox="pop://userxyzmiumaumail.somehost.com"/>
            else if ( aMailboxURI.Match( 
                    KIpsSosAOPopUserNoAuthPattern ) == 0 && !isImap )
                {
                if ( HandleEmnPopNoAuthURI( aMailboxURI, username, incomingServer ) )
                    {
                    retIndex = loop;
                    }
                }
            else
                {
                }
            }
        else
            {
            }
        }
    return retIndex;  
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOEMNResolver::ParameterDispatchTEMNElement( 
    const TDesC8& aParameters,
    TEMNElement& aElement ) const
    {
    FUNC_LOG;
    // Presume that wrong type of parameter is given
    TInt err = KErrNotSupported;

    // Unpack parameters to see to which mailbox EMN is for
    TPckgBuf<TEMNElement> paramPack;

    // Make sure that the parameter length matches TEMNElement length and
    // extract mailbox and timestamp values.
    if ( aParameters.Size() == sizeof( TEMNElement ) )
        {

        paramPack.Copy( aParameters );
        
        aElement.mailbox.Zero();
        aElement.mailbox.Append( paramPack().mailbox );
        
        aElement.timestamp = paramPack().timestamp;
                
        err = KErrNone;
        }
    else
        {
        err = KErrArgument;
        }
    return err;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOEMNResolver::HandleEmnImapUserURI(
    const TDesC& aURI,
    const TDesC8& aUsername,
    const TDesC& aServer ) const    
    {
    FUNC_LOG;
    // Try to match with following EMN message
    // <emn mailbox="imap://usernamemiumaumail.somehost.com/folder;UIDVALIDITY=385759045/;UID=20"/>
    // <emn mailbox="imap://usernamemiumaumail.somehost.com/"/>
    
    // endPart = usernamemiumaumail.somehost.com/folder;UIDVALIDITY=385759045/;UID=20
    TInt pos = aURI.Find( KIpsSosAOForwardSlashes );
    // Step over forward slashes
    pos += 2;
    // Strip "imap://" away
    TPtrC endPart( aURI.Right( aURI.Length() - ( pos ) ) );

    // Username is from beginning to @ character
    pos = endPart.Find( KIpsSosAOAtSign );    
    TBuf8<KIpsSosAOTextBufferSize> user;
    user.Copy( endPart.Left( pos ) ); 
    
    // Strip username and '@' character away
    endPart.Set( endPart.Right( endPart.Length() - pos - 1 ) );
    TPtrC server( endPart );
    
    // Do we have a winner?
    TBool ret = ( aUsername.Compare( user ) == 0 &&
             server.Find( aServer ) == 0 );
    return ret;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOEMNResolver::HandleEmnPopUserURI(
    const TDesC& aURI,
    const TDesC8& aUsername,
    const TDesC& aServer ) const
    {
    FUNC_LOG;
    // Try to match with following EMN message
    // <emn mailbox="pop://userxyz;auth=3598302miumaumail.somehost.com"/>
    
    TInt pos = aURI.Find( KIpsSosAOForwardSlashes );
    // Step over forward slashes
    pos += 2;
    // Strip "pop://" away
    TPtrC endPart( aURI.Right( aURI.Length() - ( pos ) ) );
    
    // Username is from beginning to ; character
    pos = endPart.Find( KIpsSosAOSemicolon );    
    TBuf8<KIpsSosAOTextBufferSize> user;
    user.Copy( endPart.Left( pos ) ); 

    // ";auth=3598302" omitted
    
    // server name
    pos = endPart.Find( KIpsSosAOAtSign );
    // Step over at sign
    pos++;
    
    // server = mail.somehost.com
    TPtrC server( endPart.Right( endPart.Length() - pos ) );
    
    // Do we have a winner?
    TBool ret = ( aUsername.Compare( user ) == 0 &&
             aServer.Compare( server ) == 0 );
    return ret;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOEMNResolver::HandleEmnPopNoAuthURI( 
    const TDesC& aURI,
    const TDesC8& aUsername,
    const TDesC& aServer ) const
    {
    FUNC_LOG;
    // Try to match with following EMN message
    // <emn mailbox="pop://userxyzmiumaumail.somehost.com"/>

    TInt pos = aURI.Find( KIpsSosAOForwardSlashes );
    // Step over forward slashes
    pos += 2;
    // Strip "pop://" away
    TPtrC endPart( aURI.Right( aURI.Length() - ( pos ) ) );
        
    // Username is from beginning to @ character
    pos = endPart.Find( KIpsSosAOAtSign );
    TBuf8<KIpsSosAOTextBufferSize> user;
    user.Copy( endPart.Left( pos ) ); 
    // Step over at sign
    pos++;
    // server = mail.somehost.com
    TPtrC server( endPart.Right( endPart.Length() - pos ) );    

    // Do we have a winner?
    TBool ret = ( aUsername.Compare( user ) == 0 &&
             aServer.Compare( server ) == 0 );
    return ret;
    }

// End of File

