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
* Description:  TestSyncAgent is a STIF TestModule to test the Synch Agent API
*
*/



#ifndef TESTCNTDATASTOREEXTN_H
#define TESTCNTDATASTOREEXTN_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <nsmlcontactsdatastoreextension.h>

// Logging path
_LIT( KTestCntDataStoreExtnLogPath, "\\logs\\testframework\\TestCntDataStoreExtn\\" ); 
// Log file
_LIT( KTestCntDataStoreExtnLogFile, "TestCntDataStoreExtn.txt" ); 

// FORWARD DECLARATIONS
class CTestCntDataStoreExtn;


// CLASS DECLARATION
/**
*  TestCntDataStoreExtn test class for STIF Test Framework TestScripter.
*/
NONSHARABLE_CLASS(CTestCntDataStoreExtn) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestCntDataStoreExtn* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTestCntDataStoreExtn();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        CTestCntDataStoreExtn( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        //ADD NEW METHOD DEC HERE
        /**
        * Export the contacts
        */
        TInt ExportContactsL( CStifItemParser& aItem );
        
        /**
        * Import the contacts
        */
        TInt ImportContactsL( CStifItemParser& aItem );
        
        /**
        * Read contact
        */
        TInt ReadContactL( CStifItemParser& aItem );
        
        /**
        * Delete contact
        */
        TInt DeleteContactL( CStifItemParser& aItem );
        
        /**
        * Delete Contacts
        */
        TInt DeleteContactsL( CStifItemParser& aItem );
        
        
        /**
        * Delete All Contacts
        */
        TInt DeleteAllContactsL( CStifItemParser& aItem );
        
        /**
        * List Store
        */
        TInt ListStoresL( CStifItemParser& aItem );
        
        /**
        * Machine ID
        */
        TInt MachineIdL( CStifItemParser& aItem );
        
        /**
        * List the contacts
        */
        TInt ListContactsL( CStifItemParser& aItem );
        
    private:    // Data
        CNsmlContactsDataStoreExtension* iCntDataStoreExtension;
        CArrayFixFlat<TUid>* iEntryArray;
				
    };

#endif      // TESTCNTDATASTOREEXTN

// End of File
