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
* Description: Definition of CPELogExtensionWrapper class.
*
*/



#ifndef C_CPELOGEXTENSIONWRAPPER_H
#define C_CPELOGEXTENSIONWRAPPER_H

#include <e32base.h>
#include <telloggingextension.h>

/**
 *  Provides container friendly interface for extension plugin handling.
 *    
 *  @lib loghandling.dll
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CPELogExtensionWrapper ) : public CTelLoggingExtension
    {

public:

    /**
     * Two-phased constructor.
     * @param   aImplementationUid  An Ecom plugin implementation identifier.
     */
    static CPELogExtensionWrapper* NewL( TUid aImplementationUid );

    /**
     * Two-phased constructor.
     * @param   aImplementationUid  An Ecom plugin implementation identifier.
     */
    static CPELogExtensionWrapper* NewLC( TUid aImplementationUid );

    /**
    * Destructor.
    */
    virtual ~CPELogExtensionWrapper();

    /**
     * Provides comparison algorithm for CPELogExtensionWrapper objects using
     * extension plugin implementation identifier as a search key.
     * 
     * @param   aKey        A search key.
     * @param   aWrapper    An object to match with a key.
     * @since   S60 v5.1
     * @return  ETrue if match, EFalse otherwise.
     */
    static TBool MatchByUid( 
        const TUid* aKey, const CPELogExtensionWrapper& aWrapper );
    
    // from base class CTelLoggingExtension
    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    void InitializeL( TUint aServiceId, const TDesC& aOrigAddress );
    
    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    TInt GetPhoneNumber( RBuf& aPhoneNumber );

    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    TInt GetVoipAddress( RBuf& aVoipAddress );
    
    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    TInt GetMyAddress( RBuf& aMyAddress );
    
    /**
    * From CTelLoggingExtension.
    * @see CTelLoggingExtension.
    */
    TInt GetRemotePartyName( RBuf& aRemotePartyName );

private:

    CPELogExtensionWrapper() {};
    
    CPELogExtensionWrapper( TUid aImplementationUid );

    void ConstructL();
    
    /**
     * Creates logging extension plugin.
     * @since   S60 v5.1
     * @return A new logging extension plugin instance.
     */
    CTelLoggingExtension* CreateLoggingExtensionL() const;
    
    /**
     * Returns identifier of the wrapped extension plugin.
     * @since   S60 v5.1
     * @return Identifier of the extension plugin.
     */
    TUid Identifier() const;

private: // data

    /**
     * Extension plugin identifier.
     */
    TUid iPluginUid;

    /**
     * Logging extension plugin.
     * Own.
     */
    CTelLoggingExtension* iPlugin;

    };

#endif // C_CPELOGEXTENSIONWRAPPER_H
