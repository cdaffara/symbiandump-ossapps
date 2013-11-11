/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Handles MMS settings in provisioning.
*
*/


#ifndef CWPWAPMMSADAPTER_H
#define CWPWAPMMSADAPTER_H

// INCLUDES
#include "CWPWAPAdapterBase.h"

// FORWARD DECLARATIONS
class CWPCharacteristic;

// CLASS DECLARATION

/**
 * CWPWAPMMSAdapter handles MMS settings.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class CWPWAPMMSAdapter : public CWPWAPAdapterBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWPWAPMMSAdapter* NewL();

        /**
        * Destructor.
        */
        ~CWPWAPMMSAdapter();

    public: // from CWPWAPAdapterBase
        
        void ApplicationL( CWPCharacteristic& aCharacteristic );
        
    private:  // New functions
        /**
        * C++ default constructor.
        */
        CWPWAPMMSAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        // Title for MMS items. Owns.
        HBufC* iTitle;

        // Default name for MMS items. Owns.
        HBufC* iDefaultName;
    };

#endif  // CWPWAPMMSADAPTER_H
            
// End of File
