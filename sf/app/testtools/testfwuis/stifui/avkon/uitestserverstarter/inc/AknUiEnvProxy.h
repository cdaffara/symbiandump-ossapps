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
* Description: This file contains the header file of the Akn UI 
* Proxy class implementations.
*
*/

#ifndef AKNUIENVPROXY_H_
#define AKNUIENVPROXY_H_

//  INCLUDES
#include <stifinternal/UiEnvProxy.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CUITestServerStarterAppUi;

// CLASS DECLARATION

// DESCRIPTION
// Akn implementation of UI Proxy interface.
class CAknUiEnvProxy: public CUiEnvProxy
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
	    // None
	
	public:  // Constructors and destructor
	    /**
	    * C++ destructor.
	    */
	    ~CAknUiEnvProxy();
	
	public: // New functions
	    /**
	    * NewL is first phase of two-phased constructor.
	    */		
		static CAknUiEnvProxy* NewL( CUITestServerStarterAppUi* aAppUi );
		
    public: // Functions from base classes

		/**
		 * Parses key code.
		 */
		virtual TInt ParseKeyCode( const TDesC& aKeyCodeName, TUint& aKeyCode ) const;
	
		/**
		 * Parses key scan code.
		 */
		virtual TInt ParseKeyScanCode( const TDesC& aKeyScanCodeName, TInt& aKeyScanCode ) const;
	
		/**
		 * Parses key modifier.
		 */
		virtual TInt ParseModifier( const TDesC& aModifierName, TUint& aModifier ) const;
	    
		/**
		 * Parse pointer event type
		 */
		virtual TInt ParsePointerEventType( const TDesC& aPointerEventTypeName, TUint& aPointerEventType ) const;

		/**
		 * Brings UI control container to foreground.
		 */
		virtual void BringToForeground();

		/**
		 * Sends UI control container to background.
		 */
		virtual void SendToBackground();

		/**
		 * Sends local key event to UI control.
		 */
		virtual void PressKeyL( TRequestStatus* aStatus, TUint aKeyCode, TInt aKeyScanCode = 0, 
				   TUint aKeyModifiers = 0, TInt aKeyRepeats = 0 );

		/**
		 * Sends global key event to UI control.
		 */
		virtual void PressKeyL( TUint aKeyCode, TInt aKeyScanCode = 0, 
				   TUint aKeyModifiers = 0, TInt aKeyRepeats = 0 );

		/**
		 * Sends text to UI control.
		 */
		virtual void TypeTextL( TRequestStatus* aStatus, const TDesC& aText );

		/**
		 * Sends global text to UI control.
		 */
		virtual void TypeTextL( const TDesC& aText );
		
		/**
		 * Send pointer event
		 */
		virtual void SendPointerEventL( TUint aType, const TPoint& aPosition );		
		
		/**
		 * Send local pointer event
		 */
		virtual void SendPointerEventL( TRequestStatus* aStatus, TUint aType, const TPoint& aPosition );
		
    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None
        
    private:	// New functions
	    /**
	    * C++ default constructor.
	    */
		CAknUiEnvProxy();
    
	    /**
	    * By default Symbian OS constructor is private.
	    */
    	void ConstructL( CUITestServerStarterAppUi* aAppUi );
    
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
    	CUITestServerStarterAppUi* iAppUi;	// Pointer to AppUi
		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None
};

#endif // AKNUIENVPROXY_H_

// End of File
