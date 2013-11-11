/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store session.
*
*/



#ifndef __MESSAGE_STORE_SESSION_H__
#define __MESSAGE_STORE_SESSION_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

#include "MessageStoreClientServer.h"
#include "ContainerStore.h"
#include "ContainerStoreSearchHandler.h"

// ====================
// FORWARD DECLARATIONS
// ====================

class CMessageStoreServer;
class CMruAddress;
class CBufferedMessageCreator;


// =======
// CLASSES
// =======

class CSearchResult : public CBase
    {
    public:    
    
        static CSearchResult* NewL( TContainerId aMessageId, TContainerId aFolderId, const TDesC8& aPropertyBuf );
        static CSearchResult* SearchCompleteResultL();
        
        virtual ~CSearchResult();
        
        TContainerId MessageId()   { return iMessageId; }
        TContainerId FolderId()    { return iFolderId; }
        const RBuf8& PropertyBuf() { return iPropertyBuf; }
        
    protected:
        
        CSearchResult( TContainerId aMsgId, TContainerId aFolderId );
        void ConstructL( const TDesC8& aPropertyBuf );
        
    private:
        TContainerId  iMessageId;
        TContainerId  iFolderId;
        RBuf8         iPropertyBuf;    
    };

// ==============================================================================================
// CLASS: CMessageStoreSession
//
// This class represents a session to the message store server.
// ==============================================================================================
class CMessageStoreSession : public  CSession2, 
                             private MContainerStoreObserver,
                             private MContainerStoreSearchClient
    {
    public:

        // ==============
        // PUBLIC METHODS
        // ==============
        
        CMessageStoreSession( CMessageStoreServer& aMessageStoreServer ) ;
        
        virtual ~CMessageStoreSession();

        // Called by the server when the container store is temporarily unavailable, due to conditions
        // such as backup/restore in progress.
        void ContainerStoreUnavailable();

        // Called by the server when the container store is available again.
        void ContainerStoreAvailable();

        // Called by the server to propogate events to the session clients.
        void SendEventToObserver( TMsgStoreEvent &aEvent );

        // inherited from CSession2
        void CreateL();

    private:

        // ===============
        // PRIVATE METHODS
        // ===============

        // aAlwaysSend indicates whether or not matches should be sent, even if no matches have been
        // found.
        void SendMatchesToClient( TBool aAlwaysSend = EFalse );
        void DoWriteMatchesToMessageL();

        // IPC message handler functions
        void DoAuthenticatedL( const RMessage2& aMessage );
        void DoAuthenticateL( const RMessage2& aMessage );
        void DoClearAuthenticationL( const RMessage2& aMessage );
        void DoHasPasswordL( const RMessage2& aMessage );
        void DoSetPasswordL( const RMessage2& aMessage );
        void DoChangePasswordL( const RMessage2& aMessage );
        void DoEnableEncryptionL( const RMessage2& aMessage );
        void DoDisableEncryptionL( const RMessage2& aMessage );
        void DoWipeEverythingL( const RMessage2& aMessage );
        void DoStartObserving( const RMessage2& aMessage );    
        void DoStopObserving( const RMessage2& aMessage );    
        void DoGetEventsL( const RMessage2& aMessage );    
        void DoCreateAccountL( const RMessage2& aMessage ); 
        void DoOpenAccountL( const RMessage2& aMessage ); 
        void DoRenameAccountL( const RMessage2& aMessage ); 
        void DoDeleteAccountL( const RMessage2& aMessage ); 
        void DoListAccountsL( const RMessage2& aMessage ); 
        void DoDeleteContainerL( const RMessage2& aMessage );    
        void DoCreateContainerL( const RMessage2& aMessage ); 
        void DoCommitContainerL( const RMessage2& aMessage );
        void DoAbandonContainerL( const RMessage2& aMessage );
        void DoMoveContainerL( const RMessage2& aMessage );
        void DoCopyContainerL( const RMessage2& aMessage );
        void DoChildrenCountsL( const RMessage2& aMessage );
        void DoTotalCountsL( const RMessage2& aMessage );
        void DoChildrenIdsL( const RMessage2& aMessage );
        void DoPropertiesListL( const RMessage2& aMessage );
        void DoFetchPropertiesL( const RMessage2& aMessage );
        void DoUpdatePropertiesL( const RMessage2& aMessage );
        void DoUpdatePropertyL( const RMessage2& aMessage );
        void DoContentLengthL( const RMessage2& aMessage ); 
        void DoFetchContentToBufferL( const RMessage2& aMessage ); 
        void DoFetchContentToFileL( const RMessage2& aMessage ); 
        void DoReplaceContentWithBufferL( const RMessage2& aMessage ); 
        void DoReplaceContentWithFileL( const RMessage2& aMessage ); 
        void DoAppendToContentL( const RMessage2& aMessage ); 
        void DoPrependToContentL( const RMessage2& aMessage );
        void DoRemoveContentL( const RMessage2& aMessage ); 
        void DoOpenContentFileL( const RMessage2& aMessage );
        void DoPrepareSearchL( const RMessage2& aMessage );
        void DoGetMatches( const RMessage2& aMessage );
        void DoCancelSearch( const RMessage2& aMessage );
        void DoClearSearchResultCache( const RMessage2& aMessage );
        void DoStartSortingL( const RMessage2& aMessage );
        void DoGetSortedRowsL( const RMessage2& aMessage );
        void DoGetIteratorGroupCountL( const RMessage2& aMessage );
        void DoGetSortedIdsAndFlagsL( const RMessage2& aMessage );
        void DoGetSortedIndexOfL( const RMessage2& aMessage );
        void DoGetSortedIdsL( const RMessage2& aMessage );
        void DoGetSortedIdsAndGroupCountL( const RMessage2& aMessage );
        void DoEndSortingL( const RMessage2& aMessage );
        
        void DoSetMaxMruAdressCountL( const RMessage2& aMessage );
        void DoSetMruAddressListL( const RMessage2& aMessage );
        void DoGetMruAddressListL( const RMessage2& aMessage );

        void DoBeginBatchInsertL( const RMessage2& aMessage );
        void DoFinishBatchInsertL( const RMessage2& aMessage );
        
        // inherited from CSession2
        void ServiceL(const RMessage2& aMessage);
        void ServiceError(const RMessage2& aMessage, TInt aError);

        // inherited from MContainerStoreObserver
        virtual void ContainerModified( TOperation aOperation, 
                const RArray<TContainerId>& aHierarchy, 
                TContainerId aNewParentId, 
                const TDesC8& aQuickProperties );
        
        virtual void AuthenticationChanged( TBool aAuthenticated );
        virtual void AccountModified( TOperation aOperation, TInt32 aOwnerId, const TDesC8& aName, const TDesC8& aNewName, TContainerId aMailboxId );

        // inherited from MSearchClient
        virtual void MatchFound( TContainerId aMsgId, TContainerId aFolderId, const TDesC8& aPropertyBuf );
        virtual void SearchComplete( TInt aStatus );
        
        void ReadString8ArrayL(  const RMessage2& aMessage, TInt aIndex, RPointerArray<HBufC8>& aArray );
        void ReadString16ArrayL( const RMessage2& aMessage, TInt aIndex, RPointerArray<HBufC>& aArray );
        void ReadIdArrayL( const RMessage2& aMessage, TInt aIndex, RArray<TContainerId>& aArray );
        
        // FUNCTIONS TO SUPPORT AUTOMATED UNIT TESTING
        #ifdef _DEBUG
            void DoInjectBackupRestoreEventL( const RMessage2& aMessage );
            void DoInjectPointSecEvent( const RMessage2& aMessage );
            void DoSimulateLowDiskSpaceL( const RMessage2& aMessage );
            void DoGetEncryptionStateL( const RMessage2& aMessage );
            void DoShutdownServer( const RMessage2& aMessage );
        #endif

        void DoMatchFoundL( TContainerId aMsgId, TContainerId aFolderId, const TDesC8& aPropertyBuf );
        
        CBufferedMessageCreator& GetBufferedMessageCreatorL();
        
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
        
        CMessageStoreServer&   iServer;

        // These variables are used in DoPropertiesListL, which has a client request that could span
        // several messages.    
        RArray<TContainerId>               iIds;
        RPointerArray<HBufC8>              iPropertyNames;
        RPointerArray<HBufC>               iSearchStrings;
        TBool                              iUsingQuery;
        TMsgStorePropertiesListQueryParams iQueryParams;

        // observer-related attributes    
        RMessage2              iObserverMessage;    
        RArray<TMsgStoreEvent> iEventQueue;
        TBool                  iIsObserving;

        // search-related attributes
        RMessage2                          iGetMatchesMessage;
        CContainerStoreSearchHandler*      iSearchHandler;
        RPointerArray<CSearchResult>       iMatchMessages;
        RPointerArray<CMruAddress>         iMruAddressArray;
        RArray<TMsgStoreId>                iSortSessionIds;
        
        CBufferedMessageCreator* iBufferedMessageCreator;
        
        __LOG_DECLARATION

    }; // end class CMessageStoreSession

#endif //__MESSAGE_STORE_SESSION_H__
