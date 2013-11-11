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


#include "emailtrace.h"
#include "ipsplgheaders.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsPlgMsgIterator* CIpsPlgMsgIterator::NewL(
    CIpsPlgSosBasePlugin& aPlugin,
    CMsvSession& aMsvSession,
    const TFSMailMsgId& aMailboxId,
    const TFSMailMsgId aFolderId,
    const TFSMailDetails aDetails,
    const RArray<TFSMailSortCriteria>& aSorting)
    {
    FUNC_LOG;
    CIpsPlgMsgIterator* self = 
        new( ELeave ) CIpsPlgMsgIterator( aPlugin, aMailboxId, aDetails );
    CleanupStack::PushL( self );
    self->ConstructL( aMsvSession, aFolderId, aSorting );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CIpsPlgMsgIterator* CIpsPlgMsgIterator::NewL(
    CIpsPlgSosBasePlugin& aPlugin,
    CMsvEntry* aFolderEntry, 
    const TFSMailMsgId& aMailboxId,
    const TFSMailDetails aDetails,
    const RArray<TFSMailSortCriteria>& aSorting)
    {
    FUNC_LOG;
    CIpsPlgMsgIterator* self = 
        new( ELeave ) CIpsPlgMsgIterator( aPlugin, aMailboxId, aDetails );
    CleanupStack::PushL( self );
    self->ConstructL( aFolderEntry, aSorting );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Basic destructor
// ---------------------------------------------------------------------------
//
CIpsPlgMsgIterator::~CIpsPlgMsgIterator()
    {
    FUNC_LOG;
    delete iFolderEntry;
    delete iMsgMapper;
    delete iMsgSortKey;
    delete iMsgSwapper;
    delete iMessages;
    iSortingCriteria.Reset();
    }

// ---------------------------------------------------------------------------
// Searches the message matching with aCurrentMessageId and requests the
// chunk of messages following the matching message
// ---------------------------------------------------------------------------
//
TBool CIpsPlgMsgIterator::NextL(
    TFSMailMsgId aCurrentMessageId, 
    TUint aCount, 
    RPointerArray<CFSMailMessage>& aMessages)
    {
    FUNC_LOG;
    TBool result = EFalse;
    TInt baseIndex;
    
    if ( !iMessages )
        {
        // Messages are sorted before first reading the messages 
        Sort();
        iMessages = FilterMessagesL();
        }

    if ( !aCurrentMessageId.IsNullId() )
        {
        baseIndex = iMessages->Find( aCurrentMessageId.Id() );
        
        // aCurrentMessageId is not included to the result set
        if ( baseIndex != KErrNotFound )
            {
            baseIndex += 1;
            }
        }
    else
        {
        // start from the beginning of the message list
        baseIndex = 0;
        }
        
    if ( ( baseIndex != KErrNotFound ) && 
         ( baseIndex < iMessages->Count() ) )
        {
        result = NextL(baseIndex, iMessages, aCount, aMessages);
        }

    return result;
    }

// ---------------------------------------------------------------------------
// Searches message matching with the search string and returns a chunk of
// messages starting from the 1st matched message
// ---------------------------------------------------------------------------
//
TBool CIpsPlgMsgIterator::NextL(
    const TDesC&  aStartWith, 
    TUint aCount, 
    RPointerArray<CFSMailMessage>& aMessages)
    {
    FUNC_LOG;
    TBool result = EFalse;
    TInt status;
    TInt baseIndex;
    
    if ( !iMessages )
        {
        // Messages are sorted before first reading the messages 
        Sort();
        iMessages = FilterMessagesL();
        }

    status = SearchL( iMessages, aStartWith, baseIndex );
    
    if ( status == KErrNone )
        {
        result = NextL( baseIndex, iMessages, aCount, aMessages );
        }

    return result;
    }

// ---------------------------------------------------------------------------
// Like NextL() with similar parameters, the method searches the message 
// matching with aCurrentMessageId and requests a chunk of messages preceding 
// the matching message
// ---------------------------------------------------------------------------
//
TBool CIpsPlgMsgIterator::PreviousL(
    TFSMailMsgId aCurrentMessageId, 
    TUint aCount, 
    RPointerArray<CFSMailMessage>& aMessages)
    {
    FUNC_LOG;
    TBool result = EFalse;
    TInt baseIndex;
    
    if ( !iMessages )
        {
        // Messages are sorted before first reading the messages 
        Sort();
        iMessages = FilterMessagesL();
        }

    if ( !aCurrentMessageId.IsNullId() )
        {
        baseIndex = iMessages->Find(aCurrentMessageId.Id());
        
        // aCurrentMessageId is not included to the result set
        if ( baseIndex != KErrNotFound )
            {
            baseIndex -= 1;
            }
        }
    else
        {
        // check whether we should start from the end of 
        // the message list in the case of a NULL ID
        baseIndex = iMessages->Count() - 1;
        }

    // Actually, if the matching message is the first one, baseIndex is equal
    // to -1 which is also the value of KErrNotFound. So, the condition could be
    // simpler, but it would not inappropriate to trust that the numerical value 
    // of the error code is known.
    if ( ( baseIndex != KErrNotFound ) &&
         ( baseIndex >= 0 ) )
        {
        result = PreviousL(baseIndex, iMessages, aCount, aMessages);
        }

    return result;
    }

// ---------------------------------------------------------------------------
// Searches the first message matching with the search string and requests
// a chunk of messages including the matching message and messages preceding 
// it.
// The return value tells currently only, if there are more messages, but it
// is not checked whether they are deleted. It is probably more important
// to return as fast as possible than check the remaining messages. The same
// concerns PreviousL().
// ---------------------------------------------------------------------------
//
TBool CIpsPlgMsgIterator::PreviousL(
    const TDesC&  aStartWith, 
    TUint aCount, 
    RPointerArray<CFSMailMessage>& aMessages)
    {
    FUNC_LOG;
    TBool result = EFalse;
    TInt status;
    TInt baseIndex;
    
    // Messages are sorted always before reading the messages 
    Sort();

    CMsvEntrySelection* messages = FilterMessagesL();
    CleanupStack::PushL( messages );

    status = SearchL( iMessages, aStartWith, baseIndex );
    
    if (  status == KErrNone ) 
        {
        result = PreviousL(baseIndex, iMessages, aCount, aMessages);
        }

    return result;
    }

// ---------------------------------------------------------------------------
// Loops over the requested messages noticing the ends of the message array
// ---------------------------------------------------------------------------
//
TBool CIpsPlgMsgIterator::NextL(
    TInt aStart,
    CMsvEntrySelection* aMessageEntries,
    TUint aCount,
    RPointerArray<CFSMailMessage>& aMessages)
    {
    FUNC_LOG;
    TInt i = aStart;
    TInt counter( 0 );
    CFSMailMessage* fsMsg;
        
    while (( counter < aCount ) && ( i < aMessageEntries->Count() ) )
        {
        const TMsvEmailEntry& entry( 
            iFolderEntry->ChildDataL( aMessageEntries->At(i) ) );
        if ( ( EDisconnectedDeleteOperation != entry.DisconnectedOperation() ) &&
        	(( entry.iMtm != KSenduiMtmImap4Uid )  || !entry.DeletedIMAP4Flag() ) &&
        	 ( entry.iType == KUidMsvMessageEntry ) )
            {
            fsMsg = iMsgMapper->GetMailMessageL( iMailboxId, entry, 
                iRequestedDetails );
            aMessages.AppendL( fsMsg );
            counter++;
            }
        i++;
        }
    
    return ( i < aMessageEntries->Count() );
    }

// ---------------------------------------------------------------------------
// Loops over the requested messages noticing the ends of the message array.
// The implementation is somewhat more complex than the corresponding NextL()
// because the messages are processed in the order they are inserted to the
// result array.
// ---------------------------------------------------------------------------
//
TBool CIpsPlgMsgIterator::PreviousL(
    TInt aEnd,
    CMsvEntrySelection* aMessageEntries,
    TUint aCount,
    RPointerArray<CFSMailMessage>& aMessages)
    {
    FUNC_LOG;
    __ASSERT_DEBUG( ( aEnd < aMessageEntries->Count() ), 
        User::Panic( KIpsPlgPanicCategory, EIpsPlgInvalidMessageIndex ) );
     
    TInt i = aEnd;
    TInt counter( 0 );
    CFSMailMessage* fsMsg; 
    
    while (( counter < aCount ) && ( i >= 0 ) )
        {
        const TMsvEmailEntry& entry( 
            iFolderEntry->ChildDataL( aMessageEntries->At(i) ) );
        if ( ( EDisconnectedDeleteOperation != entry.DisconnectedOperation() ) &&
        	(( entry.iMtm != KSenduiMtmImap4Uid )  || !entry.DeletedIMAP4Flag() ) &&	
   	 		 ( entry.iType == KUidMsvMessageEntry ) )
            {
            fsMsg = iMsgMapper->GetMailMessageL( iMailboxId, entry, 
                iRequestedDetails );
            aMessages.InsertL( fsMsg, 0 );
            counter++;
            }
        i--;
        }
    
    return ( i > 0 );
    }

// ---------------------------------------------------------------------------
// Constructor. Initializes the static data members.
// ---------------------------------------------------------------------------
//
CIpsPlgMsgIterator::CIpsPlgMsgIterator( 
    CIpsPlgSosBasePlugin& aPlugin,
    const TFSMailMsgId& aMailboxId,
    const TFSMailDetails aDetails )
    : iPlugin( aPlugin ), iRequestedDetails( aDetails ), 
      iMailboxId( aMailboxId )
    {
    FUNC_LOG;
    // none
    }

// ---------------------------------------------------------------------------
// Second level constructor. Creates the data members to the heap.
// Creates also the folder CMsvEntry.
// ---------------------------------------------------------------------------
//
void CIpsPlgMsgIterator::ConstructL(        
    CMsvSession& aMsvSession,
    const TFSMailMsgId aFolderId,
    const RArray<TFSMailSortCriteria>& aSorting )
    {
    FUNC_LOG;
    iFolderEntry  = aMsvSession.GetEntryL( aFolderId.Id() );
    
    StoreSortCriteriaL(aSorting);
    iMsgSortKey   = new (ELeave) TIpsPlgMsgKey( *iFolderEntry, iSortingCriteria );
    iMsgSwapper   = new (ELeave) TIpsPlgMsgSwap( *iFolderEntry );
    iSortingOn    = 
        ( aSorting.Count() > 0 ) && ( aSorting[0].iField != EFSMailDontCare);
    iMsgMapper    = CIpsPlgMsgMapper::NewL( aMsvSession, iPlugin );
    iMsvSession   = &aMsvSession;
    }
    
// ---------------------------------------------------------------------------
// Second level constructor. Creates the data members to the heap.
// Version which gets the folder CMsvEntry as a parameter.
// ---------------------------------------------------------------------------
//
void CIpsPlgMsgIterator::ConstructL(        
    CMsvEntry* aFolderEntry, 
    const RArray<TFSMailSortCriteria>& aSorting )
    {
    FUNC_LOG;
    iFolderEntry  = aFolderEntry;
    StoreSortCriteriaL(aSorting);
    iMsgSortKey   = new (ELeave) TIpsPlgMsgKey( *iFolderEntry, iSortingCriteria );
    iMsgSwapper   = new (ELeave) TIpsPlgMsgSwap( *iFolderEntry );
    iSortingOn    = 
        ( aSorting.Count() > 0 ) && ( aSorting[0].iField != EFSMailDontCare);
    iMsgMapper = CIpsPlgMsgMapper::NewL( aFolderEntry->Session(), iPlugin );
    iMsvSession = NULL;
    }
    
// ---------------------------------------------------------------------------
// Sorting is based on the quick sort algorithm implemented in User module
// ---------------------------------------------------------------------------

void CIpsPlgMsgIterator::Sort()
    {
    FUNC_LOG;
    if ( iSortingOn )
        {
        User::QuickSort( iFolderEntry->Count(), *iMsgSortKey, *iMsgSwapper );
        }
    }
    
// ---------------------------------------------------------------------------
// Searches the first message matching the search string.
// This function is supported only when the messages are sorted by the subject
// or by the sender (or the receiver in Drafts/Outbox/Sent folders) as 
// a primary sorting key. The fields to be compared is selected on the basis 
// of the sorting key.
// ---------------------------------------------------------------------------

TInt CIpsPlgMsgIterator::SearchL( 
    CMsvEntrySelection* aMessageEntries,
    const TDesC& aStartWith,
    TInt& aIndex )
    {
    FUNC_LOG;
    TInt result( KErrNotFound );
    TInt i;
    TInt findStatus;
    TInt offset;
    TFSMailSortField primaryKey = EFSMailDontCare;
    TPtrC subject;

    // Check whether the search is supported
    if ( iSortingCriteria.Count() > 0 )
        {
        primaryKey = iSortingCriteria[0].iField;
        }
    if ( !iSortingOn ||
         ( ( primaryKey != EFSMailSortBySender ) &&
           ( primaryKey != EFSMailSortBySubject ) ) )
        {
        result = KErrNotSupported;
        }
        
    // Actual search        
    i = 0;
    while ( ( result == KErrNotFound ) && ( i < aMessageEntries->Count() ) )
        {
        const TMsvEntry& entry( 
            iFolderEntry->ChildDataL( aMessageEntries->At(i) ) );

        findStatus = KErrNotFound;

        if ( primaryKey == EFSMailSortBySender )
            {
            findStatus = entry.iDetails.FindF( aStartWith );
            }
        if ( primaryKey == EFSMailSortBySubject )
            {
            // Strip the prefixes (Re:, Fwd: etc) from the subject before
            // matching
            offset = iMsgSortKey->FindSubjectStart( entry.iDescription );
            subject.Set( 
                entry.iDescription.Ptr() + offset, 
                entry.iDescription.Length() - offset );
            findStatus = subject.FindF( aStartWith );
            }
        
        // Checks whether a matching message has been found
        if ( findStatus == 0 ) 
            {
            result = KErrNone;
            aIndex = i;
            }

        i++;
        }
        
    return result;
    }
// ---------------------------------------------------------------------------
// Filters messages in global folders (Drafts/Sent/Outbox) that belongs to
// specified mailbox. When messages in inbox asked, returns selection from 
// wanted mailbox.
// ---------------------------------------------------------------------------
CMsvEntrySelection* CIpsPlgMsgIterator::FilterMessagesL()
	{
    FUNC_LOG;
	TMsvId serviceId;
	TMsvEntry tEntry;
	TMsvEntry mailboxServiceEntry;
	CMsvEntrySelection* filteredEntries;
	
	if( iMsvSession && iFolderEntry->Entry().Parent() == KMsvLocalServiceIndexEntryIdValue )
		{
		User::LeaveIfError(
			iMsvSession->GetEntry( iMailboxId.Id(), serviceId, tEntry ) );
		User::LeaveIfError(
		    iMsvSession->GetEntry( tEntry.iRelatedId, serviceId, mailboxServiceEntry ) );
			
		filteredEntries = iFolderEntry->ChildrenWithServiceL( serviceId );
		}
	else
		{
 		filteredEntries = iFolderEntry->ChildrenWithServiceL( iMailboxId.Id() );
		}
	return filteredEntries;
	}

// -----------------------------------------------------------------------------
// stores sort criteria given by user
// -----------------------------------------------------------------------------
void CIpsPlgMsgIterator::StoreSortCriteriaL( 
     const RArray<TFSMailSortCriteria>& aSorting )
    {
    for ( TInt i=0; i < aSorting.Count(); i++ )
        {
        const TFSMailSortCriteria& criteria  = aSorting[i];
        TFSMailSortCriteria fsCriteria;
        fsCriteria.iField = criteria.iField;
        fsCriteria.iOrder = criteria.iOrder;
        iSortingCriteria.AppendL(fsCriteria);
        }
    }

