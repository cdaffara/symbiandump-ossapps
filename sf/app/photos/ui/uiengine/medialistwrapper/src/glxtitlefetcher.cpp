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
* Description:    Title fetcher
*
*/





// INCLUDE FILES
#include "glxtitlefetcher.h"

#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <mglxmedialist.h>
#include <glxattributecontext.h>
#include <glxuistd.h>
#include "glxlog.h"
#include "glxtracer.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxTitleFetcher::CGlxTitleFetcher(MGlxTitleFetcherObserver& aObserver,
                        CMPXCollectionPath* aPath) :
   iObserver(aObserver), iPath(aPath)
    {
    TRACER("CGlxTitleFetcher::CGlxTitleFetcher");
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
    
void CGlxTitleFetcher::ConstructL()
    {
    TRACER("CGlxTitleFetcher::ConstructL");
    // Go back one level to obtain the ID
    iPathId = iPath->Id();
    // Go back another level from which to create a media list containing 
    // this path
    iPath->Back();
    
    if( iPath->Levels() <= 0 )
        {
        //For This Level, Opening a collection is nothing but open EGlxCollectionPluginShowInMainListView
        //We dont need this in 10.1
        return;
        }
    
    iBackMediaList = MGlxMediaList::InstanceL(*iPath);
    
    /// @todo: This idSpaceId must be calculated properly
    TGlxIdSpaceId idSpaceId = iBackMediaList->IdSpaceId(0);
    
    TGlxMediaId id(iPathId);
    TBool requestAttributes = ETrue;
    TInt index = iBackMediaList->Index(idSpaceId, id);
    
    if ( index != KErrNotFound )
        {
        const TGlxMedia& item = iBackMediaList->Item(index);
    	const CGlxMedia* media = item.Properties();

    	// get the Title from the attribute if it is already present
    	if (media && media->IsSupported(KMPXMediaGeneralTitle)
    	    && index != KErrNotFound)
    		{
            const TDesC&  title = media->ValueText(KMPXMediaGeneralTitle);
            // notify the observer		
            iObserver.HandleTitleAvailableL(title); 
            requestAttributes = EFalse;
    		}
    	}
    
    if ( requestAttributes )
	    {
        // Otherwise add a context setting range to cover all items
        iContext = CGlxDefaultAttributeContext::NewL();
        iContext->SetRangeOffsets(10000,10000);
        iContext->AddAttributeL(KMPXMediaGeneralTitle);
        // Add the context at a high priority
        iBackMediaList->AddContextL( iContext,
                                     KGlxFetchContextPriorityTitleFetcher );
        // Set self as observer
        iBackMediaList->AddMediaListObserverL(this);
	    }
	}


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxTitleFetcher* CGlxTitleFetcher::NewL(MGlxTitleFetcherObserver& aObserver,
                        CMPXCollectionPath* aPath)
    {
    CGlxTitleFetcher* self = new (ELeave) CGlxTitleFetcher(aObserver, aPath);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxTitleFetcher::~CGlxTitleFetcher()
    {
    TRACER("CGlxTitleFetcher::~CGlxTitleFetcher ()");

    if(iBackMediaList)
        {
        if(iContext)
            {
            iBackMediaList->RemoveContext(iContext);
            delete iContext;
            }
        iBackMediaList->RemoveMediaListObserver(this);           
		iBackMediaList->Close();
        }
    
    }
// ---------------------------------------------------------------------------
// CGlxTitleFetcher::HandleItemAddedL
// ---------------------------------------------------------------------------
//
void CGlxTitleFetcher::HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
        MGlxMediaList* aList)  
    {    
    TRACER("CGlxTitleFetcher::HandleItemAddedL ()");
    if(aList == iBackMediaList)
           {
           for(TInt index = aStartIndex;index <= aEndIndex;index++)
           // Is it the item (path) that we want
               {
               if(aList->Item(index).Id().Value() == (TUint32)iPathId)
                       {
                       const TGlxMedia& item = aList->Item(index);
                       const CGlxMedia* media = item.Properties();
                       // get the Title from the attribute
                       if (media && media->IsSupported(KMPXMediaGeneralTitle))
                           {
                           const TDesC&  title = media->ValueText(KMPXMediaGeneralTitle);
                           // notify the observer      
                           iObserver.HandleTitleAvailableL(title); 
                           iBackMediaList->RemoveContext(iContext);
                           delete iContext;
                           iContext = NULL;                
                           }
                       }
                }
           }
    }
// ---------------------------------------------------------------------------
// CGlxTitleFetcher::HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxTitleFetcher::HandleAttributesAvailableL(TInt aItemIndex, 	
		const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* aList)
    {
    TRACER("CGlxTitleFetcher::HandleAttributesAvailableL ()");
    // Is it the list that we want    
    if(aList == iBackMediaList)
        {
        // Is it the item (path) that we want
        if(aList->Item(aItemIndex).Id().Value() == (TUint32)iPathId)
            {
            const TGlxMedia& item = aList->Item(aItemIndex);
			const CGlxMedia* media = item.Properties();
			// get the Title from the attribute
			if (media && media->IsSupported(KMPXMediaGeneralTitle))
				{
                const TDesC&  title = media->ValueText(KMPXMediaGeneralTitle);
	            // notify the observer		
                iObserver.HandleTitleAvailableL(title); 
                iBackMediaList->RemoveContext(iContext);
                delete iContext;
                iContext = NULL;
		        
				}
            }
        }
    }



//  End of File
