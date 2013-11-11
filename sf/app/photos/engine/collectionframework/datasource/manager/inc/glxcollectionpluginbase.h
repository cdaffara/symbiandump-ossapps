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
* Description:   This is the base class for Gallery Collection Plugins
*
*/




#ifndef C_GLXCOLLECTIONPLUGINBASE_H
#define C_GLXCOLLECTIONPLUGINBASE_H

// INCLUDES
#include <e32cmn.h>
#include <mpxcollectionplugin.h>
#include <glxcollectiongeneraldefs.h>
#include <glxfilterproperties.h>
#include <glxmediaid.h>

#include "glxrequest.h"
#include "mglxdatasourcerequestobserver.h"
#include "mglxdatasourceupdateobserver.h"

// FORWARD DECLARATIONS
class MGlxDataSource;
class CGlxStringCache;

// CONSTANTS
const TInt KGlxCollectionRootLevel = 1;
const TUid KGlxDefaultDataSourceUid = {0x20000A10}; // KGlxDataSourceMdeImplementationUid

// CLASS DECLARATION

/**
 *	CGlxCollectionPluginBase class 
 *
 *	Plug-in base.
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class CGlxCollectionPluginBase : public CMPXCollectionPlugin,
		public MGlxDataSourceRequestObserver,
		public MGlxDataSourceUpdateObserver
    {
public: // Constructors and destructor

    /**
    * Constructor
    */
    IMPORT_C CGlxCollectionPluginBase();

    /**
    * Destructor
    */
    IMPORT_C virtual ~CGlxCollectionPluginBase();
    
private: // Functions from base classes
// from CMPXCollectionPlugin
    /** 
    * DEPRECATED, Executes a command on the selected collection
    *
    * @param aCmd a command
    * @param aArg optional argument
    */
    IMPORT_C void CommandL(TMPXCollectionCommand aCmd, TInt aArg = 0);

    /** 
    * Executes a command
    *
    * @param aCmd a command
    */
    IMPORT_C void CommandL(CMPXCommand& aCmd); 
    /** 
    * Navigates to the given path
    *
    * @param aPath a path
    * @param aAttrs, attributes requested
    * @param aFilter, filter to apply or NULL if none
    */
    IMPORT_C void OpenL(const CMPXCollectionPath& aPath,
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
    IMPORT_C void MediaL(const CMPXCollectionPath& aPath, 
                        const TArray<TMPXAttribute>& aAttrs,
                        const TArray<TCapability>& aCaps,
                        CMPXAttributeSpecs* aSpecs);
    /** 
    * Cancel outstanding request
    */
    IMPORT_C void CancelRequest();
    /**
    *  Adds an item or items to the collection
    *
    *  @param aNewProperties, Properties of the item
    */
    IMPORT_C void AddL(const CMPXMedia& aNewMedia);
    
    /**
    * Remove a collection path
    * Note that the selection indicies are hidden within the path
    * @param aPath, path to remove
    *
    */
    IMPORT_C void RemoveL(const CMPXCollectionPath& aPath );
    
    /**
    *  Remove an item or items from the collection
    *
    *  @param aProperties, Properties of the item. It may cantain URI only
    *                      or meta data, all of items matched properties 
    *                      will be removed.
    */
    IMPORT_C void RemoveL(const CMPXMedia& aMedia);
    /**
    *  Sets/updates the media for the item
    *  specified in the path
    *
    *  @param aMedia, new value
    */
    IMPORT_C void SetL(const CMPXMedia& aMedia);

    /**
    *  Find a list of items matched (async)
    *
    *  @param aCriteria, properties to be searched
    *  @param aAttrs, attributes to return
    */
    IMPORT_C void FindAllL(const CMPXSearchCriteria& aCriteria, 
                          const TArray<TMPXAttribute>& aAttrs);
    
    /**
    * Find a list of items matched (sync)
    *
    *  @param aCriteria, properties to be searched
    *  @param aAttrs, attributes to return
    *  @return results of the search        
    */
    IMPORT_C CMPXMedia* FindAllSyncL(const CMPXSearchCriteria& aCriteria,
                                    const TArray<TMPXAttribute>& aAttrs);
                                    
    /**
    * Get the list of supported capabilities
    * @return TCollectionCapability, bitmask of supported capabilities
    */
    IMPORT_C TCollectionCapability GetCapabilities();

private:
// from MGlxDataSourceRequestObserver	
	/**
	 * Asynchronous call to retrieve 
	 * 
	 * @param aListId list for this query relates to
	 * @return error code
	 */
	IMPORT_C virtual void HandleResponse(CMPXMedia* aResponse, CGlxRequest* aRequest, const TInt& aError);
		
private:
// from MGlxDataSourceUpdateObserver
	/**
	 * Notification that new items have been found in the data source (file 
	 * system, etc.)
	 *
	 * @param aItems ids of the items that were been found. 
	 */
	 IMPORT_C virtual void HandleMessage(CMPXMessage& aMessage);
	
protected:
	CGlxRequest* GetRequestFactoryL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs);

	/**
	 * Checks to see if any additional attributes are required to construct required
	 * CPI specific attribute
	 *
	 * @param aCpiAttribute CPI specific attribute to be constructed later
	 * @param aAttributeArray modifiable list of attributes to be retrieved from data source
	 */	 
	IMPORT_C virtual void CpiAttributeAdditionalAttributes(const TMPXAttribute& aCpiAttribute, RArray<TMPXAttribute>& aAttributeArray);     	

	/**
	 * Modifies the response to include all requested CPI specific attributes or Leaves.
	 *
	 * @param aResponse Data Source constructed response to which CPI specific attributes should be added (also contains any required data)
	 * @param aCpiAttributes list of CPI specific attributes to be constructed
	 * @param aMediaIds list of CPI specific media Ids
	 */	 
	IMPORT_C virtual void HandleCpiAttributeResponseL(CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> aMediaIds);
	
	IMPORT_C virtual TBool IsUpdateMessageIgnored(CMPXMessage& aMessage);

	/**
	 * Loads a string from a resurce file
	 *
	 * @param aResourceFile - name of the resource file (does not contain the path).
	 * @param aResourceId - Id of the string to be loaded
	 * @return HBufC* containing the required string.
	 */	 
    IMPORT_C HBufC* LoadLocalizedStringLC(const TDesC& aResourceFile, const TInt aResourceId);
    
	/**
	 * initialize the TextResolver for the symbian code
	 */
    IMPORT_C void initializeTextResolverSymbian();
    
    /**
     * returns localised string.
     * @param aStringName - name of the string that need to be localised.
     * @return HBufC* containing the required string.
     */
    IMPORT_C HBufC* LoadLocalizedStringLC(const TDesC& aStringName);
    
    IMPORT_C virtual TGlxFilterProperties DefaultFilter(TInt aLevel);

protected: 
	/**
	 * Datasouce (owned)
	 */
 	MGlxDataSource* iDataSource;
 	
 	/**
 	 * collection plugin path (owned)
 	 */
 	CMPXCollectionPath* iPath;

private:
    /**
 	 * String Cache
 	 */ 	    
    CGlxStringCache*    iStringCache;
    };

#endif      // C_GLXCOLLECTIONPLUGINBASE_H

