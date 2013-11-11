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
* Description: This file implements class CEmailMessageSearchAsync
*
*/

#include <emailclientapi.hrh>
#include "emailmessagesearch.h"
#include "emailmessage.h"

/**
 * Global semaphore name. Semaphore blocks parallel service access.
 */
_LIT( KGlobalSemaphoreToPreventParallelCall, "12mymessaging.nokia.com34" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEmailMessageSearchAsync::NewL
// ---------------------------------------------------------------------------
CEmailMessageSearchAsync* CEmailMessageSearchAsync::NewL(
    CPluginData& aPluginData,
    const TMailboxId& aMailboxId )
    {
    CEmailMessageSearchAsync* self = 
        new ( ELeave ) CEmailMessageSearchAsync( aPluginData, aMailboxId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CEmailMessageSearchAsync::ConstructL()
    {
    iPlugin = iPluginData.ClaimInstanceL();
    // Open existing semaphore, or create a new one
    if ( KErrNone != iGate.OpenGlobal( KGlobalSemaphoreToPreventParallelCall, EOwnerProcess ) )
        {
        User::LeaveIfError( 
            iGate.CreateGlobal( KGlobalSemaphoreToPreventParallelCall, 1, EOwnerProcess ) );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMessageSearchAsync::CEmailMessageSearchAsync(    
    CPluginData& aPluginData,
    const TMailboxId& aMailboxId )
    : iPluginData( aPluginData ), iMailboxId( aMailboxId.iId ),
    iCriteria(), iObserver( NULL ), iRemote( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMessageSearchAsync::~CEmailMessageSearchAsync()
    {
    iSearchStrings.Close();
    iPluginData.ReleaseInstance();
    iGate.Close();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CEmailMessageSearchAsync::InterfaceId() const
    {
    return KEmailIFUidSearch;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::Release()
    {
    if (KErrNone != iGate.Wait(1))
        {
        this->CancelSearch();
        }
    delete this;
    }

// -----------------------------------------------------------------------------
// Sets sort order for search results.
// Leaves KErrNotReady if search is ongoing.
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::SetSortCriteriaL( const TEmailSortCriteria& aCriteria )
    {
    IsSearchGoingOnL();

    switch (aCriteria.iField)
        {
        case TEmailSortCriteria::EDontCare:
            iCriteria.iField = EFSMailDontCare;
            break;

        case TEmailSortCriteria::EByDate:
            iCriteria.iField = EFSMailSortByDate;
            break;

        case TEmailSortCriteria::EBySender:
            iCriteria.iField = EFSMailSortBySender;
            break;

        case TEmailSortCriteria::EByRecipient:
            iCriteria.iField = EFSMailSortByRecipient;
            break;

        case TEmailSortCriteria::EBySubject:
            iCriteria.iField = EFSMailSortBySubject;
            break;

        case TEmailSortCriteria::EByPriority:
            iCriteria.iField = EFSMailSortByPriority;
            break;

        case TEmailSortCriteria::EByFlagStatus:
            iCriteria.iField = EFSMailSortByFlagStatus;
            break;

        case TEmailSortCriteria::EByUnread:
            iCriteria.iField = EFSMailSortByUnread;
            break;

        case TEmailSortCriteria::EBySize:
            iCriteria.iField = EFSMailSortBySize;
            break;

        case TEmailSortCriteria::EByAttachment:
            iCriteria.iField = EFSMailSortByAttachment;
            break;

        default:
            User::Leave( KErrNotSupported );
            break;

        }
    if (aCriteria.iAscending)
        {
        iCriteria.iOrder = EFSMailAscending;
        }
    else
        {
        iCriteria.iOrder = EFSMailDescending;
        }
    // Release gate
    iGate.Signal();
    }

// -----------------------------------------------------------------------------
// Adds a search key. Leaves KErrNotReady if search is ongoing.
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::AddSearchKeyL( const TDesC& aSearchKey ) 
    {
    IsSearchGoingOnL();

    iSearchStrings.AppendL(&aSearchKey);
    // Release gate
    iGate.Signal();
    }

/// -----------------------------------------------------------------------------
// Enables/disables search from remote email server.
// Leaves KErrNotReady if search is ongoing.
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::SetRemoteSearchL( TBool aRemote )  
    {    
    IsSearchGoingOnL();

    iRemote = aRemote;
    // Release gate
    iGate.Signal();
    // Currently plugins do not support this function
    User::Leave( KErrNotSupported );    
    }

// -----------------------------------------------------------------------------
// Indicates whether remote search is enabled.
// -----------------------------------------------------------------------------
TBool CEmailMessageSearchAsync::IsRemoteSearch() const
    {
    // Currently plugins do not support this function
    return EFalse; 
    }

// -----------------------------------------------------------------------------
// Starts search, all methods affecting search attribures leave
// KErrNotReady while search is ongoing.
// @param aObserver called when results are available.
//     
void CEmailMessageSearchAsync::StartSearchL( MEmailSearchObserver& aObserver )
    {
    IsSearchGoingOnL();

    iObserver = &aObserver;
    const TFSMailMsgId fsMailboxId( iPluginData.Uid(), iMailboxId.iId );
    RArray <TFSMailMsgId> folderIds;

    /** Search API */

    /**
     * Asyncronous call for starting search for given string. Only one search can be
     * performed at a time.
     *
     *
     * This function will search for message's containing the given search string.
     * The search will be performed on the all message fields: To, Cc, Bcc, subject, body.
     * The search client will be notified of each found message,
     * and upon completion of the search.  Only one search can be performed at a time.  
     *  
     * To change the sort order in the search result, use the same search string in the
     * but change the aSortCriteria parameter.  The store "caches" the search
     * results generated by the same search string.
     *    
     * The function will leave with KErrInUse if a search is already in progress.
     *
     * /note Only works if the store is in an authenticated state,
     *  otherwise this function leaves with KErrNotReady
     * 
     * @paran aMailBoxId id of the mailbox where messages are to be searched
     * @param aFolderIds list of folders where messages are to be searched
     *        global or folder specific search depends on the size of array is 0 or not.
     * @param aSearchStrings text strings that will be searched from different message fields.
     * @param aSortCriteria sort criteria for the results
     * @param aSearchObserver client observer that will be notified about search status.
     *
     */
    iPlugin->SearchL( fsMailboxId,
        folderIds,
        iSearchStrings,
        iCriteria,
        *this );
    // Gate is kept closed as search is asynchronous. Gate will be reopen after search is completed, i.e.
    // CEmailMessageSearchAsync::SearchCompleted.
    }

// -----------------------------------------------------------------------------
// Cancels search.
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::Cancel()
    {
    if (KErrNone != iGate.Wait(1))
        {        
        CancelSearch();
        }
    
    // Release gate
    iGate.Signal();    
    }

// -----------------------------------------------------------------------------
//  * @return search status:
//  *     < 0 : Search has failed
//  *     KRequestPending : search is ongoing. note that status may be
//  *         KRequestPending after HandleResultL callback because results 
//  *         may be given in chunks of results. Size of chunk depends on
//  *         implementation and may vary.
//  *     KErrNone : initial state, or search has finished
// -----------------------------------------------------------------------------
TInt CEmailMessageSearchAsync::Status() const
    {
    TInt ret(KErrNone);
    if (KErrNone != iGate.Wait(1))
        {
        // Search is going on
        ret = KRequestPending;
        }
    else
        {
        // Release gate
        iGate.Signal();       
        }
    //if we really locked iGate here, we release it in else 
    //brach. If it was already locked, we must not release it here!
    // coverity[missing_unlock]
    return ret; 
    }

// -----------------------------------------------------------------------------
// Resets all search attribures. Cancels search if ongoing. 
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::Reset()
    {
    if ( KErrNone != iGate.Wait( 1 ) )
        {
        this->CancelSearch();
        }
    
    iCriteria = TFSMailSortCriteria();
    iSearchStrings.Reset();
    
    // Release gate
    iGate.Signal();
    
    };

// -----------------------------------------------------------------------------
// Notifies the email search API client that a match has been found
// 
// @param aMatchMessage contains a pointer to the matched message.
//         Ownership is transfered to the observer.
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::MatchFoundL( CFSMailMessage* aMatchMessage )
{
    User::LeaveIfNull( iObserver );
    CEmailMessage *result = CEmailMessage::NewL(iPluginData, aMatchMessage, EClientOwns );
    iObserver->HandleResultL( result );
}

// -----------------------------------------------------------------------------
// Notifies the email search API client that the search has completed
// -----------------------------------------------------------------------------
 void CEmailMessageSearchAsync::SearchCompletedL()
{
    User::LeaveIfNull( iObserver );
    iObserver->SearchCompletedL();
    // Search is now complete, release gate.
    iGate.Signal();
}

// -----------------------------------------------------------------------------
// Asks client if search engine should change search priority 
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::ClientRequiredSearchPriority(TInt* /*apRequiredSearchPriority*/)
    {
    }

// -----------------------------------------------------------------------------
// Function leaves if search is going on. Otherwise it doesn't do anything.
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::IsSearchGoingOnL() const
    {
    if ( KErrNone != iGate.Wait( 1 ) )
        {
        // Leave now, search is going on
        User::Leave( KErrNotReady );
        }
    }

// -----------------------------------------------------------------------------
// Cancels search
// -----------------------------------------------------------------------------
void CEmailMessageSearchAsync::CancelSearch()
    {
    /**
     * Cancels current search. Does nothing if there is not any search.
     * The search client will not be called back after this function is called.
     *
     */
    const TFSMailMsgId fsMailboxId( iPluginData.Uid(), iMailboxId.iId );
    iPlugin->CancelSearch( fsMailboxId );
    }
// End of file
