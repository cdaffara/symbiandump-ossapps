/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines classes CIpsPlgSearch, TSearchParams, TMsgContainer
*
*/


#ifndef IPSPLGSEARCH_H
#define IPSPLGSEARCH_H

class CFSMailMessage;
class CIpsPlgMsgIterator;
class MFSMailBoxSearchObserver;
class CIpsPlgSearchOp;
class CIpsPlgMsgMapper;
class CIpsPlgSosBasePlugin;
class CCnvCharacterSetConverter;

/**
 * Class to handle searching from emails.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
NONSHARABLE_CLASS( CIpsPlgSearch ) : 
    public CBase
    {

public:

    /**
     * Symbian 2nd phase construcror
     *
     * @return None
     */
    static CIpsPlgSearch* NewL(
        CMsvSession& aMsvSession,
        CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Symbian 2nd phase construcror
     *
     * @return None
     */
    static CIpsPlgSearch* NewLC(
        CMsvSession& aMsvSession,
        CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Class destructor
     *
     * @return
     */
    virtual ~CIpsPlgSearch();

    /**
     * Cancels ongoing search operation.
     *
     * @return
     */
    void Cancel();

    /**
     * Clears the search result cache.
     *
     * @return
     */
    void ClearCache();

    /**
     * Searches specific string from the mails.
     *
     * @return
     */
    void SearchL( 
        const TFSMailMsgId& aMailBoxId,
        const RArray<TFSMailMsgId>& aFolderIds,
        const RPointerArray<TDesC>& aSearchStrings,
        const TFSMailSortCriteria& aSortCriteria,
        MFSMailBoxSearchObserver& aSearchObserver );


    /**
     * @return Message server session.
     */
    CMsvSession& Session();

    /**
     * @return Currently searched mailbox.
     */
    TMsvId MailboxId();

    /**
     * @return Currently searched folder.
     */
    TMsvId FolderId();

    /**
     * @return Currently searched mailbox id.
     */
    const TFSMailMsgId& CurrentMessage() const;

    /**
     * @return Search string in converted form for the mail.
     */
    const CDesC16Array& SearchStringsL();

    /**
     * @param aMessage Matching message.
     */
    void MatchFoundL();
    
    /**
     * Send note about search being finished.
     */
    void SearchFinishedL();
    
    /**
     * @return Next message on the line. Ownership to caller.
     */
    CImEmailMessage* NextMessageL();    
    
    /**
     * @return NULL, when mail not fetched.
     * @return Object with ownership, if the mail is fetched.
     */
    CImHeader* MessageHeaderL();
    
    /**
     * On return, parameter contains the subject of the message in the buffer.
     * Method takes care of reserving the needed memory to the buffer,
     * but caller is responsible for freeing it.
     * @param aSubject Subject to be returned.
     */
    void GetSubjectL( RBuf& aSubject );

    /**
     * On return, parameter contains the sender of the message in the buffer.
     * Method takes care of reserving the needed memory to the buffer,
     * but caller is responsible for freeing it.
     * @param aSender Sender to be returned.
     */
    void GetSenderL( RBuf& aSender );
    
    /**
     * Puts the items in the cache into a new order.
     */
    void Sort();

    /**
     * Collects id's of all messages in the mailbox.
     */
    void CollectMessagesL();

    /**
     * server asks client if to change the search priority (when calling)
     */
    void ClientRequiredSearchPriority( TInt *apRequiredSearchPriority );
    
protected:

    /**
     * Class constructor
     *
     * @return None
     */
    CIpsPlgSearch( 
        CMsvSession& aMsvSession, 
        CIpsPlgSosBasePlugin& aPlugin );

private: 
    
    /**
     * Symbian 2nd phase construct
     */
    void ConstructL();
    
    class TSearchParams;
    class TMsgContainer;
    
    /**
     * Finds all the folders from the mailbox excluding "deleted" folder.
     *
     * @param aParams Parameter object to be filled with folders.
     */
    void GetFoldersL( TSearchParams& aParams );
    
private:
    
    /**
     * Adds provided folder to list and searches for subfolders.
     *
     * @param aFolderId Id of the folder to be searched.
     * @param aFolders List of folders to be updated.
     */
    void GetFoldersL( 
        const TMsvId aFolderId, 
        RArray<TFSMailMsgId>& aFolders );

    /**
     * @return Next item in the list based on sorting order.
     */
    TMsgContainer& NextItem();

    /**
     * Recursive quicksort algorithm.
     *
     * @param aMessages List of messages to be sorted.
     * @param aLeft Left index for list to be sorted.
     * @param aRight Right index for list to be sorted.
     */
    void QuickSort( 
        RArray<TMsgContainer>& aMessages, 
        TInt aLeft, 
        TInt aRight );
        
    /**
     * Send current active message to observer.
     *
     * @return Id of the message sent.
     */
    TFSMailMsgId SendToObserverL();

    /**
     * Class to handle internal search parameters.
     */
    class TSearchParams
        {   
    
    public:
    
    // New functions

        inline TSearchParams() {};

        /**
         * Constructor
         */
        inline TSearchParams( 
            const RPointerArray<TDesC>& aSearchStrings,
            const TFSMailMsgId& aMailbox,
            const RArray<TFSMailMsgId> aFolderIds,
            const TFSMailSortCriteria& aSortCriteria );

        /**
         * Destructor
         */
        inline virtual ~TSearchParams();

        /**
         * Copies the content of the source parameter.
         *
         * @param aParams Parameters to be copied.
         * @return New modified parameter
         */
        inline TSearchParams& operator=( const TSearchParams& aParams );

        /**
         * Adds more content to current search.
         *
         * @param aParams Parameters to be copied.
         * @return New modified parameter
         */
        inline void operator+=( const TSearchParams& aParams );
        
        /**
         * Checks if the parameters contains same attributes. 
         * Note: This does not check, if the items are identical, 
         *       but that there are no changes. For example, if
         *       original has less search folders than previous one
         *       but all the folders match with new one, the parameters
         *       are considered same.
         *
         * @param aParams Parameters to be compared.
         * @return ETrue, when the content matches.
         */
        TBool operator==( const TSearchParams& aParams ) const;
            
        /**
         * Duplicates the items in the search array.
         *
         * @param aSearchStrings Strings to be duplicated.
         * @leave For fun.
         */
        inline void SetSearchStringsL(
            const RPointerArray<TDesC>& aSearchStrings );

        /**
         * Duplicates the items in the search array.
         *
         * @param aSearchStrings Strings to be duplicated.
         * @leave For fun.
         */
        inline void SetSearchStringsL(
            const CDesC16Array& aSearchStrings );
            
        /**
         * Duplicates the items in the folder id array.
         *
         * @param aFolderIds Ids to be duplicated.
         */
        inline void SetFolderIdsL(
            const RArray<TFSMailMsgId>& aFolderIds );

        /**
         * Adds new folders to current search.
         *
         * @since FS v1.0
         * @param aFolderIds List of folder ids.
         */
        inline void AddFolderIdsL(
            const RArray<TFSMailMsgId>& aFolderIds );

        /**
         * @return ETrue, when folder exists in the array.
         */
        inline TBool FolderExists( const TFSMailMsgId& aFolderId );

        /**
         * Deletes items in the string array and the array itself.
         */
        inline void RemoveArray();
            
    // Data    
    
        /**
         * Container for character converted strings.
         */
        CDesC16Array* iSearchStrings;    
        
        /**
         * Mailbox, which is being searched.
         */
        TFSMailMsgId iMailbox;

        /**
         * Folders to be searched.
         */
        RArray<TFSMailMsgId> iFolderIds;
            
        /**
         * Sorting rules.
         */
        TFSMailSortCriteria iSortCriteria;
        };

    /**
     * Helper class for entry compare.
     */
    class TMsgContainer
        {
    public:        
    
    // New Functions
    
        /**
         * @param aCriteria Sorting criterias.
         */


        /*TMsgContainer(
            const TMsvId& aId,
            const TFSMailSortCriteria& aCriteria,
            CMsvSession& aMsvSession );*/

        TMsgContainer( const TMsvEntry& aEntry,
            const TFSMailSortCriteria& aCriteria );

        /**
         * @param aMsgContainer MsgContainer to be compared.
         * @return ETrue, when content is greater than in parameter.
         */
        TBool operator>( const TMsgContainer& aMsgContainer ) const;

        /**
         * @param aMsgContainer MsgContainer to be compared.
         * @return ETrue, when content is greater than or equal in parameter.
         */
        TBool operator>=( const TMsgContainer& aMsgContainer ) const;

        /**
         * @param aMsgContainer MsgContainer to be compared.
         * @return ETrue, when content is greater than or equal in parameter.
         */
        TBool operator<=( const TMsgContainer& aMsgContainer ) const;

        /**
         * @param aMsgContainer MsgContainer to be compared.
         * @return ETrue, when content is greater than in parameter.
         */
        TMsgContainer& operator=( const TMsgContainer& aMsgContainer );

        /**
         * @return Id of the entry.
         */
        inline TMsvId Id() const;

    private:        
    
    // New Functions        

        /**
         * @return 
         */
        TMsvEntry Entry( const TMsvId aId ) const;

        /**
         *
         */
        static TMsvEntry Entry( CMsvSession& aMsvSession, const TMsvId aId );

        /**
         * Creates string objects and pushes them to cleanup stack
         *
         * @param aLeft First email
         * @param aRight Second email
         * 
         * @param aLeftSubject Subject for the first email
         * @param aRightSubject Subject for the second email
         * 
         * @param aLeftSender Sender for the first email
         * @param aRightSender Sender for the second email
         */
        void GetSenderStringsCC( 
            const TMsvId aLeft, 
            const TMsvId aRight,
            HBufC*& aLeftSender,      
            HBufC*& aRightSender ) const;
        
        void GetSubjectStringsCC( 
            const TMsvId aLeft, 
            const TMsvId aRight,
            HBufC*& aLeftSubject,
            HBufC*& aRightSubject ) const;
                        
    private:
        
    // Data        
        
        /**
         * Entry to be used for compare operations.
         */
        //TMsvId iId;
                
        /**
         * Reference to existing sort criteria.
         */
        const TFSMailSortCriteria& iCriteria;
        
        TMsvEntry iEntry;

        /**
         * Message server session.
         */
        //CMsvSession& iMsvSession;
        };

    /**
     * Parameters from previous search.
     */
    TSearchParams iPreviousSearch;
    
    /**
     * Parameters of currently ongoing search.
     */
    TSearchParams iCurrentSearch;  

    /**
     * Cache to store the search result.
     */
    RArray<TFSMailMsgId> iCache;

    /**
     * Message server session.
     */
    CMsvSession& iMsvSession;
        
    /**
     * Entry to do all the message store access work.
     * Pwned.
     */
    CMsvEntry* iActiveEntry;
    
    /**
     * Current message in search.
     */
    TFSMailMsgId iCurrentMessage;
    
    /**
     * Current search operation. NULL, when empty.
     * Owned.
     */
    CIpsPlgSearchOp* iOperation;
    
    /**
     * The client observer for search events.
     * Not owned.
     */
    MFSMailBoxSearchObserver* iObserver;        
    
    /**
     * Owning plugin.
     */
    CIpsPlgSosBasePlugin& iPlugin;
    
    /**
     * The message converter.
     * Owned.
     */
    CIpsPlgMsgMapper* iMapper; 
    
    /**
     * Character converter.
     * Owned.
     */
    //CCnvCharacterSetConverter* iCharConv;
    
    /**
     * Collection of Id's of all messages in the mailbox.
     * Owned.
     */
    RArray<TMsgContainer> iEmailMessages;
    };

#include "ipsplgsearch.inl"

#endif /* IPSPLGSEARCH_H */

// End of File
