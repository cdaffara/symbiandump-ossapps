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
* Description:    Utility class to manage attribute retrieval errors
*
*/




#include "glxerrormanager.h"

//#include <glxerrors.h>

#include "glxfetcherrorarray.h"
#include "glxcachemanager.h"
#include "glxmedialist.h"
#include <glxtracer.h>                         // For Logs
#include <glxlog.h>                         // For Logs

/** Error attribute content ID */
const TInt KGlxErrorContentId = 0x200071AC;
const TMPXAttributeData KGlxMediaErrorArray = { KGlxErrorContentId, 0x01 }; // CGlxFetchErrorArray

const TInt KGlxErrorTimeOut = -10033;

// -----------------------------------------------------------------------------
// Return errors
// -----------------------------------------------------------------------------
//
inline CGlxFetchErrorArray* Errors( const CGlxMedia& aMedia ) 
    {
    TRACER("GlxErrorManager::Errors");
    
    return const_cast<CGlxFetchErrorArray*>(
        static_cast<const CGlxFetchErrorArray*>( aMedia.ValueCObject( 
            KGlxMediaErrorArray ) ) );
    }

// -----------------------------------------------------------------------------
// SetAttributeErrorL
// -----------------------------------------------------------------------------
//
void GlxErrorManager::SetAttributeErrorL(CGlxMedia* aItem, const RArray<TMPXAttribute>& aAttributes, TInt aError)
    {
    TRACER("GlxErrorManager::SetAttributeErrorL");
    
    if ( aItem )
        {
        CGlxFetchErrorArray* errorArray = NULL;
            
        if ( aItem->IsSupported(KGlxMediaErrorArray) )
            {
    		const CGlxFetchErrorArray* existingArray = static_cast<const CGlxFetchErrorArray*>(aItem->ValueCObject(KGlxMediaErrorArray));
    		
    		if ( existingArray )
    		    {
                errorArray = CGlxFetchErrorArray::NewLC(existingArray);
                }
            }
            
        if ( !errorArray )            
            {
            errorArray = new(ELeave) CGlxFetchErrorArray();
            CleanupStack::PushL(errorArray);
            }

        TInt attrCount = aAttributes.Count();
        
        for ( TInt attrIndex = 0; attrIndex < attrCount; attrIndex++ )
            {
            errorArray->AddErrorL(TGlxFetchError(aAttributes[attrIndex], aError));
            }
        
        aItem->SetCObjectValueL(KGlxMediaErrorArray, errorArray);

        CleanupStack::Pop(errorArray);
        }            
    }
    
// -----------------------------------------------------------------------------
// HasAttributeErrorL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt GlxErrorManager::HasAttributeErrorL(const CGlxMedia* aMedia, 
        const TMPXAttribute& aAttribute)
    {
    TRACER("GlxErrorManager::HasAttributeErrorL");
    
    TInt retVal = KErrNone;
    
    if ( aMedia )
        {
        const CGlxFetchErrorArray* errors = Errors( *aMedia );
		
		if ( errors )
		    {
		    TInt index = errors->FindError(aAttribute);
		    if ( KErrNotFound != index )
		        {
    		    TGlxFetchError error = errors->Error(index);
		        retVal = error.iError;
		        
	            if ( IsExpired( error ) )
	                {
	                retVal = KErrNone;
	                }
	                
	            if ( KErrNone != retVal )
	                {
	                if ( IsTemporaryError(retVal) )
	                    {
    		            // Inform cache manager that at least one temporary error still exists
    		            CGlxCacheManager* cacheManager = CGlxCacheManager::InstanceL();
    		            cacheManager->SetTemporaryErrorFlag();
    		            cacheManager->Close();
	                    }
	                }
		        }
		    }
        }
        
    return retVal;
    }
    
// -----------------------------------------------------------------------------
// HasAttributeErrorL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt GlxErrorManager::HasAttributeErrorL(const CGlxMedia* aMedia, 
        TInt aContentId)
    {
    TRACER("GlxErrorManager::HasAttributeErrorL");
    
    TInt retVal = KErrNone;
    if ( aMedia )
        {
        const CGlxFetchErrorArray* errors = Errors( *aMedia );
		
		if ( errors )
		    {
		    TInt index = errors->FindError(aContentId);
		    if ( KErrNotFound != index )
		        {
    		    TGlxFetchError error = errors->Error(index);
		        retVal = error.iError;
		        
	            if ( IsExpired( error ) )
	                {
	                retVal = KGlxErrorTimeOut;
	                }
	                
	            if ( KErrNone != retVal && KGlxErrorTimeOut != retVal)
	                {
	                if ( IsTemporaryError(retVal) )
	                    {
    		            // Inform cache manager that at least one temporary error still exists
    		            CGlxCacheManager* cacheManager = CGlxCacheManager::InstanceL();
    		            cacheManager->SetTemporaryErrorFlag();
    		            cacheManager->Close();
	                    }
	                }
		        }
		        
		    }
        }
        
    return retVal;
    }

// -----------------------------------------------------------------------------
// IsTemporaryError
// -----------------------------------------------------------------------------
//
TBool GlxErrorManager::IsTemporaryError(TInt aErrorCode)
    {
    TRACER("GlxErrorManager::IsTemporaryError");
 	GLX_DEBUG2("GlxErrorManager::IsTemporaryError() aErrorCode=%d", aErrorCode);
	               	 	    
    /// @todo Implement this when list of temporary and permanent errors are known. Currently assume
    ///       all errors are temporary
    switch ( aErrorCode )
        {
        case KErrArgument: // fallthrough
        case KErrNotSupported: // fallthrough
        case KErrNoMemory:      // fallthrough
        case KErrCorrupt:       // fallthrough
        case KErrDiskFull:      
        // add other permanent error codes here
            return EFalse;
            
        default:
            return ETrue;
        };
    }
    
// -----------------------------------------------------------------------------
// IsErrorStillVal
// -----------------------------------------------------------------------------
//
TBool GlxErrorManager::IsExpired( const TGlxFetchError& aError )
    {
    TRACER("GlxErrorManager::IsExpired");
    
    if ( IsTemporaryError(aError.iError) )
        {
        TTime now;
        now.UniversalTime();

        TTime errorTime(aError.iTimestamp);

        if ( now - TTimeIntervalSeconds(KGlxTemporaryErrorValidityPeriodInSeconds) > errorTime )
            {
            return ETrue;
            }
        }
        
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// HasError
// -----------------------------------------------------------------------------
//
TBool GlxErrorManager::HasError(const CGlxMedia* aMedia)
    {
    TRACER("GlxErrorManager::HasError");
    
    if ( aMedia )
        {
        return ( Errors( *aMedia ) != NULL );
        }
    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// ClearExpiredAndUnusedErrorsL
// -----------------------------------------------------------------------------
//
void GlxErrorManager::ClearExpiredAndUnusedErrorsL( CGlxMedia& aMedia, 
        const RArray<TMPXAttribute>& aAttributesInUse )
    {
    TRACER("GlxErrorManager::ClearExpiredAndUnusedErrorsL");
    
    CGlxFetchErrorArray* errors = Errors( aMedia );
    
    if ( errors )
        {
        // Iterate through the list of errors backwards, so that removal is more
        // efficient, and loop control easier
        for ( TInt i = errors->ErrorCount() - 1; i >= 0; i-- )
            {
            TGlxFetchError error = errors->Error( i );
            // remove the error if it has expired, or if the attribute 
            // that the error is for is no longer in use
            if ( IsExpired( error ) || 
                 KErrNotFound == aAttributesInUse.Find( error.iAttr, TMPXAttribute::Match ) )
                {
                errors->Remove( i );
                }
            }
            
        // no further errors are remaining, so can remove the error array 
        // attribute from the media object
        if ( errors->ErrorCount() == 0 )
            {
            aMedia.DeleteAttribute( KGlxMediaErrorArray );
            }
        }
    }

// -----------------------------------------------------------------------------
// Return error attribute's id
// -----------------------------------------------------------------------------
//
TMPXAttribute GlxErrorManager::ErrorAttribute()
    {
    TRACER("GlxErrorManager::ErrorAttribute");
    
    return KGlxMediaErrorArray;
    }
