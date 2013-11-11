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
* Description:  Simple plugin to cache media objects
*
*/



#ifndef CMPXINMEMORYPLUGIN_H
#define CMPXINMEMORYPLUGIN_H

// INCLUDES
#include <e32cmn.h>
#include <mpxcollectionplugin.h>
#include <mpxcollectionmessagedefs.h>

// FORWARD DECLARATIONS
class CMPXMedia;
class CMPXMediaArray;
class CMPXDrmMediaUtility;

// CONSTANTS

// CLASS DECLARATION

/**
*  CMPXInMemoryPlugin class 
*
*  Plug-in basically provides temporary in memory browsing
*  @lib mpxinmemoryplugin.lib
*/
NONSHARABLE_CLASS(CMPXInMemoryPlugin) : public CMPXCollectionPlugin
    {
public: // Constructors and destructor

    
    /**
    * Two-phased constructor
    *
    * @param aInitParams, initialization parameter
    * @return object of constructed
    */
    static CMPXInMemoryPlugin* NewL();

    /**
    * Destructor
    */
    virtual ~CMPXInMemoryPlugin();
    
protected: // Functions from base classes
    /**
    *  from CMPXCollectionPlugin
    */

    /** 
    * From CMPXCollectionPlugin
    * Navigates to the given path
    * @param aPath: a path
    * @param aAttrs, attributes requested
    * @param aFilter, filter to apply or NULL if none
    */
    void OpenL(const CMPXCollectionPath& aPath,
               const TArray<TMPXAttribute>& aAttrs,
               CMPXFilter* aFilter); 
    
    /**
    * From CMPXCollectionPlugin     
    * Get the extended properties of the current file (async)
    * @param aPath: a path   
    * @param aAttrs: array of attributes requested             
    * @param aCaps platsec capabilities of client requesting media; plug-in should also
    *        verify its process capabilities
    * @aParam aSpecs, specifications for attributes
    */
    void MediaL(const CMPXCollectionPath& aPath, 
                const TArray<TMPXAttribute>& aAttrs,
                const TArray<TCapability>& aCaps,
                CMPXAttributeSpecs* aSpecs);
            
    /** 
    * Cancel outstanding request
    */
    void CancelRequest();

    /** 
    * Executes a command on the selected collection
    *
    * @param aCmd a command
    */
    void CommandL(TMPXCollectionCommand aCmd, TInt aArg);

    /**
    * From CMPXCollectionPlugin
    */
    void CommandL(CMPXCommand& aCmd);
    
    /**
    *  Adds item(s) to the collection
    *
    *  @param aNewProperties, Properties of the item
    */
    void AddL(const CMPXMedia& aNewMedia);
    
    /**
    * Remove a collection path
    * @param aPath, path to remove
    *
    */
    void RemoveL(const CMPXCollectionPath& aPath );
    
    /**
    *  Remove an item or items from the collection
    *
    *  @param aProperties, Properties of the item. It may cantain URI only
    *                      or meta data, all of items matched properties 
    *                      will be removed.
    */
    void RemoveL(const CMPXMedia& aMedia);
    
    /**
    *  Sets/updates the media for the item
    *  specified in the path
    *
    *  @param aMedia, new value
    */
    void SetL(const CMPXMedia& aMedia);
    
    /**
    *  Find a list of items matched 
    *
    *  @param aMedia, properties to be searched
    *  @param aAttrs, attributes to return from find
    */
    void FindAllL(const CMPXMedia& aMedia, const TArray<TMPXAttribute>& aAttrs);

    /**
    * Find a list of items matched (sync)
    *
    *  @param aMedia, properties to be searched
    *  @param aAttrs, attributes to return
    *  @return results of the search        
    */
    CMPXMedia* FindAllSyncL(const CMPXMedia& aMedia,
                            const TArray<TMPXAttribute>& aAttrs);
                            
    /**
    * Get the list of supported capabilities
    *
    * @return TCollectionCapability, bitmask of supported capabilities
    */
    TCollectionCapability GetCapabilities();
                        
protected: // New Functions
    
    /**
    * Send change events back to the observer
    * @param aId, embedded context that was changed
    * @param aChange, change type
    */
    void HandleChangeL(const TMPXItemId& aId, TMPXChangeEventType aChange );

    /**
    * Internal function to Add a Media
    * @param aMedia media to add
    */
    void DoAddL( const CMPXMedia& aMedia );
    
    /**
    * Internal function to Set a Media
    * @param aMedia media to set
    */
    void DoSetL( const CMPXMedia& aMedia );
        
protected: 
    /**
    * Constructor
    */
    CMPXInMemoryPlugin();
    
    /**
    *  2nd phase constructor
    */
    void ConstructL();
    
protected:
    CMPXDrmMediaUtility*     iDrmMediaUtility;
    RPointerArray<CMPXMedia> iTemporaryData;
    RArray<TInt>             iEmbeddedContext;
    };

#endif      // CMPXINMEMORYPLUGIN_H

