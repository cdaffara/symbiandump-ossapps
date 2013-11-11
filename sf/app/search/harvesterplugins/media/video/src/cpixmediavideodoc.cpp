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
* Description:  MDS to CPIX Audio Document fetcher 
*
*/


#include "cpixmediavideodoc.h"
#include <mdeobject.h>
#include <mdccommon.h>
#include <mdeitem.h>
#include <mdeconstants.h>
#include <mdeobjectdef.h>
#include "harvesterserverlogger.h"
#include "OstTraceDefinitions.h"
#include "csearchdocument.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpixmediavideodocTraces.h"
#endif

_LIT( KFormatDateTime, "%04d %02d %02d %02d %02d");    // yyyy mm dd hh mm
_LIT( KExcerptFormat, "%04d/%02d/%02d %02d:%02d");   //yyyy/mm/dd hh:mm
// -----------------------------------------------------------------------------
// CCPIXDocFetcher::NewL()
// -----------------------------------------------------------------------------
//
CCPIXMediaVideoDoc* CCPIXMediaVideoDoc::NewL()
    {
    CCPIXMediaVideoDoc* self = CCPIXMediaVideoDoc::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::NewLC()
// -----------------------------------------------------------------------------
//
CCPIXMediaVideoDoc* CCPIXMediaVideoDoc::NewLC()
    {
    CCPIXMediaVideoDoc* self = new (ELeave) CCPIXMediaVideoDoc();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::~CCPIXMediaAudioDoc()
// -----------------------------------------------------------------------------
//
CCPIXMediaVideoDoc::~CCPIXMediaVideoDoc()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::CCPIXMediaAudioDoc()
// -----------------------------------------------------------------------------
//
CCPIXMediaVideoDoc::CCPIXMediaVideoDoc()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::ConstructL()
// -----------------------------------------------------------------------------
//
void CCPIXMediaVideoDoc::ConstructL()
    {
    CCPIXDocFetcher::ConstructL();//Must have
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::GetCpixDocumentL()
// -----------------------------------------------------------------------------
//
CSearchDocument* CCPIXMediaVideoDoc::GetCpixDocumentL(const CMdEObject& aObject, 
                                              const TDesC& aAppClass,
                                              CMdEObjectDef& aObjectDef)
    {
    OstTraceFunctionEntry0( CCPIXMEDIAVIDEODOC_GETCPIXDOCUMENTL_ENTRY );
    //Get basic document
    CPIXLOGSTRING("CCPIXMediaVideoDoc::GetCpixDocumentL()");
    CSearchDocument* index_item = CCPIXDocFetcher::GetCpixDocumentL(aObject,aAppClass,aObjectDef);
    ResetExcerpt(); //Reset excerpt initially
    TInt slashpos = GetUri().LocateReverse('\\');
    TPtrC name = GetUri().Mid( (slashpos+1) );

    CMdEProperty* property(NULL);
    //Add Name field to document excerpt, ONLY if title field is not empty.
    CMdEPropertyDef& titlePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Object::KTitleProperty );
    if(aObject.Property( titlePropDef, property ) != KErrNotFound)
        {
        CMdETextProperty* textProperty = ( CMdETextProperty* ) property;
        if( textProperty->Value().Compare(KNullDesC) > 0 )
            {
//#ifdef USE_HIGHLIGHTER    
            index_item->AddHLDisplayFieldL(textProperty->Value());
            AddToFieldExcerptL(name); //Add name to excerpt field
//#else  
//            AddToFieldExcerptL(name); //Add name to excerpt field
//#endif
            }
            else
            {
//#ifdef USE_HIGHLIGHTER    
            index_item->AddHLDisplayFieldL(name);
//#endif
            }
        }
    //URI and Excerpt is done add additional properties here 
    
    CMdEPropertyDef& artistPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KArtistProperty );
    if(aObject.Property( artistPropDef, property )!= KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                           MdeConstants::MediaObject::KArtistProperty,
                           textProperty->Value(),
                           CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
       AddToFieldExcerptL(textProperty->Value());
       }
    CMdEPropertyDef& authorPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KAuthorProperty );
    if(aObject.Property( authorPropDef, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                         MdeConstants::MediaObject::KAuthorProperty,
                         textProperty->Value(),
                         CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
       AddToFieldExcerptL(textProperty->Value());
       }      
    
    CMdEPropertyDef& commentPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KCommentProperty );
    if(aObject.Property( commentPropDef, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                   MdeConstants::MediaObject::KCommentProperty,
                   textProperty->Value());
       AddToFieldExcerptL(textProperty->Value());
       }
    
    //Get description property
    CMdEPropertyDef& descriprionPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KDescriptionProperty );
    if(aObject.Property( descriprionPropDef, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                   MdeConstants::MediaObject::KDescriptionProperty,
                   textProperty->Value());
       AddToFieldExcerptL(textProperty->Value());
       }
    
    CMdEPropertyDef& copyrightPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KCopyrightProperty );
    if(aObject.Property( copyrightPropDef, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                      MdeConstants::MediaObject::KCopyrightProperty,
                      textProperty->Value());
       AddToFieldExcerptL(textProperty->Value());
       } 
    //Get genre property
    CMdEPropertyDef& genrePropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KGenreProperty );
    if(aObject.Property( genrePropDef, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                   MdeConstants::MediaObject::KGenreProperty,
                   textProperty->Value());
       AddToFieldExcerptL(textProperty->Value());
       }
    //Size property
    CMdEPropertyDef& sizePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Object::KSizeProperty);
    if(aObject.Property( sizePropDef, property )!= KErrNotFound)
       {
       //Add field to document       
       CMdEUint32Property& sizeProperty = static_cast < CMdEUint32Property& > (*property );
       TBuf<32> buf;
       buf.Format(_L("%u"), sizeProperty.Value());
       AddFiledtoDocumentL(*index_item,
                       MdeConstants::Object::KSizeProperty,
                       buf,
                       CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
       AddToFieldExcerptL(buf);
       }    
    //Get resolution property
    CMdEPropertyDef& resolutionPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KResolutionUnitProperty );
    if(aObject.Property( resolutionPropDef, property ) != KErrNotFound)
       {
       //Add field to document       
       CMdEUint16Property& resolutionProperty = static_cast < CMdEUint16Property& > (*property );               
       TBuf<32> buf;
       buf.Format(_L("%d"), resolutionProperty.Value());
       AddFiledtoDocumentL(*index_item,
                          MdeConstants::MediaObject::KResolutionUnitProperty,
                          buf,
                          CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
       
       }    
    //Get legal property
    CMdEPropertyDef& legalPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KLegalProperty );
    if(aObject.Property( legalPropDef, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                   MdeConstants::MediaObject::KLegalProperty,
                   textProperty->Value());
       AddToFieldExcerptL(textProperty->Value());
       }
    //Get track property
    CMdEPropertyDef& trackPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KTrackProperty );
    if(aObject.Property( trackPropDef, property ) != KErrNotFound)
       {
       //Add field to document       
       CMdEUint16Property& trackProperty = static_cast < CMdEUint16Property& > (*property );               
       TBuf<32> buf;
       buf.Format(_L("%d"), trackProperty.Value());
       AddFiledtoDocumentL(*index_item,
                          MdeConstants::MediaObject::KTrackProperty,
                          buf);
       
       }
    // Capturedate property
    CMdEPropertyDef& captureDatePropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KCaptureDateProperty);
    if(aObject.Property( captureDatePropDef, property ) != KErrNotFound)
      {
      //Add field to document
      CMdETimeProperty& timeProperty = static_cast < CMdETimeProperty& > (*property);
      TDateTime time = timeProperty.Value().DateTime();
      TBuf<32> buf;
      buf.Format( KFormatDateTime, time.Year(), 
                                   time.Month() + 1, 
                                   time.Day() + 1, 
                                   time.Hour(), 
                                   time.Minute());
      
      AddFiledtoDocumentL(*index_item,
                         MdeConstants::MediaObject::KCaptureDateProperty,
                         buf,
                         CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
      //Format for excerpt field
      buf.Format( KExcerptFormat, time.Year(), 
                             time.Month() + 1, 
                             time.Day() + 1, 
                             time.Hour(), 
                             time.Minute());
      AddToFieldExcerptL(buf);
      }
    //Get LastModifiedDate property
    CMdEPropertyDef& dateTimePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Object::KLastModifiedDateProperty);
    if(aObject.Property( dateTimePropDef, property ) != KErrNotFound)
      {
      //Add field to document      
      CMdETimeProperty& timeProperty = static_cast < CMdETimeProperty& > (*property);
      TDateTime time = timeProperty.Value().DateTime();
      TBuf<32> buf;
      buf.Format( KFormatDateTime, time.Year(),
                                   time.Month() + 1,
                                   time.Day() + 1,
                                   time.Hour(),
                                   time.Minute());
      
      AddFiledtoDocumentL(*index_item,
                         MdeConstants::Object::KLastModifiedDateProperty,
                         buf,
                         CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
      //Format for excerpt field
      buf.Format( KExcerptFormat, time.Year(), 
                               time.Month() + 1, 
                               time.Day() + 1, 
                               time.Hour(), 
                               time.Minute());
      AddToFieldExcerptL(buf);
      }
    //Get duration property
    CMdEPropertyDef& durationPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KDurationProperty );
    if(aObject.Property( durationPropDef, property ) != KErrNotFound)
       {
       //Add field to document       
       CMdEReal32Property& durationProperty = static_cast < CMdEReal32Property& > (*property );               
       TBuf<32> buf;
       buf.Format(_L("%f"), durationProperty.Value());
       AddFiledtoDocumentL(*index_item,
                          MdeConstants::MediaObject::KDurationProperty,
                          buf,
                          CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
       }
    if( iExcerpt )
        {
        index_item->AddExcerptL(*iExcerpt);
        }
    OstTraceFunctionExit0( CCPIXMEDIAVIDEODOC_GETCPIXDOCUMENTL_EXIT );
    return index_item;
    }
