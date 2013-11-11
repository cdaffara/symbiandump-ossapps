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
* TestServerThreadStarter class.
*
*/


#ifndef TESTSERVERTHREADSTARTER_H_
#define TESTSERVERTHREADSTARTER_H_

//  INCLUDES
#include "e32base.h"


// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestThreadContainerRunnerFactory;

// CLASS DECLARATION

// DESCRIPTION
// Class used to create thread in which testserver execution code is placed.
class CTestServerThreadStarter: public CActive
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
	    // None
	
	public:  // Constructors and destructor
	    /**
	    * C++ destructor.
	    */
		~CTestServerThreadStarter();
	
	public: // New functions
	    /**
	    * NewL is first phase of two-phased constructor.
	    */		
		static CTestServerThreadStarter* NewL();
	
		/**
		 * Performs testserver execution request.
		 */
		TInt RunTestServerThread( CTestThreadContainerRunnerFactory* aTestThreadContainerRunnerFactory );
	public: // Functions from base classes
		/**
		 * RunL derived from CActive handles the completed requests.
		 */
		void RunL();
		
		/**
		 * DoCancel derived from CActive handles the Cancel.
		 */
		void DoCancel();

    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None
        
    private:	// New functions
    	/**
    	 * Thread function in which testserver execution code is placed.
    	 */
		static TInt TestServerStarterThreadFunction( TAny* aParameters );
	    
		/**
	    * C++ default constructor.
	    */
		CTestServerThreadStarter();

	    /**
	    * By default Symbian OS constructor is private.
	    */
		void ConstructL();		
	
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
		RTimer iTimer;	// Timer which complets testserver execution request
		CTestThreadContainerRunnerFactory* iTestThreadContainerRunnerFactory; 	// Pointer to test thread container
																				// runner factory passed as a parameter
																				// to function which executes testserver
		TThreadId iMainThreadId;	// UITestServerStarter main thread id 
		TInt iReturnCode;			// TestServer execution return code
		RSemaphore iServerThreadStartedSemaphore;	// Semaphore which indicates that testserver execution thread
													// was successfully created		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None
	};



#endif // TESTSERVERTHREADSTARTER_H_

// End of File
