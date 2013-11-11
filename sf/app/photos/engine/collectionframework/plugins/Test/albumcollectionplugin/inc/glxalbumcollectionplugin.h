/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Album collection plugin definition
*
*/



#ifndef C_GLXALBUMCOLLECTIONPLUGIN_H
#define C_GLXALBUMCOLLECTIONPLUGIN_H

// INCLUDES

#include <e32cmn.h>
#include "glxcollectionplugintestbase.h"
#include "mglxtnthumbnailcreatorclient.h"
#include <mpxmediageneraldefs.h>
#include <glxmediaid.h>

// FORWARD DECLARATIONS

class CMPXMedia;
class CGlxtnThumbnailCreator;
class CGlxAcpProgressNotifier;

// CONSTANTS

// CLASS DECLARATION

/**
* Album collection plugin 
*/
NONSHARABLE_CLASS(CGlxAlbumCollectionPlugin) : public CGlxCollectionPluginTestBase,
		public MGlxtnThumbnailCreatorClient
    {
public: // Constructors and destructor
    /**
    * Two-phased constructor
    *
    * @param aObs observer
    * @return object of constructed
    */
    static CGlxAlbumCollectionPlugin* NewL(TAny* aObs);

    /**
    * Destructor
    */
    ~CGlxAlbumCollectionPlugin();
    
private: // Functions from base classes
// from CMPXCollectionPlugin
    /** 
    * DEPRECATED, Executes a command on the selected collection
    *
    * @param aCmd a command
    * @param aArg optional argument
    */
    virtual void CommandL(TMPXCollectionCommand aCmd, TInt aArg = 0);

    /** 
    * Executes a command
    *
    * @param aCmd a command
    */
    virtual void CommandL(CMPXCommand& aCmd); 
    /** 
    * Navigates to the given path
    *
    * @param aPath a path
    * @param aAttrs, attributes requested
    * @param aFilter, filter to apply or NULL if none
    */
    virtual void OpenL(const CMPXCollectionPath& aPath,
                   const TArray<TMPXAttribute>& aAttrs,
                   CMPXFilter* aFilter); 
    /** 
    *  Media properties of the current file (async)
    *  Note: if selection is set in aPath, HandleMedia will return an array of 
    *        media properties of current selected items.
    *        if no selection is set in aPath, HandleMedia will return media 
    *        properities of current item.
    *
    * @param aPath path for the media file
    * @param aAttr attributes requested
    * @param aCaps platsec capabilities of client requesting media; plug-in should also
    *        verify its process capabilities
    * @aParam aSpecs, specifications for attributes
    */
    virtual void MediaL(const CMPXCollectionPath& aPath, 
                        const TArray<TMPXAttribute>& aAttrs,
                        const TArray<TCapability>& aCaps,
                        CMPXAttributeSpecs* aSpecs);
    /**
    *  Adds an item or items to the collection
    *
    *  @param aNewProperties, Properties of the item
    */
    virtual void AddL(const CMPXMedia& aNewMedia);
    
    /**
    * Remove a collection path
    * Note that the selection indicies are hidden within the path
    * @param aPath, path to remove
    *
    */
    virtual void RemoveL(const CMPXCollectionPath& aPath );
    
    /**
    *  Remove an item or items from the collection
    *
    *  @param aProperties, Properties of the item. It may cantain URI only
    *                      or meta data, all of items matched properties 
    *                      will be removed.
    */
    virtual void RemoveL(const CMPXMedia& aMedia);
    /**
    *  Sets/updates the media for the item
    *  specified in the path
    *
    *  @param aMedia, new value
    */
    virtual void SetL(const CMPXMedia& aMedia);

    /**
    *  Find a list of items matched (async)
    *
    *  @param aCriteria, properties to be searched
    *  @param aAttrs, attributes to return
    */
    virtual void FindAllL(const CMPXSearchCriteria& aCriteria, 
                          const TArray<TMPXAttribute>& aAttrs);
    
    /**
    * Find a list of items matched (sync)
    *
    *  @param aCriteria, properties to be searched
    *  @param aAttrs, attributes to return
    *  @return results of the search        
    */
    virtual CMPXMedia* FindAllSyncL(const CMPXSearchCriteria& aCriteria,
                                    const TArray<TMPXAttribute>& aAttrs);
                                    
    /**
    * Get the list of supported capabilities
    * @return TCollectionCapability, bitmask of supported capabilities
    */
    virtual TCollectionCapability GetCapabilities();

    /**
    * Generate the next available title from the given title (async).
    * Generated title is returned via callback HandleGenerateTitle
    *
    * @param aCategory specifies the category for the title
    * @param aBaseTitle specifies the base title for new title generation
    */
    virtual void GenerateTitleL( TMPXGeneralCategory aCategory,
                                 const TDesC& aBaseTitle );
                                 
    /**
    * Generate the next available title from the given title (sync).
    *
    * @param aCategory specifies the category for the title
    * @param aBaseTitle specifies the base title for new title generation
    * @return generated title
    */
    virtual HBufC* GenerateTitleSyncL( TMPXGeneralCategory aCategory,
                                       const TDesC& aBaseTitle );

private:

	virtual void HandleThumbnailRequestCompleteL(const TGlxMediaId& iId,
		TGlxThumbnailQuality aQuality, TInt aErrorCode);

private:    // From MGlxtnThumbnailCreatorClient
    void ThumbnailFetchComplete(const TGlxMediaId& aItemId,
                            TGlxThumbnailQuality aQuality, TInt aErrorCode);
    void ThumbnailDeletionComplete(const TGlxMediaId& aItemId, TInt aErrorCode);
    void FilterAvailableComplete( const RArray<TGlxMediaId>& aIdArray,
                                    TInt aErrorCode );
    void FetchFileInfoL(CGlxtnFileInfo* aInfo, const TGlxMediaId& aItemId,
                    TRequestStatus* aStatus);
    void CancelFetchUri(const TGlxMediaId& aItemId);
    MGlxtnThumbnailStorage* ThumbnailStorage();

private: 
    /**
    * Constructor
    * @param aObs MMPXCollectionPluginObserver instance
    */
    CGlxAlbumCollectionPlugin(MMPXCollectionPluginObserver* aObs);
    void ConstructL();
    
private:
	CGlxtnThumbnailCreator* iThumbnailCreator;

	struct CItem : public CBase
		{
		~CItem()
			{
			delete iFilename;
			delete iTitle;
			delete iDrive;
			}
		void SetFilenameL(const TDesC& aFilename)
			{
			HBufC* fn = aFilename.AllocL();
			delete iFilename;
			iFilename = fn;
			}
		void SetTitleL(const TDesC& aTitle)
			{
			HBufC* title = aTitle.AllocL();
			delete iTitle;
			iTitle = title;
			}
		void SetDriveL(const TDesC& aDrive)
			{
			HBufC* drive = aDrive.AllocL();
			delete iDrive;
			iDrive = drive;
			}

		TGlxMediaId iId;
		HBufC* iFilename; 
		HBufC* iTitle;
		TTime  iDateTime;
		TInt   iFileSize;
		HBufC*  iDrive; 
		};

	void AddItemL(const TDesC& aFileName, 
	              const TDesC& aTitle, 
	              RPointerArray<CItem>& aDatabase, 
	              TTime aDateTime = TTime(0), 
	              TInt  aFileSize = 0,
	              const TDesC& aDrive = KNullDesC);

	CItem* Item(const TGlxMediaId& aItemId) const;

    CItem* iSelf;
	RPointerArray<CItem> _iItemDBHackAlbums;    // TEMP
	RPointerArray<CItem> _iItemDBHackContent;   // TEMP
	RFs iFs;

    // Media to send back to client
    CMPXMedia* 			iMedia;
    CMPXMedia* 			iOpenEntries;

	// TEMP RW
	TInt iItemAddedId;	
	
	CGlxAcpProgressNotifier* iProgressNotifier;
	
    };

#endif  // C_GLXALBUMCOLLECTIONPLUGIN_H
