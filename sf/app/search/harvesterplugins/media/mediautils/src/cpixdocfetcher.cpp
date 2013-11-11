/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MDS to CPIX Document fetcher 
 *
*/


#include "cpixdocfetcher.h"
#include "csearchdocument.h"
#include <mdeobject.h>
#include <mdccommon.h>
#include <mdeitem.h>
#include <mdeconstants.h>
#include <mdeobjectdef.h>
#include "harvesterserverlogger.h"


// -----------------------------------------------------------------------------
// CCPIXDocFetcher::NewL()
// -----------------------------------------------------------------------------
//
CCPIXDocFetcher* CCPIXDocFetcher::NewL()
    {
    CCPIXDocFetcher* self = CCPIXDocFetcher::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::NewLC()
// -----------------------------------------------------------------------------
//
CCPIXDocFetcher* CCPIXDocFetcher::NewLC()
    {
    CCPIXDocFetcher* self = new (ELeave) CCPIXDocFetcher();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::~CCPIXDocFetcher()
// -----------------------------------------------------------------------------
//
CCPIXDocFetcher::~CCPIXDocFetcher()
    {
    iFs.Close();
    delete iExcerpt;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::~ConstructL() 
// -----------------------------------------------------------------------------
//
void CCPIXDocFetcher::ConstructL()
    {
    //Heap variables if any
    User::LeaveIfError(iFs.Connect());
    iExcerpt = NULL;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::CCPIXDocFetcher() 
// -----------------------------------------------------------------------------
//
CCPIXDocFetcher::CCPIXDocFetcher()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::ResetExcerpt() 
// -----------------------------------------------------------------------------
//
void CCPIXDocFetcher::ResetExcerpt()
    {
    if(iExcerpt)
        {
        delete iExcerpt;
        iExcerpt = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::GetCpixDocumentL() 
// -----------------------------------------------------------------------------
//
CSearchDocument* CCPIXDocFetcher::GetCpixDocumentL(const CMdEObject& aObject,
                                                   const TDesC& aAppClass,
                                                   CMdEObjectDef& aObjectDef)
    {
    CPIXLOGSTRING("START CCPIXDocFetcher::GetCpixDocumentL");
    CSearchDocument* index_item = NULL;
    if(aObject.Uri() != KNullDesC) //Uri is must have property we will store items which has URI
        {
        SetAbsoluteURI(aObject.Uri());//Convert URI to absolute path 
        index_item = CSearchDocument::NewLC(GetUri(), aAppClass);
        
        TInt slashpos = GetUri().LocateReverse('\\');
        
        TPtrC name = GetUri().Mid( (slashpos+1) );
        //Uri is our Document ID
        AddFiledtoDocumentL(*index_item,
                               KNameField, //URI as Name field
                               name,
                               CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
        //Store media ID for client to generate path and launch corresponding Item
        TBuf<KMaxMediaLength> mediaBuf;        
        
        //As the MDS database is storing incorrect MediaIds, fetch the uniqueid from the 
        //file system.If the issue is fixed in 10.1 then Ids from MDS can be directly used
        
        //GetMediaIdDescriptor(aObject.MediaId(),mediaBuf);
        GetMediaIdDescriptorFromFsL(aObject.Uri(),mediaBuf);
        //Uri is our Document ID
        AddFiledtoDocumentL(*index_item, KMediaIdField, mediaBuf,
                             CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
        CPIXLOGSTRING("CCPIXDocFetcher::GetCpixDocumentL Standard Property URI, Media ID Added");
        
        //Get the media file extension and store
        TBuf<KMaxExtLength> extension;        
        GetExtension(aObject.Uri(),extension);
        AddFiledtoDocumentL( *index_item, KExtensionField, extension,
                CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
        
        CMdEProperty* property(NULL);
        CMdEPropertyDef& titlePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Object::KTitleProperty );
        /*Requirement Excerpt should have all item specific field in priority order so has to be handled
         *at plugin specific document handler
         */
        if(aObject.Property( titlePropDef, property ) != KErrNotFound)
           {
           //Add field to document
           CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
           if(textProperty->Value() != KNullDesC)
               {
               AddFiledtoDocumentL(*index_item,
                                     MdeConstants::Object::KTitleProperty,
                                     textProperty->Value(),
                                     CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
               }
           }
        
        //Item type as MIME type
        CMdEPropertyDef& mimeTypePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Object::KItemTypeProperty);
        if(aObject.Property( mimeTypePropDef, property )!= KErrNotFound)
           {
           //Add field to document
           CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
           AddFiledtoDocumentL(*index_item,
                               _L(CPIX_MIMETYPE_FIELD),
                               textProperty->Value(),
                               CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
           }
        //Get ratings field
        CMdEPropertyDef& ratingsPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KRatingProperty );
        if(aObject.Property( ratingsPropDef, property )!= KErrNotFound)
           {
           //Add field to document
           if( property->Def().PropertyType() == EPropertyInt32 )
               {
               CMdEInt32Property& ratingProperty = static_cast < CMdEInt32Property& > (*property );               
               TBuf<32> buf;
               buf.Format(_L("%d"), ratingProperty.Value());
               AddFiledtoDocumentL(*index_item,
                                  MdeConstants::MediaObject::KRatingProperty,
                                  buf );
               }
           }
        CleanupStack::Pop(index_item);//pop up
        }    
    CPIXLOGSTRING("END CCPIXDocFetcher::GetCpixDocumentL");
    return index_item;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::GetUri() 
// -----------------------------------------------------------------------------
//
const TDesC& CCPIXDocFetcher::GetUri()
    {
    return iUri;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::GetMediaIdDescriptor() 
// -----------------------------------------------------------------------------
//
void CCPIXDocFetcher::GetMediaIdDescriptor(const TUint32& aMediaId,
                                           TDes& aMediaIDBuf)
    {
    aMediaIDBuf.Format(_L("%d"),aMediaId);
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::GetMediaIdDescriptorFromFs() 
// -----------------------------------------------------------------------------
//
void CCPIXDocFetcher::GetMediaIdDescriptorFromFsL(const TDesC& aUri,
                                           TDes& aMediaIDBuf)
    {
    TVolumeInfo volumeInfo;
    TInt driveNumber;
    TChar driveLetter;
    driveLetter = aUri[KDrivePosition];    
    User::LeaveIfError(iFs.CharToDrive(driveLetter,driveNumber));
    User::LeaveIfError(iFs.Volume(volumeInfo,driveNumber));
    aMediaIDBuf.Format(KFormat, volumeInfo.iUniqueID);    
    }

void CCPIXDocFetcher::GetExtension(const TDesC& aUri,
                              TDes& aExtension)
    {
    TInt pos = aUri.LocateReverse('.');
    if(pos > 0)
        {
         aExtension.Copy(aUri.Mid(pos+1));
        }
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::AddToFieldExcerptL() 
// -----------------------------------------------------------------------------
//
void CCPIXDocFetcher::AddToFieldExcerptL(const TDesC& aExcerptValue)
    {
    if(!iExcerpt)
        {
        iExcerpt = HBufC::NewL(KInitialExcerptLength);
        }
    if(aExcerptValue.Compare(KNullDesC) != 0)//value is not Null
        {
        TInt currentSize = iExcerpt->Size();
        TInt newSize = currentSize + aExcerptValue.Size() + 1;
        if(newSize > currentSize) //New size is bigger so we have to reallocate
            {
            iExcerpt = iExcerpt->ReAllocL(newSize);
            }
        TPtr ptr = iExcerpt->Des();
        ptr.Append(aExcerptValue);
        ptr.Append(KExcerptDelimiter);
        }
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::SetAbsoluteURI() 
// -----------------------------------------------------------------------------
//
void CCPIXDocFetcher::SetAbsoluteURI(const TDesC& aUri)
    {
    iUri.Copy(aUri.Mid(KStartPosition));
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::AddFiledtoDocumentL() 
// -----------------------------------------------------------------------------
//
void CCPIXDocFetcher::AddFiledtoDocumentL(CSearchDocument& aSearchDoc,
                                          const TDesC& aFiledName,
                                          const TDesC& aValue,
                                          TInt aConfig)
    {
    if(aValue != KNullDesC)
        aSearchDoc.AddFieldL(aFiledName, aValue, aConfig);
    else
        aSearchDoc.AddFieldL(aFiledName, KNullDesC, aConfig);    
    }
//End of life
