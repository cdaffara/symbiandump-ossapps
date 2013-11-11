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
* Description:   Command object factory
*
*/



#ifndef T_GLXCOMMANDFACTORY_H
#define T_GLXCOMMANDFACTORY_H

#include <e32base.h>
#include <glxmediageneraldefs.h>
#include <glxmediaid.h>
#include <lbsposition.h> 
#include <mpxcommand.h>

class CMPXCollectionPath;
class MGlxMediaList;

/**
 * TGlxCommandFactory
 *
 * Factory that creates CMPXCommand objects that can be issued via 
 * CGlxMediaListCommandHandler::IssueCommandL
 *
 * @lib glxcommoncommandhandlers.lib
 */
 NONSHARABLE_CLASS (TGlxCommandFactory)
    {
public:
    /**
     * Creates a basic command, when a method to create a specific command is 
     * not available. Use of the more specific methods below instead of this method is encouranged.
     *
     * @param aCommandId Id of the MPX command 
     * @param aPath aSourcePath path for using as source items (i.e., selection information)
     *                          and collection plugin id
     *                          Will be included in the command object in
     *                          KMPXCommandGeneralSourceIds
     * @return a command object that has the following attributes filled in:
     *          KMPXCommandGeneralId
     *          KMPXCommandGeneralCollectionId
     *          KMPXCommandGeneralSourcePath (if requested)
     */
    IMPORT_C static CMPXCommand* BasicCommandLC(TInt aCommandId, const CMPXCollectionPath& aSourcePath);   

    /**
     * Creates a command object to add a new container (album or tag).
     * 
     * @param aTitle Title (name) of the new container.
     * @param aCollectionId Id of the collection (plugin) to which the new container
     *                      will be added.
     * @return a command object. 
     */
    IMPORT_C static CMPXCommand* AddContainerCommandLC(const TDesC& aTitle, TInt aCollectionUid);
    
    /**
     * Creates a command object to add items to a container.
     *                 Only items that are focused (using CMPXCollectionPath::Set())
     *                 or selected will be added.
     * @param aSourceItems Items to add to the container.
     * @param aTargetContainers Containers that items will be added to. 
     *                 Only containers that are focused (using CMPXCollectionPath::Set()) 
     *                 or selected will have the items added to them.
     * @return a command object. 
     */
    IMPORT_C static CMPXCommand* AddToContainerCommandLC(
        const CMPXCollectionPath& aSourceItems,
        const CMPXCollectionPath& aTargetContainers);
        
    /**
     * Creates a command object to add items to a container
     * @param aUri source item uri.
     * @param aTargetContainers Containers that item will be added to. 
     *                 Only containers that are focused (using CMPXCollectionPath::Set()) 
     *                 or selected will have the item added to them.
     * @return a command object. 
     */
    IMPORT_C static CMPXCommand* AddToContainerCommandLC(
            const TDesC& aUri, 
            const CMPXCollectionPath& aTargetContainers);

    /**
     * Creates a command object to move items to a different drive
     * @param aTargetDrive Drive to move files to, in format "C:\"
     * @param aSourcePath Path to be used for selection and collection plugin id.
     *                 Only items that are focused (using CMPXCollectionPath::Set())
     *                 or selected will be moved.
     * @return a command object.
     */
    IMPORT_C static CMPXCommand* MoveCommandLC(const TDesC& aTargetDrive, 
        const CMPXCollectionPath& aSourcePath);

    /**
     * Creates a command object to copy items to a different drive
     * @param aTargetDrive Drive to copy files to, in format "C:\".
     *                 Only items that are focused (using CMPXCollectionPath::Set())
     *                 or selected will be copied.
     * @param aSourcePath Path to be used for selection and collection plugin id.
     * @return a command object. 
     */
    IMPORT_C static CMPXCommand* CopyCommandLC(const TDesC& aTargetDrive, 
        const CMPXCollectionPath& aSourcePath);

    /**
     * Creates a command object for renaming a media object (container or file)
     * @param aNewTitle New title (or name) for the object.
     * @param aList Media list with the item that should be renamed in focus.
     *              Panics if more than one item has been selected on the media list.
     *              Panics if media list is empty.
     *              Panics if media list has a static item in focus.
     * @return a command object.
     */
    IMPORT_C static CMPXCommand* RenameCommandLC(const TDesC& aNewTitle, 
        const CMPXCollectionPath& aSourcePath);

    /**
     * Creates a command object for deleting a media object. If media 
     * object is a file, it will be deleted when the command is issued.
     * @param aSourcePath Path to be used for selection and collection plugin id
     *                 Only items that are focused (using CMPXCollectionPath::Set())
     *                 or selected will be deleted.
     * @return a command object.
     */
    IMPORT_C static CMPXCommand* DeleteCommandLC(const CMPXCollectionPath& aSourcePath);

    /**
     * Creates a command object to remove items from a container.
     * 
     * @param aSourcePath Path to be used for selection and collection plugin id.
     *                 Uses the parent item in the path as the container to remove from.
     *                 Only items that are focused (using CMPXCollectionPath::Set())
     *                 or selected will be removed.
     * @return a command object. 
     */
    IMPORT_C static CMPXCommand* RemoveFromContainerCommandLC(const CMPXCollectionPath& aSourcePath);

    /**
     * Creates a command object to remove items from a specific container.
     * 
     * @param aContainerId Id of the container to remove from.
     * @param aSourcePath Path to be used for selection and collection plugin id.
     *                 Only items that are focused (using CMPXCollectionPath::Set())
     *                 or selected will be removed.
     * @return a command object. 
     */
    IMPORT_C static CMPXCommand* RemoveFromContainerCommandLC(TGlxMediaId aContainerId, 
        const CMPXCollectionPath& aSourcePath);
    
    /**
     * Creates a command object to set the description of an object.
     * @param aDescription Description to set.
     * @param aSourcePath Path to be used for selection and collection plugin id.
     *                 Only items that are focused (using CMPXCollectionPath::Set())
     *                 or selected will have their descriptions set.
     * @return a command object.
     */
    IMPORT_C static CMPXCommand* SetDescriptionCommandLC(const TDesC& aDescription, 
        const CMPXCollectionPath& aSourcePath);

    /**
     * Creates a command object to delete the location information associated with an object.
     * @param aSourcePath Path to be used for selection and collection plugin id
     *                 Only items that are focused (using CMPXCollectionPath::Set())
     *                 or selected will have their location deleted.
     * @return a command object.
     */
    IMPORT_C static CMPXCommand* DeleteLocationCommandLC(const CMPXCollectionPath& aSourcePath);
    
    /**
     * Creates a command that cleans up thumbnails.
     * @todo Improve method description.
     * @return a command object.
     */
    IMPORT_C static CMPXCommand* ThumbnailCleanupCommandLC();
    
    };

#endif // T_GLXCOMMANDFACTORY_H
