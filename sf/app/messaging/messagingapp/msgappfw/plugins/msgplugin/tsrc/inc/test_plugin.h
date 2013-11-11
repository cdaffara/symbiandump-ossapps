/*
#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:   Main test class header for CS Message Plugin
#*/

#ifndef TEST_PLUGIN_H_
#define TEST_PLUGIN_H_

//  INCLUDES
#include <badesca.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

// USER INCLUDES
#include <mcsplugineventobserver.h>
#include <ccsmsg.h>
#include <ccsmsghandler.h>
#include <ccsdefs.h>
#include <ccsconversationentry.h>
#include <ccsmsgpluginutility.h>
#include <smsclnt.h>
#include <mmsclient.h>
#include <cmsvmimeheaders.h>
#include <mmsvattachmentmanager.h>
#include <CAsyncWaiter.h>
#include <mtclreg.h>
#include <biouids.h>
#include <msgbiouids.h>


// Logging path
_LIT( KTestPluginLogPath, "\\logs\\testframework\\test_plugin\\" ); 
// Log file
_LIT( KTestPluginTestLogFile, "test_plugin.txt" ); 
_LIT( KTestPluginTestLogFileWithTitle, "test_plugin_[%S].txt" );

// FORWARD DECLARATIONS
class CCsMsg; 
class CCsMsgHandler;
class CCsMsgPluginUtility;
class MCsMsgObserver; 
class CCsConversationEntry;
class CCsMsgPluginUtility;
class CMsvSession;
class CClientMtmRegistry;
class CSmsClientMtm;
class CMmsClientMtm;
class CMsvMimeHeaders;
class CAsyncWaiter;
class CMmsSettings;

/**
 *  Message plugin class
 **/

NONSHARABLE_CLASS(Ctest_plugin) : public CScriptBase,public MCsPluginEventObserver,public MMsvSessionObserver
    {
    private:

            /**
            * C++ default constructor.
            */
            Ctest_plugin(CTestModuleIf& aTestModuleIf );

            /**
            * By default Symbian 2nd phase constructor is private.
            */
            void ConstructL();

            // Prohibit copy constructor if not deriving from CBase.
            // ?classname( const ?classname& );
            // Prohibit assigment operator if not deriving from CBase.
            // ?classname& operator=( const ?classname& );

            /**
            * Frees all resources allocated from test methods.
            * @since ?Series60_version
            */
            void Delete();

            /**
                    * Example test method.
                    * @since ?Series60_version
                    * @param aItem Script line containing parameters.
                    * @return Symbian OS error code.
                    */
            virtual TInt ExampleL( CStifItemParser& aItem );            

    public:  // Constructors and destructor

            /**
             * Two-phased constructor.
             */
            static Ctest_plugin* NewL( CTestModuleIf& aTestModuleIf );

			/**
			 * From CScriptBase Runs a script line.
			 * @since ?Series60_version
			 * @param aItem Script line containing method name and parameters
			 * @return Symbian OS error code
			 *         */        
            virtual TInt RunMethodL( CStifItemParser& aItem );
			/**
			 * Destructor.
			 */
            virtual ~Ctest_plugin();
            
    private:    // Data
            
        	CCsMsg* iPlugin;
        
            /**
             * MsvServer session object
             */
            CMsvSession* iSession;

            /**
             * Mtm registry object
             */   
            CClientMtmRegistry* iMtmRegistry;

            /**
             * Sms Client mtm object
             */
            CSmsClientMtm* iSmsClientMtm;
            
            /**
             * MMS Client mtm object
             */
            CMmsClientMtm* iMmsClientMtm;
         
           	/**
            * List contains the CCsConversationEntry objects that needs to be 
        	* verified with the list obtained from sms plugin
        	*/    
        	RPointerArray<CCsConversationEntry> iTestConversationEntryList;

        	/**
        	* List obtained from sms plugin 
        	*/    
        	RPointerArray<CCsConversationEntry> iResultConversationEntryList;
       
        	
    public://Mcsplugineventobserver
            /**
             * AddConversations.
             * Handle Conversation event from Plugins
             * @param aConversationEntryLists    List of conversation entries.
             */
          void AddConversations(const RPointerArray<CCsConversationEntry>& aConversationEntryLists);

            /**
             * ModifyConversations.
             * Update Conversation event from Plugins
             * @param aConversationEntryLists    List of conversation entries.
             */
          void ModifyConversations(const RPointerArray<CCsConversationEntry>& aConversationEntryLists);

            /**
             * DeleteConversations.
             * Delete Conversation event from Plugins
             * @param aConversationEntryLists    List of conversation entries.
             */
           void DeleteConversations(const RPointerArray<CCsConversationEntry>& aConversationEntryLists);
            /**
             * CachingCompleted.
             * Caching complete event from Plugins
             */
            void CachingCompleted();

            /**
             * CachingError.
             * Caching error event from Plugins
             * @param aError    Caching Error infomation.
             */
            void CachingError(const TInt aError) ;
            
            /**
                     * RefreshConversations
                     * Refresh all data from this plugin.
                     */
            void RefreshConversations();

    public://from MmsvSessionObserver
            /**
             * Handles the MsvServer updates. Implemented for MMsvSessionObserver
             */
            void HandleSessionEventL(TMsvSessionEvent aEvent, TAny *aArg1, TAny *aArg2, TAny *aArg3);        
    	
    public: 
           
            virtual TInt CreateCCsMsgL( CStifItemParser& aItem );    	
            virtual TInt SendCcsMessageL( CStifItemParser& /*aItem*/ );
            virtual TInt CacheFromCcsPluginL( CStifItemParser& /*aItem*/ );
            virtual TInt CreateCcsPluginL( CStifItemParser& /*aItem*/ );
            virtual TInt SmsMsgPluginValidationL( CStifItemParser& aItem );
            virtual TInt MmsMsgPluginValidationL( CStifItemParser& aItem );
            virtual TInt BioMsgPluginValidationL( CStifItemParser& aItem );
            
            TInt Compare(CCsConversationEntry *aResult, CCsConversationEntry *aTest);
            /** 
                     * Cleans messages from all foldes
                     */        
             void CleanAllFoldersL();

             /** 
                     * Populates the messages in different folders which are used 
                     * to verify the caching logic of ccs plugin 
                     */                
             void CreateInitialCacheL();
             
             /** 
                      * Cleans messages from the specified folder
                      * @param aBoxId , folder from where messages has to be deleted
                      * @param aDeleteOne , if set only one message is deleted
                      */
            void CleanMessagesInFolderL(TMsvId aBoxId ,TBool aDeleteOne = EFalse); 

            /** 
             * Creates messages in the different folders and populates
             * the iTestConversationEntryList 
             * @param aFolderId , the folder in which message is to be created
             * @param aUnReadFlag , Used while creating message in inbox folder
             */
            void CreateMessageL(const TMsvId aFolderId,TBool aUnReadFlag = EFalse); 
            void CreateMmsMessageL(const TMsvId aFolderId,TBool aUnReadFlag = EFalse);
            void HandleAttachementL(CMsvStore* store,TFileName& sourceFileName );
            void CreateSmartMessageL(const TMsvId aFolderId,TBool aUnReadFlag = EFalse);
    };

#endif /* TEST_PLUGIN_H_ */
