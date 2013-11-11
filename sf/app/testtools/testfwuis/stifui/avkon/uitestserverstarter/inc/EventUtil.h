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
* Description: This file contains TEventUtil class declaration.
*
*/

#ifndef EVENTUTIL_H_
#define EVENTUTIL_H_

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION
// Utility class for key codes parsing.
class TEventUtil
    {
    private: // Enumerations
    	// none

    public: // Enumerations
		enum TCustomPointerEventType { EButton1 = 1000, EButton2, EButton3 };
    
    public: // Constructors and destructor

    public: // Constructors and destructor

    public: // New functions
    	/**
    	 * Returns descriptive key code name.
    	 */
        static TPtrC GetKeyCodeName( TUint aKeyCode );
        
        /**
         * Parses key code.
         */ 
        static TInt ParseKeyCode( const TDesC& aKeyCodeName, TUint& aKeyCode );

		/**
		 * Returns descriptive key scan code name.
		 */
        static TPtrC GetKeyScanCodeName( TInt aKeyScanCode );
        
        /**
         * Parses key scan code.
         */
        static TInt ParseKeyScanCode( const TDesC& aKeyScanCodeName, TInt& aKeyScanCode );

        /**
         * Returns descriptive key modifier name.
         */
        static TPtrC GetModifierName( TUint aModifier );
        
        /**
         * Parses key modifier.
         */
        static TInt ParseModifier( const TDesC& aModifierName, TUint& aModifier );
        
        /**
         * Parses pointer event type.
         */
        static TInt ParsePointerEventType( const TDesC& aPointerEventTypeName, TUint& aPointerEventType );

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif /*EVENTUTIL_H_*/

// End of File
