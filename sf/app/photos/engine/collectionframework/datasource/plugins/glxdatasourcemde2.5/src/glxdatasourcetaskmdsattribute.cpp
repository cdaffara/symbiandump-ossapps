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



#include "glxdatasourcetaskmdsattribute.h"

#include <glxcollectionplugincamera.hrh>
#include <glxcollectionplugindownloads.hrh>
#include <glxcollectionpluginmonths.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxerrors.h>
#include <glxfilterfactory.h>
#include <glxgetrequest.h>
#include <glxgetrequest.h>
#include <glxidlistrequest.h>
#include <glxlog.h>
#include <glxmediacollectioninternaldefs.h>
#include <glxmediageneraldefs.h>
#include <glxrequest.h>
#include <glxthumbnailattributeinfo.h>
#include <glxthumbnailrequest.h>
#include <glxtnfileinfo.h>
#include <glxtracer.h>
#include <lbsposition.h>
#include <mdeconstants.h>
#include <mdelogiccondition.h>
#include <mdeobject.h>
#include <mdeobjectdef.h>
#include <mdeobjectquery.h>
#include <mdepropertydef.h>
#include <mderelationdef.h>
#include <mdesession.h>
#include <mdetextproperty.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h> 
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>
#include <imageconversion.h>
#include <caf/content.h>
#include "glxdatasourcemds.h"
#include "glxdatasourcemds.hrh"
#include "glxdatasourcemdsutility.h"

// CONSTANTS

_LIT(KPropertyDefNameAlbumType, "Type");
_LIT(KPropertyDefNameItemType, "ItemType");
_LIT(KPropertyDefNameCreationDate, "CreationDate");
_LIT(KPropertyDefNameLastModifiedDate, "LastModifiedDate");
_LIT(KPropertyDefNameSize, "Size");
_LIT(KPropertyDefNameTitle, "Title");
_LIT(KPropertyDefNameDuration, "Duration");
_LIT(KPropertyDefNameComment, "Comment");
_LIT(KPropertyDefNameWidth, "Width");
_LIT(KPropertyDefNameHeight, "Height");
_LIT(KPropertyDefNameDRM, "DRM");
_LIT(KPropertyDefNameFrameCount, "FrameCount");
_LIT(KPropertyDefNameLatitude, "Latitude");
_LIT(KPropertyDefNameLongitude, "Longitude");

// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------
//  
CGlxDataSourceTaskMdeAttributeMde::CGlxDataSourceTaskMdeAttributeMde(
        CGlxGetRequest* aRequest,MGlxDataSourceRequestObserver& aObserver,
            CGlxDataSource* aDataSource)
    : CGlxDataSourceTaskMde(aRequest, aObserver, aDataSource)
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::CGlxDataSourceTaskMdeAttribute()")
    iFilterProperties = TGlxFilterFactory::ExtractAttributes(iRequest->Filter());
    }

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------
//  
CGlxDataSourceTaskMdeAttributeMde::~CGlxDataSourceTaskMdeAttributeMde()
	{
    TRACER("CGlxDataSourceTaskMdeAttribute::~CGlxDataSourceTaskMdeAttribute()")
    delete iMediaArray;
    iQueryAttributes.Close();
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeAttribute::ExecuteRequestL
// ----------------------------------------------------------------------------
//      
void CGlxDataSourceTaskMdeAttributeMde::ExecuteRequestL()
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::ExecuteRequestL()")    
#ifdef _DEBUG
    iStartTime.HomeTime(); 
#endif
 
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
        
    __ASSERT_DEBUG(request->MediaIds().Count() > 0, User::Invariant());
    
    if (request->MediaIds().Count() > 1)
        {
        iMediaArray = CMPXMediaArray::NewL();
        }
    
    if (request->MediaIds()[0] == KGlxCollectionRootId)
    	{
        __ASSERT_DEBUG(request->MediaIds().Count() == 1, User::Invariant());
        AddCollectionAttributesL(iResponse);
    	}
    else
    	{
        __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
        CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);   
        
        switch(iFilterProperties.iItemType)
            {
            case EGlxFilterVideoAndImages:
            case EGlxFilterImage:
            case EGlxFilterVideo:
                {
            	QueueImageVideoObjectQueriesL(request->MediaIds(), iFilterProperties);
        	    if (LocationAttributeRequested())
        	        {
        	        QueueLocaitonQueryL();
        	        }
                break;
                }
            case EGlxFilterAlbum:
                {
            	QueueAlbumObjectQueryL(request->MediaIds());
                break;
                }
            case EGlxFilterTag:
                {
                QueueTagObjectQueryL(request->MediaIds());
                break;
                }
            case EGlxFilterMonth:
                {
                QueueMonthObjectQueryL(request->MediaIds());
                break;
                }
            }
        
    	}

    DoNextQueryL();
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::DoHandleQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::DoHandleQueryCompletedL(CMdEQuery& /*aQuery*/)
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::DoHandleQueryCompletedL()")   
       
    switch (iQueryTypes[0])
        {
        case ELocationAttributeQuery:
            DoHandleLocationQueryCompletedL();
        break;
        case EAttributeQuery:
            DoHandleAttributeQueryCompletedL();
        break;
        case EImageVideoQuery:
            DoHandleImageVideoQueryCompletedL();
        break;
        case EContainerFirstItemQuery:
            // Fall through to panic, DoHandleQueryCompletedL can't handle this query type.
        default:
            Panic(EGlxPanicLogicError);
        break;
        }  
#ifdef _DEBUG
    iStopTime.HomeTime(); 
    GLX_DEBUG2("CGlxDataSourceTaskMdeAttributeMde:DoHandleQueryCompletedL() took %d us",
                     (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif  
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeAttribute::DoNextQueryL
// ----------------------------------------------------------------------------
//    
void CGlxDataSourceTaskMdeAttributeMde::DoNextQueryL()
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::DoNextQueryL()")
    if (iQueries.Count())
        {
        ExecuteQueryL();
        }
    else
    	{
    	DoHandleAttributeResponseCompleteL();
    	}
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeAttribute::DoHandleAttributeResponseCompleteL
// ----------------------------------------------------------------------------
//     
void CGlxDataSourceTaskMdeAttributeMde::DoHandleAttributeResponseCompleteL()
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::DoHandleAttributeResponseCompleteL()")
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
    if(request->MediaIds().Count() > 1)
        {
        iResponse->SetCObjectValueL(KMPXMediaArrayContents, iMediaArray);
        iResponse->SetTObjectValueL(KMPXMediaArrayCount, iMediaArray->Count());
        delete iMediaArray;
        iMediaArray = NULL;
        }
    HandleRequestComplete(KErrNone);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::AddAttributesLC
// ----------------------------------------------------------------------------
//      
void CGlxDataSourceTaskMdeAttributeMde::AddAttributesL(CMdEObject& aObject, CMPXMedia* aEntry)
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::AddAttributesL()")
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    
    CGlxDataSource::TContainerType containerType = DataSource()->ContainerType(&aObject);
    if( containerType != CGlxDataSource::EContainerTypeNotAContainer )
        {
        AddContainerAttributesL(aEntry, &aObject, containerType);
        }
    else 
        {
        CGlxDataSource::TItemType itemType = DataSource()->ItemType(&aObject);
        if ( itemType != CGlxDataSource::EItemTypeNotAnItem )
            {
            AddItemAttributesL(aEntry, &aObject, itemType);
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }       
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::AddCollectionAttributesL
// ----------------------------------------------------------------------------
//     
void CGlxDataSourceTaskMdeAttributeMde::AddCollectionAttributesL(CMPXMedia* aEntry)
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::AddCollectionAttributesL()")
    __ASSERT_DEBUG(aEntry, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
    
    for ( TInt i = 0; i < request->Attributes().Count(); i++ )
        {
        if ( request->Attributes()[i] == KMPXMediaGeneralId )
            {
            aEntry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, (
                    TMPXItemId)request->CollectionPluginUid().iUid);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralType )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralType, EMPXNoType);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralCategory )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXCollection);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralTitle )
            {
            request->AppendCpiAttributeL(KMPXMediaGeneralTitle);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSystemItem )
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralSystemItem, ETrue);
            }
            
        //Attributes to get the Count of Images in Collection                
        else if	( request->Attributes()[i] == KGlxMediaItemTypeImage )    
			{
			TGlxMediaId container = TGlxMediaId(KGlxCollectionRootId);
			CMdEObjectDef* objectDef = &DataSource()->ObjectDef();
			TBool isContent = ETrue;
			TGlxFilterProperties filterProperties = iFilterProperties;

			switch(iRequest->CollectionPluginUid().iUid)
				{
				case KGlxCollectionPluginCameraImplementationUid:
					{   
#if 0 	/// AB camera album				     		
					container = DataSource()->CameraAlbumId();
					objectDef = &DataSource()->AlbumDef();
#endif					
					filterProperties.iItemType = EGlxFilterImage;
					QueueObjectQueryL(*objectDef, isContent, EAttributeQuery,
					        EQueryResultModeCount, container, 
					        request->Attributes()[i], aEntry,  filterProperties);
					break;                    	
					}
				default:
					{
					break;	
					}
				}
			}    
			       	
		//Attributes to get the Count of Videos in Collection                    	
         else if ( request->Attributes()[i] == KGlxMediaItemTypeVideo )    
			{
			TGlxMediaId container = TGlxMediaId(KGlxCollectionRootId);
			CMdEObjectDef* objectDef = &DataSource()->ObjectDef();
			TBool isContent = ETrue;
			TGlxFilterProperties filterProperties = iFilterProperties;

			switch(iRequest->CollectionPluginUid().iUid)
				{
				case KGlxCollectionPluginCameraImplementationUid:
					{   
#if 0 	/// AB camera album					     		
					container = DataSource()->CameraAlbumId();
					objectDef = &DataSource()->AlbumDef();
#endif					
					filterProperties.iItemType = EGlxFilterVideo;
					QueueObjectQueryL(*objectDef, isContent, EAttributeQuery, 
					        EQueryResultModeCount, container,
					        request->Attributes()[i],aEntry,  filterProperties);
					break;                    	
					}
				default:
					{
					break;	
					}
				}
			}
            
            
        else if ( ( request->Attributes()[i] == KMPXMediaGeneralCount ) || 
                ( request->Attributes()[i] == KGlxMediaCollectionInternalUsageCount ) )
            {
            TGlxMediaId container = TGlxMediaId(KGlxCollectionRootId);
            CMdEObjectDef* objectDef = &DataSource()->ObjectDef();
            TBool isContent = ETrue;
            TGlxFilterProperties filterProperties = iFilterProperties;
            
            switch(iRequest->CollectionPluginUid().iUid)
                {

                case KGlxCollectionPluginAlbumsImplementationUid:
                    {
                    isContent = EFalse;
                    objectDef = &DataSource()->AlbumDef();
                    break;
                    }
                case KGlxCollectionPluginCameraImplementationUid:
                    {
					filterProperties.iOrigin = EGlxFilterOriginCamera;
                    break;
                    }
                case KGlxTagCollectionPluginImplementationUid: 
                    {
                    isContent = EFalse;
                    objectDef = &DataSource()->TagDef();
                    break;
                    }
                case KGlxCollectionPluginMonthsImplementationUid:
                    {
					filterProperties.iOrigin = EGlxFilterOriginAll;
                    break;
                    }
                default:
                    {
                    // default gallery query returns all objects as per filter
                    break;
                    }
                }
            QueueObjectQueryL(*objectDef, isContent, EAttributeQuery, 
                    EQueryResultModeCount, container, request->Attributes()[i], 
                    aEntry,  filterProperties);
            }
        else if ( request->Attributes()[i] == KMPXMediaColDetailSpaceId )
            {
            aEntry->SetTObjectValueL(KMPXMediaColDetailSpaceId, 
                    KGlxDataSourceMdeImplementationUid);
            }
        else if ( request->Attributes()[i] == KGlxMediaCollectionInternalStartDate )
            {
            GLX_DEBUG1("CGlxDataSourceTaskMdeAttributeMde::AddCollectionAttributesL - KGlxMediaCollectionInternalStartDate");
            TGlxMediaId container = TGlxMediaId(KGlxCollectionRootId);
            TGlxFilterProperties filterProperties = iFilterProperties;
            filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
            filterProperties.iSortDirection = EGlxFilterSortDirectionAscending;
            filterProperties.iOrigin = EGlxFilterOriginAll;
            filterProperties.iMaxCount = 1 ;             

            QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery, 
                    EQueryResultModeItem, container, 
                    KGlxMediaCollectionInternalStartDate, aEntry, filterProperties);
            }
        else if ( request->Attributes()[i] == KGlxMediaCollectionInternalEndDate )
            {
            GLX_DEBUG1("CGlxDataSourceTaskMdeAttributeMde::AddCollectionAttributesL - KGlxMediaCollectionInternalEndDate");
            TGlxMediaId container = TGlxMediaId(KGlxCollectionRootId);
            TGlxFilterProperties filterProperties = iFilterProperties;
            filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
            filterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
            filterProperties.iOrigin = EGlxFilterOriginAll;
            filterProperties.iMaxCount = 1 ;             

            QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery, 
                    EQueryResultModeItem, container, 
                    KGlxMediaCollectionInternalEndDate, aEntry, filterProperties);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSlideshowableContent )
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralSlideshowableContent, 0);
            }
		else if ( request->Attributes()[i] == KMPXMediaDrmProtected )
			{
            // collection itself is not protected
            aEntry->SetTObjectValueL(KMPXMediaDrmProtected, EFalse); 
			}
        
        else if ( request->Attributes()[i] == KGlxMediaGeneralDRMRightsValid )            	
        	{
        	aEntry->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid, EGlxDrmRightsValid); 
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }


    
// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::AddContainerAttributesL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::AddContainerAttributesL(CMPXMedia* aEntry, 
        CMdEObject* aContainer, CGlxDataSource::TContainerType aType)
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::AddContainerAttributesL()")
    __ASSERT_DEBUG(aEntry, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(aContainer, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);

    /// check property defs are valid and type is correct
    for ( TInt i = 0; i < request->Attributes().Count(); i++ )
        {
        if ( request->Attributes()[i] == KMPXMediaGeneralId )
            {
            aEntry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, 
                    (TMPXItemId)aContainer->Id());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralType )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralType, EMPXGroup);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralCategory )
            {
            if( CGlxDataSource::EContainerTypeAlbum == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXAlbum);
                }
            else if( CGlxDataSource::EContainerTypeTag == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXTag);
                }
            else if( CGlxDataSource::EContainerTypeMonth == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXMonth);
                }
            else
                {
                User::Leave(KErrNotSupported);
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralUri )
            {
            aEntry->SetTextValueL(KMPXMediaGeneralUri, aContainer->Uri());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralTitle )
            {
            CMdEProperty* title;
            CMdEPropertyDef& titleProperty = aContainer->Def().GetPropertyDefL(
                    KPropertyDefNameTitle);
            TInt titleIndex = aContainer->Property(titleProperty, title);
            if(titleIndex == KErrNotFound)
                {
                TParsePtrC parser(aContainer->Uri());
                aEntry->SetTextValueL(KMPXMediaGeneralTitle, parser.Name());
                }
            else
                {
                aEntry->SetTextValueL(KMPXMediaGeneralTitle, static_cast<CMdETextProperty*>
                (title)->Value());
                }
            switch (aType)
                {
                case CGlxDataSource::EContainerTypeAlbum:
                    {
                    CMdEProperty* albumType;
                    CMdEPropertyDef& albumTypeProperty = 
                    aContainer->Def().GetPropertyDefL( KPropertyDefNameAlbumType);
                    TInt albumTypeIndex = aContainer->Property(albumTypeProperty, albumType);
                    if( KErrNotFound != albumTypeIndex )
                        {
                        TUint16 albumTypeValue = 
                        static_cast<CMdEUint16Property*>(albumType)->Value();
                        if ( albumTypeValue != MdeConstants::Album::EAlbumUser ) 
                            {
                            request->AppendCpiAttributeL(KMPXMediaGeneralTitle);
                            aEntry->SetTObjectValueL(KGlxMediaCollectionInternalSystemItemType, 
                                    albumTypeValue);
                            }
                        }
                    break;
                    }
                case CGlxDataSource::EContainerTypeTag:
                    {
                    break;
                    }
                case CGlxDataSource::EContainerTypeMonth:
                    {
                    request->AppendCpiAttributeL(KMPXMediaGeneralTitle);
                    CMdEProperty* time;
                    CMdEPropertyDef& timeProperty = aContainer->Def().GetPropertyDefL(
                            KPropertyDefNameCreationDate);
                    TInt timeIndex = aContainer->Property(timeProperty, time);
                    if( KErrNotFound == timeIndex )
                        {
                        User::Leave(KErrCorrupt);
                        }

                    aEntry->SetTObjectValueL(KGlxMediaCollectionInternalStartDate, 
                            static_cast<CMdETimeProperty*>(time)->Value());
                    break;
                    }
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDate )
            {
            CMdEProperty* time;
            CMdEPropertyDef& timeProperty = aContainer->Def().GetPropertyDefL(
                    KPropertyDefNameCreationDate);
            TInt timeIndex = aContainer->Property(timeProperty, time);
            if( KErrNotFound == timeIndex )
                {
                User::Leave(KErrCorrupt);
                }

            aEntry->SetTObjectValueL(KMPXMediaGeneralDate, 
                    static_cast<CMdETimeProperty*>(time)->Value());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralSize )
            {
            CMdEProperty* size;
            CMdEPropertyDef& sizeProperty = aContainer->Def().GetPropertyDefL(
                    KPropertyDefNameSize);
            TInt sizeIndex = aContainer->Property(sizeProperty, size);
            TUint sizeValue;
            if(sizeIndex == KErrNotFound)
                {
                sizeValue = 0;
                }
            else
                {
				sizeValue = size->Uint32ValueL();
                }
            aEntry->SetTObjectValueL(KMPXMediaGeneralSize, sizeValue);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDrive )
            {
            TParsePtrC parser(aContainer->Uri());
            aEntry->SetTextValueL(KMPXMediaGeneralDrive, parser.Drive());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralMimeType )
            {
            CMdEProperty* mimeType;
            CMdEPropertyDef& mimeTypeProperty = aContainer->Def().GetPropertyDefL(
                    KPropertyDefNameItemType);
            TInt mimeTypeIndex = aContainer->Property(mimeTypeProperty, mimeType);
            if( KErrNotFound == mimeTypeIndex)
                {
                aEntry->SetTextValueL(KMPXMediaGeneralMimeType, KNullDesC); 
                }
            else
                {
                aEntry->SetTextValueL(KMPXMediaGeneralMimeType,
                        static_cast<CMdETextProperty*>(mimeType)->Value());
                }
            }
        else if ( ( request->Attributes()[i] == KMPXMediaGeneralCount ) || 
                ( request->Attributes()[i] == KGlxMediaCollectionInternalUsageCount ) )
            {
            switch (aType)
                {
                case CGlxDataSource::EContainerTypeAlbum:
                case CGlxDataSource::EContainerTypeTag:
                    {                                                           
                    // for 10.1 we need just images - so just getting images inside an album will do
                    TGlxFilterProperties filterProperties = iFilterProperties;
                    filterProperties.iItemType = EGlxFilterImage;
                    filterProperties.iNoDRM = ETrue;
                    
                    QueueObjectQueryL(aContainer->Def(), ETrue, EAttributeQuery,
                                      EQueryResultModeCount, TGlxMediaId(aContainer->Id()),
                                      request->Attributes()[i], aEntry, filterProperties);
                    
                    //TUint32 countTypeIndex = aContainer->UsageCount();
                    //aEntry->SetTObjectValueL(request->Attributes()[i], countTypeIndex);
                    break;
                    }
                case CGlxDataSource::EContainerTypeMonth:
                    {
                    iFilterProperties.iOrigin = EGlxFilterOriginAll;                    
                    TGlxFilterProperties filterProperties = iFilterProperties;
                    AddMonthFilterL(aContainer, filterProperties);
                    QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery,
                            EQueryResultModeCount, TGlxMediaId(KGlxCollectionRootId),
                            request->Attributes()[i], aEntry, filterProperties);
                    break;
                    }
                }
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSystemItem )
            {
            TBool systemItem = EFalse;
            switch (aType)
                {
                case CGlxDataSource::EContainerTypeAlbum:
                    {
                    CMdEProperty* albumType;
                    CMdEPropertyDef& albumTypeProperty = 
                    aContainer->Def().GetPropertyDefL(   KPropertyDefNameAlbumType);
                    TInt albumTypeIndex = aContainer->Property(albumTypeProperty,
                            albumType);
                    if( KErrNotFound != albumTypeIndex )
                        {
                        TInt albumTypeValue = 
                        static_cast<CMdEUint16Property*>(albumType)->Value();
                        if( (albumTypeValue == MdeConstants::Album::EAlbumSystemCamera) || 
                        		(albumTypeValue == MdeConstants::Album::EAlbumSystemFavourite ) )
                            {
                            systemItem = ETrue;
                            }
                        }
                    break;
                    }
                case CGlxDataSource::EContainerTypeTag:
                    {
                    systemItem = EFalse;
                    break;
                    }
                case CGlxDataSource::EContainerTypeMonth:
                    {
                    systemItem = ETrue;
                    break;
                    }
                }
            aEntry->SetTObjectValueL(KGlxMediaGeneralSystemItem, systemItem);
            }
            
         //Attributes to get the Count of Images in Container                
        else if	( request->Attributes()[i] == KGlxMediaItemTypeImage )    
			{
			switch (aType)
				{
				case CGlxDataSource::EContainerTypeMonth:
					{
					TGlxFilterProperties filterProperties = iFilterProperties;
					AddMonthFilterL(aContainer, filterProperties);
					filterProperties.iItemType = EGlxFilterImage;
#if 0 	/// AB camera album					
					QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery,
					        EQueryResultModeCount, DataSource()->CameraAlbumId(), 
					        request->Attributes()[i], aEntry, filterProperties);
#endif					
					break;
					}			             					
				default:
					{
					break;	
					}
				}
			}           	
		// Attributes to get the Count of Videos in Container 		
						                   	
         else if ( request->Attributes()[i] == KGlxMediaItemTypeVideo )    
			{
			switch (aType)
				{
				case CGlxDataSource::EContainerTypeMonth:
					{
					TGlxFilterProperties filterProperties = iFilterProperties;
					AddMonthFilterL(aContainer, filterProperties);
					filterProperties.iItemType = EGlxFilterVideo;
#if 0 	/// AB camera album					
					QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery,
					        EQueryResultModeCount, DataSource()->CameraAlbumId(),
					        request->Attributes()[i], aEntry, filterProperties);
#endif					
					break;
					}
				default:
					{
					break;	
					}
				}
			}
			
        else if ( request->Attributes()[i] == KMPXMediaColDetailSpaceId )
            {
            aEntry->SetTObjectValueL(KMPXMediaColDetailSpaceId, 
                    KGlxDataSourceMdeImplementationUid);
            }
        else if ( request->Attributes()[i] == KGlxMediaCollectionInternalStartDate )
            {
            // nothing returned for container but is valid for CPI to request
            }
        else if ( request->Attributes()[i] == KGlxMediaCollectionInternalEndDate )
            {
            // nothing returned for container but is valid for CPI to request
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralDimensions )
            {
            // return zero size for container (thumbnail only used in preview list)
            aEntry->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(0,0));
            }
        else if ( request->Attributes()[i] == KMPXMediaDrmProtected )
            {
            // container itself is not protected
            aEntry->SetTObjectValueL(KMPXMediaDrmProtected, EFalse); 
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralFramecount )
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralFramecount, 1);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSlideshowableContent )
            {
            TGlxFilterProperties filterProperties = iFilterProperties;
            filterProperties.iItemType = EGlxFilterImage;
            filterProperties.iNoDRM = ETrue;
            filterProperties.iExcludeAnimation = ETrue;
            switch (aType)
                {
                case CGlxDataSource::EContainerTypeAlbum:
                case CGlxDataSource::EContainerTypeTag:
                    {
                    QueueObjectQueryL(aContainer->Def(), ETrue, EAttributeQuery,
                            EQueryResultModeCount, TGlxMediaId(aContainer->Id()),
                            request->Attributes()[i], aEntry, filterProperties);
                    break;
                    }
                case CGlxDataSource::EContainerTypeMonth:
                    {
                    filterProperties.iOrigin = EGlxFilterOriginAll;               
                    AddMonthFilterL(aContainer, filterProperties);
                    QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery,
                            EQueryResultModeCount, TGlxMediaId(KGlxCollectionRootId),
                            request->Attributes()[i], aEntry, filterProperties);
                    break;
                    }
                }
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralLastModifiedDate )
            {
            CMdEProperty* time;
            CMdEPropertyDef& timeProperty = aContainer->Def().GetPropertyDefL(
                    KPropertyDefNameLastModifiedDate);
            TInt timeIndex = aContainer->Property(timeProperty, time);
            if( KErrNotFound == timeIndex) 
                {
                User::Leave(KErrCorrupt);
                }
            aEntry->SetTObjectValueL(KGlxMediaGeneralLastModifiedDate, 
                    static_cast<CMdETimeProperty*>(time)->Value());
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralDimensions )
            {
            TSize dimensions(0,0);
            
            aEntry->SetTObjectValueL(KGlxMediaGeneralDimensions, dimensions);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralDRMRightsValid )
        	{
        	aEntry->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid, EGlxDrmRightsValid); 
        	}
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::AddItemAttributesL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::AddItemAttributesL(CMPXMedia* aEntry, 
        CMdEObject* aItem, CGlxDataSource::TItemType aType)
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::AddItemAttributesL()")
    __ASSERT_DEBUG(aEntry, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(aItem, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);

    /// check property defs are valid and type is correct
    for ( TInt i = 0; i < request->Attributes().Count(); i++ )
        {   
        if ( request->Attributes()[i] == KMPXMediaGeneralId )
            {
            aEntry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, 
                    (TMPXItemId)aItem->Id());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralType )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralCategory )
            {
            if( CGlxDataSource::EItemTypeImage == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
                }
            else if( CGlxDataSource::EItemTypeVideo == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXVideo);
                }
            else
                {
                User::Leave(KErrNotSupported);
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralUri )
            {
            aEntry->SetTextValueL(KMPXMediaGeneralUri, aItem->Uri());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralTitle )
            {
            CMdEProperty* title;
            CMdEPropertyDef& titleProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameTitle);
            TInt titleIndex = aItem->Property(titleProperty, title);
            if( KErrNotFound == titleIndex )
                {
                TParsePtrC parser(aItem->Uri());
                aEntry->SetTextValueL(KMPXMediaGeneralTitle, parser.Name());
                }
            else
                {
                aEntry->SetTextValueL(KMPXMediaGeneralTitle, 
                        static_cast<CMdETextProperty*>(title)->Value());
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDate )
            {
            CMdEProperty* time;
            CMdEPropertyDef& timeProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameCreationDate);
            TInt timeIndex = aItem->Property(timeProperty, time);
            if( KErrNotFound == timeIndex) 
                {
                User::Leave(KErrCorrupt);
                }
            aEntry->SetTObjectValueL(KMPXMediaGeneralDate,
                    static_cast<CMdETimeProperty*>(time)->Value());
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralLastModifiedDate )
            {
            CMdEProperty* time;
            CMdEPropertyDef& timeProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameLastModifiedDate);
            TInt timeIndex = aItem->Property(timeProperty, time);
            if( KErrNotFound == timeIndex) 
                {
                User::Leave(KErrCorrupt);
                }
            aEntry->SetTObjectValueL(KGlxMediaGeneralLastModifiedDate, 
                    static_cast<CMdETimeProperty*>(time)->Value());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralSize )
            {
            CMdEProperty* size;
            CMdEPropertyDef& sizeProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameSize);
            TInt sizeIndex = aItem->Property(sizeProperty, size);
            User::LeaveIfError(sizeIndex);
            
            aEntry->SetTObjectValueL(KMPXMediaGeneralSize, size->Uint32ValueL());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDrive )
            {
            TParsePtrC parser(aItem->Uri());
            aEntry->SetTextValueL(KMPXMediaGeneralDrive, parser.Drive());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralMimeType )
            {
            CMdEProperty* mimeType;
            CMdEPropertyDef& mimeTypeProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameItemType);
            TInt mimeTypeIndex = aItem->Property(mimeTypeProperty, mimeType);
			if(mimeTypeIndex == KErrNotFound)
				{
				//MDS retrieves the Mimetype of the file the moment it is notified about 
				//the new file.But in case of new downloaded video file ,MDS is notified about 
				//it the moment download starts. As mimetype is available only after the download completes, 
				//so MDS fails to give the mimetype of the file. But mimetype can also be retrieved 
				//from CContent class once the download completes.
				RBuf mimeTypeData;
				CleanupClosePushL(mimeTypeData);
				mimeTypeData.CreateL(KMaxFileName);
				ContentAccess::CContent* content = ContentAccess::CContent::NewLC(aItem->Uri());				  
				TInt err = content->GetStringAttribute(ContentAccess::EMimeType, mimeTypeData);
				CleanupStack::PopAndDestroy(content);
				if(err != KErrNone)
					{
					mimeTypeData.Copy(KNullDesC);					
					}	
				aEntry->SetTextValueL(KMPXMediaGeneralMimeType,mimeTypeData);
				CleanupStack::PopAndDestroy(&mimeTypeData);									
				}
			else
				{
				aEntry->SetTextValueL(KMPXMediaGeneralMimeType, 
                    static_cast<CMdETextProperty*>(mimeType)->Value());		
				}				
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDuration )
            {
            if( CGlxDataSource::EItemTypeImage == aType )
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralDuration, 0);
                }
            else
                {
                CMdEProperty* duration;
                CMdEPropertyDef& durationProperty = aItem->Def().GetPropertyDefL(
                        KPropertyDefNameDuration);
                TInt durationIndex = aItem->Property(durationProperty, duration);
                if(durationIndex == KErrNotFound)
	                {	                
	                aEntry->SetTObjectValueL(KMPXMediaGeneralDuration,0);	                        
	                }
                else
	                {
	                aEntry->SetTObjectValueL(KMPXMediaGeneralDuration,
                        static_cast<CMdEReal32Property*>(duration)->Value());	
	                }                
                }
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSystemItem)
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralSystemItem, EFalse);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralDimensions )
            {
            TSize dimensions(0,0);
            
            CMdEProperty* xDim;
            CMdEPropertyDef& xDimProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameWidth);
            TInt xDimIndex = aItem->Property(xDimProperty, xDim);
            if( KErrNotFound == xDimIndex )
                {
                //Do nothing
                }
            else
                {
                dimensions.iWidth = static_cast<CMdEUint16Property*>(xDim)->Value();
                }
            
            CMdEProperty* yDim;
            CMdEPropertyDef& yDimProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameHeight);
            TInt yDimIndex = aItem->Property(yDimProperty, yDim);
            if( KErrNotFound == yDimIndex )
                {
                //Do nothing
                }
            else
                {
                dimensions.iHeight = static_cast<CMdEUint16Property*>(yDim)->Value();
                }
            if ( ( dimensions.iWidth == 0 ) || ( dimensions.iHeight == 0 ) )
                {
                if( CGlxDataSource::EItemTypeImage == aType)
                    {
                    // EXIF header is corrupt, must read size from file.
                    CImageDecoder* decoder = NULL;

                    TRAPD(err, decoder = CImageDecoder::FileNewL( 
                            DataSource()->FileServerSession(), aItem->Uri(),
                            CImageDecoder::EOptionNone ));
                    if (err == KErrNone)
                    	{
                    	dimensions = decoder->FrameInfo().iOverallSizeInPixels;
                    	}
                    delete decoder;
                    }
                }
            
            aEntry->SetTObjectValueL(KGlxMediaGeneralDimensions, dimensions);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralFramecount )
            {
            CMdEProperty* framecount;
            TInt fcount = 1;
            // AB test this
            CMdEPropertyDef* framecountProperty = NULL;
            TRAP_IGNORE(framecountProperty = &aItem->Def().GetPropertyDefL(
                    KPropertyDefNameFrameCount));
            if( framecountProperty )
                {
                TInt framecountIndex = aItem->Property(*framecountProperty, framecount);
                if( KErrNotFound != framecountIndex )
                    {
                    fcount = framecount->Int32ValueL();
                    }
                }
             aEntry->SetTObjectValueL(KGlxMediaGeneralFramecount, fcount);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralComment )
            {
            CMdEProperty* comment;
            CMdEPropertyDef& commentProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameComment); /// using Exif Comment field for comment as spec is not clear
            TInt commentIndex = aItem->Property(commentProperty, comment);
            if( KErrNotFound == commentIndex)
                {
                aEntry->SetTextValueL(KMPXMediaGeneralComment, KNullDesC); 
                }
            else
                {
                aEntry->SetTextValueL(KMPXMediaGeneralComment, 
                        static_cast<CMdETextProperty*>(comment)->Value());
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaDrmProtected )
            {
            CMdEProperty* drmProtected;
            CMdEPropertyDef& drmProtectedProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameDRM); 
            TInt drmProtectedIndex = aItem->Property(drmProtectedProperty, drmProtected);
            if( KErrNotFound == drmProtectedIndex)
                {
                aEntry->SetTObjectValueL(KMPXMediaDrmProtected, EFalse); 
                }
            else
                {
                aEntry->SetTObjectValueL(KMPXMediaDrmProtected,
                        static_cast<CMdEBoolProperty*>(drmProtected)->Value());
                }
            }
		else if ( request->Attributes()[i] == KGlxMediaGeneralDRMRightsValid )            
			{
            CMdEProperty* drmProtected;
            CMdEPropertyDef& drmProtectedProperty = aItem->Def().GetPropertyDefL(
                    KPropertyDefNameDRM); 
            TInt drmProtectedIndex = aItem->Property(drmProtectedProperty, drmProtected);
            if( KErrNotFound == drmProtectedIndex)
                {
                aEntry->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid,
                        EGlxDrmRightsValidityUnknown); 
                }
            else
                {
                if( static_cast<CMdEBoolProperty*>(drmProtected)->Value() )
                    {
                    aEntry->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid, 
                            EGlxDrmRightsValidityUnknown); 
                    }
                else
                    {
                    aEntry->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid,
                            EGlxDrmRightsValid); 
                    }
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralCount )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralCount, 1);
            }
        else if ( request->Attributes()[i] == KMPXMediaColDetailSpaceId )
            {
            aEntry->SetTObjectValueL(KMPXMediaColDetailSpaceId, 
                    KGlxDataSourceMdeImplementationUid);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSlideshowableContent )
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralSlideshowableContent, -1);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralLocation)
            {
            // Set the attribute to a TCoordinate initialised to NaN. 
            aEntry->SetTObjectValueL(KGlxMediaGeneralLocation, TCoordinate());
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::AddLocationAttributeToMediaL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::AddLocationAttributeToMediaL(
        CMPXMedia& aMedia, const TItemId& aLocationId)
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::AddLocationAttributeToMediaL()")
    CMdEObject* location = DataSource()->Session().GetObjectL(aLocationId, 
            DataSource()->LocationDef()); 
    if(!location)
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PushL(location);
    CMdEProperty* longitude = NULL;
    CMdEPropertyDef& longitudePropertyDef = location->Def().GetPropertyDefL(
            KPropertyDefNameLongitude);
    TInt longitudeIndex = location->Property(longitudePropertyDef, longitude);
    CMdEProperty* latitude = NULL;
    CMdEPropertyDef& latitudePropertyDef = location->Def().GetPropertyDefL(
            KPropertyDefNameLatitude);
    TInt latitudeIndex = location->Property(latitudePropertyDef, latitude);

    if (longitudeIndex > KErrNotFound && latitudeIndex > KErrNotFound)
        {
        TCoordinate coordinate(static_cast< CMdEReal64Property *>(latitude)->Value(),
                static_cast< CMdEReal64Property *>(longitude)->Value());
        aMedia.SetTObjectValueL(KGlxMediaGeneralLocation, coordinate); 
        }
    
    CleanupStack::PopAndDestroy(location);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::DoHandleAttributeQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::DoHandleAttributeQueryCompletedL()
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::DoHandleAttributeQueryCompletedL()")
    __ASSERT_DEBUG(iQueryAttributes.Count(), Panic(EGlxPanicIllegalState));
    CMdEQuery* query =  iQueries[0];
    
    if( query->ResultMode() == EQueryResultModeItem )
        {
        __ASSERT_DEBUG(( iQueryAttributes[0].iAttribute == 
        KGlxMediaCollectionInternalStartDate || iQueryAttributes[0].iAttribute == 
        KGlxMediaCollectionInternalEndDate), Panic(EGlxPanicIllegalState));
    	CMdEPropertyDef& creationDateDef = DataSource()->ObjectDef().GetPropertyDefL(
    	        KPropertyDefNameCreationDate);
        if (creationDateDef.PropertyType() != EPropertyTime)
        	{
        	User::Leave(KErrCorrupt);
        	}
        TTime startMonth(0);	
        TTime endMonth(0);	
        GLX_DEBUG2("CGlxDataSourceTaskMdeAttributeMde::DoHandleAttributeQueryCompletedL iQueries[0]->Count()=%d", iQueries[0]->Count());    
        TInt timeIndex(0) ;
        if(iQueries[0]->Count() )
            {
            GLX_DEBUG2("CGlxDataSourceTaskMdeAttributeMde::DoHandleAttributeQueryCompletedL query->Count()=%d", query->Count());    
            if(iQueryAttributes[0].iAttribute == KGlxMediaCollectionInternalStartDate)
            	{
                CMdEProperty* startTime;
                CMdEObject& startObject = (CMdEObject&)query->ResultItem(0);
                TInt timeIndex = startObject.Property(creationDateDef, startTime);
                if( KErrNotFound == timeIndex )
                    {
                    User::Leave(KErrCorrupt);
                    }
                startMonth = static_cast<CMdETimeProperty*>(startTime)->Value();
                iQueryAttributes[0].iMedia->SetTObjectValueL(
                        KGlxMediaCollectionInternalStartDate, startMonth);
            	}
            else if(iQueryAttributes[0].iAttribute == KGlxMediaCollectionInternalEndDate)
            	{
                CMdEProperty* endTime;
                CMdEObject& endObject = (CMdEObject&)query->ResultItem(0);
                timeIndex = endObject.Property(creationDateDef, endTime);
                if( KErrNotFound == timeIndex )
                    {
                    User::Leave(KErrCorrupt);
                    }
                endMonth = static_cast<CMdETimeProperty*>(endTime)->Value();
                iQueryAttributes[0].iMedia->SetTObjectValueL(
                        KGlxMediaCollectionInternalEndDate, endMonth);
            	}
            }
        else
            {
            iQueryAttributes[0].iMedia->SetTObjectValueL(
                    KGlxMediaCollectionInternalStartDate, startMonth);
            iQueryAttributes[0].iMedia->SetTObjectValueL(
                    KGlxMediaCollectionInternalEndDate, endMonth);
            }
        }
    else if( EQueryResultModeCount == query->ResultMode() )
        {
        iQueryAttributes[0].iMedia->SetTObjectValueL(
                iQueryAttributes[0].iAttribute, query->Count());
        }
    else
        {
        Panic(EGlxPanicIllegalState);
        }
    iQueryAttributes.Remove(0);
    }       

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::DoHandleLocationQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::DoHandleLocationQueryCompletedL()
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::DoHandleLocationQueryCompletedL()")
    TInt queryResultsCount = iQueries[0]->Count();
    
    for (TInt queryResultsPos = 0; queryResultsPos < queryResultsCount; queryResultsPos++)
        {
        CMdERelation& relation = static_cast<CMdERelation&>(
                iQueries[0]->ResultItem(queryResultsPos));
        CMPXMedia* targetMedia = NULL;
        if (iMediaArray)
            {
            TInt mediaArrayCount = iMediaArray->Count();
            for (TInt mediaArrayPos = 0; mediaArrayPos < mediaArrayCount; mediaArrayPos++)
                {
                CMPXMedia* media = (*iMediaArray)[mediaArrayPos];
                TMPXItemId id =  media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                if (id.iId1 == relation.LeftObjectId())
                    {
                    targetMedia = media;
                    break;
                    }
                }
            }
        else
            {
            targetMedia = iResponse;
#ifdef _DEBUG
            TMPXItemId id =  targetMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
            __ASSERT_DEBUG(id.iId1 == relation.LeftObjectId(), Panic(EGlxPanicLogicError));
#endif
            }
        
            __ASSERT_DEBUG(targetMedia, Panic(EGlxPanicLogicError));
            AddLocationAttributeToMediaL(*targetMedia, relation.RightObjectId());
          
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::DoHandleImageVideoQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::DoHandleImageVideoQueryCompletedL()
	{
    TRACER("CGlxDataSourceTaskMdeAttribute::DoHandleImageVideoQueryCompletedL()")
	CMdEQuery* query = iQueries[0];
	
    TInt queryResultsCount = query->Count();
    GLX_DEBUG2("CGlxDataSourceTaskMdeAttributeMde::DoHandleImageVideoQueryCompletedL()"
            " queryResultsCount=%d", queryResultsCount);
    
    if( ( queryResultsCount == 1 ) && ( !iMediaArray ) )
        {
        CMdEObject& object = static_cast<CMdEObject&>(query->ResultItem(0));
        
        delete iResponse;
        // setiing iResponse to NULL to remove CodeScanner warning
        iResponse = NULL;
        iResponse = CMPXMedia::NewL();
        AddAttributesL(object, iResponse);
        }
    else
        {    
        for (TInt i = 0; i < queryResultsCount; i++)
            {
            CMdEObject& object = static_cast<CMdEObject&>(query->ResultItem(i));
            
            CMPXMedia* entry = CMPXMedia::NewL();
            CleanupStack::PushL(entry);
            iMediaArray->AppendL(entry);
            CleanupStack::Pop(entry);
            AddAttributesL(object, (*iMediaArray)[iMediaArray->Count() - 1]);
            }
        }    
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::QueueLocaitonQueryL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::QueueLocaitonQueryL()
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::QueueLocaitonQueryL()")
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
        
    CMdEQuery* query = DataSource()->Session().NewRelationQueryL(
            *DataSource()->NamespaceDef(), this); 
    CleanupStack::PushL(query);
    
    CMdELogicCondition& rootCondition = query->Conditions();
    CMdERelationCondition& containerRelationCondition = 
    rootCondition.AddRelationConditionL(DataSource()->ContainsLocationDef(),
            ERelationConditionSideRight);
    CMdELogicCondition& itemLogicCondition  = containerRelationCondition.LeftL();
    CMdELogicCondition&  locationLogicCondition = containerRelationCondition.RightL();
    locationLogicCondition.AddObjectConditionL(DataSource()->LocationDef());
    locationLogicCondition.SetOperator(ELogicConditionOperatorOr);
    itemLogicCondition.SetOperator(ELogicConditionOperatorOr);
    
    TInt mediaIdCount = request->MediaIds().Count();
    
    RArray<TItemId> mediaIdArray;
    CleanupClosePushL(mediaIdArray);
    
    for (TInt i = 0; i < mediaIdCount; i++)
        {
        mediaIdArray.AppendL(request->MediaIds()[i].Value());
        }
    
    itemLogicCondition.AddObjectConditionL(mediaIdArray);
    CleanupStack::PopAndDestroy(&mediaIdArray);

    query->SetResultMode(EQueryResultModeItem);     
    
    CleanupStack::Pop(query);
    AppendQueryL(query, ELocationAttributeQuery);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTask::ExecuteQueryL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttributeMde::ExecuteQueryL()
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::ExecuteQueryL()")
    switch (iQueryTypes[0])
        {
        case EAttributeQuery:
            {
            iQueries[0]->FindL(MaxQueryResultsCount(iQueryAttributes[0].iFilterProperties));
            }
        break;
        case EImageVideoQuery: // fall through
        case ELocationAttributeQuery:
            {
            iQueries[0]->FindL();
            }
        break;
        default:
            {
            Panic(EGlxPanicUnsupportedQueryType);
            }
        break;
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::LocationAttributeRequested
// ----------------------------------------------------------------------------
//
TBool CGlxDataSourceTaskMdeAttributeMde::LocationAttributeRequested()
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::LocationAttributeRequested()")
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
    
    TInt attributeArrayCount = request->Attributes().Count();
    for (TInt i = 0; i < attributeArrayCount; i++)
        {
        if (request->Attributes()[i] == KGlxMediaGeneralLocation)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTask::QueueObjectQueryL
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeAttributeMde::QueueObjectQueryL(CMdEObjectDef& aObjectDef,
        TBool aIsContent, TGlxQueryType aQueryType, 
        TQueryResultMode aResultMode, const TGlxMediaId& aContainerId, 
        TMPXAttribute aAttribute, CMPXMedia* aEntry, 
        const TGlxFilterProperties& aFilterProperties)
    {
    TRACER("CGlxDataSourceTaskMdeAttribute::QueueObjectQueryL()")   
    CMdEObjectDef* queryBaseObject = &aObjectDef;
    if( aIsContent )
        {
        switch(aFilterProperties.iItemType)
            {
            case EGlxFilterImage: 
                {
                queryBaseObject = &DataSource()->ImageDef();
                break;
                }
            case EGlxFilterVideo:
                {
                queryBaseObject = &DataSource()->VideoDef();
                break;
                }
            default:
                {
                queryBaseObject = &DataSource()->ObjectDef();
                break;
                }
            }
        }

    CMdEQuery* query = DataSource()->Session().NewObjectQueryL(*DataSource()->NamespaceDef(), 
            *queryBaseObject, this);
    CleanupStack::PushL(query);
    query->SetResultMode(aResultMode);
    SetQueryConditionsL(*query, aFilterProperties, aContainerId, aObjectDef);
  
    iQueryAttributes.AppendL(TGlxQueryAttribute(aAttribute, aEntry, aFilterProperties));

    CleanupStack::Pop(query);
    
   	TRAPD(err, AppendQueryL(query,aQueryType));
   	
   	if (err != KErrNone)
   		{
   		iQueryAttributes.Remove(iQueryAttributes.Count() - 1);
   		}
    }
