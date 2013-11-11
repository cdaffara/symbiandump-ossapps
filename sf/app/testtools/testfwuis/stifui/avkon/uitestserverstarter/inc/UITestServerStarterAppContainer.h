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
* UITestServerStarter AppUi container class.
*
*/

#ifndef UITESTSERVERSTARTERAPPCONTAINER_H_
#define UITESTSERVERSTARTERAPPCONTAINER_H_

//  INCLUDES
#include <coecntrl.h>
#include <coemop.h>


// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

// DESCRIPTION
// UITestServerStarter standard AppUi container class.
class CUITestServerStarterAppContainer: public CCoeControl
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
	    // None
	
	public:  // Constructors and destructor
		/**
		 * Virtual Destructor.
		 */
		~CUITestServerStarterAppContainer();

	private:  // Constructors and destructor
		/**
		 * C++ default constructor.
		 */
		CUITestServerStarterAppContainer();

		/**
		 * Perform the second phase construction of a
		 * CUITestServerStarterAppView object.
		 */
		void ConstructL( const TRect& aRect );
		
	public: // New functions
		/**
		 * Two-phased constructor.
		 * Create a CUITestServerStarterAppContainer object, which will draw itself to aRect.
		 */
		static CUITestServerStarterAppContainer* NewL( const TRect& aRect );

		/**
		* Two-phased constructor.
		* Create a CUITestServerStarterAppContainer object, which will draw itself
		*/
		static CUITestServerStarterAppContainer* NewLC( const TRect& aRect );

		
		/**
		 * This function is used to allow controls to ask their owners 
		 * for access to other objects that they own
		 */
		TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
	public: // Functions from base classes
		// None
	
    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None
        
    private:	// New functions
    	// None

    private:	// Functions from base classes
    
    	/**
    	 * Draws the control
    	 */
        void Draw( const TRect& aRect ) const;
        
        /**
         * Responds to changes to the size and position of the 
         * contents of this control
         */
        void SizeChanged();
    
        //void HandleResourceChange( TInt aType );
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
    	CAknsBasicBackgroundControlContext* iBgContext; // Skin background object context pointer
		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None
	};

#endif /*UITESTSERVERSTARTERAPPCONTAINER_H_*/

// End of File
