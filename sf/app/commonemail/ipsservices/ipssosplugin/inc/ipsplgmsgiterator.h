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
* Description:  Freestyle message iterator implementation for Symbian Message
*                store
*
*/


#ifndef IPSPLGMSGITERATOR_H
#define IPSPLGMSGITERATOR_H

#include <e32def.h>             // NONSHARABLE_CLASS
//<cmail>
#include "MFSMailIterator.h"    // mfsmailiterator
//</cmail>

class TIpsPlgMsgKey;
class TIpsPlgMsgSwap;
class CMsvEntry;
class CMsvSession;
class CIpsPlgMsgMapper;
class CMsvEntrySelection;
class CIpsPlgSosBasePlugin;

/**
 *  Freestyle message iterator implementation for Symbian Message store
 *  
 *  Provides a service to read messages from Symbian Message Store
 *  according to the iterator pattern in the specified order.
 *
 *  @code
 *   TMsvId folderId;
 *   CMsvSession* msvSession;
 *   RArray<TFSMailSortCriteria> sortCriteria;
 *   RPointerArray<CFSMailMessage> messages;
 *   TBool moreAvailable;
 *   ..
 *   CMsvEntry* folderEntry = msvSession->GetEntryL( folderId );
 *   CIpsPlgMsgIterator* iterator = CIpsPlgMsgIterator::NewL(
 *       entry, EFSMsgDataDate, sortCriteria );
 *   CleanupStack::PushL( iterator );
 *   // 'Empty' TFSMailId can be used to refer to the first message
 *   moreAvailable = iterator->NextL( TFSMailId(), KIteratorBlockSize, 
 *       messages );
 *   CleanupStack::PopAndDestroy( iterator );
 *  @endcode
 *
 *  @lib ipssosplugin.lib
 *  @since 
 */
NONSHARABLE_CLASS( CIpsPlgMsgIterator )
    : public CBase, public MFSMailIterator
    {
public:
    /**
     * Two-phased constructor.
     * @param aPlugin     Owning plugin
     * @param aMsvSession Symbian messaging framework session
     * @param aMailboxId  Mailbox identifier
     * @param aFolderId   Identifier of the folder to be iterated
     * @param aDetails    Specifies the data to be included to the message 
     *                    objects returned by the iterator
     * @param aSorting    Specifies the order of the messages in which they are
     *                    iterated
     */
     static CIpsPlgMsgIterator* NewL(
        CIpsPlgSosBasePlugin& aPlugin,
        CMsvSession& aMsvSession,
        const TFSMailMsgId& aMailboxId,
        const TFSMailMsgId aFolderId,
        const TFSMailDetails aDetails,
        const RArray<TFSMailSortCriteria>& aSorting);
    
    /**
     * Two-phased constructor.
     * @param aPlugin      Owning plugin
     * @param aFolderEntry Pointer to the folder entry to be iterated. 
     *                     The ownership is transferred to the iterator
     * @param aMailboxId  Mailbox identifier
     * @param aDetails     Specifies the data to be included to the message 
     *                     objects returned by the iterator
     * @param aSorting     Specifies the order of the messages in which they are
     *                     iterated
     */
    static CIpsPlgMsgIterator* NewL(
        CIpsPlgSosBasePlugin& aPlugin,
        CMsvEntry* aFolderEntry, 
        const TFSMailMsgId& aMailboxId,
        const TFSMailDetails aDetails,
        const RArray<TFSMailSortCriteria>& aSorting);
    

    /**
    * Destructor.
    */
    virtual ~CIpsPlgMsgIterator();

public: // from MFSMailIterator

    /**
     * From MFSMailIterator
     *
     * @since 
     */
    virtual TBool NextL(
        TFSMailMsgId aCurrentMessageId, 
        TUint aCount, 
        RPointerArray<CFSMailMessage>& aMessages);
    
    /**
     * From MFSMailIterator
     *
     * @since 
     */
    virtual TBool NextL(
        const TDesC&  aStartWith, 
        TUint aCount, 
        RPointerArray<CFSMailMessage>& aMessages);
    
    /**
     * From MFSMailIterator
     *
     * @since 
     */
    virtual TBool PreviousL(
        TFSMailMsgId aCurrentMessageId, 
        TUint aCount, 
        RPointerArray<CFSMailMessage>& aMessages);
    
    /**
     * From MFSMailIterator
     *
     * @since 
     */
    virtual TBool PreviousL(
        const TDesC&  aStartWith, 
        TUint aCount, 
        RPointerArray<CFSMailMessage>& aMessages);

private:
    /**
     * Returns the requested number of messages after the given message
     * from the message selection
     * @param aStart          Index of the first message to be returned
     * @param aMessageEntries Message ID array from which the messages are 
     *                        selected
     * @param aCount          Number of messages to be returned
     * @param aMessages       Array of return values
     * @return ETrue, if there are more messages after the last returned 
     *         message
     */
    TBool NextL(
        TInt aStart,
        CMsvEntrySelection* aMessageEntries,
        TUint aCount,
        RPointerArray<CFSMailMessage>& aMessages);

    /**
     * Returns the requested number of messages before the given message
     * from the message selection
     * @param aStart          Index of the last message to be returned
     * @param aMessageEntries Message ID array from which the messages are 
     *                        selected
     * @param aCount          Number of messages to be returned
     * @param aMessages       Array of return values
     * @return ETrue, if there are more messages after the last returned 
     *         message
     */
    TBool PreviousL(
        TInt aEnd,
        CMsvEntrySelection* aMessageEntries,
        TUint aCount,
        RPointerArray<CFSMailMessage>& aMessages);

    /**
     * Sorts the message array
     */
    void Sort();
    
    /**
     * Searches from the message array a message the subject or the sender
     * of which starts with the search string.
     * @param aMessageEntries Message ID array
     * @param aStartWith      Search string
     * @return KErrNotSupported: search not supported with the active sort 
     *         criteria, KErrNotFound: matching message not found, KErrNone: 
     *         matching message found
     */
    TInt SearchL( 
        CMsvEntrySelection* aMessageEntries,
        const TDesC&  aStartWith,
        TInt& aIndex );
    
    CMsvEntrySelection* FilterMessagesL();

private:

    CIpsPlgMsgIterator();
    CIpsPlgMsgIterator( 
        CIpsPlgSosBasePlugin& aPlugin,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailDetails aDetails );
    
    void ConstructL(        
        CMsvSession& aMsvSession,
        const TFSMailMsgId aFolderId,
        const RArray<TFSMailSortCriteria>& aSorting);
    
    void ConstructL(        
        CMsvEntry* aFolderEntry, 
        const RArray<TFSMailSortCriteria>& aSorting);
    
    void StoreSortCriteriaL( 
        const RArray<TFSMailSortCriteria>& aSorting );
    
private: // data

    
    /**
     * Owning plugin
     */
    CIpsPlgSosBasePlugin& iPlugin;

    /**
     * Entry of the folder to be iterated.
     * Own.
     */
    CMsvEntry* iFolderEntry;
    
    CMsvEntrySelection* iMessages;
    /**
     * Controls data to be returned
     */    
    TFSMailDetails iRequestedDetails;

    /**
     * Symbian -> FS message data converter
     */    
    CIpsPlgMsgMapper* iMsgMapper;

    /**
     * Sorting objects
     */    
    TIpsPlgMsgKey*                  iMsgSortKey;
    TIpsPlgMsgSwap*                 iMsgSwapper;
    TBool                           iSortingOn;
    RArray<TFSMailSortCriteria>     iSortingCriteria;

    /**
     * Mailbox identifier
     */ 
    const TFSMailMsgId iMailboxId;
    
    //Not Owned
    CMsvSession* iMsvSession;
    };

#endif // IPSPLGMSGITERATOR_H
