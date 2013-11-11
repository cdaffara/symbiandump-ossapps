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
* Description:   Command request
*
*/



#ifndef GLXCOMMANDPARSER_H_
#define GLXCOMMANDPARSER_H_

#include <glxmediaid.h>
#include <glxmediageneraldefs.h>
#include <lbsposition.h>
#include <mpxcommand.h>

/**
 * MGlxCommandParserCallback command handling interface.
 */
NONSHARABLE_CLASS (MGlxCommandParserCallback)
    {
public:	      
    
    /**
     * Add container.
     * @param aContainerName New container name.
     */
    virtual void AddContainerL(const TDesC& aContainerName) = 0;
    
    /**
     * Add items to container by id.
     * @param aSourceIds Items to be added.
     * @param aTargetContainers Containers to add items to.
     */
    virtual void AddToContainerL(const RArray<TGlxMediaId>& aSourceIds, const RArray<TGlxMediaId>& aTargetContainers) = 0;

    /**
     * Add item to containers by URI.
     * @param aSourceUri URI of item to add to the container.
     * @param aTargetContainers Containers to add items to.
     */      
    virtual void AddToContainerL(const TDesC& aSourceUri, const RArray<TGlxMediaId>& aTargetContainers) = 0;    
        
    /**
     * Copy files to another drive.
     * @param aSourceIds Items to be copied.
     * @param aDrive Drive to copy items to.
     */
    virtual void CopyL(const RArray<TGlxMediaId>& aSourceIds, const TDesC& aDrive) = 0;
    
    /**
     * Move files to another drive.
     * @param aSourceIds Items to be moved.
     * @param aDrive Drive to move items to.
     */
    virtual void MoveL(const RArray<TGlxMediaId>& aSourceIds, const TDesC& aDrive) = 0;
    
    /**
     * Remove items from a container.
     * @param aItemIds Items to be removed.
     * @aContainerId Container id.
     */
    virtual void RemoveFromContainerL(const RArray<TGlxMediaId>& aItemIds, const TGlxMediaId& aContainerId) = 0;
    
    /**
     * Delete files, Remove container.
     * @param aItemIds Items to be removed.
     */
    virtual void DeleteL(const RArray<TGlxMediaId>& aItemIds) = 0;
    
    /**
     * Rename files, Rename container.
     * @param aSourceItemId Item or container to be renamed.
     * @param aTitle New name (Without file extension for files).
     */
    virtual void RenameL(const TGlxMediaId& aSourceItemId, const TDesC& aTitle) = 0;
    
    /**
     * Set description.
     * @param aItemIds Items to apply the new description to.
     * @param aDescription The new description.
     */
    virtual void SetDescriptionL(const RArray<TGlxMediaId>& aItemIds, const TDesC& aDescription) = 0;
    
   
    /**
     * Set capature location
     * @param aItemIds Items to apply the capture location to.
     * @param aCoordinate The capture location.
     */
    virtual void SetCaptureLocationL(const RArray<TGlxMediaId>& aItemIds, const TCoordinate& aCoordinate) = 0;
    
    /**
     * Clean up thumbnails.
     */
    virtual void ThumbnailCleanupL() = 0;
    };
    
/**
 * TGlxCommandParser Parses CMPXCommand objects.
 * 
 */
NONSHARABLE_CLASS (TGlxCommandParser)
    {
public:
    /**
     * Parse a command.
     * This method will leave with KErrAttribute if aCommand is not valid.
     * @param aCommandHandler A MGlxCommandParserCallback derived object that will be synchronously called back to handle the command.
     * @param aCommand Command to be parsed.
     */
    IMPORT_C static void ParseL(MGlxCommandParserCallback& aCommandHandler, const CMPXCommand& aCommand);
    
    /**
     * Get the Ids of a command request.
     * @param aCollectionUid Contains the collection UID on completion of the call.
     * @param aSessionId Contains the session id on completion of the call.
     * @param aCommand Command to be parsed.
     * @return ETrue if the command has a session ID else EFalse.
     */
    IMPORT_C static TBool IdsL(TUid& aCollectionUid, TUint32& aSessionId, const CMPXCommand& aCommand);
    
private:
/// @todo minor: add method and parameter descriptions  
	/**
	 * Get an array of TGlxMediaIds from a CMPXCommand object.
	 * @param aAttribute required attribute (attribute must be of type CMPXCollectionPath)
	 * @param aIdArray on return contains a list of Ids
	 * @param aCommand command object that aAttribute will be requested from.
	 */
    static void ArrayValueL(const TMPXAttribute &aAttribute, RArray<TGlxMediaId>& aIdArray, const CMPXCommand& aCommand);
/// @todo minor: add method and parameter descriptions    
    /**
     * Get an attribute of arbitary type from CMPXCommand object. 
     * @param aAttribute required attribute.
	 * @param aCommand command object that aAttribute will be requested from.
	 * This method will leave with KErrArgument if the requested attribute is not found.
     */
    template<typename T> static T ValueL(const TMPXAttribute & aAttribute, const CMPXCommand& aCommand);
/// @todo minor: add method and parameter descriptions    
	/**
	 * Get a descriptor from CMPXCommand object. 
	 * @param aAttribute required attribute.
	 * @param aCommand command object that aAttribute will be requested from.
	 * @return Requested attribute.
	 * This method will leave with KErrArgument if the requested attribute is not found.
	 */
	static const TDesC& DescriptorValueL(const TMPXAttribute & aAttribute, const CMPXCommand& aCommand);
    };

#endif // GLXCOMMANDPARSER_H_
