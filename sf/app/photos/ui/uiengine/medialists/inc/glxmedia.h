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




#ifndef C_GLXMEDIA_H
#define C_GLXMEDIA_H

#include <e32base.h>
#include <glxmediageneraldefs.h>
#include <mpxmediageneraldefs.h>

#include "glxmediaid.h"
#include "glxthumbnailinfo.h"

class CGlxMedia; 
class CGlxMediaList; 
class CGlxThumbnailAttribute;
class CGlxUstringConverter;
class MGlxMediaUser;
class TCoordinate;
/**
 *  TGlxMedia
 *
 *  Reference to a media item in the item pool.
 *
 * @ingroup mlm_media_list_manager_design
 */
NONSHARABLE_CLASS( TGlxMedia )
    {
	friend class CGlxMediaList; // To avoid exposing a SetProperties(...) interface
	//Only to access AttributeTypeId to get the MPX attribute type
	friend class GlxUStringConverter;
public:
    inline TGlxMedia( const TGlxMediaId& aId ) : iId( aId ) { iItem = NULL; };
    inline TGlxMedia( const TGlxMediaId& aId, CGlxMedia* aProperties ) : iId( aId ) { iItem = aProperties; };
    inline TGlxMedia( const TGlxMedia& aItem ) : iId( aItem.iId ) { iItem = aItem.iItem; };

    /**
     * Constructor
     * Not inlined as not intended to be used outside the dll 
     * Only intended to be used for static items
     * @param aMedia Media object from which to take the id, and to which to point
     */
    TGlxMedia( CGlxMedia& aMedia );
  
    /**
     * @return Id of this media item
     */
    inline TGlxMediaId Id() const { return iId; };
    
    /**
     * @return Properties object of this media item
     * @note May return NULL if the properties are not yet available
     */ 
    inline const CGlxMedia* Properties() const { return iItem; };
    
    /**
     * Compare two media items by id
     * This can be used in array's Find etc. methods
     */
    IMPORT_C static TBool MatchById(const TGlxMedia& aMedia1, const TGlxMedia& aMedia2);
    
    /**
     * Tests to see if the Media is a static.
     * returns False if item is not static 
     *         True is item is static
     */
    IMPORT_C TBool IsStatic() const;
    
    /**
     * Get the command associated with a static media item.
     * @param aCmd Reference to variable into which to place the command
     * @return EFalse if unable to get command - aCmd is unchanged. 
     *         ETrue if command successfully put in aCmd.
     */
    IMPORT_C TBool GetStaticItemCommand(TInt& aCmd) const;

    /**
     * Get the date associated with the media.
     * @param aDate Reference to variable into which to place the date
     * @return EFalse if unable to get date - aDate is unchanged. 
     *         ETrue if date successfully put in aDate.
     */
    IMPORT_C TBool GetDate(TTime& aDate) const;

    /**
     * Get the date associated with the media.
     * @param aDate Reference to variable into which to place the date
     * @return EFalse if unable to get date - aDate is unchanged. 
     *         ETrue if date successfully put in aDate.
     */
    IMPORT_C TBool GetLastModifiedDate(TTime& aDate) const;

    /**
     * Get the general category associated with the media.
     * @param aCategory Reference to variable into which to place the category
     * @return The category or EMPXNoCategory if unsuccessful. 
     */
    IMPORT_C TMPXGeneralCategory Category() const;

    /**
     * Get the dimensions associated with the media.
     * @param aSize Reference to variable into which to place the dimensions
     * @return EFalse if unable to get dimensions - aSize is unchanged. 
     *         ETrue if dimensions successfully put in aSize.
     */
    IMPORT_C TBool GetDimensions(TSize& aSize) const;
    
    /**
     * Get the (file) size associated with the media.
     * @param aSize Reference to variable into which to place the (file) size
     * @return EFalse if unable to get size - aSize is unchanged. 
     *         ETrue if size successfully put in aSize.
     */
    IMPORT_C TBool GetSize(TUint& aSize) const;
    
    /**
     * Get the duration associated with the media.
     * @param aDuration Reference to variable into which to place the duration
     * @return EFalse if unable to get size - aDuration is unchanged. 
     *         ETrue if duration successfully put in aDuration.
     */
    IMPORT_C TBool GetDuration(TReal32& aDuration) const;

    /**
     * Get the frame count associated with the media.
     * @param aFrameCount Reference to variable into which to place count
     * @return EFalse if unable to get count - aFrameCount is unchanged. 
     *         ETrue if count successfully put in aFrameCount.
     */
    IMPORT_C TBool GetFrameCount(TInt& aFrameCount) const;

    /**
     * Get the general count associated with the media.
     * @param aGeneralCount Reference to variable into which to place count
     * @return EFalse if unable to get count - aGeneralCount is unchanged. 
     *         ETrue if count successfully put in aGeneralCount.
     */
    IMPORT_C TBool GetContainedItemCount(TInt& aGeneralCount) const;

    /**
     * Get the general count associated with the media.
     * @param aGeneralCount Reference to variable into which to place count
     * @return EFalse if unable to get count - aGeneralCount is unchanged. 
     *         ETrue if count successfully put in aGeneralCount.
     */
    IMPORT_C TBool GetSlideshowPlayableContainedItemCount(TInt& aGeneralCount) const;

   /**
     * Get the coordinate associated with the media.
     * @param aCoordinate Reference to variable into which to place the coordinate
     * @return EFalse if unable to get coordinate - aCoordinate is unchanged. 
     *         ETrue if coordinate successfully put in aCoordinate.
     */
    IMPORT_C TBool GetCoordinate(TCoordinate& aCoordinate) const;

    /**
     * Get the Icon Info associated with the media.
     * @param aInfo Reference to variable into which to place the icon Info
     * @return EFalse if unable to get info - aInfo is unchanged. 
     *         ETrue if info successfully put in aInfo.
     */
    IMPORT_C TBool GetIconInfo(TIconInfo& aInfo) const;
    
    /**
     * Returns the DRM protection indication associated with the media.
     * @return EFalse if not protected. 
     *         ETrue if protected (also by default).
     */
    IMPORT_C TBool IsDrmProtected() const;
    
    /**
     * Get the DRM protection indicator associated with the media.
     * @param aIsProtected Reference to variable into which to place the DRM indicator
     * @return EFalse if unable to get indicator - aIsProtected is unchanged. 
     *         ETrue if indicator successfully put in aIsProtected.
     */
    IMPORT_C TBool GetDrmProtected(TBool& aIsProtected) const;

    /**
     * Get the DRM protection indicator associated with the media.
     * @param aIsProtected Reference to variable into which to place the DRM indicator
     * @return EFalse if unable to get indicator - aIsProtected is unchanged. 
     *         ETrue if indicator successfully put in aIsProtected.
     */
    IMPORT_C TBool GetDrmValidity(TGlxMediaGeneralRightsValidity& aIsValid) const;

    /**
     * Get the System Item indicator associated with the media.
     * @param aIsSystemItem Reference to variable into which to place the SI indicator
     * @return EFalse if unable to get indicator - aIsSystemItem is unchanged. 
     *         ETrue if indicator successfully put in aIsSystemItem.
     */
    IMPORT_C TBool GetSystemItem(TBool& aIsSystemItem) const;

    /**
     * Get the title associated with the media.
     * @return descriptor reference to title if found, else to an empty descriptor.
     */
    IMPORT_C const TDesC& Title() const;

    /**
     * Get the subtitle associated with the media.
     * @return descriptor reference to subtitle if found, else to an empty descriptor.
     */
    IMPORT_C const TDesC& SubTitle() const;

    /**
     * Get the comment associated with the media.
     * @return descriptor reference to comment if found, else to the NULL descriptor.
     */
    IMPORT_C const TDesC& Comment() const;
    
    /**
     * Get the URI associated with the media.
     * @return Uri for success or KNullDesC for failure
     */
    IMPORT_C const TDesC& Uri() const;

    /**
     * Get the MimeType associated with the media.
     * @return MimeType for success or KNullDesC for failure
     */
    IMPORT_C const TDesC& MimeType() const;

    /**
     * Get a thumbnail attribute associated with the media.
     * @param aAttribute The attribute to obtain
     * @return A pointer to the thumbnail attribute or NULL if unsuccessful. 
     */
    IMPORT_C const CGlxThumbnailAttribute* TGlxMedia::ThumbnailAttribute(
                                             TMPXAttribute& aAttribute) const;
    
    /**
     * Find the closest available thumbnail to a given size.
     * @param aAttribute Set to the closest available thumbnail, if any
     * @param aDesiredSize Size of thumbnail required
     * @param aDrmValid If false, the function will not return a thumbnail
                bigger than the requested size
     * @return EFalse if no thumbnails available. 
     *         ETrue if aAttribute has been set.
     */
    IMPORT_C TBool GetClosestThumbnail( TMPXAttribute& aAttribute,
                        const TSize& aDesiredSize, TBool aDrmValid ) const;
                        
    /**
     * Can the media be played in slideshow
     * @return EFalse if not playable. 
     *         ETrue if playable. 
     */                        
    IMPORT_C TBool IsSlideShowPlayableContent() const;                        

    /**
     * Returns the IdSpaceId associated with the media.
     * @return TGlxIdSpaceId of the media
     */
    IMPORT_C TGlxIdSpaceId IdSpaceId() const;

    /** 
     * Set media object pointer
     * not inlined, since will not be exported
     * @param aMedia Media object, or NULL
     * @param aMediaUser User of media
     * @param aIndex Index of media object in media list user
     */
    void SetMedia( CGlxMedia* Media, MGlxMediaUser& aMediaUser, TInt aIndex = KErrNotFound);
    
    /** 
     * Update media object
     * Used to update the index into the media list user
     * @param aMediaUser User of media
     * @param aIndex Index of media object in media list user
     */
    void UpdateMedia( MGlxMediaUser& aMediaUser, TInt aIndex );
    
    IMPORT_C void DeleteLocationAttribute();

private:
    /**
     * Unique identifier of this media item
     */
    TGlxMediaId iId;

    /**
     * Pointer to the shared instance of media item properties
     * Not owned
     */
	CGlxMedia* iItem; 
    };

/**
 *  CGlxMedia 
 *
 *  Basic properties for a media item
 *
 *  @lib glxmedialists.lib
 */
NONSHARABLE_CLASS( CGlxMedia ) : public CBase
    {
public:

    /**
     * Constructor
     * @param aId media item ID
     */
    IMPORT_C CGlxMedia( const TGlxMediaId& aId );

    /**
     * Destructor
     */
    IMPORT_C ~CGlxMedia();
       
    /**
     * @param aId New Id for this media item.
     */
    inline void SetId(const TGlxMediaId& aId) { iId = aId; }  // TEMPORARY, merge to attribute array
    
    /**
     * @return Id of this media item
     */
    inline TGlxMediaId Id() const { return iId; }; // TEMPORARY, merge to attribute array

public:

    /**
     *  The attributes provided in this media object 
     *
     *  @return array of attributes indicating the attributes for the values contained 
     *  in this object
     */
    inline const TArray<TMPXAttribute> Attributes() const;
    
    /**
     *  Does this object contain the value for a given attribute 
     *
     *  @param aAttribute, the specified attribute
     *  @return whether supported or not
     */
    inline TBool IsSupported(const TMPXAttribute& aAttribute) const;
    
    /**
     *  The number of attribute values provided in this media object 
     *
     *  @return count of attribute values in this object
     */
    inline TInt Count() const;
    
    /**
     *  The attribute for a specific index 
     *
     *  @param aIndex the index from 0 to Count()-1, covering all the values
     *                provided in this object
     *  @return the attribute corresponding to the value at the specified index
     */
    inline const TMPXAttribute& Attribute(TInt aIndex) const;
    
    /**
     *  The index of a given attribute 
     *
     *  @param aAttribute the specified attribute
     *  @return the index, or KErrNotFound if not present in this object
     */
    inline TInt Index(const TMPXAttribute& aAttribute) const;
        
    /**
     *  The value for a specific attribute 
     *
     *  @param aAttribute, the attribute whose value is queried
     *  @return value of the attribute
     */
    template<typename T> 
    inline T ValueTObject(const TMPXAttribute& aAttribute) const;

    /**
     *  Get the value for a specific attribute, indicating success 
     *
     *  @param aValue, reference to object into which to place the value
     *  @param aAttribute, the attribute whose value is queried
     *  @return EFalse if unable to get value - aValue is unchanged. 
     *         ETrue if value successfully put in aValue.
     */
    template<typename T> 
    inline TBool GetValueTObject(T& aValue, const TMPXAttribute& aAttribute) const;

        
    /**
     *  The value for a specific attribute 
     *
     *  @param aAttribute, the attribute whose value is queried
     *  @return value of the attribute
     */
    IMPORT_C const TDesC& ValueText(const TMPXAttribute& aAttribute) const;

    /**
     *  Get the text for a specific attribute, indicating success 
     *
     *  @param aText, reference to object into which to place a pointer to the text
     *  @param aAttribute, the attribute whose value is queried
     *  @return EFalse if unable to get text - aText is unchanged. 
     *         ETrue if pointer to text successfully put in aText.
     */
    IMPORT_C TBool GetValueText(TPtrC& aText, 
                                const TMPXAttribute& aAttribute) const;

   
    /**
     *  The value for a specific attribute 
     *
     *  @param aAttribute, the attribute whose value is queried
     *  @return value of the attribute
     */
    IMPORT_C const CBase* ValueCObject(const TMPXAttribute& aAttribute) const;
    
    /**
     * Clears all the attributes and values
     */
    IMPORT_C void Reset();

    /**
     * Add a new attribute value to this object, or modifies existing
     * value if already present 
     *
     *  @param aAttribute the attribute whose value is added/modified
     *  @return value of the attribute
     */    
    IMPORT_C void SetTextValueL(const TMPXAttribute& aAttribute,
                                const TDesC& aValue);
      
    /**
     * Add a new attribute value to this object, or modifies existing
     * value if already present 
     *
     *  @param aAttribute the attribute whose value is added/modified
     *  @return value of the attribute
     */
    template<typename T> 
    inline void SetTObjectValueL(const TMPXAttribute& aAttribute, T aValue);
    
    /**
     * Add a new attribute value to this object, or modifies existing
     * value if already present 
     *
     *  @param aAttribute the attribute whose value is added/modified
     *					  Takes ownership, unless leaves.
     *  @return value of the attribute
     */
    IMPORT_C void SetCObjectValueL(const TMPXAttribute& aAttribute, CBase* aValue_TakesOwnership);    

    /**
     * Delete an attribute of the media item
     * @param aAttribute The attribute that is deleted
     */
    IMPORT_C void DeleteAttribute(const TMPXAttribute& aAttribute);
    
    /**
     * Delete an attribute of the media item
     * @param aAttributeIndex index of the attribute to delete
     */
    void DeleteAttribute( TInt aAttributeIndex );

	/**
	 * Handles modification of item
	 *
	 * @param aAttributes Attributes that have been modified
	 */
	IMPORT_C void HandleModified(const RArray<TMPXAttribute>& aAttributes);

    /**
     * Add a new attribute value to this object, or modifies existing
     * value if already present 
     *
     *  @param aAttribute the attribute whose value is added/modified
     *  @param aValue value of the attribute
     *  @param aType type of the attribute
     */
    IMPORT_C void SetValueL(const TMPXAttribute& aAttribute, TAny* aValue, TMPXAttributeType aType);

public: // Media-list-manager-internal management interface
	/**
	 * Reserve space for users to be added, so AddUser can be called with 
	 * danger of failure
	 * This should not be exported
	 * @param aReservationCount space is allocated for this number of users
	 */
	void ReserveUsersL( TInt aReservationCount );

	/**
	 * Add the list as a user of the media object
     * The index into list is also stored, to allow clients that
     * are processing users of a media, to index directly into the list
	 * This should not be exported
	 * Reservation must have been made before calling this
	 * @param aUser the object to be added as a user
     * @param aIndex the index into the media list user
	 */
	void AddUser( MGlxMediaUser& aUser, TInt aIndex );  

	/**
	 * Remove the list as a user list of the media object
	 * This should not be exported
	 * @param aUser the user to be removed 
	 */
	void RemoveUser( const MGlxMediaUser& aUser );  
	
	/**
	 * Determines if the list is a user
	 * This should not be exported
	 * @return ETrue if the object is a user of this media object
	 */
	TBool IsUser( const MGlxMediaUser& aUser ) const;  
	
	/**
	 * Count of user lists
	 * This should not be exported
	 * @return The number of users of this media object
	 */
	TInt UserCount() const;  
	
	/**
	 * User by index
	 * This should not be exported
	 * @param aIndex index of the user
	 * @return User by index
	 */
	MGlxMediaUser& User(TInt aIndex) const;  

    /**
     * Index in the media list user
     * This should not be exported
     * @param aIndex index of the user
     * @return Index in the media list user
     */
    TInt IndexInUser(TInt aIndex) const;

    /**
     * Update index in the media list user
     * This should not be exported
     * @param aUser the user to be updated
     * @param aIndex the index into the media list user
     */
    void UpdateUser( MGlxMediaUser& aUser, TInt aIndex );
    
    /**
     * Returns the TMPXAttributeType corresponding to TMPXAttribute
     * @param aAttribute the attribute whose type is required
     */
    TMPXAttributeType AttributeTypeId(const TMPXAttribute& aAttribute) const;

private:
	struct TValue
		{
		inline TValue(TAny* aValue, TMPXAttributeType aType);
		TAny* iValue;
		TMPXAttributeType iType;
		};
	
	/**
	 * Deletes a value in the right way, depending on its type
	 */
	void Delete(TValue& aValue);
		
private:
    struct TMediaUser
        {
        /**
         * Find match by MGlxMediaUser
         */
        static TBool MatchUser(const TMediaUser& aUser1, 
                               const TMediaUser& aUser2);

        /**
         * Constructor
         */
        TMediaUser(MGlxMediaUser* aMediaUser, TInt aIndex = KErrNotFound);

        MGlxMediaUser* iMediaUser;
        TInt iIndex;
        };
		
private:
    RArray<TValue> iValues; // Array index corresponds to iAttributes
    RArray<TMPXAttribute> iAttributes; // Array index corresponds to iValues
	
	/**
	 * Media item id 
	 */
	TGlxMediaId iId; // TEMPORARY, merge to attribute array

    /**
     * Objects by which this object is used by. When the array is empty,
     * the item does not have any users (and can be deleted).
     * Only exposed to CGlxMediaListManager
     */
    RArray< TMediaUser > iUsers; // TEMPORARY, merge to attribute array
    
    /// Number of users currently reserved on iUsers
    __DEBUG_ONLY( TInt _iUserReservationCount; )
        
    __DECLARE_TEST;
	};

#include "glxmedia.inl"

#endif // C_GLXMEDIA_H
