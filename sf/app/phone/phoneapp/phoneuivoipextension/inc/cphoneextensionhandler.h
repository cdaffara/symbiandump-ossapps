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
* Description:  Extension handler class.
*
*/


#ifndef C_CPHONEEXTENSIONHANDLER_H
#define C_CPHONEEXTENSIONHANDLER_H

// INCLUDES
#include <telmenuextension.h>
#include <spdefinitions.h>
 
// FORWARD DECLARATIONS
class CPhMenuExtensionWrapper;

// CLASS DECLARATION
/**
*  Extension handler class
*/
NONSHARABLE_CLASS( CPhoneExtensionHandler ): public CBase
    {
        
    public:      

        /**
         * Destructor.
         */
        virtual ~CPhoneExtensionHandler();

        /**
         * Creates the extension handler class
         */
        static CPhoneExtensionHandler* NewL();
                        
        /**
         * Modifies menu when there is active call.
         * @param aServiceId Service id of the call.
         * @param aArray Active call array
         * @param aResourceId Menu resource id.
         * @param aMenuPane Menu pane
         */
        void ModifyInCallMenuL( 
                TUint aServiceId,
                RArray<CTelMenuExtension::TCallInfo>& aArray, 
                TInt aResourceId, 
                CEikMenuPane& aMenuPane );

        /**
         * Handles custom menu command.
         * @param aServiceId Service id of the call.
         * @param aCommand Command id.
         * @return ETrue if command handled.
         */
        TBool HandleCommandL( 
                TUint aServiceId,
                TInt aCommand );
            
    protected:

        CPhoneExtensionHandler();
        
        void ConstructL();
        
    private:                
        
        
        /**
         * Resolves extensions implementation uid for 
         * given service id.
         * @leave KErrNotFound if extension not exists for service.
         * @param aServiceId Service id
         * @param aPropertyName Service table property name.
         * @return Implementaion uid.
         */
        TUid ResolveImplementationUidL( TUint32 aServiceId, 
                                        TServicePropertyName aPropertyName );

        /**
         * Adds unattended transfer menu item to menu.
         * @param aMenuPane Menu pane.
         */
        void AddUnattendedTransferMenuItemL( CEikMenuPane& aMenuPane );

        /**
         * Returns menu extension by service id.
         * @leave Leaves if not found.
         * @param  aServiceId Service id of call
         */
        CTelMenuExtension* MenuExtensionL( TUint aServiceId );
        
        
    private: // Data

        
        /**
         * Menu extension plugins.
         * Own.
         */
        RPointerArray<CPhMenuExtensionWrapper> iPlugins;
    };

#endif // C_CPHONEVOIPUTILITY_H

// End of File
