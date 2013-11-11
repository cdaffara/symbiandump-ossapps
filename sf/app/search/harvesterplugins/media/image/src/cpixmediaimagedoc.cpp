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
* Description:  MDS to CPIX Image Document fetcher 
 *
*/


#include "cpixmediaimagedoc.h"
#include <mdeobject.h>
#include <mdccommon.h>
#include <mdeitem.h>
#include <mdeconstants.h>
#include <mdeobjectdef.h>
#include "harvesterserverlogger.h"
#include "OstTraceDefinitions.h"
#include "csearchdocument.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpixmediaimagedocTraces.h"
#endif


_LIT( KFormatDateTime, "%04d %02d %02d %02d %02d");    // yyyy mm dd hh mm
_LIT( KExcerptFormat, "%04d/%02d/%02d %02d:%02d");   //yyyy/mm/dd hh:mm
// -----------------------------------------------------------------------------
// CCPIXDocFetcher::NewL()
// -----------------------------------------------------------------------------
//
CCPIXMediaImageDoc* CCPIXMediaImageDoc::NewL()
    {
    CCPIXMediaImageDoc* self = CCPIXMediaImageDoc::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::NewLC()
// -----------------------------------------------------------------------------
//
CCPIXMediaImageDoc* CCPIXMediaImageDoc::NewLC()
    {
    CCPIXMediaImageDoc* self = new (ELeave) CCPIXMediaImageDoc();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::~CCPIXMediaImageDoc()
// -----------------------------------------------------------------------------
//
CCPIXMediaImageDoc::~CCPIXMediaImageDoc()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::CCPIXMediaImageDoc()
// -----------------------------------------------------------------------------
//
CCPIXMediaImageDoc::CCPIXMediaImageDoc()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::ConstructL()
// -----------------------------------------------------------------------------
//
void CCPIXMediaImageDoc::ConstructL()
    {
    CCPIXDocFetcher::ConstructL();//Must have
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::GetCpixDocumentL()
// -----------------------------------------------------------------------------
//
CSearchDocument* CCPIXMediaImageDoc::GetCpixDocumentL(const CMdEObject& aObject, 
                                              const TDesC& aAppClass,
                                              CMdEObjectDef& aObjectDef)
    {
    OstTraceFunctionEntry0( CCPIXMEDIAIMAGEDOC_GETCPIXDOCUMENTL_ENTRY );
    //Get basic document
    CPIXLOGSTRING("START CCPIXMediaImageDoc::GetCpixDocumentL");
    CSearchDocument* index_item = CCPIXDocFetcher::GetCpixDocumentL(aObject,aAppClass,aObjectDef);
    ResetExcerpt(); //Reset excerpt initially
    //URI and Excerpt is done add additional properties here
    CMdEProperty* property(NULL);
    
    CMdEPropertyDef& titlePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Object::KTitleProperty );
       if(aObject.Property( titlePropDef, property ) != KErrNotFound)
          {
          //Add field to document excerpt.
          CMdETextProperty* textProperty = ( CMdETextProperty* ) property;
          AddToFieldExcerptL(textProperty->Value());
          }
    
//#ifdef USE_HIGHLIGHTER
       TInt slashpos = GetUri().LocateReverse('\\');
       TPtrC name = GetUri().Mid( (slashpos+1) );
       index_item->AddHLDisplayFieldL(name);
//#endif   
    //Get user comment field
    CMdEPropertyDef& commentPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KCommentProperty );
    if(aObject.Property( commentPropDef, property )!= KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                           MdeConstants::MediaObject::KCommentProperty,
                           textProperty->Value());
       AddToFieldExcerptL(textProperty->Value());
       }
    
    //Get user description field
    CMdEPropertyDef& descriptionPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KDescriptionProperty );
    if(aObject.Property( descriptionPropDef, property )!= KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                           MdeConstants::MediaObject::KDescriptionProperty,
                           textProperty->Value());
       AddToFieldExcerptL(textProperty->Value());
       }
    
    //Get user copyright field
    CMdEPropertyDef& copyRightPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KCopyrightProperty );
    if(aObject.Property( copyRightPropDef, property )!= KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                           MdeConstants::MediaObject::KCopyrightProperty,
                           textProperty->Value());
       AddToFieldExcerptL(textProperty->Value());
       }

    //Get user genre field
    CMdEPropertyDef& genrePropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KGenreProperty );
    if(aObject.Property( genrePropDef, property )!= KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                           MdeConstants::MediaObject::KGenreProperty,
                           textProperty->Value()); 
       AddToFieldExcerptL(textProperty->Value());
       }

    //Get legal property field
    CMdEPropertyDef& legalPropDef = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KLegalProperty );
    if(aObject.Property( legalPropDef, property )!= KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                           MdeConstants::MediaObject::KLegalProperty,
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
    //Get DateTimeOrginal property
    CMdEPropertyDef& dateTimeOrgPropDef = aObjectDef.GetPropertyDefL(MdeConstants::Image::KDateTimeOriginalProperty);
    if(aObject.Property( dateTimeOrgPropDef, property ) != KErrNotFound)
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
                         MdeConstants::Image::KDateTimeOriginalProperty,
                         buf,
                         CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
      //Format for excerpt field
      buf.Format( KExcerptFormat, time.Year(), 
                               time.Month() + 1, 
                               time.Day() + 1, 
                               time.Hour(), 
                               time.Minute());

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
    if( iExcerpt )
        {
        index_item->AddExcerptL(*iExcerpt);
        }

    //Latitude error currently 
    /* TODO- Bhuvi Location is a seperate object for each image object if any such relation
     * exist it has to be searched from Relation table and from there we have to open 
     * location object and query Longitude and Latitudes
     */
    /*
    CMdEPropertyDef& latitudePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Location::KLatitudeProperty);
    if(aObject.Property( latitudePropDef, property ) != KErrNotFound)
      {
      TBuf<32> buf;
      TRealFormat format(32,16); //Real format
      format.iTriLen = 0;
      //Add field to document
      if(EPropertyReal32 == property->Def().PropertyType())
          {
          CMdEReal32Property& real32Property = (CMdEReal32Property&) property;
          buf.Num( real32Property.Value(), format );
          }
      else
          if(EPropertyReal64 == property->Def().PropertyType())
          {
          CMdEReal64Property& real64Property = (CMdEReal64Property&) property;
          buf.Num( real64Property.Value(), format );
          }
      AddFiledtoDocumentL(*index_item,
                         MdeConstants::Location::KLatitudeProperty,
                         buf,
                         CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
      }
    //Longitude
    CMdEPropertyDef& longitudePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Location::KLongitudeProperty);
    if(aObject.Property( longitudePropDef, property ) != KErrNotFound)
      {
      TBuf<32> buf;
      TRealFormat format(32,16); //Real format
      format.iTriLen = 0;
      //Add field to document
      if(EPropertyReal32 == property->Def().PropertyType())
          {
          CMdEReal32Property& real32Property = (CMdEReal32Property&) property;
          buf.Num( real32Property.Value(), format );
          }
      else
          if(EPropertyReal64 == property->Def().PropertyType())
          {
          CMdEReal64Property& real64Property = (CMdEReal64Property&) property;
          buf.Num( real64Property.Value(), format );
          }
      AddFiledtoDocumentL(*index_item,
                         MdeConstants::Location::KLongitudeProperty,
                         buf,
                         CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
      }
      */
    CPIXLOGSTRING("END CCPIXMediaImageDoc::GetCpixDocumentL");
    OstTraceFunctionExit0( CCPIXMEDIAIMAGEDOC_GETCPIXDOCUMENTL_EXIT );
    return index_item;
    }
//End of life
