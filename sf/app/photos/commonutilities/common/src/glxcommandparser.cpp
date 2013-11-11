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




#include "glxcommandparser.h"

#include <glxcommandgeneraldefs.h>
#include <glxmediageneraldefs.h>
#include <mpxcollectionpath.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmediageneraldefs.h>

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandParser::ParseL
// -----------------------------------------------------------------------------
//	
EXPORT_C void TGlxCommandParser::ParseL(MGlxCommandParserCallback& aCommandHandler, const CMPXCommand& aCommand)
    {
    TUint32 commandId = ValueL<TUint32>(KMPXCommandGeneralId, aCommand);

    RArray<TGlxMediaId> sourceIdArray;
    CleanupClosePushL(sourceIdArray);
  
    RArray<TGlxMediaId> targetIdArray;
    CleanupClosePushL(targetIdArray);
  
    switch (commandId)
        {
        case KGlxCommandIdAdd:
            { 
            TBool hasTitle = aCommand.IsSupported(KMPXMediaGeneralTitle);
            TBool hasSourceIds = aCommand.IsSupported(KMPXCommandGeneralSourceIds);
            TBool hasUri = aCommand.IsSupported(KMPXCommandGeneralSourceUri);
            TBool hasTargetIds = aCommand.IsSupported(KMPXMediaGeneralContainerId);
            TBool hasDrive = aCommand.IsSupported(KMPXMediaGeneralDrive);
        
            if (hasSourceIds)
                {            
                ArrayValueL(KMPXCommandGeneralSourceIds, sourceIdArray, aCommand);
                }
            if (hasTargetIds)
                {
                ArrayValueL(KMPXMediaGeneralContainerId, targetIdArray, aCommand);  
                }

/// @todo minor: Rowland Cook 12/06/2007 Could this be restrctured to be more efficient?
//                                       IE - Test for invalid combiations before retrieving the data. 
            
            if(hasTitle)
                {
                if(hasUri || hasSourceIds || hasDrive || hasTargetIds)
                    {
                    User::Leave(KErrArgument);
                    }
                aCommandHandler.AddContainerL(DescriptorValueL(KMPXMediaGeneralTitle, aCommand));
                }
            else if (hasUri)
                {
                if (hasSourceIds || hasDrive || !hasTargetIds)
                    // Illegal parameter combination
                    {
                    User::Leave(KErrArgument);
                    }
                aCommandHandler.AddToContainerL(DescriptorValueL(KMPXCommandGeneralSourceUri, aCommand), targetIdArray);   
                }
            else if (hasSourceIds)
                {
                if (hasTargetIds)
                    {
                    if (hasDrive)
                    	// Illegal parameter combination
                    	{
                    	User::Leave(KErrArgument);
                    	}
                    aCommandHandler.AddToContainerL(sourceIdArray, targetIdArray);
                    }
                else if (hasDrive)
                    {
                    aCommandHandler.CopyL(sourceIdArray, DescriptorValueL(KMPXMediaGeneralDrive, aCommand));
                    }
                else
                    // There isn't a target
                    {
                    User::Leave(KErrArgument);
                    }
                }
            else // There isn't a source
                {
                User::Leave(KErrArgument);
                }
            break;
            }
        case KGlxCommandIdRemove:
            {
            ArrayValueL(KMPXCommandGeneralSourceIds, sourceIdArray, aCommand);  
            TGlxMediaId containerId (ValueL<TMPXItemId>(KMPXMediaGeneralContainerId, aCommand));
            if (containerId == KGlxContainerDevice)
            	{
            	aCommandHandler.DeleteL(sourceIdArray);
            	}
            else
            	{
            	aCommandHandler.RemoveFromContainerL(sourceIdArray, containerId);
            	}
            break;
            }
        case KGlxCommandIdSet:
            {    
            TBool hasTitle = aCommand.IsSupported(KMPXMediaGeneralTitle);
            TBool hasDrive = aCommand.IsSupported(KMPXMediaGeneralDrive);
            TBool hasComment = aCommand.IsSupported(KMPXMediaGeneralComment);
            TBool hasLocation = aCommand.IsSupported(KGlxMediaGeneralLocation);
            
            ArrayValueL(KMPXCommandGeneralSourceIds, sourceIdArray, aCommand);
            
            if (hasTitle)
                // Rename operation
                {
                if (hasDrive || hasComment || hasLocation || sourceIdArray.Count() < 1)
                    // Unsupported attribute combination
                    {
                    User::Leave(KErrArgument);
                    }
                 const TDesC& title = DescriptorValueL(KMPXMediaGeneralTitle, aCommand);
                 aCommandHandler.RenameL(sourceIdArray[0], title);
                }
            else if (hasDrive)
                {
                if(hasComment  || hasLocation)
                    // Unsupported attribute combination
                    {
                    User::Leave(KErrArgument);
                    }
                const TDesC& drive = DescriptorValueL(KMPXMediaGeneralDrive, aCommand);
                aCommandHandler.MoveL(sourceIdArray, drive);
                }
            else if (hasComment)
                {
                if ( hasLocation )
                    // Unsupported attribute combination
                    {
                    User::Leave(KErrArgument);
                    }
                const TDesC& comment = DescriptorValueL(KMPXMediaGeneralComment, aCommand);
                aCommandHandler.SetDescriptionL(sourceIdArray, comment);
                }
            else if (hasLocation)
            	{
            	TCoordinate coordinate = aCommand.ValueTObjectL<TCoordinate>(KGlxMediaGeneralLocation);
            	aCommandHandler.SetCaptureLocationL(sourceIdArray, coordinate);
            	}
            else
            	{
                
                User::Leave(KErrArgument);
            	}
            break;
            }
        case KGlxCommandThumbnailCleanup:
        	{
        	aCommandHandler.ThumbnailCleanupL();
        	break;
        	}
        }
        CleanupStack::PopAndDestroy(&targetIdArray); 
        CleanupStack::PopAndDestroy(&sourceIdArray);
 
    }

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandParser::IdsL
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool TGlxCommandParser::IdsL(TUid& aCollectionUid, TUint32& aSessionId, const CMPXCommand& aCommand)
    {	
	aCollectionUid = ValueL<TUid>(KMPXCommandGeneralCollectionId, aCommand);    
    if ( aCommand.IsSupported(KMPXCommandGeneralSessionId) )
    	{
    	aSessionId = aCommand.ValueTObjectL<TUint32>(KMPXCommandGeneralSessionId);
    	return ETrue;
    	}
    else
    	{
    	return EFalse;
    	}
    }

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TTGlxCommandParser::ArrayValueL
// -----------------------------------------------------------------------------
//	
void TGlxCommandParser::ArrayValueL(const TMPXAttribute & aAttribute, RArray<TGlxMediaId>& aArray, const CMPXCommand& aCommand)
    {
    if (!aCommand.IsSupported(aAttribute) || aCommand.Type(aAttribute) != EMPXTypeCObject)
        {
        User::Leave(KErrArgument);
        }
  
    const CMPXCollectionPath* path = aCommand.ValueCObjectL<CMPXCollectionPath>(aAttribute);
	RArray<TMPXItemId> list;
    path->SelectionL(list);
  
    if (list.Count() == 0) // append the current item
        {
        aArray.AppendL(TGlxMediaId(path->Id())); 
        } 
    else
    	{
    	for( TInt i = 0 ; i < list.Count() ; i++ )
    		{
    		aArray.AppendL(TGlxMediaId(list[i]));
    		}
    	}
    }

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandParser::ValueL
// -----------------------------------------------------------------------------
//	
template<typename T> T TGlxCommandParser::ValueL(const TMPXAttribute & aAttribute, const CMPXCommand& aCommand)
    {
    if ( !aCommand.IsSupported(aAttribute) )
    	{
        User::Leave(KErrArgument); 	
    	}
    return aCommand.ValueTObjectL<T>(aAttribute);
    }

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// -----------------------------------------------------------------------------
// TGlxCommandParser::DescriptorValueL
// -----------------------------------------------------------------------------
//	
const TDesC& TGlxCommandParser::DescriptorValueL(const TMPXAttribute& aAttribute, const CMPXCommand& aCommand)
    {
    if (!aCommand.IsSupported(aAttribute) || aCommand.Type(aAttribute) != EMPXTypeText)
        {
        User::Leave(KErrArgument);
        }
    return aCommand.ValueText(aAttribute);
    }
