/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Blocking attribute retriever
*
*/



#include "glxattributeretriever.h"

#include <eikenv.h>
#include <AknUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>


#include <mpxattributespecs.h>
#include <mpxcollectionpath.h>

#include <glxlog.h>
#include <glxmedialist.h>
#include <glxpanic.h>
#include <mglxfetchcontext.h>
#include <mglxmedialistobserver.h>



/**
 * Observer that should be implemented by users of the
 * CGlxAttributeRetriever class to inform them when attribute retrieval is complete
 */
class MGlxAttributeRetrieverObserver
    {
public:
    /**
     * Called when attribute retrieval is complete
     * @param aError symbian os error code.
     */
    virtual void AttributeRetrievalCompleteL(TInt aError) = 0;
    };
    
/**
 *  CGlxAttributeRetriever
 *  This class informs the MGlxAttributeRetrieverObserver when all
 *  requested metadata has been retrived.
 *	@ingroup mlm_media_list_manager_design
 *  @lib glxuiutlities.lib
 */
NONSHARABLE_CLASS( CGlxAttributeRetriever ) 
    : public CActive, public MGlxMediaListObserver
    {
public:
    /**
     * Constructor
     *  @param aObserver observer to be informed when attribute retrieval is complete
     */
    CGlxAttributeRetriever(MGlxAttributeRetrieverObserver& aObserver);

    /**
    * Destructor
    */
    ~CGlxAttributeRetriever();

    /**
    * Retrieve the attrubutes
    * @param aContext the fetch context containing the attribute to be retrieved
    * @param aList the medialist containing the item for which the attributes are retrieved
    * @return Error code
    */
    void RetrieveL(const MGlxFetchContext* aContext, MGlxMediaList* aList);

    void CancelRetrieve();
    
private: // from CActive
    void RunL();
    void DoCancel();
    
public: // From MGlxMediaListObserver
    void HandleAttributesAvailableL(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, MGlxMediaList* aList);
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    void HandlePopulatedL(MGlxMediaList* aList);
	void HandleError(TInt aError);

private:
    /**
     * Asynchronously notifies the observer is attribute retrieval is complete
     */
    void NotifyObserverIfCompleteL();

    /**
     * Completes the active object causing a call from the 
     * active scheduler to RunL()
     * @param aError error code passed to RunL()
     * (test in RunL using iStatus.Int())
     */
    void CompleteSelf(TInt aError);

private:
    /// pointer to a Fetch Context associated with the desired attribute. Not owned
    const MGlxFetchContext* iContext;

    /// pointer to the Media List. Not owned
    MGlxMediaList* iList;

    /**
     * observer
     */
    MGlxAttributeRetrieverObserver& iObserver;
    
    /**
     * Internal cache of objects still to retrieve
     */
    TInt iRequestCount;
    };

/**
 * Abstract interface for a blocking attribute retriever
 */
class MGlxBlockingAttributeRetriever
    {
public:    
    /**
     * Retrieve the attrubutes (call blocks until attribute retrieval is complete)
     * @param aContext the fetch context containing the attribute to be retrieved
     * @param aList the medialist containing the item for which the attributes are retrieved
     * @return Error code
     */
    virtual TInt RetrieveL(const MGlxFetchContext* aContext, MGlxMediaList* aList) = 0;
    virtual ~MGlxBlockingAttributeRetriever() {};
    };
    
/**
 *  This class blocks until all requested metadata has been retrieved
 */  
class CGlxSynchronousAttributeRetriever : public CBase,
                                          public MGlxBlockingAttributeRetriever,
                                          public MGlxAttributeRetrieverObserver
    {
public:
    static CGlxSynchronousAttributeRetriever* NewLC();
    
public: // from MGlxBlockingAttributeRetriever
    /**
     * See @ref MGlxBlockingAttributeRetriever::RetrieveL
     */
    TInt RetrieveL(const MGlxFetchContext* aContext, MGlxMediaList* aList);

private: // from MGlxAttributeRetrieverObserver
    /**
     * See @ref MGlxAttributeRetrieverObserver::AttributeRetrievalCompleteL
     */
    void AttributeRetrievalCompleteL(TInt aError);
    
private:
    /**
     * StopScheduler
     */
     void StopScheduler();

private:
    /**
     * Second stage constructor
     */
    void ConstructL();
    
    /**
     * Destructor
     */
    ~CGlxSynchronousAttributeRetriever();
    
private:
    /// Active scheduler wait object. Owned
    CActiveSchedulerWait* iSchedulerWait;
    
    /**
     * Attribute retriever (owned)
     */
    CGlxAttributeRetriever* iAttributeRetriever;
    
    /**
     * Attribute retrieval error
     */
    TInt iError;
    };
    
// -----------------------------------------------------------------------------
// RetrieveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt GlxAttributeRetriever::RetrieveL(const MGlxFetchContext& aContext, 
        MGlxMediaList& aList, TBool /*aShowDialog*/)
    {
    CGlxSynchronousAttributeRetriever* retriever = NULL;
    retriever = CGlxSynchronousAttributeRetriever::NewLC();
    TInt err = retriever->RetrieveL(&aContext, &aList);
    CleanupStack::PopAndDestroy(retriever); 
    return err;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//  
CGlxAttributeRetriever::CGlxAttributeRetriever(MGlxAttributeRetrieverObserver& aObserver)
    : CActive(EPriorityHigh), iObserver(aObserver)  // We want out RunL to be serviced ASAP
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
CGlxAttributeRetriever::~CGlxAttributeRetriever()
    {
    Cancel();

    if ( iList )
        {
        iList->RemoveMediaListObserver( this );
        }
    }


// -----------------------------------------------------------------------------
// CGlxAttributeRetriever::RetrieveL
// -----------------------------------------------------------------------------
//	
void CGlxAttributeRetriever::RetrieveL(const MGlxFetchContext* aContext, MGlxMediaList* aList)
    {
    iContext = aContext;
    iList = aList;
    iList->AddMediaListObserverL(this);
    
    if(iList->IsPopulated())
        {
        NotifyObserverIfCompleteL();
        }
    }

void CGlxAttributeRetriever::CancelRetrieve()
    {
    // Simply remove media list observer to prevent call backs from the media list
    iList->RemoveMediaListObserver(this);
    }

// -----------------------------------------------------------------------------
// CGlxAttributeRetriever::RunL
// -----------------------------------------------------------------------------
//  
void CGlxAttributeRetriever::RunL()
    {
    iObserver.AttributeRetrievalCompleteL(iStatus.Int());
    }

// -----------------------------------------------------------------------------
// CGlxAttributeRetriever::DoCancel
// -----------------------------------------------------------------------------
// 
void CGlxAttributeRetriever::DoCancel()
    {
    // No need to do anything
    }

// -----------------------------------------------------------------------------
// CGlxAttributeRetriever::NotifyObserverIfCompleteL()
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::NotifyObserverIfCompleteL()
    {
    iRequestCount--;
    
    if( iRequestCount <= 0 )
        {
        iRequestCount = iContext->RequestCountL(iList);
        if(iRequestCount <= 0)
            {
            iList->RemoveMediaListObserver(this); // prevent any more callbacks from the media list
            CompleteSelf(iRequestCount); // request count is an error
            }
         }
    }

// ---------------------------------------------------------------------------
// CGlxDataSource::CompleteSelf
// ---------------------------------------------------------------------------
//
void CGlxAttributeRetriever::CompleteSelf(TInt aError)
    {
    TRequestStatus* status=&iStatus;
    User::RequestComplete(status, aError);
    SetActive();    
    }

// -----------------------------------------------------------------------------
// HandleAttributesAvailableL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleAttributesAvailableL(TInt /* aItemIndex */, 
    const RArray<TMPXAttribute>& /* aAttributes */, MGlxMediaList* /*aList*/)
    {
    NotifyObserverIfCompleteL();
    }

// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleItemAddedL(TInt /* aStartIndex */, 
        TInt /* aEndIndex */, MGlxMediaList* /* aList */)
    {
    // No implementaion
    }

// -----------------------------------------------------------------------------
// HandleItemRemovedL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleItemRemovedL(TInt /* aStartIndex */, 
        TInt /* aEndIndex */, MGlxMediaList* /* aList */)
    {
    // No implementaion
    }

// -----------------------------------------------------------------------------
// HandleItemModifiedL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleItemModifiedL(const RArray<TInt>& /* aItemIndexes */, 
        MGlxMediaList* /* aList */)
    {
    // No implementaion
    }

// -----------------------------------------------------------------------------
// HandleFocusChangedL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /* aType */, 
        TInt /* aNewIndex */, TInt /* aOldIndex */, MGlxMediaList* /* aList */)
    {
    // No implementaion
    }

// -----------------------------------------------------------------------------
// HandleMediaL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleMediaL(TInt /* aListIndex */, 
        MGlxMediaList* /* aList */)
    {
    // No implementaion
    }

// -----------------------------------------------------------------------------
// HandleItemSelectedL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleItemSelectedL(TInt /* aIndex */, 
        TBool /* aSelected */, MGlxMediaList* /* aList */)
    {
    // No implementaion
    }

// -----------------------------------------------------------------------------
// HandleMessageL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleMessageL(const CMPXMessage& /* aMessage */, 
    MGlxMediaList* /* aList */)
    {
    // No implementaion
    }

// -----------------------------------------------------------------------------
// HandleError
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandleError(TInt /* aError */)
    {
    // An error has been reported. But is it ours?
    // If the request is not complete then the error is not ours
    iRequestCount = 0;
    TRAP_IGNORE(NotifyObserverIfCompleteL());
    
    }

// -----------------------------------------------------------------------------
// HandlePopulatedL
// -----------------------------------------------------------------------------
//
void CGlxAttributeRetriever::HandlePopulatedL(MGlxMediaList* /*aList*/)
    {
    // If the media list is empty we will never get the HandleAttributesAvailableL callback,
    // therefore the call to NotifyObserversIfCompleteL below is necessary.
    NotifyObserverIfCompleteL();
    }

// -----------------------------------------------------------------------------
// CGlxSynchronousAttributeRetriever
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// CGlxSynchronousAttributeRetriever::NewLC
// -----------------------------------------------------------------------------
//  
CGlxSynchronousAttributeRetriever* CGlxSynchronousAttributeRetriever::NewLC()
    {
    CGlxSynchronousAttributeRetriever* self = new (ELeave) CGlxSynchronousAttributeRetriever();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CGlxSynchronousAttributeRetriever::RetrieveL
// -----------------------------------------------------------------------------
//  
TInt CGlxSynchronousAttributeRetriever::RetrieveL(const MGlxFetchContext* aContext, MGlxMediaList* aList)
    {
    iAttributeRetriever->RetrieveL(aContext,aList);
    iSchedulerWait->Start();
    return iError;
    }

// -----------------------------------------------------------------------------
// CGlxSynchronousAttributeRetriever::AttributeRetrievalCompleteL
// -----------------------------------------------------------------------------
//
void CGlxSynchronousAttributeRetriever::AttributeRetrievalCompleteL(TInt aError)
    {
    iError = aError;
    StopScheduler();
    }
                                              
// -----------------------------------------------------------------------------
// StopScheduler
// -----------------------------------------------------------------------------
//
void CGlxSynchronousAttributeRetriever::StopScheduler()
    {
    if (iSchedulerWait)
        {
        if (iSchedulerWait->IsStarted())
            {
            iSchedulerWait->AsyncStop();
            }
        }
    }

// -----------------------------------------------------------------------------
// CGlxSynchronousAttributeRetriever::ConstructL
// -----------------------------------------------------------------------------
//  
void CGlxSynchronousAttributeRetriever::ConstructL()
    {
    iSchedulerWait = new (ELeave) CActiveSchedulerWait();
    iAttributeRetriever = new (ELeave) CGlxAttributeRetriever(*this);
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//  
CGlxSynchronousAttributeRetriever::~CGlxSynchronousAttributeRetriever()
    {
    delete iSchedulerWait;
    delete iAttributeRetriever;
    }
