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
* Description: This file implements classes TMsgContainer, Search, Search, Search. 
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"


// To skip deleted folder
_LIT( KIpsPlgDeleted, "Deleted" );

// ======== CLASS TSEARCHPARAMS ======== 

// ======== MEMBER FUNCTIONS ======== 

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TSearchParams::operator==()
// ---------------------------------------------------------------------------
//
TBool CIpsPlgSearch::TSearchParams::operator==( 
    const TSearchParams& aParams ) const
    {
    FUNC_LOG;
    // This variable indicates if the search is same as previous search or
    // not. If the search is new, previous data can be reused.
    TBool isNew = EFalse;
    
    // Search strings must have an exact match
    TInt newCnt = aParams.iSearchStrings->Count();
    TInt oldCnt = iSearchStrings ? iSearchStrings->Count() : KErrNotFound;
    if ( newCnt == oldCnt )
        {
        while ( !isNew && --newCnt >= 0 )
            {
            while ( !isNew && --oldCnt >= 0 )
                {
                // Even if one of the items fails, the search is different.
                if ( ( *aParams.iSearchStrings )[newCnt].Compare( 
                    ( *iSearchStrings )[oldCnt] ) )
                    {
                    isNew = ETrue;
                    }
                }
            }
        }
    else
        {
        // This search has different 
        isNew = ETrue;
        }
    
    // Search strings are the same than in previous round. If search
    // folders are added, but not removed, then search can continue
    // from last location.
    if ( !isNew )
        {
        isNew = EFalse;
        newCnt = aParams.iFolderIds.Count();
        oldCnt = iFolderIds.Count();

        // If folder count has been added, look for added folders.
        // If there is folder removed, search has to be started again.
        if ( newCnt >= oldCnt )
            {
            TInt hits = 0;
            TBool found = EFalse;
            
            // All of the old folders must be found from the new folders.
            for ( TInt old = 0; old < oldCnt; old++ )
                {
                found = EFalse;
                
                for ( TInt item = 0; !found && item < newCnt; item++ )
                    {
                    found = iFolderIds[old] == aParams.iFolderIds[item];
                    hits += found;
                    }
                }    
            
            isNew = hits != oldCnt;                                        
            }                                
        }
    // Test is about: "is the new search the same as previous?" 
    // so if the result of the above analyze is "new search", in that 
    // case we must return the answer: "no, search is not the same".
    // Confusing huh?
    return !isNew;
    }
    
// ======== CLASS TMSGCONTAINER ======== 

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::TMsgContainer()
// ---------------------------------------------------------------------------
//
/*
CIpsPlgSearch::TMsgContainer::TMsgContainer(
    const TMsvId& aId,
    const TFSMailSortCriteria& aCriteria,
    CMsvSession& aMsvSession )
    :
    iId( aId ),
    iCriteria( aCriteria ),
    iMsvSession( aMsvSession )
    {
    FUNC_LOG;
    }
*/
CIpsPlgSearch::TMsgContainer::TMsgContainer( const TMsvEntry& aEntry,
    const TFSMailSortCriteria& aCriteria ) :
    iCriteria( aCriteria ), iEntry( aEntry )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::TMsgContainer()
// ---------------------------------------------------------------------------
//
TBool CIpsPlgSearch::TMsgContainer::operator>( 
    const TMsgContainer& aMsgContainer ) const
    {
    FUNC_LOG;
    TBool result = EFalse;
    
    switch ( iCriteria.iField )
        {
    default:        
    case EFSMailDontCare:
        break;
        
    case EFSMailSortByDate:
        result = iEntry.iDate > aMsgContainer.iEntry.iDate;
        break;

    case EFSMailSortBySender:
        {
        /*HBufC* leftSender = NULL;
        HBufC* rightSender = NULL;
        
        GetSenderStringsCC( 
            iId, 
            aMsgContainer.Id(),  
            leftSender, 
            rightSender );
        
        result = *leftSender > *rightSender;
        CleanupStack::PopAndDestroy( 2, leftSender );*/
        result = iEntry.iDetails > aMsgContainer.iEntry.iDetails;
        }
        break;
    
    case EFSMailSortBySubject:
        {
        /*HBufC* leftSubject = NULL;
        HBufC* rightSubject = NULL;
        
        GetSubjectStringsCC( 
            iId, 
            aMsgContainer.Id(), 
            leftSubject, 
            rightSubject );

        result = *leftSubject > *rightSubject;
        CleanupStack::PopAndDestroy( 2, leftSubject );*/
        result = iEntry.iDescription > aMsgContainer.iEntry.iDescription;
        }
        break;

    case EFSMailSortByPriority:
        result = iEntry.Priority() > aMsgContainer.iEntry.Priority();
        break;
    
    case EFSMailSortByFlagStatus:
        result = iEntry.New() > aMsgContainer.iEntry.New();
        break;
    
    case EFSMailSortByUnread:
        result = iEntry.Unread() > aMsgContainer.iEntry.Unread();
        break;
    
    case EFSMailSortBySize:
        result = iEntry.iSize > aMsgContainer.iEntry.iSize;
        break;
    
    case EFSMailSortByAttachment:
        result = iEntry.iDate > aMsgContainer.iEntry.iDate;
        break;
        }
           
    return result;        
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::TMsgContainer()
// ---------------------------------------------------------------------------
//
TBool CIpsPlgSearch::TMsgContainer::operator>=( 
    const TMsgContainer& aMsgContainer ) const
    {
    FUNC_LOG;
    TBool result = EFalse;
    
    switch ( iCriteria.iField )
        {
    default:        
    case EFSMailDontCare:
        break;
        
    case EFSMailSortByDate:
        result = iEntry.iDate >= aMsgContainer.iEntry.iDate;
        break;
        
    case EFSMailSortBySender:
        {
        /*HBufC* leftSender = NULL;
        HBufC* rightSender = NULL;
        
        GetSenderStringsCC( 
            iId, 
            aMsgContainer.Id(),  
            leftSender, 
            rightSender );
        
        result = *leftSender >= *rightSender;
        CleanupStack::PopAndDestroy( 2, leftSender );*/
        result = iEntry.iDetails > aMsgContainer.iEntry.iDetails;
        }
        break;
        
    case EFSMailSortBySubject:
        {
        /*HBufC* leftSubject = NULL;
        HBufC* rightSubject = NULL;
        
        GetSubjectStringsCC( 
            iId, 
            aMsgContainer.Id(), 
            leftSubject, 
            rightSubject );

        result = *leftSubject >= *rightSubject;
        CleanupStack::PopAndDestroy( 2, leftSubject );*/
        result = iEntry.iDescription > aMsgContainer.iEntry.iDescription;
        }
        break;

    case EFSMailSortByPriority:
        result = iEntry.Priority() >= aMsgContainer.iEntry.Priority();
        break;

    case EFSMailSortByFlagStatus:
        result = iEntry.New() >= aMsgContainer.iEntry.New();
        break;

    case EFSMailSortByUnread:
        result = iEntry.Unread() >= aMsgContainer.iEntry.Unread();
        break;

    case EFSMailSortBySize:
        result = iEntry.iSize >= aMsgContainer.iEntry.iSize;
        break;

    case EFSMailSortByAttachment:
        result = iEntry.iDate >= aMsgContainer.iEntry.iDate;
        break;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::TMsgContainer()
// ---------------------------------------------------------------------------
//
TBool CIpsPlgSearch::TMsgContainer::operator<=( 
    const TMsgContainer& aMsgContainer ) const
    {
    FUNC_LOG;
    TBool result = EFalse;
    
    switch ( iCriteria.iField )
        {
    default:        
    case EFSMailDontCare:
        break;
        
    case EFSMailSortByDate:
        result = iEntry.iDate <= aMsgContainer.iEntry.iDate;
        break;

    case EFSMailSortBySender:
        {
        /*HBufC* leftSender = NULL;
        HBufC* rightSender = NULL;
        
        GetSenderStringsCC( 
            iId, 
            aMsgContainer.Id(),  
            leftSender, 
            rightSender );
        
        result = *leftSender <= *rightSender;
        CleanupStack::PopAndDestroy( 2, leftSender );*/
        result = iEntry.iDetails > aMsgContainer.iEntry.iDetails;
        }
        break;
    
    case EFSMailSortBySubject:
        {
        /*HBufC* leftSubject = NULL;
        HBufC* rightSubject = NULL;
        
        GetSubjectStringsCC( 
            iId, 
            aMsgContainer.Id(), 
            leftSubject, 
            rightSubject );

        result = *leftSubject <= *rightSubject;
        CleanupStack::PopAndDestroy( 2, leftSubject );*/
        result = iEntry.iDescription > aMsgContainer.iEntry.iDescription;
        }
        break;

    case EFSMailSortByPriority:
        result = iEntry.Priority() <= aMsgContainer.iEntry.Priority();
        break;

    case EFSMailSortByFlagStatus:
        result = iEntry.New() <= aMsgContainer.iEntry.New();
        break;

    case EFSMailSortByUnread:
        result = iEntry.Unread() <= aMsgContainer.iEntry.Unread();
        break;

    case EFSMailSortBySize:
        result = iEntry.iSize <= aMsgContainer.iEntry.iSize;
        break;

    case EFSMailSortByAttachment:
        result = iEntry.iDate <= aMsgContainer.iEntry.iDate;
        break;
        }
        
    return result;        
    }

/*
// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::GetSenderStringsCC()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::TMsgContainer::GetSenderStringsCC( 
    const TMsvId aLeft, 
    const TMsvId aRight,
    HBufC*& aLeftSender,
    HBufC*& aRightSender ) const
    {
    FUNC_LOG;
    TMsvEntry entry = Entry( aLeft );
    aLeftSender = entry.iDetails.Alloc();
    TRAP_IGNORE( CleanupStack::PushL( aLeftSender ) );
    
    entry = Entry( aRight );
    aRightSender = entry.iDetails.Alloc();
    TRAP_IGNORE( CleanupStack::PushL( aRightSender ) );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::GetSubjectStringsCC()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::TMsgContainer::GetSubjectStringsCC( 
    const TMsvId aLeft, 
    const TMsvId aRight,
    HBufC*& aLeftSubject,
    HBufC*& aRightSubject ) const
    {
    FUNC_LOG;
    TMsvEntry entry = Entry( aLeft );
    aLeftSubject = entry.iDescription.Alloc();
    TRAP_IGNORE( CleanupStack::PushL( aLeftSubject ) );
    
    entry = Entry( aRight );
    aRightSubject = entry.iDescription.Alloc();
    TRAP_IGNORE( CleanupStack::PushL( aRightSubject ) );
    }
*/
// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::TMsgContainer()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch::TMsgContainer& CIpsPlgSearch::TMsgContainer::operator=( 
    const TMsgContainer& aMsgContainer )
    {
    FUNC_LOG;
    iEntry = aMsgContainer.iEntry;
    /*Id() = aMsgContainer.Id();*/
    return *this;
    }
/*
// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::Entry()
// ---------------------------------------------------------------------------
//
TMsvEntry CIpsPlgSearch::TMsgContainer::Entry( const TMsvId aId ) const
    {
    FUNC_LOG;
    return CIpsPlgSearch::TMsgContainer::Entry( iMsvSession, aId );
    }
    
// ---------------------------------------------------------------------------
// CIpsPlgSearch::TMsgContainer::Entry()
// ---------------------------------------------------------------------------
//
TMsvEntry CIpsPlgSearch::TMsgContainer::Entry( 
    CMsvSession& aMsvSession, 
    const TMsvId aId )
    {
    FUNC_LOG;
    TMsvId service;
    TMsvEntry entry;
    aMsvSession.GetEntry( aId, service, entry );
    return entry;
    }
    */

// ======== CLASS CIPSPLGSEARCH ========     
    
// ======== CONSTRUCTORS & DESTRUCTOR ========

// ---------------------------------------------------------------------------
// CIpsPlgSearch::NewL()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch* CIpsPlgSearch::NewL(
    CMsvSession& aMsvSession,
    CIpsPlgSosBasePlugin& aPlugin )
    {
    FUNC_LOG;
    CIpsPlgSearch* self = CIpsPlgSearch::NewLC( aMsvSession, aPlugin );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::NewLC()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch* CIpsPlgSearch::NewLC(
    CMsvSession& aMsvSession,
    CIpsPlgSosBasePlugin& aPlugin )
    {
    FUNC_LOG;
    CIpsPlgSearch* self = new( ELeave ) CIpsPlgSearch( aMsvSession, aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::~CIpsPlgSearch()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch::~CIpsPlgSearch()
    {
    FUNC_LOG;
    // Any running search operations must be cancelled before deletion.
    if ( iOperation )
        {
        iOperation->Cancel();
        }
        
    delete iOperation;
    iOperation = NULL;
    
    iObserver = NULL;
    
    delete iActiveEntry;
    iActiveEntry = NULL;
    
    delete iMapper;
    iMapper = NULL;
    
    iCache.Reset();
    iEmailMessages.Reset();
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::CIpsPlgSearch()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch::CIpsPlgSearch(
    CMsvSession& aMsvSession,
    CIpsPlgSosBasePlugin& aPlugin )
    :
    iMsvSession( aMsvSession ),
    iActiveEntry( NULL ),
    iOperation( NULL ),
    iObserver( NULL ),
    iPlugin( aPlugin ),
    iMapper( NULL )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::ConstructL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::ConstructL()
    {
    FUNC_LOG;
    iMapper = CIpsPlgMsgMapper::NewL( iMsvSession, iPlugin );
    iActiveEntry = iMsvSession.GetEntryL( KMsvLocalServiceIndexEntryId );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::Cancel()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::Cancel()
    {
    FUNC_LOG;
    // Cancel the operation before deleting.
    if ( iOperation )
        {
        iOperation->Cancel();
        }
        
    // All checks are based on pointer value, so make it NULL
    delete iOperation;
    iOperation = NULL;
    
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::ClearCache()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::ClearCache()
    {
    FUNC_LOG;
    iCache.Reset();
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::GetFoldersL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::GetFoldersL( TSearchParams& aParams )
    {
    FUNC_LOG;
    iActiveEntry->SetEntryL( aParams.iMailbox.Id() );
    
    if ( iActiveEntry->Entry().iMtm == KSenduiMtmPop3Uid )
        {
        aParams.iFolderIds.AppendL( 
            TFSMailMsgId( iPlugin.PluginId(), aParams.iMailbox.Id() ) );
        return;
        }
    const TInt folders = iActiveEntry->Count();
    for ( TInt folder = 0; folder < folders; folder++ )
        {
        const TMsvEntry& entry = ( *iActiveEntry )[folder];
        if ( entry.iType == KUidMsvFolderEntry &&
             entry.iDescription.Compare( KIpsPlgDeleted ) )
            {
            GetFoldersL( entry.Id(), aParams.iFolderIds );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::GetFoldersL()
// ---------------------------------------------------------------------------
void CIpsPlgSearch::GetFoldersL( 
    const TMsvId aFolderId, 
    RArray<TFSMailMsgId>& aFolders )
    {
    FUNC_LOG;
    // Add this folder to array.
    aFolders.AppendL( TFSMailMsgId( iPlugin.PluginId(), aFolderId ) );
    
    CMsvEntry* entry = iMsvSession.GetEntryL( aFolderId );
    CleanupStack::PushL( entry );

    const TInt count = entry->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        const TMsvEntry& tEntry = (*entry)[loop];
        if ( tEntry.iType == KUidMsvFolderEntry )
            {
            GetFoldersL( tEntry.Id(), aFolders );
            }        
        }

    CleanupStack::PopAndDestroy( entry );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::SearchL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::SearchL( 
    const TFSMailMsgId& aMailBoxId,
    const RArray<TFSMailMsgId>& aFolderIds,
    const RPointerArray<TDesC>& aSearchStrings,
    const TFSMailSortCriteria& aSortCriteria,
    MFSMailBoxSearchObserver& aSearchObserver )
    {
    FUNC_LOG;
    if ( iOperation )
        {
        // if there is ongoing search, it must be canceled
        // according to UI specs
        Cancel();
        }

    // Create search parameters group for init.
    TSearchParams newSearch( 
        aSearchStrings, 
        aMailBoxId, 
        aFolderIds,  
        aSortCriteria );

    // If folder list is empty, whole mailbox needs to be searched.
    if ( !newSearch.iFolderIds.Count() )
        {
        GetFoldersL( newSearch );
        }
    
    // If search strings are same than last time, then sorting is same.
    // Clear previous cache and continue.
    if ( iPreviousSearch == newSearch )
        {
        // At this point, append new folders to list.
        iCurrentSearch += newSearch;
        iPreviousSearch += newSearch;
        }
    else
        {
        ClearCache();
        iCurrentSearch = newSearch;
        iPreviousSearch = newSearch;
        }
            
    iCurrentMessage.SetPluginId( aMailBoxId.PluginId() );
    iObserver = &aSearchObserver;
        
    iOperation = CIpsPlgSearchOp::NewL( *this );
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::Session()
// ---------------------------------------------------------------------------
//
CMsvSession& CIpsPlgSearch::Session()
    {
    FUNC_LOG;
    return iMsvSession;
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::MailboxId()
// ---------------------------------------------------------------------------
//
TMsvId CIpsPlgSearch::MailboxId()
    {
    FUNC_LOG;
    return iCurrentSearch.iMailbox.Id();
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::FolderId()
// ---------------------------------------------------------------------------
//
TMsvId CIpsPlgSearch::FolderId()
    {
    FUNC_LOG;
    return iActiveEntry->EntryId();
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::CurrentMessage()
// ---------------------------------------------------------------------------
//
const TFSMailMsgId& CIpsPlgSearch::CurrentMessage() const
    {
    FUNC_LOG;
    return iCurrentMessage;
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::SearchStringsL()
// ---------------------------------------------------------------------------
//
const CDesC16Array& CIpsPlgSearch::SearchStringsL()
    {
    FUNC_LOG;
    return *iCurrentSearch.iSearchStrings;
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::MatchFound()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::MatchFoundL()
    {
    FUNC_LOG;
    // Set current email as an entry and create messages for both cache and
    // observer, for now like this.
    iActiveEntry->SetEntryL( iCurrentMessage.Id() );                
    iCache.AppendL( SendToObserverL() );
    }
    
// ---------------------------------------------------------------------------
// CIpsPlgSearch::SendToObserverL()
// ---------------------------------------------------------------------------
//
TFSMailMsgId CIpsPlgSearch::SendToObserverL()
    {
    FUNC_LOG;
    CFSMailMessage* obsMail = iMapper->GetMailMessageL( *iActiveEntry );
    TFSMailMsgId id = obsMail->GetMessageId(); 
    // obsMail ownership is transferred
    iObserver->MatchFoundL( obsMail );    
    return id;       
    }
    
// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::SearchFinished()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::SearchFinishedL()
    {
    FUNC_LOG;
    iObserver->SearchCompletedL();
    
    delete iOperation;
    iOperation = NULL;
    }
    
// ---------------------------------------------------------------------------
// CIpsPlgSearch::NextItem()
// ---------------------------------------------------------------------------
//
CIpsPlgSearch::TMsgContainer& CIpsPlgSearch::NextItem()
    {
    FUNC_LOG;
    if ( iCurrentSearch.iSortCriteria.iOrder == EFSMailAscending )
        {
        return iEmailMessages[0];
        }
    else
        {
        return iEmailMessages[iEmailMessages.Count() - 1];
        }
    }
   
// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::NextMessageL()
// ---------------------------------------------------------------------------
//
CImEmailMessage* CIpsPlgSearch::NextMessageL()
    {
    FUNC_LOG;
    // Items, that are already in cache, are not needed to be searched.
    while ( iEmailMessages.Count() )
        {
        TFSMailMsgId id(
            iCurrentSearch.iMailbox.PluginId().iUid, NextItem().Id() );
            
        TInt index=KErrNotFound;
        TInt count = iCache.Count();
        
        for( TInt i=0; i<count; i++ )
            {
            if ( iCache[i].Id() == id.Id() )
                {
                index = i;
                }
            }
        
        if ( index > KErrNotFound )
            {
            iActiveEntry->SetEntryL( id.Id() );
            
            if( iCurrentSearch.iSortCriteria.iOrder == EFSMailAscending )
            	{
                iEmailMessages.Remove( 0 );
            	}
            else
            	{
            	iEmailMessages.Remove( iEmailMessages.Count()-1 );
            	}            
            SendToObserverL();
            }
        else
            {
            break;
            }
        }

    // Return next available message.        
    if ( iEmailMessages.Count() )
        {
        iCurrentMessage.SetId( NextItem().Id() );
        
        if( iCurrentSearch.iSortCriteria.iOrder == EFSMailAscending )
        	{
            iEmailMessages.Remove( 0 );
        	}
        else
        	{
        	iEmailMessages.Remove( iEmailMessages.Count()-1 );
        	}
        iActiveEntry->SetEntryL( iCurrentMessage.Id() );
        return CImEmailMessage::NewL( *iActiveEntry );
        }
    else
        {
        // Empty, return Null index, so the search can be finished.
        return NULL;
        }
    }    
    
// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::MessageHeaderL()
// ---------------------------------------------------------------------------
//
CImHeader* CIpsPlgSearch::MessageHeaderL()
    {
    FUNC_LOG;
    CImHeader* header = NULL;
    CMsvStore* store = iActiveEntry->EditStoreL();
    CleanupStack::PushL( store );    
    
    if ( store->IsPresentL( KUidMsgFileIMailHeader ) )
        {        
        header = CImHeader::NewLC();
        header->RestoreL( *store );
        CleanupStack::Pop( header );
        }
        
    CleanupStack::PopAndDestroy( store );        
    return header;
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::GetSubjectL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::GetSubjectL( RBuf& aSubject )
    {
    FUNC_LOG;
    TMsvEntry email = iActiveEntry->Entry();
    aSubject.CreateL( email.iDescription );
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::GetSenderL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::GetSenderL( RBuf& aSubject )
    {
    FUNC_LOG;
    TMsvEntry email = iActiveEntry->Entry();
    aSubject.CreateL( email.iDetails );
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::Sort()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::Sort()
    {
    FUNC_LOG;
    // Here we use quick sort algorithm
    QuickSort( iEmailMessages, 0, iEmailMessages.Count() - 1 );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearch::QuickSort()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::QuickSort( 
    RArray<TMsgContainer>& aMessages, 
    TInt aLeft, 
    TInt aRight )
    {
    FUNC_LOG;
    if( aMessages.Count() <= 1 )
        {
        return;
        }
    TInt leftHold = aLeft;
    TInt rightHold = aRight;
    TMsgContainer pivot = aMessages[aLeft];

    while ( aLeft < aRight )
        {
        while ( ( aMessages[aRight] >= pivot ) && ( aLeft < aRight ) )
            {            
            aRight--;
            }
            
        if (aLeft != aRight)
            {
            aMessages[aLeft] = aMessages[aRight];
            aLeft++;
            }

        while ( ( aMessages[aLeft] <= pivot ) && ( aLeft < aRight ) )
            {            
            aLeft++;
            }

        if ( aLeft != aRight )
            {
            aMessages[aRight] = aMessages[aLeft];
            aRight--;
            }
        }

    aMessages[aLeft] = pivot;
    
    TInt pivotIndex = aLeft;
    aLeft = leftHold;
    aRight = rightHold;
    if ( aLeft < pivotIndex )
        {        
        QuickSort( aMessages, aLeft, pivotIndex-1);
        }
    
    if ( aRight > pivotIndex )
        {        
        QuickSort( aMessages, pivotIndex+1, aRight);
        }
    }

// ---------------------------------------------------------------------------
// From class MIpsPlgSearch.
// CIpsPlgSearch::CollectMessagesL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearch::CollectMessagesL()
    {
    FUNC_LOG;
    TMsvId mailboxId = iCurrentSearch.iMailbox.Id();
	TMsvId serviceId;
	TMsvEntry tEntry;
	TMsvEntry mailboxServiceEntry;
	
    // Lets make a huge list of message id's. 
    iEmailMessages.Reset();    
    
    const TInt folders = iCurrentSearch.iFolderIds.Count();
    for ( TInt folder = 0; folder < folders; folder++ )
        {
        iActiveEntry->SetEntryL( iCurrentSearch.iFolderIds[folder].Id() );  
    	if( iActiveEntry->Entry().Parent() == KMsvLocalServiceIndexEntryIdValue )
    		{
    		User::LeaveIfError(
    			iMsvSession.GetEntry( mailboxId, serviceId, tEntry ) );
    		User::LeaveIfError(
    		    iMsvSession.GetEntry( tEntry.iRelatedId, serviceId, mailboxServiceEntry ) );
    		}
    	else
    		{
     		serviceId = mailboxId;
    		}

        const TInt msgs = iActiveEntry->Count();
        for ( TInt msg = 0; msg < msgs; msg++ )
            {
            const TMsvEntry& entry = ( *iActiveEntry )[msg];
            if ( entry.iType == KUidMsvMessageEntry &&
            	 entry.iServiceId == serviceId &&
                 ( entry.iMtm.iUid == KSenduiMtmSmtpUidValue ||
                   entry.iMtm.iUid == iPlugin.MtmId().iUid ) )
                {
                iEmailMessages.AppendL( TMsgContainer( entry,
                    iCurrentSearch.iSortCriteria ) );
                }
            }
        }
    }

//Ask client if it wants to change the search prority (i.e. to enable search for contact)
void CIpsPlgSearch::ClientRequiredSearchPriority( TInt *apRequiredSearchPriority )
	{
	FUNC_LOG;
	if ( iObserver )
	  iObserver->ClientRequiredSearchPriority( apRequiredSearchPriority );
	}

    
// End of File

