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
* UITestServerStarter Document class.
*
*/

#ifndef UITESTSERVERSTARTERDOCUMENT_H_
#define UITESTSERVERSTARTERDOCUMENT_H_

//  INCLUDES
#include <AknDoc.h>

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
class CEikApplication;

// CLASS DECLARATION

// DESCRIPTION
/**
 * CClockTestDocument application class.
 * An instance of class CClockTestDocument is the Document part of the
 * AVKON application framework for the ClockTest example application.
 */
class CUITestServerStarterDocument : public CAknDocument
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
	    // None

	private:  // Constructors and destructor
		// None
	public: // New functions
	
	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CUITestServerStarterDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CUITestServerStarterDocument.
	 */
	static CUITestServerStarterDocument* NewL (CEikApplication& aApp);
	
	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CUITestServerStarterDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CUITestServerStarterDocument.
	 */
	static CUITestServerStarterDocument* NewLC (CEikApplication& aApp);
	
	/**
	 * ~CUITestServerStarterDocument
	 * Virtual Destructor.
	 */
	virtual ~CUITestServerStarterDocument();
	
	public: // Functions from base classes
	
		/**
		 * CreateAppUiL
		 * From CEikDocument, CreateAppUiL.
		 * Create a CUITestServerStarterAppUi object and return a pointer to it.
		 * The object returned is owned by the Uikon framework.
		 * @return Pointer to created instance of AppUi.
		 */
		CEikAppUi* CreateAppUiL ();
	
    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None

    private:	// New functions
	
		/**
		 * ConstructL
		 * 2nd phase constructor.
		 */
		void ConstructL ();
		
		/**
		 * CUITestServerStarterDocument.
		 * C++ default constructor.
		 * @param aApp Application creating this document.
		 */
		CUITestServerStarterDocument( CEikApplication& aApp );
    
	private:	// Functions from base classes
        // None    
    
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
        // None
		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None	
	};


#endif /*UITESTSERVERSTARTERDOCUMENT_H_*/

// End of File
