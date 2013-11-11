/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef CMESSAGEMONITOR_H
#define CMESSAGEMONITOR_H

//  INCLUDES 

#include <e32base.h>
#include <msvapi.h>
#include "cmessageplugin.h"
#include <common.h>

// FORWARD DECLARATIONS

class CMessageMonitor : public CActive
  	{
	public:
		/**
		* NewL
		* @return
		*/
		static CMessageMonitor* NewL(  CMessagePlugin& aMessagePlugin, CMsvSession& aSession );

		/**
		* Destructor
		*/
		virtual ~CMessageMonitor();
				
		/**
		 * HandleMsgMovedL handle a moved message
		 */
		void HandleMsgMovedL( const TMsvId aNewFolderId, 
							  const TMsvId aOldFolderId, 
							  const CMsvEntrySelection& aSelection );

		/**
		 * HandleMsgCreatedChangedL handles a new or changed message
		 * @param CMsvEntrySelection aSelection list of messages TMsvIds that have changed
		 * @param TMsvId aFolderId Folder that contains these changed messages
		 * @param MMsvSessionObserver::TMsvSessionEvent aEvent type of event, not used currently
		 */	
	    void HandleMsgCreatedChangedL( const CMsvEntrySelection& aSelection,
                                          	        const TMsvId aFolderId,
                                          	        const MMsvSessionObserver::TMsvSessionEvent aEvent );
	    
		/**
		 * HandleMsgCreatedChangedL handles a deleted message
		 * @param CMsvEntrySelection aSelection list of messages TMsvIds that have been deleted
		 */		
        void HandleMsgDeletedL( const CMsvEntrySelection& aSelection );
        
        void ResumeL();
        
	protected:
		/** 
		 * inherited from CActive
		 */
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
        
	private:

		/**
		* From Constructor
		*/
		CMessageMonitor(  CMessagePlugin& aMessagePlugin, CMsvSession& aSession );
		
		/**
		* 2nd phase construction
		*/
		void ConstructL();

		/**
		 * OverwriteOrAddToQuereL is a helper for MessageItemL. Adds only unique TMsvId's to the processing queue.
		 * @param TMsvId aMsvId message to be added to the list
		 * @param TCPixActionType aAction action type to be added to the list
		 * @param TMsvId aFolderId id of the folder where message is located
		 */
		void OverwriteOrAddToQuereL(TMsvId aMsvId, TCPixActionType aAction, TMsvId aFolderId);
		
		/**
		 * MessageItemL adds a message to the delayed queue, and schedules the RunL
		 * @param TMsvId aMsvId message to be added to the list
		 * @param TCPixActionType aAction action type to be added to the list
		 * @param TMsvId aFolderId id of the folder where message is located
		 */
		void MessageItemL(TMsvId aMsvId, TCPixActionType aAction, TMsvId aFolderId);

	private:
		enum TState 
		{ 
			EStateNone = 0, 
			EStateWaiting 
		};
		
		struct TMessageRecord 
		{
			TMsvId iMsvId;
			TCPixActionType iAction;
			TMsvId iFolderId;
		};
	private:
		// Queue of documents to be indexed
		RArray<TMessageRecord> iMessages; 
		
		// State machine's state
		TState iState;

		// Timer which is used to delay indexing messages
		RTimer iTimer;
		
		CMsvSession&		iMsvSession;
		CMessagePlugin&      iMessagePlugin;
#ifdef HARVESTERPLUGINTESTER_FRIEND
    friend class CHarvesterPluginTester;
#endif
	};

#endif //  CMESSAGEMONITOR_H
