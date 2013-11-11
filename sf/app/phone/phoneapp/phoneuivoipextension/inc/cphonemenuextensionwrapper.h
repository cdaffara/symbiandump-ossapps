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
* Description: Definition of CPhMenuExtensionWrapper class.
*
*/



#ifndef C_CPHONEMENUEXTENSIONWRAPPER_H
#define C_CPHONEMENUEXTENSIONWRAPPER_H

#include <e32base.h>
#include <telmenuextension.h>

/**
 *  Provides container friendly interface for extension plugin handling.
 *    
 *  @lib PhoneUIVoIPExtension.dll
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CPhMenuExtensionWrapper ) : public CTelMenuExtension
    {

public:

    /**
     * Two-phased constructor.
     * @param   aImplementationUid  An Ecom plugin implementation identifier.
     */
    static CPhMenuExtensionWrapper* NewL( TUid aImplementationUid );

    /**
     * Two-phased constructor.
     * @param   aImplementationUid  An Ecom plugin implementation identifier.
     */
    static CPhMenuExtensionWrapper* NewLC( TUid aImplementationUid );

    /**
    * Destructor.
    */
    virtual ~CPhMenuExtensionWrapper();

    /**
     * Provides comparison algorithm for CPhMenuExtensionWrapper objects using
     * extension plugin implementation identifier as a search key.
     * 
     * @param   aKey        A search key.
     * @param   aWrapper    An object to match with a key.
     * @since   S60 v5.1
     * @return  ETrue if match, EFalse otherwise.
     */
    static TBool MatchByUid( 
        const TUid* aKey, const CPhMenuExtensionWrapper& aWrapper );
    
    // from base class CTelMenuExtension

    /**
    * From CTelMenuExtension.
    * @see CTelMenuExtension.
    */
    void ModifyExtensionsCallMenuL( 
            const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
            CEikMenuPane& aMenuPane );

    /**
    * From CTelMenuExtension.
    * @see CTelMenuExtension.
    */
    void ModifyExtensionsTouchPaneButtonsL( 
            const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
            MTelTouchPaneExtension& aTouchPane );
    
    /**
    * From CTelMenuExtension.
    * @see CTelMenuExtension.
    */
    TBool HandleCommandL( TInt aCommand );

private:

    CPhMenuExtensionWrapper() {};
    
    CPhMenuExtensionWrapper( TUid aImplementationUid );

    void ConstructL();
    
    /**
     * Creates menu extension plugin.
     * @since   S60 v5.1
     * @return A new menu extension plugin instance.
     */
    CTelMenuExtension* CreateMenuExtensionL() const;
    
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
     * Menu extension plugin.
     * Own.
     */
    CTelMenuExtension* iPlugin;

    };

#endif // C_CPHONEMENUEXTENSIONWRAPPER_H
