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
* Description: ECom interface for Email Client API
*
*/

#ifndef __MEMAILACTIONS
#define __MEMAILACTIONS

#include <e32cmn.h>
#include <memailmessage.h>

namespace EmailInterface {

class MMessageIterator;
class TEmailSortCriteria;

/**
* Observer interface for handling email search results.
*/
class MEmailSearchObserver
{
public:
    /**
     * Called when search has next message available.
     * This callback is called several times until all results
     * have been delivered.
     *
     * @param message found in search, ownership is transferred.
     *          
     */ 
    virtual void HandleResultL( 
        MEmailMessage* aMessage ) = 0;

    /**
     * Notifies that the search has completed and no more results are expected.
     */
    virtual void SearchCompletedL() = 0;
};

/**
 * Search interface for messages. Results are provided asynchronoysly.
 * By default all found mailboxes are included in search.
 * @code
   
   MEmailMailbox* mailbox = NULL;
   // obtain mailbox here... 
   // ...and now get search interface
   MEmailMessageSearchAsync* search = mailbox->MessageSearchL();
   CleanupReleasePushL( *search );
   TEmailSortCriteria criteria;
   criteria.iAscending = ETrue;
   criteria.iField = TEmailSortCriteria::EByDate;                 
   search->SetSortCriteriaL( criteria );
   search->StartSearchL( *this ); // this implements MEmailSearchObserver
   search->Cancel(); // cancel search
   CleanupStack::PopAndDestroy(); // search
   
   @endcode
   
 * @since S60 v5.0
 */
class MEmailMessageSearchAsync : public MEmailInterface
{
public:
        
    /**
    * Sets sort order for search results.
    * Leaves KErrNotReady if search is ongoing.
    */
    virtual void SetSortCriteriaL( const TEmailSortCriteria& aCriteria ) = 0;
    
    /**
    * Adds a search key. Leaves KErrNotReady if search is ongoing.
    */
    virtual void AddSearchKeyL( const TDesC& aSearchKey ) = 0;
                                                                  
    /**
    * Enables/disables search from remote email server.
    * Leaves KErrNotReady if search is ongoing.
    */
    virtual void SetRemoteSearchL( TBool aRemote ) = 0;                                                                  
    
    /**
    * Indicates whether remote search is enabled.
    */
    virtual TBool IsRemoteSearch() const = 0;
    
    /**
     * Starts search, all methods affecting search attribures leave
     * KErrNotReady while search is ongoing.
     * @param aObserver called when results are available.
     */     
    virtual void StartSearchL( MEmailSearchObserver& aObserver ) = 0;

    /**
     * Cancels search.
     */
    virtual void Cancel() = 0;
                                             
    /** returns search status 
      * @return search status:
      *     < 0 : Search has failed
      *     KRequestPending : search is ongoing. note that status may be
      *         KRequestPending after HandleResultL callback because results 
      *         may be given in chunks of results. Size of chunk depends on
      *         implementation and may vary.
      *     KErrNone : initial state, or search has finished
      */
    virtual TInt Status() const = 0;
    
    /**
     * Resets all search attribures. Cancels search if ongoing. 
     */
    virtual void Reset() = 0;
};

} //EmailInterface

#endif // __MEMAILACTIONS

// End of file.
