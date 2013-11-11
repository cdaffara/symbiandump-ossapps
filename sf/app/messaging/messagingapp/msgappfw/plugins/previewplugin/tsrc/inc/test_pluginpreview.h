
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
# Description:   Main test class header for CS Preview Plugin
#*/

#ifndef TEST_PLUGINPREVIEW_H_
#define TEST_PLUGINPREVIEW_H_

//  INCLUDES
#include <badesca.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

//  USER INCLUDE
#include <UniDataModel.h>
//#include <thumbnailmanagerobserver.h>
#include <thumbnailmanager.h>
#include <mcsplugineventobserver.h>
#include <mmsvattachmentmanager.h>
#include <CAsyncWaiter.h>
#include <cmsvmimeheaders.h>
#include <mmsclient.h>
#include <mtclreg.h>
#include <SendUiConsts.h>


#include <ccspreviewplugin.h>
#include <ccspreviewpluginhandler.h>

// Logging path
_LIT( KTestPluginLogPath, "\\logs\\testframework\\test_pluginpreview\\" ); 
// Log file
_LIT( KTestPluginTestLogFile, "test_pluginpreview.txt" ); 
_LIT( KTestPluginTestLogFileWithTitle, "test_pluginpreview_[%S].txt" );


// FORWARD DECLARATION
class CCsPlugin;
class CMsvSession;
class MCsPluginEventObserver;
class MMsvSessionObserver;
class CCsConversationEntry;
class CCsPreviewPlugin;
class CClientMtmRegistry;
class CMmsClientMtm;
class MCsPluginEventObserver;
class CCsPreviewPluginHandler;

/**
 *  Message plugin class
 **/

NONSHARABLE_CLASS(Ctest_pluginpreview) : public CScriptBase,public MMsvSessionObserver,public MThumbnailManagerObserver,public 					 MCsPluginEventObserver,public MUniDataModelObserver
{
	
	private:

		/**
		* C++ default constructor.
		*/
		Ctest_pluginpreview(CTestModuleIf& aTestModuleIf );

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
        static Ctest_pluginpreview* NewL( CTestModuleIf& aTestModuleIf );

		/**
		* From CScriptBase Runs a script line.
		* @since ?Series60_version
		* @param aItem Script line containing method name and parameters
		* @return Symbian OS error code
		* */        

        virtual TInt RunMethodL( CStifItemParser& aItem );
		/**
		* Destructor.
		*/
        virtual ~Ctest_pluginpreview();
            
    	private:    

        CCsPreviewPlugin* iPlugin;
        
        /**
        * MsvServer session object
        * Own
        */
        CMsvSession* iSession;

        /**
        * Mtm registry object
        * Own
        */
         CClientMtmRegistry* iMtmRegistry;

         /**
         * Mms client mtm object
         * Own.
         */
         CMmsClientMtm* iMmsMtm;
         CThumbnailManager* iThumbnailManager;        	
    
        public: 
    	
  		  /**
  		  *  HandleCachingCompleted
   		  *  Sends the cache completed status to server
   		  */
   		 void HandleCachingCompleted();

   		 /**
    	 *  HandleCachingError
         *  Sends the error occured during the caching to server
         */
   		 void HandleCachingError(const TInt aError);

        public:
  		  //MMsvSessionObserver
 		   /**
   		  * Handles the MsvServer updates. Implemented for MMsvSessionObserver
    		  */
   		 void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
           	  TAny* aArg2, TAny* aArg3);

	public:
   		 //MUniDataModelObserver
    		/**
   		  */
 		   void RestoreReady(TInt aParseResult, TInt aError);
 		   
	public://Mcsplugineventobserver
	         /**
	         * AddConversations.
	         * Handle Conversation event from Plugins
	         * @param aConversationEntryLists    List of conversation entries.
	         */
	          void AddConversations(const RPointerArray<CCsConversationEntry>& /*aConversationEntryLists*/){}

	         /**
	         * ModifyConversations.
	         * Update Conversation event from Plugins
	         * @param aConversationEntryLists    List of conversation entries.
	         */
	         void ModifyConversations(const RPointerArray<CCsConversationEntry>& /*aConversationEntryLists*/){}

	         /**
	         * DeleteConversations.
	         * Delete Conversation event from Plugins
	         * @param aConversationEntryLists    List of conversation entries.
	         */
	          void DeleteConversations(const RPointerArray<CCsConversationEntry>& /*aConversationEntryLists*/){}
	          /**
	           * CachingCompleted.
	           * Caching complete event from Plugins
	          */
	          void CachingCompleted(){}

	          /**
	            * CachingError.
	            * Caching error event from Plugins
	            * @param aError    Caching Error infomation.
	            */
	            void CachingError(const TInt /*aError*/) {}
	            
	            /**
	             * RefreshConversations
	             * Refresh all data from this plugin.
	             */
	            void RefreshConversations(){}
	            
	public:
  		       // Callbacks from MThumbnailManagerObserver for getting thumbnails
  		      void ThumbnailPreviewReady(MThumbnailData& aThumbnail,
       		     TThumbnailRequestId aId);

    		  void ThumbnailReady(TInt aError, MThumbnailData& aThumbnail,
           	     TThumbnailRequestId aId);
    public: 
           
               virtual TInt CreateCCsPreviewPluginL( CStifItemParser& /*aItem*/ );    
		       virtual TInt MmsMsgPluginValidationL( CStifItemParser& /*aItem*/ );
               virtual TInt SendMmsMessageL( CStifItemParser& /*aItem*/ );
 
               void CleanAllFoldersL(); 
               void CreateMmsMessageL(const TMsvId aFolderId,TBool aUnReadFlag = EFalse);
		       void HandleAttachementL(CMsvStore* store,TFileName& sourceFileName );
		       void CleanMessagesInFolderL(TMsvId aBoxId ,TBool aDeleteOne = EFalse); 
		       // void CreateInitialCacheL();
    };

#endif /* test_pluginpreview_H_ */
