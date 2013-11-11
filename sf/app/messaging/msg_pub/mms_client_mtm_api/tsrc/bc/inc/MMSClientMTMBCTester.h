/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:         MMSClientMTMBCTester test module.
*
*/










#ifndef MMSCLIENTMTMBCTESTER_H
#define MMSCLIENTMTMBCTESTER_H


// INCLUDES
#include "StifTestModule.h"
#include "MMSClientMTMBCTesterController.h"
#include <StifLogger.h>


// CONSTANTS
// MACROS
//#define ?macro ?macro_def
#define TEST_MODULE_VERSION_MAJOR 30
#define TEST_MODULE_VERSION_MINOR 9
#define TEST_MODULE_VERSION_BUILD 6

// Logging path
_LIT( KMMSClientMTMBCTesterLogPath, "\\MMSClientMTMBCTester\\" );
// Log file
_LIT( KMMSClientMTMBCTesterLogFile, "MMSClientMTMBCTesterLog.txt" );


// Function pointer related internal definitions
// Visual studio 6.0 (__VC32__) needs different type of handling 
#ifdef __VC32__
#define GETPTR
#else
#define GETPTR &
#endif  
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}


// FORWARD DECLARATIONS
class CMMSClientMTMBCTester;


// DATA TYPES
typedef TInt (CMMSClientMTMBCTester::* TestFunction)(const TInt, TTestResult&);


// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*/
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };


// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*/
class TCaseInfo
    {
    public:
        TPtrC			iCaseName;
        TestFunction	iMethod;
        TBool			iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };


// CLASS DECLARATION

/**
*  This is the MMSClientMTMBCTester class
*/
NONSHARABLE_CLASS( CMMSClientMTMBCTester ) : public CTestModuleBase
    {
    
    public: // Constructors and destructor


        /**
        * Two-phased constructor
        */
        static CMMSClientMTMBCTester* NewL();

        /**
        * Destructor
        */
        virtual ~CMMSClientMTMBCTester();


    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the 
        *   MMSClientMTMBCTester. It is called once for every instance of 
        *   TestModuleMMSClientMTMBCTester after its creation.
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of MMSClientMTMBCTester.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from MMSClientMTMBCTester. 
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray< TTestCaseInfo >& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        *   test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& aFailureType, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
        *   test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                         const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        *
        * User may add implementation for OOM test warning handling. Usually no
        * implementation is required.           
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */); 

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        *   test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );
				/**
         * Method used to log version of test module
         */
        void SendTestModuleVersion();


    private:

        /**
        * C++ default constructor
        */
        CMMSClientMTMBCTester();

        /**
        * By default Symbian 2nd phase constructor is private
        */
        void ConstructL();
        

        /**
        * Function returning test case name and pointer to test case function.
        * @param aCaseNumber test case number
        * @return TCaseInfo 
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;
        
        
        /**
        * Function reporting on the result of a test case
        * @param aCaseNumber The test case number
        * @param aValue The value determined through test case execution
        * @param aErrorMessage The error message in case of an error
        * @param aResult The result record
        * @param aFailOnly indicates if only error situations should be handled
        * @return Truth value indicating whether the test case under evaluation
        *         passed (ETrue) or failed (EFalse)
        */
        TBool HandleResult( const TInt aCaseNumber, const TInt aValue,
                            const TDesC& aErrorMessage, TTestResult& aResult,
                            TBool aFailOnly = EFalse );

		/**
		* Helper function to create a message entry and switch the context to it
		* @param aCaseNumber: The test case number
		* @param aResult: The result record
		* @return Truth value indicating if there were problems
		*/
		TBool InitializeMessageEntry( const TInt aCaseNumber,
		                              TTestResult& aResult );

        /**
        * Actual Hardcoded test case functions are listed below.
        * Parameters:
        *   aCaseNumber: the ordinal of the test case
        *   aResult: the record containing information of the test cases result
        * Return value: the case execution status (N.B. NOT the case result)
        */

        // Creating a new message entry
        TInt CreateMessageEntry( const TInt aCaseNumber, TTestResult& aResult );

        // Creating a new service entry
        TInt CreateServiceEntry( const TInt aCaseNumber, TTestResult& aResult );


/***
* Test case that deals with CMmsSettings class can only be included and executed
* if the internal MMS Server Settings API is available.
* See MMSClientMTMBCTester.mmp for instructions on taking the case into use.
***/
#ifdef MMSCLIENTMTMBCTESTER_INTERNAL_TESTING

        // Handling the MMS settings
        TInt HandleMMSSettings( const TInt aCaseNumber, TTestResult& aResult );

#endif // MMSCLIENTMTMBCTESTER_INTERNAL_TESTING

        
        // Storing MMS settings to disk
        TInt StoreMMSSettings( const TInt aCaseNumber, TTestResult& aResult );
        
        // Restoring MMS settings from disk
        TInt RestoreMMSSettings( const TInt aCaseNumber, TTestResult& aResult );
        
        // Restoring factory MMS settings
        TInt RestoreFactoryMMSSettings( const TInt aCaseNumber,
                                        TTestResult& aResult );
        
        // Validating default service
        TInt ValidateDefaultService( const TInt aCaseNumber,
                                     TTestResult& aResult );
        
        // Handling the sender
        TInt Sender( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the message class
        TInt MessageClass( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the expiration interval
        TInt ExpirationInterval( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the expiration date
        TInt ExpirationDate( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the delivery time interval
        TInt DeliveryTimeInterval( const TInt aCaseNumber,
                                   TTestResult& aResult );
        
        // Handling the delivery date
        TInt DeliveryDate( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the priority
        TInt MessagePriority( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the sender visibility
        TInt SenderVisibility( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the delivery report request
        TInt DeliveryReportRequest( const TInt aCaseNumber,
                                    TTestResult& aResult );
        
        // Handling the read reply request
        TInt ReadReplyRequest( const TInt aCaseNumber, TTestResult& aResult );
        
        // Getting the sending date and time
        TInt SendingDate( const TInt aCaseNumber, TTestResult& aResult );
        
        // Getting the response text
        TInt ResponseText( const TInt aCaseNumber, TTestResult& aResult );
        
        // Getting the response status
        TInt ResponseStatus( const TInt aCaseNumber, TTestResult& aResult );
        
        // Getting the number of previous senders
        TInt NumberOfPreviousSenders( const TInt aCaseNumber,
                                      TTestResult& aResult );
        
        // Getting the address of a previous sender
        TInt PreviousSender( const TInt aCaseNumber, TTestResult& aResult);
        
        // Getting a previous sending date
        TInt PreviousSendingDate( const TInt aCaseNumber,
                                  TTestResult& aResult );
        
        // Getting the receive time
        TInt ReceiveTime( const TInt aCaseNumber, TTestResult& aResult );
        
        // Getting the actual message size
        TInt TransferSize( const TInt aCaseNumber, TTestResult& aResult );
        
        // Getting the location of the message
        TInt ContentLocation( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the message root part
        TInt MessageRootPart( const TInt aCaseNumber, TTestResult& aResult );
        
        // Handling the maximum image size
        TInt MaximumImageSize( const TInt aCaseNumber, TTestResult& aResult );
        
        // Getting the message size
        TInt GetMessageSize( const TInt aCaseNumber, TTestResult& aResult );
        
        // Setting the message description
        TInt SetMessageDescription( const TInt aCaseNumber,
                                    TTestResult& aResult );
        
        // Adding a new attribute
        TInt AddNewAttribute( const TInt aCaseNumber, TTestResult& aResult );
        
        // Getting an attribute
        TInt GetAttribute( const TInt aCaseNumber, TTestResult& aResult );
        
        // Checking if an attribute exists
        TInt FindAttribute( const TInt aCaseNumber, TTestResult& aResult );
        
        // Deleting an attribute
        TInt DeleteAttribute( const TInt aCaseNumber, TTestResult& aResult );
        
        // Resetting all attributes
        TInt ResetAttributes( const TInt aCaseNumber, TTestResult& aResult );
        
        // Creating an attachment synchronously
        TInt CreateAttachmentSync( const TInt aCaseNumber,
                                   TTestResult& aResult );
        
        // Creating a text attachment
        TInt CreateTextAttachment( const TInt aCaseNumber,
                                   TTestResult& aResult );
        
		// Sending a message using default service settings
		TInt SendMessage( const TInt aCaseNumber, TTestResult& aResult );
		
		// Sending a selection of messages using default service settings
		TInt SendMessageSelection( const TInt aCaseNumber,
		                           TTestResult& aResult );
		
		// Fetching all available messages
		TInt FetchAllMessages( const TInt aCaseNumber, TTestResult& aResult );
		
		// Getting the type of the MTM
		TInt GetMTMType( const TInt aCaseNumber, TTestResult& aResult );
		
		// Setting the context
		TInt SetContext( const TInt aCaseNumber, TTestResult& aResult );
		
		// Switching the context
		TInt SwitchContext( const TInt aCaseNumber, TTestResult& aResult );
		
		// Checking if the context has been set
		TInt CheckContext( const TInt aCaseNumber, TTestResult& aResult );
        
        // Restoring and storing current entry data
        TInt RestoreAndStore( const TInt aCaseNumber, TTestResult& aResult );
        
        // Validating the current message
        TInt ValidateMessage( const TInt aCaseNumber, TTestResult& aResult );
        
        // Searching for a specified text in the current message
        TInt SearchForText( const TInt aCaseNumber, TTestResult& aResult );
        
        // Creating a reply message
        TInt CreateReply( const TInt aCaseNumber, TTestResult& aResult );
        
        // Creating a forward message
        TInt CreateForward( const TInt aCaseNumber, TTestResult& aResult );
        
        // Adding an addressee
        TInt AddAddressee( const TInt aCaseNumber, TTestResult& aResult );
        
        // Adding an addressee with an alias
        TInt AddAddresseeAlias( const TInt aCaseNumber, TTestResult& aResult );
        
        // Adding a typed addressee
        TInt AddAddresseeTyped( const TInt aCaseNumber, TTestResult& aResult );
        
        // Adding a typed addressee with an alias
        TInt AddAddresseeAliasTyped( const TInt aCaseNumber,
                                     TTestResult& aResult );
        
        // Removing an addressee
        TInt RemoveAddressee( const TInt aCaseNumber, TTestResult& aResult );
        
        // Accessing the message body
        TInt AccessMessageBody( const TInt aCaseNumber, TTestResult& aResult );
        
        // Accessing the message body, const version
        TInt AccessMessageBodyConst( const TInt aCaseNumber,
                                     TTestResult& aResult );
        
        // Handling the message subject
        TInt HandleMessageSubject( const TInt aCaseNumber,
                                   TTestResult& aResult );
        
        // Querying for a capability
        TInt QueryCapability( const TInt aCaseNumber, TTestResult& aResult );
        
        // Invoking a synchronous function
        TInt InvokeSynchronousFunction( const TInt aCaseNumber,
                                        TTestResult& aResult );
        
        // Invoking an asynchronous function
        TInt InvokeAsynchronousFunction( const TInt aCaseNumber,
                                         TTestResult& aResult );
        
        // Getting access to the session
        TInt GetSession( const TInt aCaseNumber, TTestResult& aResult );
        
        // Adding a file attachment by file path
        TInt AddAttachmentFilePath( const TInt aCaseNumber,
                                    TTestResult& aResult );
        
        // Adding a file attachment by an open file handle
        TInt AddAttachmentOpenHandle( const TInt aCaseNumber,
                                      TTestResult& aResult );
        
        // Adding a file attachment as a linked file
        TInt AddAttachmentLinkedFile( const TInt aCaseNumber,
                                      TTestResult& aResult );
        
        // Adding a message entry as an attachment
        TInt AddAttachmentMessageEntry( const TInt aCaseNumber,
                                        TTestResult& aResult );
        
        // Creating an attachment asynchronously
        TInt CreateAttachmentAsync( const TInt aCaseNumber,
                                    TTestResult& aResult );
        
        // Canceling an attachment operation
        TInt CancelAttachmentOperation( const TInt aCaseNumber,
                                        TTestResult& aResult );
        
        // Creating a new message
        TInt CreateMessage( const TInt aCaseNumber, TTestResult& aResult );
        
		// Notifying of a BIO type change
		TInt NotifyBIOTypeChange( const TInt aCaseNumber,
		                          TTestResult& aResult );
        
        // Removing the default service
        TInt RemoveDefaultService( const TInt aCaseNumber,
                                   TTestResult& aResult );
        
        // Changing the default service
        TInt ChangeDefaultService( const TInt aCaseNumber,
                                   TTestResult& aResult );


    private:
    
        // Data
    
        // Pointer to the test (function) to be executed
        TestFunction iMethod;

        // Pointer to the logger
        CStifLogger * iLog;
        
        // Pointer to the active scheduler
		CActiveScheduler* iScheduler;
        
        // Pointer to the test client
		CMMSClientMTMBCTesterController* iController; 


    };

#endif // MMSCLIENTMTMBCTESTER_H

// End of file: MMSClientMTMBCTester.h
