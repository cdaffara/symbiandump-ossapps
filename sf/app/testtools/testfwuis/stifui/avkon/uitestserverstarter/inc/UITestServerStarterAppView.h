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
* UITestServerStarter AppUi view class.
*
*/

#ifndef UITESTSERVERSTARTERAPPVIEW_H_
#define UITESTSERVERSTARTERAPPVIEW_H_

//  INCLUDES
#include <aknview.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CUITestServerStarterAppContainer;

// CLASS DECLARATION

// DESCRIPTION
// UITestServerStarter standard AppUi view class.
class CUITestServerStarterAppView : public CAknView
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
	    // None
	
	public:  // Constructors and destructor
		/**
		 * Virtual Destructor.
		 */
		virtual ~CUITestServerStarterAppView();
	
	private:  // Constructors and destructor

		/**
		 * Perform the second phase construction of a
		 * CUITestServerStarterAppView object.
		 */
		void ConstructL();

		/**
		 * C++ default constructor.
		 */
		CUITestServerStarterAppView();
	
	public: // New functions
		/**
		 * Two-phased constructor.
		 * Create a CUITestServerStarterAppView object.
		 */
		static CUITestServerStarterAppView* NewL();
	
		/**
		* Two-phased constructor.
		* Create a CUITestServerStarterAppView object
		*/
		static CUITestServerStarterAppView* NewLC();

	public: // Functions from base classes
	    /**
	    * Return Uid
	    */
	    TUid Id() const;
	
	    /**
	    * Handle Commands
	    */
	    void HandleCommandL(TInt aCommand);
	
	    /**
	    * Handle size changes
	    */
	    void HandleClientRectChange();
		
    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None
        
    private:	// New functions
    	// None

    private:	// Functions from base classes
	    /**
	    * From AknView, Activates view
	    */
	    void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
	        const TDesC8& aCustomMessage);
	
	    /**
	    * From AknView, Deactivates view
	    */
	    void DoDeactivate();
    
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
    	CUITestServerStarterAppContainer* iContainer; // View container
		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None
	};

#endif // UITESTSERVERSTARTERAPPVIEW_H_

// End of File
