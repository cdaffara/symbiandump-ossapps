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
* Description:  Music derivative of the in-memory plugin
*
*/


#ifndef CMPXMUSICMEMORYPLUGIN_H
#define CMPXMUSICMEMORYPLUGIN_H

#include <apgcli.h>
#include "mpxinmemoryplugin.h"

// FORWARD DECLARATIONS
class CMPXMetadataExtractor;

/**
 *  Music Specific memory plugin
 *
 *  @lib mpx
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CMPXMusicMemoryPlugin ) : public CMPXInMemoryPlugin
    {
public:

    /**
    * Two-phased constructor
    *
    * @param aInitParams, initialization parameter
    * @return object of constructed
    */
    static CMPXMusicMemoryPlugin* NewL();

    /**
    * destructor
    */
    virtual ~CMPXMusicMemoryPlugin();

private:

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
    * Extract metadata for a given media object
    * metadata will be extracted if a URI exists
    * @param aUri, uri for the item
    * @param attrs, attributes requested
    * @return new CMPXMedia object or NULL if bad
    */
    CMPXMedia* ExtractMetadataL( const TDesC& aUri, 
                                 const TArray<TMPXAttribute>& aAttrs );
    
                                                                       
    /**
    * Set all the attributes in CMPXMedia corresponding to KMPXMediaIdDrm
    * @param aMedia, media to update
    * @param aDrmAttributes, bitwise or of the flags we want
    * @param aDrmMedia, aMedia with drm attributes
    */
    void DoSetMediaDrmL(CMPXMedia& aMedia, 
                        const TArray<TMPXAttribute>& aAttrs, 
                        const TDesC& aLocation );
private:

    /**
    * Constructor
    */
    CMPXMusicMemoryPlugin();

    /**
    * 2nd phased constructor
    */
    void ConstructL();

private: // data
    CMPXMetadataExtractor*  iMetadataExtractor; // Metadata extractor
    RFs                     iFs;                // File Session
    RApaLsSession           iAppArc;            // App arc, dummy
    RArray<TMPXAttribute>   iAttributes;        // List of attributes to fetch
    };

#endif // CMPXMUSICMEMORYPLUGIN_H
