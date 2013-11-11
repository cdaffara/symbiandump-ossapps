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
* Description:    Media item 
*
*/




#include "glxmedia.h"

#include <glxassert.h>
#include <glxthumbnailattributeinfo.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediadrmdefs.h>
#include <e32math.h>
#include <lbsposition.h>                    // for TCoordinate

#include "glxcollectiongeneraldefs.h"
#include "glxmediastaticitemdefs.h"
#include "glxthumbnailutility.h"
#include <glxtracer.h>                 //For logging

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
TGlxMedia::TGlxMedia( CGlxMedia& aMedia )
    {
    TRACER("TGlxMedia::Default Constructor");
    
    __ASSERT_DEBUG ( aMedia.Id() != KGlxIdNone, Panic( EGlxPanicIllegalArgument ) ); // Must have an id defined,
    iId = aMedia.Id();
    iItem = &aMedia;
    }

// -----------------------------------------------------------------------------
// Compare two media items by id
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TGlxMedia::MatchById(const TGlxMedia& aMedia1, const TGlxMedia& aMedia2)
	{
	TRACER("TGlxMedia::MatchById");
	
	return aMedia1.iId == aMedia2.iId;
	}

// -----------------------------------------------------------------------------
// Tests to see if the Media is a static
// -----------------------------------------------------------------------------

EXPORT_C TBool TGlxMedia::IsStatic() const
    {
    TRACER("TGlxMedia::IsStatic");
    
    if ( iItem )
        {
        return iItem->IsSupported( KGlxMediaStaticItem );
        }
        
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Gets the command associated with a static item
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetStaticItemCommand(TInt& aCmd) const
    {
    TRACER("TGlxMedia::GetStaticItemCommand");
    
    if ( iItem )
        {
        TMPXAttribute attrib(KGlxMediaIdStaticItem,
                            KGlxMediaStaticItemCommand);
        return iItem->GetValueTObject( aCmd, attrib );
        }
    // by default the cmd value is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Gets the date
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetDate(TTime& aDate) const
    {
    TRACER("TGlxMedia::GetDate");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aDate, KMPXMediaGeneralDate );
        }
    // by default the date is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Gets the last modified date
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetLastModifiedDate(TTime& aDate) const
    {
    TRACER("TGlxMedia::GetLastModifiedDate");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aDate, KGlxMediaGeneralLastModifiedDate );
        }
    // by default the date is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Gets the general category
// -----------------------------------------------------------------------------
EXPORT_C TMPXGeneralCategory TGlxMedia::Category() const
    {
    TRACER("TGlxMedia::Category");
    
    TMPXGeneralCategory cat =  EMPXNoCategory;
    if ( iItem )
        {
        // if this fails cat will remain as EMPXNoCategory
        iItem->GetValueTObject( cat, KMPXMediaGeneralCategory );
        }
    return cat;
    }
    
// -----------------------------------------------------------------------------
// Gets the dimensions
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetDimensions(TSize& aSize) const
    {
    TRACER("TGlxMedia::GetDimensions");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aSize, KGlxMediaGeneralDimensions );
        }
    // by default the size is not changed    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Gets the (file) size
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetSize(TUint& aSize) const
    {
    TRACER("TGlxMedia::GetSize");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aSize, KMPXMediaGeneralSize );
        }
    // by default the size is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Gets the duration 
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetDuration(TReal32& aDuration) const
    {
    TRACER("TGlxMedia::GetDuration");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aDuration, KMPXMediaGeneralDuration );
        }
    // by default the duration is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Gets the frame count
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetFrameCount(TInt& aFrameCount) const
    {
    TRACER("TGlxMedia::GetFrameCount");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aFrameCount, KGlxMediaGeneralFramecount );
        }
    // by default the frame count is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Gets the general count
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetContainedItemCount(TInt& aGeneralCount) const
    {
    TRACER("TGlxMedia::GetContainedItemCount");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aGeneralCount, KMPXMediaGeneralCount );
        }
    // by default the frame count is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Gets the general count
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetSlideshowPlayableContainedItemCount(TInt& aGeneralCount) const
    {
    TRACER("TGlxMedia::GetSlideshowPlayableContainedItemCount");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aGeneralCount, KGlxMediaGeneralSlideshowableContent );
        }
    // by default the frame count is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Gets the coordinate value
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetCoordinate(TCoordinate& aCoordinate) const
    {
    TRACER("TGlxMedia::GetCoordinate");
    
    TBool isSupported = EFalse;
    
    if ( iItem )
        {
        if (iItem->GetValueTObject( aCoordinate, KGlxMediaGeneralLocation ))
            {
            isSupported = ETrue;
            if ( Math::IsNaN( aCoordinate.Latitude() ) ||
                 Math::IsNaN( aCoordinate.Longitude() )  ||
                 aCoordinate.Datum() != KPositionDatumWgs84 )
                {
                isSupported = EFalse;
                }
            }
        }
    // by default the coodinate is not changed    
    return isSupported;
    }
    
// -----------------------------------------------------------------------------
// Gets the IconInfo
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetIconInfo(TIconInfo& aInfo) const
    {
    TRACER("TGlxMedia::GetIconInfo");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aInfo, KMPXMediaGeneralIcon );
        }
    // by default aInfo is not changed    
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Returns the DRM protection indication - defaults to ETrue if not found
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::IsDrmProtected() const
    {
    TRACER("TGlxMedia::IsDrmProtected");
    
    TBool isDrmProtected = EFalse;
    if ( iItem )
        {
        // ignore return value. Failure leaves isDrmProtected unchanged as ETrue
        iItem->GetValueTObject( isDrmProtected, KMPXMediaDrmProtected );
        }
    // by default is protected
    return isDrmProtected;
    }

// -----------------------------------------------------------------------------
// Gets the DRM protection
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetDrmProtected(TBool& aIsProtected) const
    {
    TRACER("TGlxMedia::GetDrmProtected");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aIsProtected, 
                                       KMPXMediaDrmProtected );
        }
    // by default aIsProtected is not changed
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Gets the DRM protection
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetDrmValidity(TGlxMediaGeneralRightsValidity& aIsValid) const
    {
    if ( iItem )
        {
        return iItem->GetValueTObject( aIsValid, 
                                       KGlxMediaGeneralDRMRightsValid );
        }
    // by default aIsValid is not changed
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Gets the System Item indicator
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetSystemItem(TBool& aIsSystemItem) const
    {
    TRACER("TGlxMedia::GetSystemItem");
    
    if ( iItem )
        {
        return iItem->GetValueTObject( aIsSystemItem, 
                                       KGlxMediaGeneralSystemItem );
        }
    // by default aIsSystemItem is not changed
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Gets the title
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& TGlxMedia::Title() const
    {
    TRACER("TGlxMedia::Title");
    
    if ( iItem )
        {
        return iItem->ValueText( KMPXMediaGeneralTitle );
        }
    return KNullDesC;    
    }
    
// -----------------------------------------------------------------------------
// Gets the subtitle
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& TGlxMedia::SubTitle() const
    {
    TRACER("TGlxMedia::SubTitle");
    
    if ( iItem )
        {
        return iItem->ValueText( KGlxMediaCollectionPluginSpecificSubTitle );
        }
    return KNullDesC;    
    }
    

// -----------------------------------------------------------------------------
// Gets the comment
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& TGlxMedia::Comment() const
    {
    TRACER("TGlxMedia::Comment");
    
    if ( iItem )
        {
        return iItem->ValueText( KMPXMediaGeneralComment );
        }
    return KNullDesC;    
    }
    
// -----------------------------------------------------------------------------
// Gets the URI indicating the success in the return value
// -----------------------------------------------------------------------------
EXPORT_C const TDesC&  TGlxMedia::Uri() const
    {
    TRACER("TGlxMedia::Uri");
    
    if( iItem )
        {
        return iItem->ValueText( KMPXMediaGeneralUri);
        }
    // by default descriptor pointer is unchanged
    return KNullDesC;
    }
    
// -----------------------------------------------------------------------------
// Gets the MimeType indicating the success in the return value
// -----------------------------------------------------------------------------
EXPORT_C const TDesC&  TGlxMedia::MimeType() const
    {
    TRACER("TGlxMedia::MimeType");
    
    if( iItem )
        {
        return iItem->ValueText( KMPXMediaGeneralMimeType);
        }
    // by default descriptor pointer is unchanged
    return KNullDesC;
    }
    
    
// -----------------------------------------------------------------------------
// Gets a thumbnail attribute
// -----------------------------------------------------------------------------
EXPORT_C const CGlxThumbnailAttribute* TGlxMedia::ThumbnailAttribute(
                                                TMPXAttribute& aAttribute) const
    {
    TRACER("TGlxMedia::ThumbnailAttribute");
    
    if( iItem && KGlxMediaIdThumbnail == aAttribute.ContentId() )
        {
        return static_cast<const CGlxThumbnailAttribute*>
                                  ( iItem->ValueCObject(aAttribute) );
        }
    else
        {
        return NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// GetClosestThumbnail
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TGlxMedia::GetClosestThumbnail( TMPXAttribute& aAttribute,
                            const TSize& aDesiredSize, TBool aDrmValid ) const
    {
    TRACER("TGlxMedia::GetClosestThumbnail");
    
    if ( iItem )
        {
        TInt index = GlxThumbnailUtility::ClosestThumbnail( aDesiredSize,
                                                        *iItem, aDrmValid );

        if ( index >= 0 )
            {
            aAttribute = iItem->Attribute(index);

            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// Is the media playable in slideshow
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TGlxMedia::IsSlideShowPlayableContent() const
    {
    TRACER("TGlxMedia::IsSlideShowPlayableContent");
    
    if (iItem)
        {
    	// Only DRM protected images are allowed
        if (iItem->IsSupported(KMPXMediaGeneralCategory) && 
            iItem->IsSupported(KMPXMediaDrmProtected) && 
            iItem->IsSupported(KGlxMediaGeneralFramecount))
            {
            if (iItem->ValueTObject<TInt>(KMPXMediaGeneralCategory) == EMPXImage &&
                !iItem->ValueTObject<TInt>(KMPXMediaDrmProtected) && 
                iItem->ValueTObject<TInt>(KGlxMediaGeneralFramecount) == 1)
            	{
                return ETrue;                	
            	}
            }
        // The media item is either not DRM protected, a video or an animated gif    
        }
    return EFalse;    			
    }

// -----------------------------------------------------------------------------
// Returns the IdSpaceId associated with the media
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxIdSpaceId TGlxMedia::IdSpaceId() const
    {
    TRACER("TGlxMedia::IdSpaceId");
    
    TGlxIdSpaceId idSpaceId( KGlxIdNone );

    if ( iItem )
        {
        iItem->GetValueTObject( idSpaceId, KMPXMediaColDetailSpaceId );
        }

    return idSpaceId;
    }

// -----------------------------------------------------------------------------
// Set media pointer
// -----------------------------------------------------------------------------
//
void TGlxMedia::SetMedia( CGlxMedia* aMedia, MGlxMediaUser& aMediaUser, TInt aIndex )
	{
	TRACER("TGlxMedia::SetMedia");
	
    __ASSERT_DEBUG( aMedia == NULL || !iItem , Panic(EGlxPanicAlreadyInitialised ) ); // Already has media
    if ( aMedia )
        {
        // Media object being added
    	aMedia->AddUser( aMediaUser, aIndex );
        }
    else if ( iItem )
        {
        // Media object being set to null, remove since old media object exists
    	iItem->RemoveUser( aMediaUser );
        }
	iItem = aMedia;
	}

// -----------------------------------------------------------------------------
// Update media
// -----------------------------------------------------------------------------
//
void TGlxMedia::UpdateMedia( MGlxMediaUser& aMediaUser, TInt aIndex )
    {
    TRACER("TGlxMedia::UpdateMedia");
    
    if ( iItem )
        {
        iItem->UpdateUser( aMediaUser, aIndex );
        }
	}
   
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxMedia::CGlxMedia(const TGlxMediaId& aId)
		: iId(aId), iUsers(2) // On average, two list as users assumed
    {
    TRACER("CGlxMedia::Default Constructor");
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxMedia::~CGlxMedia()
    {
    TRACER("CGlxMedia::Destructor");
    
    iUsers.Close();

    Reset();
    iValues.Close(); 
    iAttributes.Close();
    }

// -----------------------------------------------------------------------------
// Make space in the array of users, so can safely append later
// -----------------------------------------------------------------------------
//
void CGlxMedia::ReserveUsersL(TInt aReservationCount)
	{
	TRACER("CGlxMedia::ReserveUsersL");
	
	iUsers.ReserveL( aReservationCount );
    __DEBUG_ONLY( _iUserReservationCount = aReservationCount );
	}

// -----------------------------------------------------------------------------
// Adds a user to the list
// -----------------------------------------------------------------------------
//
void CGlxMedia::AddUser( MGlxMediaUser& aUser, TInt aIndex ) 
	{
	TRACER("CGlxMedia::AddUser");
	
	// Must not exist already
	__ASSERT_DEBUG( !IsUser( aUser ), Panic( EGlxPanicAlreadyAdded ) ); 
	// Client must have made a reservation before calling
	__ASSERT_DEBUG( _iUserReservationCount > 0, Panic( EGlxPanicLogicError ) );

    __DEBUG_ONLY( _iUserReservationCount-- );

	iUsers.AppendL( TMediaUser( &aUser, aIndex ) ); // Ignore error, cannot fail since reservation made 
	}

// -----------------------------------------------------------------------------
// RemoveUser
// -----------------------------------------------------------------------------
//
void CGlxMedia::RemoveUser( const MGlxMediaUser& aUser ) 
	{
	TRACER("CGlxMedia::RemoveUser");
	
    TMediaUser mediaUser( const_cast<MGlxMediaUser*>( &aUser ) );
	TInt index = iUsers.Find( mediaUser, &TMediaUser::MatchUser );	
    if ( KErrNotFound != index )
        {
    	iUsers.Remove( index );
        }
	}

// -----------------------------------------------------------------------------
// IsUser
// -----------------------------------------------------------------------------
//
TBool CGlxMedia::IsUser( const MGlxMediaUser& aUser ) const
	{
	TRACER("CGlxMedia::IsUser");
	
    TMediaUser mediaUser( const_cast<MGlxMediaUser*>( &aUser ) );
	return ( KErrNotFound != iUsers.Find( mediaUser, &TMediaUser::MatchUser ) );
	}

// -----------------------------------------------------------------------------
// Return user by index
// -----------------------------------------------------------------------------
//
MGlxMediaUser& CGlxMedia::User( TInt aIndex ) const
	{
	TRACER("CGlxMedia::User");
	
	return *( iUsers[ aIndex ].iMediaUser );
	}

// -----------------------------------------------------------------------------
// Return index in the media list user
// -----------------------------------------------------------------------------
//
TInt CGlxMedia::IndexInUser(TInt aIndex) const
    {
    TRACER("CGlxMedia::IndexInUser");
    
	  return iUsers[ aIndex ].iIndex;
    }

// -----------------------------------------------------------------------------
// Update index in the media list user
// -----------------------------------------------------------------------------
//
void CGlxMedia::UpdateUser( MGlxMediaUser& aUser, TInt aIndex )
    {
    TRACER("CGlxMedia::UpdateUser");
    
    TMediaUser mediaUser( &aUser );
    TInt index = iUsers.Find( mediaUser, &TMediaUser::MatchUser );	
    if ( KErrNotFound != index )
        {
        iUsers[ index ].iIndex = aIndex;
        }
	}

// -----------------------------------------------------------------------------
// Return the number of users
// -----------------------------------------------------------------------------
//
TInt CGlxMedia::UserCount() const
	{
	TRACER("CGlxMedia::UserCount");
	
	return iUsers.Count();
	}

// -----------------------------------------------------------------------------
// The value for a specific attribute
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CGlxMedia::ValueText(const TMPXAttribute& aAttribute) const
    {
    TRACER("CGlxMedia::ValueText");
    
    TInt i = Index(aAttribute);
    HBufC* ptr = NULL;
    if (i == KErrNotFound)
        {
        return KNullDesC;
        }

   // __ASSERT_DEBUG(EMPXTypeText == iValues[i].iType,
   //                                     Panic(EGlxPanicWrongAttributeType));
   if(EMPXTypeText == iValues[i].iType)
    {
    ptr = reinterpret_cast<HBufC*>(iValues[i].iValue);
    __ASSERT_DEBUG(ptr != NULL, Panic(EGlxPanicNullPointer));
    }
    
    return *ptr;
    }   

// -----------------------------------------------------------------------------
// The text from a specific attribute, indicating success
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxMedia::GetValueText(TPtrC& aText, 
                                       const TMPXAttribute& aAttribute) const
    {
    TRACER("CGlxMedia::GetValueText");
    
    TInt i = Index(aAttribute);
    if (i == KErrNotFound)
        {
        // leave aText unchanged
        return EFalse;
        }

    __ASSERT_DEBUG(EMPXTypeText == iValues[i].iType,
                                        Panic(EGlxPanicWrongAttributeType));
    HBufC* ptr = reinterpret_cast<HBufC*>(iValues[i].iValue);
    __ASSERT_DEBUG(ptr != NULL, Panic(EGlxPanicNullPointer));
    aText.Set(*ptr);
    return ETrue;
    }   

// -----------------------------------------------------------------------------
// The value for a specific attribute
// -----------------------------------------------------------------------------
//
EXPORT_C const CBase* CGlxMedia::ValueCObject(const TMPXAttribute& aAttribute) const
    {
    TRACER("CGlxMedia::ValueCObject");
    
    TInt i = Index(aAttribute);
	if (i == KErrNotFound)
        {
        return NULL;
        }

    __ASSERT_DEBUG(EMPXTypeCObject == iValues[i].iType,
                                        Panic(EGlxPanicWrongAttributeType));
	CBase* ptr = reinterpret_cast<CBase*>(iValues[i].iValue);
	__ASSERT_DEBUG(ptr != NULL, Panic(EGlxPanicNullPointer));
   	return ptr;
    }   

// -----------------------------------------------------------------------------
// Clears all the attributes and values
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxMedia::Reset() 
	{
	TRACER("CGlxMedia::Reset");
	
	// Delete attribute values
	TInt count = iValues.Count();
	__ASSERT_DEBUG(iAttributes.Count() == count, Panic(EGlxPanicLogicError));
	for (TInt i = 0; i < count; i++) 
		{
		Delete(iValues[i]);
		}
	
	// Remove also attributes and pointers to values
    iValues.Reset(); 
    iAttributes.Reset();
	}

// -----------------------------------------------------------------------------
// Deletes the value
// -----------------------------------------------------------------------------
//
void CGlxMedia::Delete(TValue& aValue) 
	{
	TRACER("CGlxMedia::Delete");
	
	TMPXAttributeType type = aValue.iType;
	if (type == EMPXTypeText)
		{
		delete reinterpret_cast<HBufC*>(aValue.iValue);
		}
	else if (type == EMPXTypeCObject)
		{
		delete reinterpret_cast<CBase*>(aValue.iValue);
		}
	else if (type == EMPXTypeTObject) 
		{
		User::Free(aValue.iValue);
		}
	}

// -----------------------------------------------------------------------------
// Sets or modifies the value
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxMedia::SetTextValueL(const TMPXAttribute& aAttribute,
                            		   const TDesC& aValue) 
	{
	TRACER("CGlxMedia::SetTextValueL");
	
	// Allocate first for leave safely
	HBufC* buf = aValue.AllocLC();
	
    TInt i = Index(aAttribute);
    if (i != KErrNotFound)
        {
       // Replace the existing value
        TValue& value = iValues[i];
        Delete(value);
        
        value.iValue = buf;
        value.iType = EMPXTypeText;
        }
    else
        {
        iValues.AppendL(TValue(buf, EMPXTypeText));
        TInt err = iAttributes.Append(aAttribute);
        if (err != KErrNone)
        	{
        	// Failed, undo the append
        	iValues.Remove(iValues.Count() - 1);
      		User::Leave(err);
        	}
		}
        
    CleanupStack::Pop(buf);
	}
	
// -----------------------------------------------------------------------------
// Sets or modifies the value
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxMedia::SetCObjectValueL(const TMPXAttribute& aAttribute,
                            		   	  CBase* aValue_TakesOwnership) 
	{
	TRACER("CGlxMedia::SetCObjectValueL");
	
    TInt i = Index(aAttribute);
    if (i != KErrNotFound)
        {
 	    // Replace the existing value
        TValue& value = iValues[i];
        Delete(value);
        
        value.iValue = aValue_TakesOwnership;
        value.iType = EMPXTypeCObject;
        }
    else
        {
        iValues.AppendL(TValue(aValue_TakesOwnership, EMPXTypeCObject));
        TInt err = iAttributes.Append(aAttribute);
        if (err != KErrNone)
        	{
        	// Failed, undo the append
        	iValues.Remove(iValues.Count() - 1);
        	User::Leave(err);
        	}
        }
	}

// -----------------------------------------------------------------------------
// DeleteAttribute
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxMedia::DeleteAttribute(const TMPXAttribute& aAttribute)
    {
    TRACER("CGlxMedia::DeleteAttribute");
    
    TInt index = Index( aAttribute );
    if ( KErrNotFound != index )
        {
        DeleteAttribute( index );
        }
    }

// -----------------------------------------------------------------------------
// AttributeTypeId
// -----------------------------------------------------------------------------
//
TMPXAttributeType CGlxMedia::AttributeTypeId(const TMPXAttribute& aAttribute) const
	{
	TRACER("CGlxMedia::AttributeTypeId");
	
	TMPXAttributeType type(EMPXTypeUnknown);
    TInt index(Index(aAttribute));
	if( index != KErrNotFound)
		{
		return iValues[index].iType;
		}
	return type;
	
	}
// -----------------------------------------------------------------------------
// DeleteAttribute
// -----------------------------------------------------------------------------
//
void CGlxMedia::DeleteAttribute( TInt aAttributeIndex )
    {
    TRACER("CGlxMedia::DeleteAttribute");
    
    GLX_ASSERT_DEBUG( 0 <= aAttributeIndex && aAttributeIndex < iValues.Count(), 
        Panic(EGlxPanicIllegalArgument), "deleting attribute out of bounds");

    // Delete the existing value
    TValue& value = iValues[aAttributeIndex];
    Delete( value );
    iValues.Remove( aAttributeIndex );
    iAttributes.Remove( aAttributeIndex );
    }
    
// -----------------------------------------------------------------------------
// Handles modification of item
// -----------------------------------------------------------------------------
EXPORT_C void CGlxMedia::HandleModified(const RArray<TMPXAttribute>& aAttributes)
	{
	TRACER("CGlxMedia::HandleModified");
	
	TInt attributeCount = aAttributes.Count();
	if (attributeCount > 0)
		{
		// Remove specified attributes
		for (TInt count = 0; count < attributeCount; ++count)
			{
			DeleteAttribute(aAttributes[count]);
			}
		}
	else
		{
		// Remove all attributes
		Reset();
		}
	}

// -----------------------------------------------------------------------------
// Sets generic TAny* value
// -----------------------------------------------------------------------------
EXPORT_C void CGlxMedia::SetValueL(const TMPXAttribute& aAttribute, TAny* aValue, TMPXAttributeType aType)
    {
    TRACER("CGlxMedia::SetValueL");
    
    TInt i = Index(aAttribute);
    if (KErrNotFound != i)
        {
        // Replace the existing value
        TValue& value = iValues[i];
        Delete(value);

        value.iValue = aValue; // Takes ownership
        value.iType = aType;
        }
    else
        {
        TInt err = iValues.Append(TValue(aValue, aType));

        // If appended value ok, try to append the attribute.
        if (KErrNone == err)
            {
            err = iAttributes.Append(aAttribute);
            if (err != KErrNone)
                {
                // Failed, undo the append
                iValues.Remove(iValues.Count() - 1);
                }
            }

        if (KErrNone != err)
            {
            User::Free(aValue);
            User::Leave(err);
            }
        }
    }
    

// ---------------------------------------------------------------------------
// Find match by MGlxMediaUser
// ---------------------------------------------------------------------------
//
TBool CGlxMedia::TMediaUser::MatchUser(const TMediaUser& aUser1, const TMediaUser& aUser2)
    {
    TRACER("CGlxMedia::TMediaUser::MatchUser");
    
    return aUser1.iMediaUser == aUser2.iMediaUser;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxMedia::TMediaUser::TMediaUser(MGlxMediaUser* aMediaUser, TInt aIndex) :
        iMediaUser( aMediaUser ),
        iIndex( aIndex )
    {
    TRACER("CGlxMedia::TMediaUser::Default Constructor");
    
    }

// ---------------------------------------------------------------------------
// Test invariant
// ---------------------------------------------------------------------------
EXPORT_C void CGlxMedia::__DbgTestInvariant() const
    {
    TRACER("CGlxMedia::__DbgTestInvariant");
    
    #ifdef _DEBUG
    
        __ASSERT_DEBUG( Id() != KGlxIdNone, Panic(EGlxPanicIllegalState));
	    // Make sure each list is a user only once
	    TInt userCount = iUsers.Count();
	    for (TInt i = 0; i < userCount; i++) 
	    	{
		    for (TInt j = i + 1; j < userCount; j++) 
		    	{
			    __ASSERT_DEBUG( !TMediaUser::MatchUser( iUsers[i], iUsers[j] ), Panic(EGlxPanicIllegalState));
		    	}
	    	}

    #endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// Delete location attribute
// -----------------------------------------------------------------------------
EXPORT_C void TGlxMedia::DeleteLocationAttribute()
    {
    TRACER("TGlxMedia::DeleteLocationAttribute");
    if ( iItem )
        {
        iItem->DeleteAttribute( KGlxMediaGeneralLocation );
        }
 
     }
