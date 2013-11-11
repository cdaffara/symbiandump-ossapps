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
* Description:   
*
*/



/**
 * @internal reviewed 11/07/2007 by M Byrne
 */


#include "glxdatasourcetaskmdscommand.h"

#include <bautils.h>
#include <e32math.h>
#include <glxcommandrequest.h>
#include <glxpanic.h>
#include <glxtracer.h>
#include <caf/manager.h>
#include <mdeconstants.h>
#include <mdeeventdef.h>
#include <mdelogiccondition.h>
#include <mdeobject.h>
#include <mdeobjectdef.h>
#include <mdeobjectquery.h>
#include <mdepropertydef.h>
#include <mderelationcondition.h>
#include <mderelationdef.h>
#include <mderelationquery.h>
#include <mdesession.h>
#include <mdetextproperty.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessageprogressdefs.h>
#include <pathinfo.h>

#include <glxstringcache.h>

#include <glxcollectionplugincamera.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginall.hrh>

#include "glxdatasourcemds.h"
#include "glxdatasourcemdsutility.h"
#include "glxdatasourcemds.hrh"

#include <glxpluginalbums.rsg>

#include <data_caging_path_literals.hrh>
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>

// CONSTANTS
_LIT(KPropertyDefNameDescription, "Comment");
_LIT(KPropertyDefNameTitle, "Title" );
_LIT(KPropertyDefNameSize, "Size");
_LIT(KPropertyDefNameCreationDate, "CreationDate");
_LIT(KPropertyDefNameLastModifiedDate, "LastModifiedDate");
_LIT(KPropertyDefNameAlbumType, "Type");
_LIT(KPropertyDefItemType, "ItemType");
// Item type for Album
_LIT( KAlbumItemType, "application/vnd.nokia.mde.album" );
// Item type for Tag
_LIT( KTagItemType, "application/vnd.nokia.mde.tag" );

const TInt KDriveLetterLength = 1;
_LIT(KColonBackslash, ":\\");
_LIT(KFileNameFormatString, "(%+02u)");

// Items to be deleted from File server at a time before calling scheduler wait
const TInt KDeletedItemCount = 10;
const TInt KDeleteOperationInterval = 1000;

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxDataSourceTaskMdeCommand::~CGlxDataSourceTaskMdeCommand()
	{
    TRACER("CGlxDataSourceTaskMdeCommand::~CGlxDataSourceTaskMdeCommand()");
	iLeftIds.Close();
    iRightIds.Close();
    delete iTitle;
    delete iObjectToRename;
    delete iStringCache;
    if(iTimer && iTimer->IsActive())
		{
		iTimer->Cancel();
		}
	delete iTimer;
    delete iSchedulerWait;    
    }


// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxDataSourceTaskMdeCommand::CGlxDataSourceTaskMdeCommand(
       CGlxCommandRequest* aRequest, MGlxDataSourceRequestObserver& aObserver, 
       CGlxDataSource* aDataSource)
    : CGlxDataSourceTaskMde(aRequest, aObserver, aDataSource) 
	{
    TRACER("CGlxDataSourceTaskMdeCommand::CGlxDataSourceTaskMdeCommand()");
	// No implementation required
	}

// ----------------------------------------------------------------------------
// Second phase constructor
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::ConstructL()
	{
    TRACER("CGlxDataSourceTaskMdeCommand::ConstructL()");
	iResponse = CMPXCommand::NewL(static_cast<CGlxCommandRequest*>(iRequest)->Command());
    DataSource()->CancelFetchThumbnail();
    
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iSchedulerWait = new (ELeave) CActiveSchedulerWait();
	}

/// @todo minor: Rowland Cook 12/06/2007 Add method decription.
// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::ExecuteRequestL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::ExecuteRequestL()
	{
    TRACER("CGlxDataSourceTaskMdeCommand::ExecuteRequestL()");
    __ASSERT_DEBUG(DataSource()->NamespaceDef(), Panic(EGlxPanicIllegalState));
    
	const CMPXCommand& command = static_cast<CGlxCommandRequest*>(iRequest)->Command();
	
	if ( command.IsSupported(KMPXCommandGeneralCollectionId) )
		{
		iCollectionUid = command.ValueTObjectL<TUid>(KMPXCommandGeneralCollectionId);
		}
	else
		{
		// if the collection Uid has not been set on the command, use the Ud of the plugin in use.
		iCollectionUid = iRequest->CollectionPluginUid();
		}
	
	TGlxCommandParser::ParseL(*this, command);
	}

// ----------------------------------------------------------------------------
// Add container
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::AddContainerL(const TDesC& aContainerName)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::AddContainerL()");
	iTitle = aContainerName.AllocL();
	AppendContainerTitleCountQueryL(ECommandAddContainer, aContainerName);
    ExecuteQueryL();
    }

// ----------------------------------------------------------------------------
// Add items to container by id
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::AddToContainerL(const RArray<TGlxMediaId>& aSourceIds,
        const RArray<TGlxMediaId>& aTargetContainers)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::AddToContainerL(const RArray<TGlxMediaId>& aSourceIds,const RArray<TGlxMediaId>& aTargetContainers)");	
	__ASSERT_DEBUG(aSourceIds.Count() && aTargetContainers.Count(), 
	        Panic(EGlxPanicEmptyArray));
	
    iLeftIds.Reset();
    iRightIds.Reset();

	CMdEQuery* query = DataSource()->Session().NewRelationQueryL(
	        *DataSource()->NamespaceDef(), this); 
	AppendQueryL(query, ECommandAddToContainer);
    
    CMdELogicCondition& rootCondition = query->Conditions();
    CMdERelationCondition& containerRelationCondition = 
    rootCondition.AddRelationConditionL(DataSource()->ContainsDef(),
            ERelationConditionSideLeft);
    CMdELogicCondition& leftLogicCondition = containerRelationCondition.LeftL();
    CMdELogicCondition& rightLogicCondition = containerRelationCondition.RightL();
    leftLogicCondition.SetOperator(ELogicConditionOperatorOr);
    rightLogicCondition.SetOperator(ELogicConditionOperatorOr);

    TItemId containerId = ContainerItemId(aTargetContainers[0]);
    // It is currenly a safe assumption that all containers are of the same type (either albums or tags)
    // and thus they will either all be on the left or all be on the right of the logic condition. Therefore, 
    // we only need to test the first container to see if it is on the left or on the right. 
    CMdEObject* container = DataSource()->Session().GetObjectL(containerId);
    if (!container)
        {
        User::Leave(KErrNotFound);
        }
    TBool containerIsLeft = DataSource()->ContainerIsLeft(container->Def());
    
    delete container;
    container = NULL;
    

    if (containerIsLeft)
        {
        // the container is on the left
        NGlxDataSourceMdsUtility::CopyMediaIdArrayL(iLeftIds, aTargetContainers);
        NGlxDataSourceMdsUtility::CopyMediaIdArrayL(iRightIds, aSourceIds);
        }
    else
        {
        // the container is on the right
        NGlxDataSourceMdsUtility::CopyMediaIdArrayL(iRightIds, aTargetContainers);
        NGlxDataSourceMdsUtility::CopyMediaIdArrayL(iLeftIds, aSourceIds);
        }
    
    leftLogicCondition.AddObjectConditionL(iLeftIds);
    rightLogicCondition.AddObjectConditionL(iRightIds);

    query->SetResultMode(EQueryResultModeItem);	 

    ExecuteQueryL();
    
    }

// ----------------------------------------------------------------------------
// Add item to containers by URI.
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::AddToContainerL(const TDesC& aSourceUri, 
		                        const RArray< TGlxMediaId >& aTargetContainers)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::AddToContainerL()");
    CMdEObject* sourceObject = DataSource()->Session().GetObjectL(aSourceUri);
    if (!sourceObject)
        {
        User::Leave(KErrNotFound);
        }
    RArray<TGlxMediaId> sourceIds;
    CleanupClosePushL(sourceIds);
    sourceIds.AppendL(TGlxMediaId(sourceObject->Id()));
    
    AddToContainerL(sourceIds, aTargetContainers);
    CleanupStack::PopAndDestroy(&sourceIds);
    }

// ----------------------------------------------------------------------------
// Copy files to another drive.
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::CopyL(const RArray<TGlxMediaId>& aSourceIds, 
        const TDesC& aDrive)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::CopyL()");
    FileOperationL(aSourceIds.Array(), aDrive, ECopy);
    }

// ----------------------------------------------------------------------------
// Move files to another drive.
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::MoveL(const RArray<TGlxMediaId>& aSourceIds, 
        const TDesC& aDrive)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::MoveL()");
    FileOperationL(aSourceIds.Array(), aDrive, EMove);
    }

// ----------------------------------------------------------------------------
// Remove items from a container.
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::RemoveFromContainerL(
        const RArray<TGlxMediaId>& aItemIds, const TGlxMediaId& aContainerId)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::RemoveFromContainerL()");
    // Answer to question in @bug above: No
    
	CMdEObject* object = NULL;
	object = DataSource()->Session().GetObjectL(aContainerId.Value());
    CleanupStack::PushL(object);
	if (!object)
    	{
    	User::Leave(KErrNotFound);
    	}
	
	CMdEQuery* query = DataSource()->Session().NewRelationQueryL(
	        *DataSource()->NamespaceDef(), this); 
	AppendQueryL(query, ECommandRemoveFromContainer); // query is now owned by the query array.
    
    CMdELogicCondition& rootCondition = query->Conditions();

    CMdERelationCondition& containerRelationCondition = 
    rootCondition.AddRelationConditionL(
            DataSource()->ContainsDef(), ERelationConditionSideLeft);
    CMdELogicCondition* containerLogicCondition = NULL;
    CMdELogicCondition* itemLogicCondition = NULL;
    // Containers are on the left for albums, right for tags
    if ( DataSource()->ContainerIsLeft(object->Def()) )
    	{
    	containerLogicCondition = &containerRelationCondition.LeftL();
    	itemLogicCondition = &containerRelationCondition.RightL();
    	}
    else
    	{
    	containerLogicCondition = &containerRelationCondition.RightL();
    	itemLogicCondition = &containerRelationCondition.LeftL();
    	}
   
    containerLogicCondition->AddObjectConditionL(aContainerId.Value());
    
    itemLogicCondition->SetOperator(ELogicConditionOperatorOr);

    NGlxDataSourceMdsUtility::AddObjectConditionL(*itemLogicCondition,aItemIds);
    
    query->SetResultMode(EQueryResultModeId);	 
    CleanupStack::PopAndDestroy(object);
    ExecuteQueryL();
    }

// ----------------------------------------------------------------------------
// Delete files, Remove container.
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DeleteL(const RArray<TGlxMediaId>& aItemIds)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::DeleteL()");
    CMdEObjectDef* containerObjectDef = NULL;
    TInt err = ContainerObjectDef(containerObjectDef);
    if (err == KErrNone)
    	{
	    QueueObjectQueryL(*containerObjectDef, 
	            aItemIds, ECommandDeleteContainers);
    	}
    // Assume that only items are left
    QueueObjectQueryL(DataSource()->ObjectDef(), 
                aItemIds, ECommandDeleteItems);
    
    ExecuteQueryL();
    }

// ----------------------------------------------------------------------------
// Rename files, Rename container.
//
/// @todo test this method.
//
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::RenameL(const TGlxMediaId& aSourceItemId, 
        const TDesC& aTitle)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::RenameL()");
    delete iTitle;
    iTitle = NULL;
	iTitle = aTitle.AllocL();
	
	RArray<TGlxMediaId> sourceIdArray;
	CleanupClosePushL(sourceIdArray);
	sourceIdArray.AppendL(aSourceItemId);
	
	QueueObjectQueryL(DataSource()->ObjectDef(), sourceIdArray , ECommandRename);
	CleanupStack::PopAndDestroy(&sourceIdArray);
	
	ExecuteQueryL();
    }

// ----------------------------------------------------------------------------
// Set description.
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::SetDescriptionL(const RArray<TGlxMediaId>& aItemIds, 
        const TDesC& aDescription)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::SetDescriptionL()");
    CMdEPropertyDef& descriptionPropertyDef = DataSource()->MediaDef().GetPropertyDefL(
            KPropertyDefNameDescription);
    if (descriptionPropertyDef.PropertyType() != EPropertyText)
    	{
    	User::Leave(KErrCorrupt);
    	}
    
    TInt count = aItemIds.Count();
    for (TInt i = 0; i < count; i++)
    	{
    	TItemId itemId = aItemIds[i].Value();
    	CMdEObject* object = DataSource()->Session().OpenFullObjectL(itemId);
    	if (!object)
    		{
    		User::Leave(KErrNotFound);
    		}
    	CleanupStack::PushL(object);
    	CMdEProperty* descriptionProperty = NULL;
    
    			
    	TInt index = object->Property(descriptionPropertyDef, descriptionProperty);
	    if (index < KErrNotFound)
	    	{
	    	User::Leave(index);
	    	}
	    if (index != KErrNotFound)
	    	{
	    	// Check whether the description string is empty
	    	if( aDescription.Length()  )
	    	    {	    	   
	    	    static_cast<CMdETextProperty*>(descriptionProperty)->SetValueL(aDescription);
	    	    }
	    	    
	        //if entered description string is blank then remove the old property
	    	else
                {
                object->RemoveProperty(index);
                }
	    	}
    	else
    	    {
    	    if( aDescription.Length()  )
	    	    {
    	    object->AddTextPropertyL(descriptionPropertyDef, aDescription);
	    	    }
    	    }
	    DataSource()->Session().CommitObjectL(*object); 
	    CleanupStack::PopAndDestroy(object);
    	}
    
    HandleRequestComplete(KErrNone);
    }


// ----------------------------------------------------------------------------
// Set capture location.
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::SetCaptureLocationL(const RArray<TGlxMediaId>& aItemIds, 
        const TCoordinate& aCoordinate)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::SetCaptureLocationL()") ;   
    if (!Math::IsNaN(aCoordinate.Latitude()) || !Math::IsNaN(aCoordinate.Longitude()))
    	{
    	User::Leave(KErrArgument);
    	}
    	
	CMdEQuery* query = DataSource()->Session().NewRelationQueryL(*DataSource()->NamespaceDef(), this); 
	AppendQueryL(query, ECommandRemoveLocation); // query is now owned by the query array.
    
    CMdELogicCondition& rootCondition = query->Conditions();

    CMdERelationCondition& containerRelationCondition = rootCondition.AddRelationConditionL(
            ERelationConditionSideLeft);
    CMdELogicCondition& locationLogicCondition = containerRelationCondition.RightL();
    CMdELogicCondition& itemLogicCondition = containerRelationCondition.LeftL();
    locationLogicCondition.AddObjectConditionL(DataSource()->LocationDef());
    
    itemLogicCondition.SetOperator(ELogicConditionOperatorOr);


    itemLogicCondition.AddObjectConditionL(reinterpret_cast<const RArray<TItemId>&>(aItemIds));
    
    query->SetResultMode(EQueryResultModeId);	 
    ExecuteQueryL();
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::ThumbnailCleanupL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::ThumbnailCleanupL()
	{
    TRACER("CGlxDataSourceTaskMdeCommand::ThumbnailCleanupL()");
	HandleRequestComplete(KErrNone);	
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::HandleQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DoHandleQueryCompletedL(CMdEQuery& aQuery)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::DoHandleQueryCompletedL()");
    TGlxQueryType queryType = iQueryTypes[0];
	
    switch (queryType)
    	{

	    case ECommandRemoveFromContainer: // don't break
	    case ECommandRemoveLocation:
			{
			TInt queryCount = aQuery.Count();
			RArray<TItemId> relationsToRemove;
			CleanupClosePushL(relationsToRemove);
			User::LeaveIfError(relationsToRemove.Reserve(queryCount));
			
			RArray<TItemId> successfullyRemovedReleations;
			CleanupClosePushL(successfullyRemovedReleations);
			User::LeaveIfError(successfullyRemovedReleations.Reserve(queryCount));
			
			for(TInt i = queryCount - 1; i >= 0; i--)
				{		
				relationsToRemove.AppendL(aQuery.ResultId(i));
				}
			
			if (queryCount)
				{
				User::LeaveIfError(DataSource()->Session().RemoveRelationsL(
				        relationsToRemove, successfullyRemovedReleations));
				}
			
			CleanupStack::PopAndDestroy(&successfullyRemovedReleations);
			CleanupStack::PopAndDestroy(&relationsToRemove);
			}
	    break;
	    case ECommandAddToContainer:
			{
			DoHandleAddToContainerQueryCompletedL(aQuery);
	        }
	    break;
	    case ECommandAddContainer:
	        { 
	        DoHandleAddContainerQueryCompletedL(aQuery);   
	        }
	        break;
	    case ECommandDeleteContainers:
	        {
	        DoHandleDeleteContainersQueryCompletedL(aQuery);
	        }
	    break;
	    case ECommandDeleteItems:
	        {
	        DoHandleDeleteItemsQueryCompletedL(aQuery);
	        }
	    break;
    	case ECommandRename:
    		{
    		DoHandleRenameQueryCompletedL(aQuery);
    		}
    	break;
    	case ECommandRenameContainer:
    		{
    		DoHandleRenameConainerQueryCompletedL(aQuery);
    		}
    	break;
    	}
    }
// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::DoNextQueryL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DoNextQueryL()
    {
    TRACER("CGlxDataSourceTaskMdeCommand::DoNextQueryL()");
    if (iQueries.Count())
        {
        ExecuteQueryL();
        }
    else
        {
        HandleRequestComplete(KErrNone);
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::FileOperationL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::FileOperationL(const TArray<TGlxMediaId>& aSourceIds, 
        const TDesC& aDrive, TFileOperation aFileOperation)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::FileOperationL()");
	ContentAccess::CManager *manager = ContentAccess::CManager::NewL();
	CleanupStack::PushL(manager);
	// The following line causes a code scanner warning advising use of EikonEnv RFs instance.
	// We don't have an EikonEnv as we are running in a server process.
	RFs rfs; // used for BaflUtils::FileExists
	
	CleanupClosePushL(rfs);
	User::LeaveIfError(rfs.Connect());
	for (TInt i = 0; i < aSourceIds.Count(); i++)
    	{
    	CMdEObject* sourceObject = DataSource()->Session().GetObjectL(aSourceIds[i].Value());
    	if (!sourceObject)
    		{
    		User::Leave(KErrNotFound);
    		}

    	CleanupStack::PushL(sourceObject);
    	
    	const TDesC& sourceFileName = sourceObject->Uri();
    	TFileName sourceRootPath;
    	
    	RootPath(sourceFileName, sourceRootPath);
    	TPtrC fileNameWithoutRoot(NULL,0);
    	if (sourceFileName.Left(sourceRootPath.Length()).CompareF(sourceRootPath) == 0)
    		// This is the expected case. The file to be copied is under the 'root' of 
    		// the drive that it is located on. (For the C:, the root is C:\data, 
    		// for the D: the root is D:\)
    		{
    		fileNameWithoutRoot.Set(sourceFileName.Right(sourceFileName.Length() - 
    		        sourceRootPath.Length()));
    		}
    	else
    		{
    		fileNameWithoutRoot.Set(sourceFileName);
    		}
    		
    	TFileName destinationFileName;
    	// Set destination file name to destination 'root' path
    	RootPath(aDrive, destinationFileName);
    	// Append the file name
    	destinationFileName.Append(fileNameWithoutRoot);
    	
    	if (destinationFileName.CompareF(sourceFileName) != 0)
    		{
    		// If source and destination are not identical, perform the copy.	
			if (BaflUtils::FileExists(rfs, destinationFileName))
				{
				// If the destination file name already exists find an available file name.
				TParse destinationFileNameParse;
				destinationFileNameParse.Set(destinationFileName,NULL,NULL); // this is a copy of the data
				TInt destinationFileNameWithoutExtensionLength = destinationFileName.Length()
				- destinationFileNameParse.Ext().Length();
				TInt i = 1;
				do
					{
			    	destinationFileName.SetLength(destinationFileNameWithoutExtensionLength);
			    	destinationFileName.AppendFormat(KFileNameFormatString,i++);
			    	destinationFileName.Append(destinationFileNameParse.Ext());		    	
					}
				while (BaflUtils::FileExists(rfs, destinationFileName));
				}
			
            // Ensure the path that we are copying to exists.
            // Fixed error ID:  ELLZ-798BP3 Lumiere 07.46.08_MC Photo: 
            // "System: System error." is displayed when copying/moving image which is saved in a user created folder.
            BaflUtils::EnsurePathExistsL(rfs,destinationFileName);
            
	    	if (aFileOperation == ECopy)
	    		{
	    		User::LeaveIfError(manager->CopyFile(sourceFileName, destinationFileName));		
	    		}
	    	else
	    		{
	    		User::LeaveIfError(manager->RenameFile(sourceFileName, destinationFileName));
	    		}

    		}
    	
    	CleanupStack::PopAndDestroy(sourceObject);
    	}
    
	CleanupStack::PopAndDestroy(&rfs);
	CleanupStack::PopAndDestroy(manager);
	
    HandleRequestComplete(KErrNone);
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::ContainerItemId
// ----------------------------------------------------------------------------
//
TItemId CGlxDataSourceTaskMdeCommand::ContainerItemId(const TGlxMediaId& aMediaId)
    {
    TRACER("TMdEItemId CGlxDataSourceTaskMdeCommand::ContainerItemId()");    
    return aMediaId.Value();
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::ContainerItemId
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::RootPath(const TDesC& aDrive, TDes& aRootPath)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::RootPath()");
	if (aDrive.Left(KDriveLetterLength).CompareF(PathInfo::PhoneMemoryRootPath().Left(
	        KDriveLetterLength)) == 0)
		{
		aRootPath = PathInfo::PhoneMemoryRootPath();
		}
	else if (aDrive.Left(KDriveLetterLength).CompareF(PathInfo::MemoryCardRootPath().Left(
	        KDriveLetterLength)) == 0)
		{
		aRootPath = PathInfo::MemoryCardRootPath();
		}
	else if (aDrive.Left(KDriveLetterLength).CompareF(PathInfo::RomRootPath().Left(
	        KDriveLetterLength)) == 0)
		{
		aRootPath = PathInfo::RomRootPath();
		}
	else
		{
		aRootPath = aDrive.Left(KDriveLetterLength);
		aRootPath.Append(KColonBackslash);
		}
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::SendProgressMessageL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::SendProgressMessageL(TInt aCurrentStep, TInt aStepCount)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::SendProgressMessageL()");
	MGlxDataSourceUpdateObserver& observer = 
	static_cast<CGlxCommandRequest*>(iRequest)->DataSourceUpdateObserver();
	
	const CMPXCommand& command = static_cast<CGlxCommandRequest*>(iRequest)->Command();
	__ASSERT_DEBUG(command.IsSupported(KMPXCommandGeneralSessionId), Panic(
	        EGlxPanicCommandHasNoGeneralSessionId));
	
	TAny* sessionId = command.ValueTObjectL<TAny*>(KMPXCommandGeneralSessionId);
	
	CMPXMessage* progressMessage = CMPXMessage::NewL();
	CleanupStack::PushL(progressMessage);
	progressMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralId, KMPXMessageContentIdProgress);	
	progressMessage->SetTObjectValueL<TInt>(TMPXAttribute(KMPXMessageContentIdProgress, 
	        EMPXMessageProgressCurrentCount), aCurrentStep);
	progressMessage->SetTObjectValueL<TInt>(TMPXAttribute(KMPXMessageContentIdProgress,
	        EMPXMessageProgressTotalCount), aStepCount);
	progressMessage->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId, sessionId);
	
	observer.HandleMessage(*progressMessage);
	
	CleanupStack::PopAndDestroy(progressMessage);
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::ContainerObjectDef
// ----------------------------------------------------------------------------
//
TInt CGlxDataSourceTaskMdeCommand::ContainerObjectDef(CMdEObjectDef*& aContainerObjectDef)
{
    TRACER("CGlxDataSourceTaskMdeCommand::ContainerObjectDef()");
    TInt err = KErrNone;
	if (iCollectionUid == TUid::Uid(KGlxTagCollectionPluginImplementationUid))
        {
        aContainerObjectDef  = &DataSource()->TagDef();      
        }
    else if (iCollectionUid  == TUid::Uid(KGlxCollectionPluginAlbumsImplementationUid))
        {
        aContainerObjectDef  = &DataSource()->AlbumDef();
        }
    else if(iCollectionUid  == TUid::Uid(KGlxCollectionPluginAllImplementationUid))
        {
        aContainerObjectDef  = &DataSource()->MediaDef();
        }
    else
    	{
    	err = KErrNotFound;
    	}
	return err;
}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::DoHandleAddToContainerQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DoHandleAddToContainerQueryCompletedL
															(CMdEQuery& aQuery)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::DoHandleAddToContainerQueryCompletedL()");
	RPointerArray<CMdEInstanceItem> relations;
	CleanupClosePushL(relations);
	 
	TInt leftCount = iLeftIds.Count();
	User::LeaveIfError(relations.Reserve(leftCount)); // this is a bigest reservation that could be required.
    for (TInt leftPos = 0; leftPos < leftCount; leftPos++)
        {
        TInt rightCount = iRightIds.Count();
        for (TInt rightPos = 0; rightPos < rightCount; rightPos++)
            {
            // Check to see if id already exists
            TBool alreadyExists = EFalse;
            for(TInt queryPos = aQuery.Count() - 1; queryPos >= 0; queryPos--)
                {
                CMdERelation& relation = static_cast<CMdERelation&>(aQuery.ResultItem(queryPos));
                if (relation.LeftObjectId() == iLeftIds[leftPos]
                    && relation.RightObjectId() == iRightIds[rightPos])
                    {
                    alreadyExists = ETrue;
                    break;
                    }
                }
            
            if (!alreadyExists)
                {
                CMdERelation* relation = DataSource()->Session().NewRelationL(
                        DataSource()->ContainsDef(), iLeftIds[leftPos], iRightIds[rightPos]);
                CleanupStack::PushL(relation);
                relations.AppendL(relation);
                CleanupStack::Pop(relation);
                }
            }
        
        }

    if (relations.Count())
    	{
    	User::LeaveIfError(DataSource()->Session().AddItemsL(relations));
    	}
    
    CleanupStack::PopAndDestroy(&relations);

    iLeftIds.Reset();
    iRightIds.Reset();
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::DoHandleAddContainerQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DoHandleAddContainerQueryCompletedL
															(CMdEQuery& aQuery)

	{
    TRACER("CGlxDataSourceTaskMdeCommand::DoHandleAddContainerQueryCompletedL()");
   	
   	//Duplicate albums check for the default albums i.e. Favourites 
   	//as title property is left blank in MDS 2.5
   	if(iCollectionUid == TUid::Uid(KGlxCollectionPluginAlbumsImplementationUid))
   		{
   		if(SearchStringL(R_ALBUM_FAVORITES_TITLE) == 0 || 
   				SearchStringL(R_ALBUM_CAMERA_TITLE) == 0 )
    		{
    		User::Leave(KErrAlreadyExists);	
    		}        	
   		}
   	   	
    if (aQuery.Count())
        {
        // An object (or more strickly objects) with the given container name already exist
        User::Leave(KErrAlreadyExists);
        }
     
    CMdEObject* object = NULL;
    CMdEObjectDef* containerObjectDef = NULL;
    TInt err = ContainerObjectDef(containerObjectDef );
    __ASSERT_ALWAYS(err == KErrNone, Panic(EGlxPanicInvalidCollectionUid));
    
    object = DataSource()->Session().NewObjectLC(*containerObjectDef, KNullDesC);
    
    // A title property def of type text is required.
    CMdEPropertyDef& titlePropertyDef = containerObjectDef->GetPropertyDefL(
            KPropertyDefNameTitle);
    if (titlePropertyDef.PropertyType() != EPropertyText)
        {
        User::Leave(KErrCorrupt);
        }
    // Set the object title.
    object->AddTextPropertyL(titlePropertyDef, *iTitle);
    
      //ItemType property def of type text is required.
    CMdEPropertyDef& itemTypePropertyDef = containerObjectDef->GetPropertyDefL(
            KPropertyDefItemType);
    if (itemTypePropertyDef.PropertyType() != EPropertyText)
    	{
    	User::Leave(KErrCorrupt);
    	}
  
    // Checks the Container type whether it is Tag or Album	
    if (iCollectionUid == TUid::Uid(KGlxTagCollectionPluginImplementationUid))
      {
      object->AddTextPropertyL (itemTypePropertyDef, KTagItemType);
      }
    else
      {
      object->AddTextPropertyL (itemTypePropertyDef, KAlbumItemType);
      }
   
   
    // A size property is required.
  
    CMdEPropertyDef& sizePropertyDef = containerObjectDef->GetPropertyDefL(
            KPropertyDefNameSize);
    if (sizePropertyDef.PropertyType() != EPropertyUint32)
        {
        User::Leave(KErrCorrupt);
        }
    object->AddUint32PropertyL(sizePropertyDef,0);

    
    // A creation date property is required.
    CMdEPropertyDef& creationDateDef = containerObjectDef->GetPropertyDefL(
            KPropertyDefNameCreationDate);
    if (creationDateDef.PropertyType() != EPropertyTime)
        {
        User::Leave(KErrCorrupt);
        }

    // A last modified date property is required.
    CMdEPropertyDef& lmDateDef = containerObjectDef->GetPropertyDefL(
            KPropertyDefNameLastModifiedDate);
    if (lmDateDef.PropertyType() != EPropertyTime)
        {
        User::Leave(KErrCorrupt);
        }
    
    TTime uTime;
    uTime.UniversalTime();
    object->AddTimePropertyL(creationDateDef, uTime);
    object->AddTimePropertyL(lmDateDef, uTime);
    
    TItemId id = DataSource()->Session().AddObjectL(*object);
    CleanupStack::PopAndDestroy(object);
    
    iResponse->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, id);
    iResponse->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, id);  
    iResponse->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType,
            EMPXItemInserted); 
	}
// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::DoHandleDeleteContainersQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DoHandleDeleteContainersQueryCompletedL
															(CMdEQuery& aQuery)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::DoHandleDeleteContainersQueryCompletedL()");
    CMdEPropertyDef& albumTypeProperty = DataSource()->AlbumDef().GetPropertyDefL(
            KPropertyDefNameAlbumType);
    TInt queryCount = aQuery.Count();
    
    RArray<TItemId> objectsForRemoval;
    CleanupClosePushL(objectsForRemoval);
    User::LeaveIfError(objectsForRemoval.Reserve(queryCount));

    RArray<TItemId> sucessfullyRemovedObjects;
    CleanupClosePushL(sucessfullyRemovedObjects);
    User::LeaveIfError(sucessfullyRemovedObjects.Reserve(queryCount));
    
    // Ensure that deletion is legal and that deletion
    for(TInt queryPos = 0; queryPos < queryCount; queryPos++)
        {
        CMdEObject& object = static_cast<CMdEObject&>(aQuery.ResultItem(queryPos));
  
        CMdEProperty* albumType;
        TInt albumTypeIndex = object.Property(albumTypeProperty, albumType);
        if (KErrNotFound != albumTypeIndex)
            {
            TInt albumTypeValue = static_cast<CMdEUint16Property*>(albumType)->Value();
            if ((albumTypeValue == MdeConstants::Album::EAlbumSystemFavourite) || 
            		(albumTypeValue == MdeConstants::Album::EAlbumSystemCamera))
               {
               User::Leave(KErrAccessDenied); 
               }
            }
        objectsForRemoval.AppendL(object.Id());
        }
    
    if (queryCount)
    	{
    	User::LeaveIfError(DataSource()->Session().RemoveObjectsL(objectsForRemoval, 
    	        sucessfullyRemovedObjects));
    	}
    
    CleanupStack::PopAndDestroy(&sucessfullyRemovedObjects);
    CleanupStack::PopAndDestroy(&objectsForRemoval);
	}
// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeCommand::DoHandleDeleteItemsContainersQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DoHandleDeleteItemsQueryCompletedL
															(CMdEQuery& aQuery)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::DoHandleDeleteItemsQueryCompletedL()");
    TInt deleteItemCounter = 0;
    ContentAccess::CManager *manager = ContentAccess::CManager::NewL();
    CleanupStack::PushL(manager);
    TInt queryCount = aQuery.Count();
    RArray<TItemId> objectsForRemoval;
    CleanupClosePushL(objectsForRemoval);
    User::LeaveIfError(objectsForRemoval.Reserve(queryCount));

    RArray<TItemId> sucessfullyRemovedObjects;
    CleanupClosePushL(sucessfullyRemovedObjects);
    User::LeaveIfError(sucessfullyRemovedObjects.Reserve(queryCount));
    //Handle to a file server session
    RFs fs; 
    // Pushing the handle in Clean-up Stack to call Close() on file server session 
    CleanupClosePushL( fs );
    //Connects a client to the file server
    User::LeaveIfError( fs.Connect() );
    
    TInt lastErr = KErrNone;
    
    // If Delete operation is cancelled before completion, 
    // iCancelled because ETrue, break out of for loop.
    for(TInt queryPos = queryCount - 1; (queryPos >= 0 && !iCancelled); queryPos--)
        {
        CMdEObject& object = static_cast<CMdEObject&>(aQuery.ResultItem(queryPos));
        //Removes the Read Only attributes of the file 
        fs.SetAtt(object.Uri(), 0, KEntryAttReadOnly);
        TInt err = manager->DeleteFile(object.Uri());
        if (err != KErrNone)
        	{
        	lastErr = err;
        	}    
        else
            {    
            // On successful deletion, delete the same from database
            objectsForRemoval.AppendL(object.Id());
            }
			 
        // After every 50 items are deleted, break from the for loop 
        // and process other pending requests if any
        if(deleteItemCounter == KDeletedItemCount)
			{	
			iTimer->Start( KDeleteOperationInterval, KDeleteOperationInterval,
							TCallBack( &SchedulerStopCallback, (TAny *)this ) );	
			iSchedulerWait->Start();  
			deleteItemCounter = 0;
			}     
        deleteItemCounter++;     
        }
    // Calling Close() on file server session 
    CleanupStack::PopAndDestroy( &fs );
    
    User::LeaveIfError(lastErr);
    
    if (queryCount)
    	{
    	// Some objects may have already been removed by the harvester
    	DataSource()->Session().RemoveObjectsL(objectsForRemoval, sucessfullyRemovedObjects);
    	}
    
    CleanupStack::PopAndDestroy(&sucessfullyRemovedObjects);
    CleanupStack::PopAndDestroy(&objectsForRemoval);
    
    CleanupStack::PopAndDestroy(manager);
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeCommand::DoHandleRenameConainerQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DoHandleRenameConainerQueryCompletedL
                                                            (CMdEQuery& aQuery)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::DoHandleRenameConainerQueryCompletedL()");
	__ASSERT_DEBUG(iObjectToRename, Panic(EGlxPanicLogicError));
	if (aQuery.Count())
		{
		User::Leave(KErrAlreadyExists);
		}
		
	CMdEObject* object = DataSource()->Session().OpenObjectL(iObjectToRename->Id(), iObjectToRename->Def());
	CleanupStack::PushL(object);
    CMdEProperty* albumType;
    // @todo AB test this
    CMdEPropertyDef* albumTypeProperty = NULL;
    TRAP_IGNORE(albumTypeProperty = &object->Def().GetPropertyDefL(KPropertyDefNameAlbumType));       
    TInt albumTypeIndex = KErrNotFound;
    // Must guard against non-existance of albumTypeProperty (e.g. when renaming a tag)
    if (albumTypeProperty)
        {
        albumTypeIndex = object->Property(*albumTypeProperty, albumType);
        }
    if( KErrNotFound != albumTypeIndex )
        {
        TInt albumTypeValue = static_cast<CMdEUint16Property*>(albumType)->Value();
        if ( albumTypeValue == MdeConstants::Album::EAlbumUserPredefined ) 
            {
            // Want to rename a predefined, localised album name so reclassify
            // the type to be a non-localised user defined album
            static_cast<CMdEUint16Property*>(albumType)->SetValueL(MdeConstants::Album::EAlbumUser);
            }
		else if ((albumTypeValue == MdeConstants::Album::EAlbumSystemFavourite) || 
				(albumTypeValue == MdeConstants::Album::EAlbumSystemCamera))
		    {
            // Cannot rename system albums
		    User::Leave(KErrAccessDenied); 
		    }
        }

    
    CMdEPropertyDef& titlePropertyDef = object->Def().GetPropertyDefL(KPropertyDefNameTitle);
    if (titlePropertyDef.PropertyType() != EPropertyText)
    	{
    	User::Leave(KErrCorrupt);
    	}
    
    CMdEProperty* titleProperty = NULL;
    TInt index = object->Property(titlePropertyDef, titleProperty);
    if (index < KErrNotFound)
    	{
    	User::Leave(index);
    	}
    if (index != KErrNotFound)
    	{
    	// Remove the old property
    	object->RemoveProperty(index);
    	}
    // Set the object title
    object->AddTextPropertyL(titlePropertyDef, *iTitle);
    
    // Get time propertydef of current session object
    CMdEPropertyDef& timePropertyDef = object->Def().GetPropertyDefL(
        KPropertyDefNameLastModifiedDate);
        
    // Check the validty of the time def
    if (timePropertyDef.PropertyType() != EPropertyTime)
    	{
    	User::Leave(KErrCorrupt);
    	}
    	
    // Get index and check the validity 
    CMdEProperty* timeProperty = NULL;
    index = object->Property(timePropertyDef, timeProperty);
    
    if (index < KErrNotFound)
    	{
    	User::Leave(index);
    	}
    	
    if (index != KErrNotFound)
    	{
    	// Remove the old property
    	object->RemoveProperty(index);
    	}
    	
    // Take current universal time
    TTime currentTime;
    currentTime.UniversalTime();

    // Set the object current time
    object->AddTimePropertyL(timePropertyDef, currentTime);
    
    DataSource()->Session().CommitObjectL(*object);
    CleanupStack::PopAndDestroy(object);
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeCommand::DoHandleRenameConainerQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::DoHandleRenameQueryCompletedL
                                                           (CMdEQuery& aQuery)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::DoHandleRenameQueryCompletedL()");
	__ASSERT_DEBUG(aQuery.Count() == 1, Panic(EGlxPanicUnexpectedQueryResultCount ));
	delete iObjectToRename;
	iObjectToRename = static_cast<CMdEObject*>(aQuery.TakeOwnershipOfResult(0));
	
    CMdEObjectDef* containerObjectDef = NULL;
    TInt err = ContainerObjectDef(containerObjectDef);
	if (err == KErrNone)
		{
		AppendContainerTitleCountQueryL(ECommandRenameContainer, *iTitle);
		}
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeCommand::AppendContainerTitleCountQueryL
// ----------------------------------------------------------------------------
//
void  CGlxDataSourceTaskMdeCommand::AppendContainerTitleCountQueryL
                        (const TGlxQueryType& aQueryType, const TDesC& aTitle)
	{
    TRACER("CGlxDataSourceTaskMdeCommand::AppendContainerTitleCountQueryL()");
	// Test to see if a container alerady exists in the database with aContainerName
    CMdEObjectDef* containerObjectDef = NULL;
    TInt err = ContainerObjectDef(containerObjectDef);
    __ASSERT_ALWAYS(err == KErrNone, Panic(EGlxPanicInvalidCollectionUid));
    
    CMdEQuery* query = DataSource()->Session().NewObjectQueryL(*DataSource()->NamespaceDef(),
            *containerObjectDef, this);
    CleanupStack::PushL(query);
    
    CMdEPropertyDef& titlePropertyDef = DataSource()->ObjectDef().GetPropertyDefL(
            KPropertyDefNameTitle);
    
    query->SetResultMode(EQueryResultModeCount);
    
    query->Conditions().AddPropertyConditionL(titlePropertyDef, 
            ETextPropertyConditionCompareEquals, aTitle);
    
    CleanupStack::Pop(query);
    
    AppendQueryL(query, aQueryType); 
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeCommand::SearchString
// ----------------------------------------------------------------------------
//	
TInt CGlxDataSourceTaskMdeCommand::SearchStringL(TInt aResourceId)
	{
	TRACER("CGlxDataSourceTaskMdeCommand::SearchStringL()");
	_LIT(KResourceFile, "z:glxpluginalbums.rsc");	
   	
   	if (!iStringCache)
        {
        iStringCache = CGlxStringCache::NewL();
        }        
    //Get the localized string from the cache    
	HBufC* string = iStringCache->LoadLocalizedStringLC(KResourceFile,aResourceId);		
	
	TInt result = string->CompareF(iTitle->Des());
	
	CleanupStack::PopAndDestroy(string);
    
    return result;    
        	
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeCommand::SchedulerStopCallback
// ----------------------------------------------------------------------------
//
TInt CGlxDataSourceTaskMdeCommand::SchedulerStopCallback(TAny* aPtr)
    {
    TRACER("CGlxDataSourceTaskMdeCommand::SchedulerStopCallback");
    
    CGlxDataSourceTaskMdeCommand* self = (CGlxDataSourceTaskMdeCommand*) aPtr;
    if ( self )
        {
        self->SchedulerStopComplete();
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// SchedulerStopComplete
// -----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeCommand::SchedulerStopComplete()
    {
    TRACER("CGlxDataSourceTaskMdeCommand::SchedulerStopComplete");  
    
    if(iTimer && iTimer->IsActive())
    	{
    	iTimer->Cancel();
    	}
    
    if(iSchedulerWait)
		{		
		iSchedulerWait->AsyncStop();    
		}  
    }
