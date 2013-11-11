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
* Description:  Defines interface for manipulating telephony call logging
*                using ECOM plugin.
*
*/


#ifndef TELLOGGINGEXTENSION_H
#define TELLOGGINGEXTENSION_H

#include <e32base.h>
#include <data_caging_path_literals.hrh>

// Constants used in ECOM implementation
const TUid KTelLoggingExtensionUid    = { 0x2001E2A2 };


/**
 * Plugin interface class for manipulating telephony
 * call logging.
 *
 * @since S60 5.0
 */
class CTelLoggingExtension : public CBase
    {
public:

    /**
    * Creates new xSP plugin having the given UID.
    * @leave Uses Leave code KErrNotFound if implementation is not found.
    * @param aImplementationUid Implementation UID of the plugin to be
    *        created.
    */
    static CTelLoggingExtension* NewL(
        TUid aImplementationUid );

    /**
    * Destructor
    */
    virtual inline ~CTelLoggingExtension();
    
    /**
    * Initialize CTelLoggingExtension after it is created with NewL.
    * @param aServiceId - This id identify what service is currently used in call.
    * @param aOrigAddress - Original address.
    */
    virtual void InitializeL( TUint aServiceId, const TDesC& aOrigAddress ) = 0;
    
    /**
    * Gets phone number to be logged to Logs events phone number field.
    * Sets aPhoneNumber parameter as empty (KNullDesC) if valid phone number is not available.
    * Phone number have to be valid for GSM calls.
    * @param aPhoneNumber - Number to be logged to the log events phone number field.  
    * @return - Return KErrNone if operation succeed.
    */
    virtual TInt GetPhoneNumber( RBuf& aPhoneNumber ) = 0;

    /**
    * Gets VoIP address to be logged to Logs events VoIP address field.
    * Sets aVoipAddress parameter as empty (KNullDesC) if valid VoIP address is not available.
    * @param aVoipAddress - Address to be logged to the log events VoIP address field.  
    * @return - Return KErrNone if operation succeed.
    */
    virtual TInt GetVoipAddress( RBuf& aVoipAddress ) = 0;
    
    /**
    * Gets users own address for Logs application.
    * Sets aMyAddress parameter as empty (KNullDesC) if valid users own address is not available.
    * My address option will be visible in Logs when this parameter is available.
    * @param aMyAddress - Address to be shown by Logs application "Show my address"
    *                     option.  
    * @return - Return KErrNone if operation succeed.
    */
    virtual TInt GetMyAddress( RBuf& aMyAddress ) = 0;
    
    /**
    * Gets remote party name.
    * Remote party name (returned by plugin) will be shown on the logs event when contact 
    * matching not succeed nor call was not made as private/anonymous call (MT call, 
    * private address/number).
    * @param aRemotePartyName - Remote party name.
    * @return - Return KErrNone if operation succeed.
    */
    virtual TInt GetRemotePartyName( RBuf& aRemotePartyName ) = 0;


private: // data

    /**
     * ECOM plugin instance UID.
     */
     TUid iDtor_ID_Key;

    };

#include "telloggingextension.inl"

#endif // TELLOGGINGEXTENSION_H


