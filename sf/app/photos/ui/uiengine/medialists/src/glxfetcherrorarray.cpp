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
* Description:    Array of attribute retrieval errors
*
*/




#include "glxfetcherrorarray.h"
#include <glxtracer.h>                         // For Logs

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxFetchErrorArray::CGlxFetchErrorArray()
    {
    TRACER("CGlxFetchErrorArray::Default Constructor");
            
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxFetchErrorArray* CGlxFetchErrorArray::NewL(const CGlxFetchErrorArray* aErrorArray)
    {
    TRACER("CGlxFetchErrorArray::NewL");
    
    CGlxFetchErrorArray* self = CGlxFetchErrorArray::NewLC(aErrorArray);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CGlxFetchErrorArray* CGlxFetchErrorArray::NewLC(const CGlxFetchErrorArray* aErrorArray)
    {
    TRACER("CGlxFetchErrorArray::NewLC");
    
    CGlxFetchErrorArray* self = new( ELeave ) CGlxFetchErrorArray();
    CleanupStack::PushL( self );
    self->CopyConstructL(aErrorArray);
    return self;
    }
// -----------------------------------------------------------------------------
// CopyConstructL
// -----------------------------------------------------------------------------
//
void CGlxFetchErrorArray::CopyConstructL(const CGlxFetchErrorArray* aErrorArray)
    {
    TRACER("CGlxFetchErrorArray::CopyConstructL");
    
    if ( aErrorArray )
        {
        TInt count = aErrorArray->iFetchErrors.Count();
        for ( TInt index = 0; index < count; index++ )
            {
            iFetchErrors.AppendL(aErrorArray->iFetchErrors[index]);
            }
        }
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxFetchErrorArray::~CGlxFetchErrorArray()
    {
    TRACER("CGlxFetchErrorArray::Destructor");
    
    iFetchErrors.Reset();
    }
    
// -----------------------------------------------------------------------------
// ErrorCount
// -----------------------------------------------------------------------------
//
TInt CGlxFetchErrorArray::ErrorCount() const
    {
    TRACER("CGlxFetchErrorArray::ErrorCount");
    
    return iFetchErrors.Count();
    }
    
// -----------------------------------------------------------------------------
// Error
// -----------------------------------------------------------------------------
//
TGlxFetchError CGlxFetchErrorArray::Error(TInt aIndex) const
    {
    TRACER("CGlxFetchErrorArray::Error");
    
    return iFetchErrors[aIndex];
    }

// -----------------------------------------------------------------------------
// AddErrorL
// -----------------------------------------------------------------------------
//
void CGlxFetchErrorArray::AddErrorL(TGlxFetchError aError)
    {
    TRACER("CGlxFetchErrorArray::AddErrorL");
    
    TInt index = FindError(aError.iAttr);
    
    if ( KErrNotFound == index )
        {
    	TLinearOrder<TGlxFetchError> orderer (&ErrorOrderById);
        iFetchErrors.InsertInOrderL(aError, orderer);
        }
    else
        {
        iFetchErrors[index] = aError;
        }
    }
    
// -----------------------------------------------------------------------------
// Remove an error
// -----------------------------------------------------------------------------
//
void CGlxFetchErrorArray::Remove( TInt aIndex )
    {
    TRACER("CGlxFetchErrorArray::Remove");
    
    iFetchErrors.Remove( aIndex );
    }

// -----------------------------------------------------------------------------
// FindError
// -----------------------------------------------------------------------------
//
TInt CGlxFetchErrorArray::FindError(TMPXAttribute aAttr) const
    {
    TRACER("CGlxFetchErrorArray::FindError");
    
    return iFetchErrors.FindInOrder(aAttr, (&ErrorOrderByKey));
    }
    
// -----------------------------------------------------------------------------
// FindError
// -----------------------------------------------------------------------------
//    
TInt CGlxFetchErrorArray::FindError(TInt aContentId) const
    {
    TRACER("CGlxFetchErrorArray::FindError");
    
    TInt count = iFetchErrors.Count();
    TInt pos = KErrNotFound;
    for(TInt i=0; i<count; i++)
        {
        if(iFetchErrors[i].iAttr.ContentId()==aContentId)
            {
            pos = i;
            break;
            }      
        }
        
    return pos;
    }

// -----------------------------------------------------------------------------
// ErrorOrderByKey
// -----------------------------------------------------------------------------
//
TInt CGlxFetchErrorArray::ErrorOrderByKey(const TMPXAttribute* aAttr, const TGlxFetchError& aItem2) 
	{
	TRACER("CGlxFetchErrorArray::ErrorOrderByKey");
	
	TInt contentId1 = aAttr->ContentId();
	TInt contentId2 = aItem2.iAttr.ContentId();
	
	if (contentId1 < contentId2) 
		{
		return -1;
		}
		
	if (contentId1 > contentId2) 
		{
		return 1;
		}

    TUint attributeId1 = aAttr->AttributeId();
    TUint attributeId2 = aItem2.iAttr.AttributeId();

	if (attributeId1 < attributeId2) 
		{
		return -1;
		}
		
	if (attributeId1 > attributeId2) 
		{
		return 1;
		}

	return 0;
	}

// -----------------------------------------------------------------------------
// ErrorOrderById
// -----------------------------------------------------------------------------
//
TInt CGlxFetchErrorArray::ErrorOrderById(const TGlxFetchError& aItem1, const TGlxFetchError& aItem2) 
	{
	TRACER("CGlxFetchErrorArray::ErrorOrderById");
	
	return ErrorOrderByKey(&(aItem1.iAttr), aItem2);
	}

