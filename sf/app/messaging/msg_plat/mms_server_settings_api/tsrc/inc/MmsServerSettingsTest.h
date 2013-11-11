/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MmsServerSettingsTest  declaration*
*/




#ifndef MMSSERVERSETTINGSTEST_H
#define MMSSERVERSETTINGSTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <msvapi.h> //For MMsvSessionObserver

// CONSTANTS


// MACROS

#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMmsServerSettingsTestLogPath, "\\logs\\testframework\\MmsServerSettingsTest\\" ); 
// Log file
_LIT( KMmsServerSettingsTestLogFile, "MmsServerSettingsTest.txt" ); 
_LIT( KMmsServerSettingsTestLogFileWithTitle, "MmsServerSettingsTest_[%S].txt" );

// FUNCTION PROTOTYPES


// FORWARD DECLARATIONS

class CMmsServerSettingsTest;
class CMmsAccount;
class CMmsSettings;
class CMmsRegisteredApplications;


// DATA TYPES


// CLASS DECLARATION

/**
*  CMmsServerSettingsTest test class for STIF Test Framework TestScripter.

*

*/
NONSHARABLE_CLASS(CMmsServerSettingsTest) : public CScriptBase ,public MMsvSessionObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmsServerSettingsTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMmsServerSettingsTest();

    public: // New functions

        

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        //From Base Class
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        

    private:

        /**
        * C++ default constructor.
        */
        CMmsServerSettingsTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        

        /**
        * Frees all resources allocated from test methods.
        * @since Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        /**
        * Test script for constructor of CMmsAccount
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        
                        
        virtual TInt CMmsAccountLoadSettingsL( CStifItemParser&  );
        
        /**
        * Test script for SaveSettingsL() of CMmsAccount
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
                
        virtual TInt CMmsAccountSaveSettingsL( CStifItemParser&  );
        /**
        * Test script for LoadScheduleSendSettingsL() of CMmsAccount
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
                
        virtual TInt CMmsAccountLoadScheduleSendSettingsL( CStifItemParser&  );
         /**
        * Test script for LoadScheduleReceiveSettingsL() of CMmsAccount
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
                
        virtual TInt CMmsAccountLoadScheduleReceiveSettingsL( CStifItemParser&  );
         /**
        * Test script for NewL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsReset( CStifItemParser& /*aItem*/ );
        /**
        * Test script for CopyL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsCopyL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for LoadSettingsL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsLoadSettingsL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for SaveSettingsL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsSaveSettingsL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for AddAccessPointL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsAddAccessPointL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for DeleteAccessPointL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsDeleteAccessPointL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for AccessPoint() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsAccessPointL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for ValidateSettings() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsValidateSettingsL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for RestoreFactorySettingsL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsRestoreFactorySettingsL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for CreateNewServiceL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsCreateNewServiceL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for CreateFolderEntryL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsCreateFolderEntryL( CStifItemParser& /*aItem*/ );
		/**
        * Test script for FolderEntryL() of CMmsSettings
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CMmsSettingsFolderEntryL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for LoadRegisteredApplicationsL() of CMmsRegisteredApps
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt LoadRegisteredApplicationsL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for NumberOfRegisteredApplicationsL() of CMmsRegisteredApps
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt NumberOfRegisteredApplicationsL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for RegisteredL() of CMmsRegisteredApps
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RegisteredL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for RegistereL() of CMmsRegisteredApps
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RegisterL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for UnRegistereL() of CMmsRegisteredApps
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt UnRegisterL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for UserFriendlyName() of CMmsRegisteredApps
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt UserFriendlyNameL( CStifItemParser& /*aItem*/ );
        /**
        * Test script for RegisteredApplicationList() of CMmsRegisteredApps
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RegisteredApplicationListL( CStifItemParser& /*aItem*/ );
        
        
    public:     // Data
        
       
		

    protected:  // Data
        
    private:    // Data
        CMmsAccount* iMmsAccount;
        CMmsSettings* iMmsSettings;
        CMmsRegisteredApplications* iMmsRegisteredApps;
        CMsvSession* iSession;
      
    public:     // Friend classes
        
    protected:  // Friend classes
       
    private:    // Friend classes
       

    };

#endif      // MMSSERVERSETTINGSTEST_H

// End of File
