/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  contains common variables and default values
*
*/


#ifndef IPSPLGCOMMON_H_
#define IPSPLGCOMMON_H_

#include <e32base.h>

#include "ipsplgsosbaseplugin.hrh"

_LIT( KIpsPlgInbox, "Inbox");
const TUid KIpsPlgImap4PluginUid = { IPSSOSIMAP4PLUGIN_IMPLEMENTATION_UID };
const TUid KIpsPlgPop3PluginUid = { IPSSOSPOP3PLUGIN_IMPLEMENTATION_UID };

const TUint KIpsPlgImap4PluginUidValue = IPSSOSIMAP4PLUGIN_IMPLEMENTATION_UID;
const TUint KIpsPlgPop3PluginUidValue = IPSSOSPOP3PLUGIN_IMPLEMENTATION_UID;

const TUid KCRUidExtendedSettingsUid   = { IPS_EXTENDED_SETTINGS_UID };

const TUid KIpsPlgPropertyCatUid  = { IPS_PROPERTY_CAT_UID };

const TInt KIpsPlgMaxPhoneIdLength  = 50;
//<qmail>
const TInt KIpsSetDataHeadersOnly           = -2;
const TInt KIpsSetDataFullBodyAndAttas      = -1;
const TInt KIpsSetDataFullBodyOnly          = -3;
//</qmail>
const TInt KContinueInterval = 3000000; // 3 sec

static _LIT_SECURITY_POLICY_PASS( KAllowAllPolicy );
static _LIT_SECURITY_POLICY_C1( KAllowWriteDeviceDataPolicy, ECapabilityWriteDeviceData );

//<qmail>
_LIT( KIpsPlgReplySubjectFormat, "%S" );
_LIT( KIpsPlgForwardSubjectFormat, "%S" );
//</qmail>

/**
 * class TIpsMailboxState
 */
class TIpsMailboxState
    {
public:
    TIpsMailboxState( TInt aMailbox, TInt aState ) :
        iMailbox(aMailbox), iState(aState) { }
    TIpsMailboxState( ) : iMailbox(0), iState( KErrNotFound ) { }
    
    TInt iMailbox;
    TInt   iState;
    };

/**
* TIpsPlgPropertyEvent
*
*/
class TIpsPlgPropertyEvent
    {
public:
    inline TIpsPlgPropertyEvent( 
            TInt aEvent, TInt aMailbox, TInt aPluginId, TInt aError ) : 
        iEvent(aEvent), iMailbox(aMailbox),
        iPluginId(aPluginId) ,iError(aError) { }
    
    inline TIpsPlgPropertyEvent( ) : 
        iEvent(0), iMailbox(0), iPluginId(KErrNotFound), iError(0) { }
    
    TInt iEvent;
    TInt iMailbox;
    TInt iPluginId;
    TInt iError;
    };

#endif /*IPSPLGCOMMON_H_*/
