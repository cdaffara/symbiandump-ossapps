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
* Description:  Defines interface for manipulating telephony contact matching
*                using ECOM plugin.
*
*/


#ifndef TELMATCHINGEXTENSION_H
#define TELMATCHINGEXTENSION_H

#include <e32base.h>
#include <badesca.h>
#include <data_caging_path_literals.hrh>

// Constants used in ECOM implementation
const TUid KTelMatchingExtensionUid    = { 0x2001E2A4 };


/**
 * Plugin interface class for manipulating telephony
 * contact matching.
 *
 * @since S60 5.0
 */
class CTelMatchingExtension : public CBase
    {
public:

    /**
    * Creates new xSP plugin having the given UID.
    * @leave Uses Leave code KErrNotFound if implementation is not found.
    * @param aImplementationUid Implementation UID of the plugin to be
    *        created.
    */
    static CTelMatchingExtension* NewL(
        TUid aImplementationUid );

    /**
    * Destructor
    */
    virtual inline ~CTelMatchingExtension();
    
    /**
    * Initialize CTelMatchingExtension after it is created with NewL.
    * @param aServiceId - This id identify what service is currently used in call.
    * @param aOrigAddress - Original address.
    */
    virtual void InitializeL( TUint aServiceId, const TDesC& aOrigAddress ) = 0;
    
    /**
    * Returns parsed address to be used in contact matching.
    * Sets aParsedAddress parameter as empty (KNullDesC) if original number should be used
    * in contact matching. 
    * @param aParsedAddress - Address parsed by plugin. This address will be used for matching.
    * @param aMeaningfulDigits - How many digits (starting from the right) are meaningful for matching.
    *                            Value of the parameter have to be between 4-20 otherwise the whole
    *                            of address will be used for matching (e.g. by value 0).
    *                            If length of the address is less than value of the parameter the whole
    *                            of address will be used for matching.
    * @return - Return KErrNone if operation succeed.
    */
    virtual TInt GetAddressForMatching( RBuf& aParsedAddress, TInt& aMeaningfulDigits ) = 0;

    /**
    * Returns contact store uris which are opened and will be used for matching.
    * @param aStoreUris - Contact store uri array.
    * @return - Return KErrNone if operation succeed.
    */
    virtual TInt GetContactStoreUris( CDesCArray& aStoreUris ) = 0;
    
    /**
    * Returns remote party name (remote party name is shown in the call bubble).
    * Sets aRemotePartyName parameter as empty (KNullDesC) if remote party name is not available.
    * Remote party name (aRemotePartyName) will be shown in the call bubble when contact 
    * matching not succeed.
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

#include "telmatchingextension.inl"


#endif // TELMATCHINGEXTENSION_H

//End of file

