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
* Description:   Tag collection plugin implementation
*
*/



// INCLUDE FILES
#include "glxtagcollectionplugin.h"
#include "glxtagcollectionplugin.hrh"


#include "glxtnthumbnailrequest.h"
#include "glxtnthumbnailcreator.h"
#include "glxtnfileinfo.h"


#include <glxpanic.h>
#include <mpxcollectionpath.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <fbs.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmedia.h>



// ----------------------------------------------------------------------------
// Append the Item to the Array
// ----------------------------------------------------------------------------


LOCAL_C void AppendL(const TGlxMediaId& aId, const TDesC& aTitle,
        CMPXMediaArray& aArray, TMPXGeneralType aType,TMPXGeneralCategory aCat)
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdGeneral);
    CMPXMedia* entry= CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PushL(entry);
    entry->SetTObjectValueL(KMPXMediaGeneralId, aId);
    entry->SetTextValueL(KMPXMediaGeneralTitle, aTitle);
    entry->SetTObjectValueL(KMPXMediaGeneralType, aType);
    entry->SetTObjectValueL(KMPXMediaGeneralCategory, aCat);
    aArray.AppendL(entry);
    CleanupStack::Pop(entry);
    CleanupStack::PopAndDestroy(&supportedIds);
    }


// ----------------------------------------------------------------------------
// Two-Phased Constructor
// ----------------------------------------------------------------------------

CGlxTagCollectionPlugin* CGlxTagCollectionPlugin::NewL(TAny* aObs)
    {
    CGlxTagCollectionPlugin* obj = new (ELeave) CGlxTagCollectionPlugin(
                            static_cast<MMPXCollectionPluginObserver*>(aObs));
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop(obj);
	return obj;
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//

 CGlxTagCollectionPlugin::~CGlxTagCollectionPlugin()
 	{ 	
 	iThumbnailCreator->Close(NULL);
 	
 	iTags.ResetAndDestroy();
 	iTagItems.ResetAndDestroy();
 	
 	delete iSelf;
 	delete iOpenEntries;
 	delete iMedia;
 	
 	iFs.Close();
 	RFbsSession::Disconnect();
 	
 	}
 	
// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
void CGlxTagCollectionPlugin::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(RFbsSession::Connect(iFs));

    iThumbnailCreator = CGlxtnThumbnailCreator::InstanceL();

    iSelf = new (ELeave) CItem;
    iSelf->iId = TGlxMediaId(KGlxTagCollectionImplementationUid);
    iSelf->SetFilenameL(KNullDesC);
    iSelf->SetTitleL(_L("Tags"));

    // TEMP: Create some dummy items
    iItemAddedId = 6000;
    for (TInt i = 0; i < 1; i++) 
        {
        AddItemL(KNullDesC, _L("USA"), iTags);
        AddItemL(KNullDesC, _L("INDIA"), iTags);
        AddItemL(KNullDesC, _L("CRICKET"), iTags);
        AddItemL(KNullDesC, _L("Aus"), iTags);               
        AddItemL(KNullDesC, _L("WI"), iTags);   
        AddItemL(KNullDesC, _L("ENG"), iTags);   
      	}

    for (TInt i = 0; i < 1; i++) 
        {
        AddItemL(_L("c:\\TAG11.jpg"), _L("TAG11"), iTagItems, TTime(_L("20061105:083546.")), 123456, _L("C:"));
        AddItemL(_L("c:\\TAG12.jpg"), _L("TAG12"), iTagItems, TTime(_L("20061105:092527.")), 125371, _L("E:"));
        AddItemL(_L("c:\\TAG13.jpg"), _L("TAG13"), iTagItems, TTime(_L("20061105:101536.")), 267276, _L("C:"));
        AddItemL(_L("c:\\TAG14.jpg"), _L("TAG14"), iTagItems, TTime(_L("20061106:132425.")), 57824, _L("C:"));    
        }
   
    }
    
// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------

CGlxTagCollectionPlugin::CGlxTagCollectionPlugin(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// Uid of the plugin
// ----------------------------------------------------------------------------

TUid CGlxTagCollectionPlugin::Uid() const
	{
	return TUid::Uid(KGlxTagCollectionImplementationUid);
	}


// ----------------------------------------------------------------------------
// Open Level specified in  the path 
// ----------------------------------------------------------------------------


void CGlxTagCollectionPlugin::OpenL(const CMPXCollectionPath& aPath,
                   const TArray<TMPXAttribute>& /* aAttrs*/,
                   CMPXFilter* /*aFilter*/)
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdContainer);
    supportedIds.AppendL(KMPXMediaIdGeneral);
    iOpenEntries = CMPXMedia::NewL(supportedIds.Array());

    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
  
//  	TInt count = aPath.Count();
//  	TInt level = aPath.Levels();
    
 	if ( aPath.Levels() == 1 ) 
 		{
 		TInt count = iTags.Count();
 		for ( TInt i = 0; i < count; i++ )
 			{
 			AppendL(iTags[i]->iId, *iTags[i]->iTitle, *array, EMPXGroup, EMPXTag);
 			}
 		}
 	else if ( aPath.Levels() == 2 ) 
 		{
 		TInt count = iTagItems.Count();
 		for ( TInt i = 0; i < count; i++ )
 			{
 			AppendL(iTagItems[i]->iId, *iTagItems[i]->iTitle, *array, EMPXItem, EMPXImage);
 			}
 		}
 	else 
 		{
 		__DEBUG_ONLY(Panic(EGlxPanicDebugUnexpectedError));
 		}
    
    iOpenEntries->SetCObjectValueL(KMPXMediaArrayContents, array);
    iOpenEntries->SetTObjectValueL(KMPXMediaArrayCount, array->Count());
    TInt count = array->Count();

    CleanupStack::PopAndDestroy(array);    
    
    AsyncNotifyL(iOpenEntries, KErrNone, CGlxAsyncNotifier::EOpen);

	
    CleanupStack::PopAndDestroy(&supportedIds);        
    }



// ----------------------------------------------------------------------------
// Add an Item to the Collection
// ----------------------------------------------------------------------------

void CGlxTagCollectionPlugin::AddItemL(const TDesC& aFileName, 
	              const TDesC& aTitle, 
	              RPointerArray<CItem>& aDatabase, 
	              TTime aDateTime , 
	              TInt  aFileSize ,
	              const TDesC& aDrive ) 
{
	
	CItem* item = new (ELeave) CItem();
	CleanupStack::PushL(item);
	item->iId = TGlxMediaId(iItemAddedId++);
	item->SetFilenameL(aFileName);
	item->SetTitleL(aTitle);
	item->iDateTime = aDateTime;
	item->iFileSize = aFileSize;
	item->SetDriveL(aDrive);
	aDatabase.AppendL(item); 
	CleanupStack::Pop(); // item		
}



CGlxTagCollectionPlugin::CItem* CGlxTagCollectionPlugin::Item(const TGlxMediaId& aItemId) const
	{
	TInt count = iTags.Count();
	for (TInt i = 0; i < count; i++)
		{
		CItem* item = iTags[i];
		if (item->iId == aItemId)
			{
			return item;
			}
		}
		
	count = iTagItems.Count();
	for (TInt i = 0; i < count; i++)
		{
		CItem* item = iTagItems[i];
		if (item->iId == aItemId)
			{
			return item;
			}
		}
				
	return NULL; 
	}





//From MGlxtnThumbnailCreatorClient

// ----------------------------------------------------------------------------
// Fetch the Thumbnail
// ----------------------------------------------------------------------------

void CGlxTagCollectionPlugin::ThumbnailFetchComplete(const TGlxMediaId& /*aItemId/*/,
                    TGlxThumbnailQuality /*aQuality/*/, TInt /*aErrorCode/*/)
	{

	}
 
    
// ----------------------------------------------------------------------------
// Delete the Thumbnail
// ----------------------------------------------------------------------------                           
void CGlxTagCollectionPlugin:: ThumbnailDeletionComplete(const TGlxMediaId& /*aItemId/*/, TInt /*aErrorCode/*/)
	{
		
	}
	
	
// ----------------------------------------------------------------------------
// FilterAvailableComplete
// ----------------------------------------------------------------------------         
	
void CGlxTagCollectionPlugin:: FilterAvailableComplete(RArray<TGlxMediaId> /*aIdArray/*/, TInt /*aErrorCode/*/)
	{
		
	}
	

// ----------------------------------------------------------------------------
// To fetch the file info of the Content 
// ----------------------------------------------------------------------------   	

void CGlxTagCollectionPlugin:: FetchFileInfoL(CGlxtnFileInfo* aInfo, const TGlxMediaId& aItemId, TRequestStatus* aStatus)
	{
	 //  	CItem* item = Item(aItemId);
	//	aInfo->SetFilePathL(*item->iFilename);		
	
    CItem* item = Item(aItemId);
    __ASSERT_DEBUG(item != NULL, Panic(EGlxPanicLogicError));

    aInfo->SetFilePathL(*item->iFilename);

    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone); 
	
	}
	
	
// ----------------------------------------------------------------------------
// Cancel the fetch Uri
// ----------------------------------------------------------------------------   		
void CGlxTagCollectionPlugin::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
	{
	
	}
	
	
// ----------------------------------------------------------------------------
// Thumbnail Storage
// ----------------------------------------------------------------------------   	
	
MGlxtnThumbnailStorage* CGlxTagCollectionPlugin:: ThumbnailStorage()
	{
		return NULL;
	}


//From CMPXCollectionPlugin

void CGlxTagCollectionPlugin::CommandL(TMPXCollectionCommand /*aCmd*/, TInt /*aArg*/ )
	{

	}


void CGlxTagCollectionPlugin::MediaL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs,const TArray<TCapability>& /*aCaps/*/,
                    CMPXAttributeSpecs* /*aSpecs/*/)
	{
	 
	 CItem* item = NULL;
	 if(aPath.Levels() == 3)
	 {
	 	item = iTagItems[aPath.Index()];
	 }
	 
	 if(aPath.Levels() == 2)
	 {
	 	item = iTags[aPath.Index()];
	 }
	 
	 if(aPath.Levels() == 1)
	 {
	 	item = iSelf;
	 }
	 
	 TInt error = KErrNone;
	 TInt bitmapHandle = 0;
     RArray<TInt> supportedIds;
	 CleanupClosePushL(supportedIds);
	 iMedia = CMPXMedia::NewL(supportedIds.Array());
	 CleanupStack::PopAndDestroy(&supportedIds);

	if ( item )
	    {
	    for ( TInt i = 0; i < aAttrs.Count(); i++ )
	        {
	        if ( aAttrs[i] == KMPXMediaGeneralId )
	            {
	            iMedia->SetTObjectValueL(KMPXMediaGeneralId, item->iId);
	            }
	        else if ( aAttrs[i] == KMPXMediaGeneralUri )
	            {
	            iMedia->SetTextValueL(KMPXMediaGeneralUri, *item->iFilename);
	            }
	        else if ( aAttrs[i] == KMPXMediaGeneralTitle )
	            {
	            iMedia->SetTextValueL(KMPXMediaGeneralTitle, *item->iTitle);
	            }
	        else if ( KGlxMediaIdBitmapHandle == aAttrs[i].ContentId() )
	            {
	            bitmapHandle = aAttrs[i].AttributeId();
	            }
	        else if ( aAttrs[i] == KMPXMediaGeneralDate )
	            {
	            iMedia->SetTObjectValueL(KMPXMediaGeneralDate, item->iDateTime);
	            }
	        else if ( aAttrs[i] == KMPXMediaGeneralSize )
	            {
	            iMedia->SetTObjectValueL(KMPXMediaGeneralSize, item->iFileSize);
	            }
	        else if ( aAttrs[i] == KMPXMediaGeneralDrive )
	            {
	            iMedia->SetTextValueL(KMPXMediaGeneralDrive, *item->iDrive);
	            }
	        else
	            {
	//                error = KErrNotSupported;
	            }
	        }
	    }
	else
	    {
	    error = KErrNotFound;
	    }

	if ( (KErrNone == error) && bitmapHandle )
	    {
	    // Get thumbnail for the item
	    TGlxThumbnailRequest request;
	    request.iId = item->iId;
		request.iSizeClass = 60;
		request.iPriorityMode = TGlxThumbnailRequest::EPrioritizeSpeed;
		request.iBitmapHandle = bitmapHandle;
	    iThumbnailCreator->FetchThumbnailL(request,
	                                       *this);	
	    }
	else
	    {
	    
		AsyncNotifyL(iMedia, error, CGlxAsyncNotifier::EMedia);  
	    }


}

void CGlxTagCollectionPlugin::AddL(const CMPXMedia& /*aNewMedia*/)
{

}


void CGlxTagCollectionPlugin::RemoveL(const CMPXMedia& /*aMedia*/)
{

}


void CGlxTagCollectionPlugin::RemoveL(const CMPXCollectionPath& /*aPath/*/ )
{

}


void CGlxTagCollectionPlugin::SetL(const CMPXMedia& /*aMedia/*/)
{

}


void CGlxTagCollectionPlugin::FindAllL(const CMPXSearchCriteria& /*aCriteria/*/, 
                      const TArray<TMPXAttribute>& /*aAttrs/*/)
{

}
                      
CMPXMedia* CGlxTagCollectionPlugin:: FindAllSyncL(const CMPXSearchCriteria& /*aCriteria/*/,const TArray<TMPXAttribute>& /*aAttrs/*/)
{
		return NULL;
}
                                                          
TCollectionCapability CGlxTagCollectionPlugin::GetCapabilities()
{
		return NULL;
}
void CGlxTagCollectionPlugin::GenerateTitleL( TMPXGeneralCategory /*aCategory/*/,const TDesC& /*aBaseTitle/*/ )
{

}
                             
HBufC* CGlxTagCollectionPlugin::GenerateTitleSyncL( TMPXGeneralCategory /*aCategory/*/, const TDesC& /*aBaseTitle/*/ )
{
		return NULL;
}

