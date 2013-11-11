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
* Description: This file contains the header file of the 
* UITestServerStarter AppUi class
*
*/

#ifndef UITESTSERVERSTARTERAPPUI_H_
#define UITESTSERVERSTARTERAPPUI_H_

//  INCLUDES
#include <aknViewAppUi.h> 

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CUITestServerStarterAppView;

// CLASS DECLARATION

// DESCRIPTION
// UITestServerStarter AppUi class
class CUITestServerStarterAppUi : public CAknViewAppUi
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
		/**
		 * Currently handled event type.
		 */
		enum TEventType { 
			ENone,		// None 
			EPressKey,  // Press key event
			ETypeText, 	// Type text event
			EPointerEvent, // Pointer event
			};
	
	public:  // Constructors and destructor
		/**
		 * C++ default constructor. This needs to be public due to
		 * the way the framework constructs the AppUi
		 */
		CUITestServerStarterAppUi();
	
		/**
		 * Virtual Destructor.
		 */
		virtual ~CUITestServerStarterAppUi();
	
	public: // New functions
		/**
		 * ConstructL.
		 * 2nd phase constructor.
		 */
		void ConstructL();

		/**
	     * Prepares AppUi to recive type text event
	     */	
		virtual void PrepareToTypeText( TInt aTextLength );
	
		/**
	     * Prepares AppUi to recive key press event
	     */	
		virtual void PrepareToPressKey();			

		/**
	     * Prepares AppUi to recive pointer event
	     */	
		virtual void PrepareToPointerEvent();			
		
		/**
	     * Notifies that key press event was recived.
	     */	
		virtual void KeyPressHandled();
	
		/**
	     * Notifies that text type event was recived.
	     */	
		virtual void TextTypeHandled();
		
		/**
	     * Notifies that pointer event was recived.
	     */	
		virtual void PointerEventHandled();
		
		/**
	     * Requests notification when key press event is handled.
	     */	
		virtual void NotifyAboutHandledKeyPress( TRequestStatus* aStatus );
	
		/**
	     * Requests notification when text type event is handled.
	     */	
		virtual void NotifyAboutHandledTextType( TRequestStatus* aStatus );	

		/**
	     * Requests notification when pointer event is handled.
	     */	
		virtual void NotifyAboutHandledPointerEvent( TRequestStatus* aStatus );	

	public: // Functions from base classes
		/**
	     * Overwritten version of CCoeAppUi::HandleWsEventL() method.
	     */	
		void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );

    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None
        
    private:	// New functions
    	// None

    private: 	// Functions from base classes
		/**
		 * From CEikAppUi, HandleCommandL.
		 * Takes care of command handling.
		 * @param aCommand Command to be handled.
		 */
		void HandleCommandL( TInt aCommand );
	        
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
		CUITestServerStarterAppView* iAppView;	// The application view owned by CUITestServerStarterAppUi		
		TRequestStatus* iEventStatus;	// Pointer to request statuc which is completed when 
										// selected event is handled.		
		TEventType iEventType;	// Currently handled event type;		
		TInt iTypeTextLength;	// Length of text send by typetext testscripter keyword
		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None
	};

#endif // UITESTSERVERSTARTERAPPUI_H_

// End of File
