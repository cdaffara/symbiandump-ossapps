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
* CUITestServerStarterApplication class.
*
*/

#ifndef UITESTSERVERSTARTERAPPLICATION_H_
#define UITESTSERVERSTARTERAPPLICATION_H_


//  INCLUDES
#include <aknapp.h>
#include "UITestServerStarter.hrh"

// CONSTANTS

// UID for the application;
const TUid KUidUITestServerStarterApp =
	{
	_UID3
	};

const TUid KMainViewId = { 1982 };
// CLASS DECLARATION

// CLASS DECLARATION
// DESCRIPTION
/**
 * UITestServerStarterApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CUITestServerStarterApplication is the application part of the
 * AVKON application framework for the CUITestServerStarterApplication application.
 */
class CUITestServerStarterApplication : public CAknApplication
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
	    // None
	
	public:  // Constructors and destructor
	    /**
	    * C++ destructor.
	    */
	
	public:
	// Functions from base classes
	
	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidUITestServerStarterApplication).
	 */
	TUid AppDllUid () const;
	
	protected: // Functions from base classes
	
	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CUITestServerStarterDocument document object. The returned
	 * pointer in not owned by the CUITestServerStarterApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL ();
	
	virtual void PreDocConstructL();
	
	
    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None	
	
    private:	// New functions
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



#endif /*UITESTSERVERSTARTERAPPLICATION_H_*/

// End of File
