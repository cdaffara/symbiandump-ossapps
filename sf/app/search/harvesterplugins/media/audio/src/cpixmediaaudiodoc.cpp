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


#include "cpixmediaaudiodoc.h"
#include <mdeobject.h>
#include <mdccommon.h>
#include <mdeitem.h>
#include <mdeconstants.h>
#include <mdeobjectdef.h>
#include "harvesterserverlogger.h"
#include "OstTraceDefinitions.h"
#include "csearchdocument.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpixmediaaudiodocTraces.h"
#endif

_LIT( KFormatDateTime, "%04d %02d %02d %02d %02d");    // yyyy mm dd hh mm
_LIT( KExcerptFormat, "%04d/%02d/%02d %02d:%02d");
// -----------------------------------------------------------------------------
// CCPIXDocFetcher::NewL()
// -----------------------------------------------------------------------------
//
CCPIXMediaAudioDoc* CCPIXMediaAudioDoc::NewL()
    {
    CCPIXMediaAudioDoc* self = CCPIXMediaAudioDoc::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::NewLC()
// -----------------------------------------------------------------------------
//
CCPIXMediaAudioDoc* CCPIXMediaAudioDoc::NewLC()
    {
    CCPIXMediaAudioDoc* self = new (ELeave) CCPIXMediaAudioDoc();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::~CCPIXMediaAudioDoc()
// -----------------------------------------------------------------------------
//
CCPIXMediaAudioDoc::~CCPIXMediaAudioDoc()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::CCPIXMediaAudioDoc()
// -----------------------------------------------------------------------------
//
CCPIXMediaAudioDoc::CCPIXMediaAudioDoc()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::ConstructL()
// -----------------------------------------------------------------------------
//
void CCPIXMediaAudioDoc::ConstructL()
    {
    CCPIXDocFetcher::ConstructL();//Must have
    
    }

// -----------------------------------------------------------------------------
// CCPIXDocFetcher::GetCpixDocumentL()
// -----------------------------------------------------------------------------
//
CSearchDocument* CCPIXMediaAudioDoc::GetCpixDocumentL(const CMdEObject& aObject, 
                                              const TDesC& aAppClass,
                                              CMdEObjectDef& aObjectDef)
    {
    OstTraceFunctionEntry0( CCPIXMEDIAAUDIODOC_GETCPIXDOCUMENTL_ENTRY );
    CPIXLOGSTRING("START CCPIXMediaAudioDoc::GetCpixDocumentL");
    //Get basic document
    CSearchDocument* index_item = CCPIXDocFetcher::GetCpixDocumentL(aObject,aAppClass,aObjectDef);
    //Reset Excerpt and append
    ResetExcerpt();
    TInt slashpos = GetUri().LocateReverse('\\');
    TPtrC name = GetUri().Mid( (slashpos+1) );
    AddToFieldExcerptL(name); //Add name to excerpt field
    //additional properties here 
    CMdEProperty* property(NULL);
    CMdEPropertyDef& artistPropDef1 = aObjectDef.GetPropertyDefL(MdeConstants::MediaObject::KArtistProperty );
    if(aObject.Property( artistPropDef1, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                             MdeConstants::MediaObject::KArtistProperty,
                             textProperty->Value(),
                             CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
       AddToFieldExcerptL(textProperty->Value());//Add artist to excerpt
       }
//#ifdef USE_HIGHLIGHTER
    /*//Get the media file extension and store
    TBuf<KMaxExtLength> extension;        
    GetExtension(aObject.Uri(),extension);
    AddToFieldExcerptL(extension);*/
    //Adding title to first line of highlighter
    CMdEPropertyDef& titlePropDef = aObjectDef.GetPropertyDefL(MdeConstants::Object::KTitleProperty );
    if(aObject.Property( titlePropDef, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       if(textProperty->Value() != KNullDesC)
           {
           index_item->AddHLDisplayFieldL(textProperty->Value());
           }
       }
//#endif  
    CMdEPropertyDef& albumPropDef = aObjectDef.GetPropertyDefL(MdeConstants::Audio::KAlbumProperty );
    if(aObject.Property( albumPropDef, property )!= KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property);
       AddFiledtoDocumentL(*index_item,
                           MdeConstants::Audio::KAlbumProperty,
                           textProperty->Value(),
                           CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
       AddToFieldExcerptL(textProperty->Value());//Add Album to excerpt
       }
    
    CMdEPropertyDef& artistPropDef = aObjectDef.GetPropertyDefL(MdeConstants::Audio::KAlbumArtistProperty );
    if(aObject.Property( artistPropDef, property ) != KErrNotFound)
       {
       //Add field to document
       CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
       AddFiledtoDocumentL(*index_item,
                     MdeConstants::Audio::KAlbumArtistProperty,
                     textProperty->Value(),
                     CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
       AddToFieldExcerptL(textProperty->Value());//Add Albumartist to excerpt
       }
    //orginal artist property
    CMdEPropertyDef& origartistPropDef = aObjectDef.GetPropertyDefL(MdeConstants::Audio::KOriginalArtistProperty );
    if(aObject.Property( origartistPropDef, property ) != KErrNotFound)
     {
     //Add field to document
     CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
     AddFiledtoDocumentL(*index_item,
                      MdeConstants::Audio::KOriginalArtistProperty,
                      textProperty->Value(),
                      CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
     AddToFieldExcerptL(textProperty->Value());//Add Original to excerpt
     }
    //Get composer property
    CMdEPropertyDef& composerPropDef = aObjectDef.GetPropertyDefL(MdeConstants::Audio::KComposerProperty );
    if(aObject.Property( composerPropDef, property ) != KErrNotFound)
    {
    //Add field to document
    CMdETextProperty* textProperty = static_cast< CMdETextProperty* > (property );
    AddFiledtoDocumentL(*index_item,
                  MdeConstants::Audio::KComposerProperty,
                  textProperty->Value(),
                  CDocumentField::EStoreYes | CDocumentField::EIndexTokenized | CDocumentField::EIndexFreeText);
    AddToFieldExcerptL(textProperty->Value());//Add composer to excerpt
    }
    //Get author property
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
     //Get comment property   
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
    //Copyright property    
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
                      buf );
       
       }
    
    index_item->AddExcerptL(*iExcerpt);

    CPIXLOGSTRING("END CCPIXMediaAudioDoc::GetCpixDocumentL");
    OstTraceFunctionExit0( CCPIXMEDIAAUDIODOC_GETCPIXDOCUMENTL_EXIT );
    return index_item;
    }
//End of life
