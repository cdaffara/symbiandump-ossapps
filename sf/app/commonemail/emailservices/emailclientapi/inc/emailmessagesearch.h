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
* Description: Definition of Email message search API.
*
*/

#ifndef EMAILMESSAGESEARCHASYNCIMPL_H_
#define EMAILMESSAGESEARCHASYNCIMPL_H_

#include <memailmessage.h>
#include <memailmessagesearch.h>
#include <emailsorting.h>

#include "CFSMailClient.h"
#include "MFSMailBoxSearchObserver.h"
#include "emailapiutils.h"

using namespace EmailInterface;


NONSHARABLE_CLASS( CEmailMessageSearchAsync) : 
    public CBase, 
    public MEmailMessageSearchAsync, 
    public MFSMailBoxSearchObserver

{
public:
    /**
    * Constructor
    * @return
    */
    static CEmailMessageSearchAsync* NewL(
        CPluginData& aPluginData,
        const TMailboxId& aMailboxId );
    
    /**
     * Destructor
     */
    ~CEmailMessageSearchAsync();
    
public: // from MEmailInterface
    TEmailTypeId InterfaceId() const;
    
    void Release();
    

public: // from MEmailMessageSearchAsync
    /**
    * Sets sort order for search results.
    * Leaves KErrNotReady if search is ongoing.
    */
    void SetSortCriteriaL( const TEmailSortCriteria& aCriteria );
    
    /**
    * Adds a search key. Leaves KErrNotReady if search is ongoing.
    */
    void AddSearchKeyL( const TDesC& aSearchKey );
                                                                  
    /**
    * Enables/disables search from remote email server.
    * Leaves KErrNotReady if search is ongoing.
    */
    void SetRemoteSearchL( TBool aRemote );                                                               
    
    /**
    * Indicates whether remote search is enabled.
    */
    TBool IsRemoteSearch() const;
    
    /**
     * Starts search, all methods affecting search attribures leave
     * KErrNotReady while search is ongoing.
     * @param aObserver called when results are available.
     */     
    void StartSearchL( MEmailSearchObserver& aObserver );

    /**
     * Cancels search.
     */
    void Cancel();
                                             
    /** returns search status 
      * @return search status:
      *     < 0 : Search has failed
      *     KRequestPending : search is ongoing. note that status may be
      *         KRequestPending after HandleResultL callback because results 
      *         may be given in chunks of results. Size of chunk depends on
      *         implementation and may vary.
      *     KErrNone : initial state, or search has finished
      */
    TInt Status() const;
    
    /**
     * Resets all search attribures. Cancels search if ongoing. 
     */
    void Reset();
    
public:  // From MFSMailBoxSearchObserver
    /** 
     * Notifies the email search API client that a match has been found
     * 
     * @param aMatchMessage contains a pointer to the matched message.
     *         Ownership is transfered to the observer.
     *
     */
     void MatchFoundL( CFSMailMessage* aMatchMessage );

    /**
     * Notifies the email search API client that the search has completed
     *
     */
     void SearchCompletedL();

//
    /**
    * Asks client if search engine should change search priority 
  	*/
    void ClientRequiredSearchPriority(TInt *apRequiredSearchPriority); 
//

    
private:
    /**
     * Constructor
     */
    CEmailMessageSearchAsync(
        CPluginData& aPluginData,
        const TMailboxId& aMailboxId );

    void ConstructL();
    
    /**
     * Function leaves if search is going on. Otherwise it doesn't do anything.
     */
    inline void IsSearchGoingOnL() const; 
    
    /**
     * Cancels search. For private use.
     */
    void CancelSearch();

private:
    
    CPluginData&    iPluginData;
    
    CFSMailPlugin*  iPlugin;

    TMailboxId      iMailboxId;
    
    TFSMailSortCriteria iCriteria;
        
    RPointerArray<TDesC> iSearchStrings;    

    MEmailSearchObserver* iObserver;
    
    mutable RSemaphore iGate;

    TBool iRemote;
};

#endif // EMAILMESSAGESEARCHASYNCIMPL_H_

// End of file

