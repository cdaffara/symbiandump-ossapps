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
* Description:   Photos / Common
*
*/



#include "glxcommandfactory.h"

#include <glxcommandgeneraldefs.h>
#include <glxmediageneraldefs.h>
#include <mpxcollectionpath.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmediageneraldefs.h>

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::BasicCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::BasicCommandLC(TInt aCommandId, 
        const CMPXCollectionPath& aPath) 
    {
    CMPXCommand* command = CMPXCommand::NewL();
    CleanupStack::PushL(command);

    // Set command id
    command->SetTObjectValueL<TInt>(KMPXCommandGeneralId, aCommandId);    
    
    // Add collection plugin id to command object
    if (aPath.Levels() > 0) 
        {
        command->SetTObjectValueL<TUid>(KMPXCommandGeneralCollectionId, TUid::Uid(aPath.Id(0)));        
        }
    // Add path to command object 
    command->SetCObjectValueL<CMPXCollectionPath>(KMPXCommandGeneralSourceIds, &const_cast<CMPXCollectionPath&>(aPath));
        
    return command;
    }

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::AddContainerCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::AddContainerCommandLC(const TDesC& aTitle, 
        TInt aCollectionUid)
    {
    CMPXCommand* command = CMPXCommand::NewL();
    CleanupStack::PushL(command);
    
    // Add collection plugin id to command object
    command->SetTObjectValueL<TUid>(KMPXCommandGeneralCollectionId, TUid::Uid(aCollectionUid));        
    
    // Set command id
    command->SetTObjectValueL<TInt>(KMPXCommandGeneralId, KGlxCommandIdAdd);    

    // Set title of new object
    command->SetTextValueL(KMPXMediaGeneralTitle, aTitle);        
    
    return command;
    }

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::AddToContainerCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::AddToContainerCommandLC(
        const CMPXCollectionPath& aSourceItems,
        const CMPXCollectionPath& aTargetContainers) 
    {
    CMPXCommand* command = BasicCommandLC(KGlxCommandIdAdd, aSourceItems);
        
    // Add path of selected items
    command->SetCObjectValueL<CMPXCollectionPath>(KMPXMediaGeneralContainerId, &const_cast<CMPXCollectionPath&>(aTargetContainers));    

    return command;      
    }

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::AddToContainerCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::AddToContainerCommandLC(
        const TDesC& aUri, 
        const CMPXCollectionPath& aTargetContainers)
    {
    
    CMPXCommand* command = CMPXCommand::NewL();
    CleanupStack::PushL(command);
    
    // Set command id
    command->SetTObjectValueL<TInt>(KMPXCommandGeneralId, KGlxCommandIdAdd);  
    
    // Set URI
    command->SetTextValueL(KMPXCommandGeneralSourceUri, aUri); 
    
    // Add path of selected items
    command->SetCObjectValueL<CMPXCollectionPath>(KMPXMediaGeneralContainerId, &const_cast<CMPXCollectionPath&>(aTargetContainers));    

    // Add collection plugin id to command object
/// @todo minor: Add comment as to the meaning of the value zero in Comparison and aTargetContainers.Id(0)
    if (aTargetContainers.Levels() > 0) 
        {
        command->SetTObjectValueL<TUid>(KMPXCommandGeneralCollectionId, TUid::Uid(aTargetContainers.Id(0)));        
        }
    
    return command;      
    }
        
        
/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::MoveCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::MoveCommandLC(const TDesC& aTargetDrive, 
    const CMPXCollectionPath& aSourcePath)
    {
    CMPXCommand* command = BasicCommandLC(KGlxCommandIdSet, aSourcePath);
    
    // Set target drive                  
    command->SetTextValueL(KMPXMediaGeneralDrive, aTargetDrive);    
    
    return command;
    }
    
/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::CopyCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::CopyCommandLC(const TDesC& aTargetDrive, 
    const CMPXCollectionPath& aSourcePath)
    {
    CMPXCommand* command = BasicCommandLC(KGlxCommandIdAdd, aSourcePath);
    
    // Set target drive
    command->SetTextValueL(KMPXMediaGeneralDrive, aTargetDrive);    
    
    return command;
    }
    
/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::RenameCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::RenameCommandLC(const TDesC& aNewTitle, 
    const CMPXCollectionPath& aSourcePath)
    {
    CMPXCommand* command = BasicCommandLC(KGlxCommandIdSet, aSourcePath);
    
    // Set title
    command->SetTextValueL(KMPXMediaGeneralTitle, aNewTitle);    
   
    return command;
    }
    
/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::DeleteCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::DeleteCommandLC(const CMPXCollectionPath& aSourcePath)
    {
    CMPXCommand* command = BasicCommandLC(KGlxCommandIdRemove, aSourcePath);

    // Set container as "device"
    command->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralContainerId, KGlxContainerDevice);    
    
    return command;
    }
    
/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::RemoveFromContainerCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::RemoveFromContainerCommandLC(
        const CMPXCollectionPath& aSourcePath)
    {
	const TInt KMinLevels = 2;
	
	CMPXCommand* command = BasicCommandLC(KGlxCommandIdRemove, aSourcePath);
	// The first level should be the UID of the collection plugin.
	// The second level should be an id of a container (tag or album).
    ASSERT(aSourcePath.Levels() > KMinLevels); 
    // Set container to remove from as container of items in path
    command->SetTObjectValueL<TMPXItemId>(
        KMPXMediaGeneralContainerId, 
        aSourcePath.Id(aSourcePath.Levels() - KMinLevels)); // The index of the level below the current level. (Levels() returns a count not an index)    
    
    return command;
    }
    
/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::RemoveFromContainerCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::RemoveFromContainerCommandLC(TGlxMediaId aContainerId, 
    const CMPXCollectionPath& aSourcePath)
    {
    CMPXCommand* command = BasicCommandLC(KGlxCommandIdRemove, aSourcePath);
    
    command->SetTObjectValueL<TMPXItemId>(
        KMPXMediaGeneralContainerId, aContainerId.Value());    
    
    return command;
    }
        
/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandFactory::SetDescriptionCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::SetDescriptionCommandLC(const TDesC& aDescription, 
        const CMPXCollectionPath& aSourcePath) 
    {
    CMPXCommand* command = BasicCommandLC(KGlxCommandIdSet, aSourcePath);
    
    // Set description
    command->SetTextValueL(KMPXMediaGeneralComment, aDescription);    
    
    return command;
    }


// -----------------------------------------------------------------------------
// TGlxCommandFactory::DeleteLocationCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::DeleteLocationCommandLC( 
		const CMPXCollectionPath& aSourcePath)
    {
    CMPXCommand* command = BasicCommandLC(KGlxCommandIdSet, aSourcePath);
    
    // Set command id
    command->SetTObjectValueL<const TCoordinate>(KGlxMediaGeneralLocation, TCoordinate());    
    
    return command;
    }

// -----------------------------------------------------------------------------
// TGlxCommandFactory::ThumbnailCleanupCommandLC
// -----------------------------------------------------------------------------
//	
EXPORT_C CMPXCommand* TGlxCommandFactory::ThumbnailCleanupCommandLC()
	{
    CMPXCommand* command = CMPXCommand::NewL();
    CleanupStack::PushL(command);
	
	// Set command id
    command->SetTObjectValueL<TInt>(KMPXCommandGeneralId, KGlxCommandThumbnailCleanup);  
	
    return command;
	}

