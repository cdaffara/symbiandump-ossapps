/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CUniSmilModel, Storage for Smil presentation data.
*
*
*/


// ========== INCLUDE FILES ================================


#include <e32def.h>
#include <eikenv.h>
#include <eikfutil.h>               // CopyFile

#include <mtclbase.h>   // for CBaseMtm
#include <msvstd.h>     // for TMsvId
#include <msvids.h>     // for KMsvTempIndexEntryId
#include <msvstore.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>

#include <bautils.h>                // NearestLanguageFile

#include <coemain.h>

// LAF
/*
#include <AknUtils.h>
#include <AknLayoutScalable_Avkon.cdl.h>
#include <AknLayoutScalable_Apps.cdl.h>
#include <LayoutMetadata.cdl.h>
*/

// DOM
#include <gmxmldocument.h>
#include <gmxmlnode.h>
#include <gmxmlelement.h>

// MsgMedia
#include <MsgMedia.hrh>
#include <MsgMediaInfo.h>
#include <MmsConformance.h>
#include <mmsversion.h>
#include <MsgTextInfo.h>


// MsgDataUTILS
#include <unidatamodel.rsg>         // Viewer/Editor Mimetype strings
#include "UniModelConst.h"              // Definitions and enums
#include "UniDataUtils.h"
#include "UniSmilSlide.h"
#include "UniSmilModel.h"
#include "UniObjectList.h"
#include "UniObject.h"
#include "UniTextObject.h"
#include "UniMimeInfo.h"
#include "UniSmilUtils.h"
#include "UniSmilParams.h"

#include "fileprotectionresolver.h"     //     EFileProtDrm2
#include "MsgMimeTypes.h"

// Logging
//#include "//MsgDataUTILSLOGGing.h"


// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

_LIT( KSmilIntFormat, "%d" );
_LIT( KSmilMsecFormat, "%dms" );
const TUint KSlideArrayGranularity( 16 );

// SMIL size estimates
const TInt KSizeMandatorySmilTags = 148;
const TInt KSizeHeadOneRegion = 85;
const TInt KSizeHeadTwoRegions = 170;
const TInt KSizeEmptyPar = 27;
const TInt KSizeNonEmptyPar = 41;
const TInt KSizeAudioTag = 38;
const TInt KSizeImageTag = 52;
const TInt KSizeTextTag = 51;
const TInt KSizeVideoTag = 53;

const TInt KSizeLowResHeight = 208;
const TInt KSizeLowResWidth = 176;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================


// ---------------------------------------------------------
// CUniSmilModel::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniSmilModel* CUniSmilModel::NewLC( RFs& aFs,
                                             CBaseMtm& aMtm,
                                             CUniDataUtils& aData,
                                             CUniObjectList& aObjectList )
    {
    CUniSmilModel* self = new ( ELeave )
        CUniSmilModel( aFs, aMtm, aData, aObjectList );

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------
// CUniSmilModel::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniSmilModel* CUniSmilModel::NewL( RFs& aFs,
                                            CBaseMtm& aMtm,
                                            CUniDataUtils& aData,
                                            CUniObjectList& aObjectList )
    {
    CUniSmilModel* self = NewLC( aFs, aMtm, aData, aObjectList );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CUniSmilModel::CUniSmilModel
//
// Constructor.
// ---------------------------------------------------------
//
CUniSmilModel::CUniSmilModel( RFs& aFs,
                             CBaseMtm& aMtm,
                             CUniDataUtils& aData,
                             CUniObjectList& aObjectList ) :
    iFs( aFs ),
    iMtm( aMtm ),
    iData( aData ),
    iObjectList( aObjectList ),
    iSlideLayout( EUniUndefinedLayout ),
    iSmilParams( 0 ),
    iScreenSize( 0, 0 ),
    iSizeEstimateUpToDate( EFalse ),
    iSizeEstimate( 0 )
    {
    }

// ---------------------------------------------------------
// CUniSmilModel::~CUniSmilModel
//
// Destructor.
// ---------------------------------------------------------
//
CUniSmilModel::~CUniSmilModel()
    {
    if (iSlideArray)
        {
        iSlideArray->ResetAndDestroy();
        }
    delete iSlideArray;
    delete iSmilParams;
    }

// ---------------------------------------------------------
// CUniSmilModel::ParseL
//
// ---------------------------------------------------------
//
EXPORT_C TInt CUniSmilModel::ParseL( CMDXMLDocument* aDom )
    {
    //MsgDataUTILSLOGGER_ENTERFN( "CUniSmilModel::ParseL()" );

    Reset();

    if ( aDom )
        {
        TRAPD( error, DoPresentationFromDomL( aDom ) );
        if ( error )
            {
            Reset();
            User::Leave( error );
            }
        else
            {
            iParseResult |= EParsedFromSmil;
            }
        }
    else
        {
        //  No SMIL -> Do presentation from object list
        if ( IsMessageSlide() )
            {
            // Determine layout. Make slide.
            DoNoSmilOneSlideL();
            }
        else
            {
            // Do slide for each media object
            DoNoSmilManySlideL();
            }
        iParseResult |= EParsedFromList;
        }

    return iParseResult;
    }


//**********************************************************
//*
//* SMIL COMPOSING FUNCTIONS
//*
//**********************************************************


// ---------------------------------------------------------
// CUniSmilModel::¨ComposeL
//
// ---------------------------------------------------------
//
EXPORT_C CMDXMLDocument* CUniSmilModel::ComposeL()
    {
    //MsgDataUTILSLOGGER_ENTERFN( "CUniSmilModel::ComposeL()" );

    // Reset region id's
    iTextRegionId.Set( KNullDesC );
    iImageRegionId.Set( KNullDesC );

    // Read default smil parameters
    if( !iSmilParams )
        {
        //MsgDataUTILSLOGGER_WRITE( "CUniSmilModel: Creating smil parameter structures." );
        iSmilParams = CUniSmilParams::NewL( iFs );
        }

    //MsgDataUTILSLOGGER_WRITE( "CUniSmilModel: Creating new DOM." );
    //delete iDom;
    //iDom = 0;
    CMDXMLDocument* dom = CMDXMLDocument::NewLC();

    // Create smil node
    TPtrC name( KSMILWREleSmil );
    // coverity[size_error][buffer_alloc]
	CMDXMLElement* root = CMDXMLElement::NewLC( ETrue, dom, name );
    // Append
    dom->DocumentElement()->AppendChild( root ); // Ownership to dom
    CleanupStack::Pop( root );

    // Create Head
    CreateHeadL( dom, root );

    // Create Body
    CreateBodyL( dom, root );

    // TODO: Move to Document and to use smil list
    //iObjectList.CreateSmilAttachmentL( smilAttaId, iDom, iFs );
    CleanupStack::Pop( dom );
    return dom;
    }



// ---------------------------------------------------------
// CUniSmilModel::RemoveEmptySlides
//
//
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilModel::RemoveEmptySlides()
    {
    TInt count = iSlideArray->Count();
    // Must be done from end to beginning because
    // slides might be removed.
    while ( count-- )
        {
        if ( !iSlideArray->At( count )->ObjectCount() )
            {
            RemoveSlide( count );
            }
        }
    }


// ---------------------------------------------------------
// CUniSmilModel::AddSlideL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilModel::AddSlideL( TInt aSlideNum )
    {
    DoAddSlideL( aSlideNum );
    }

// ---------------------------------------------------------
// CUniSmilModel::RemoveSlide
//
//
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilModel::RemoveSlide( TInt aSlideNum )
    {
    if ( !IsSlide( aSlideNum ) )
        {
        //__ASSERT_DEBUG( EFalse, DataUtilsPanic( EMmsNotASlide ) );
        return;
        }
    CUniSmilSlide* slide = iSlideArray->At( aSlideNum );
    if ( slide->ObjectCount() )
        {
        //__ASSERT_DEBUG( EFalse, DataUtilsPanic( EMmsSlideNotEmpty ) );
        return;
        }

    iSlideArray->Delete( aSlideNum );
    delete slide;
    //MsgDataUTILSLOGGER_WRITE( "CUniSmilModel: Empty slide removed." );
    iSizeEstimateUpToDate = EFalse;
    }

// ---------------------------------------------------------
// CUniSmilModel::MoveSlideL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilModel::MoveSlideL( TInt aSlideToMove, TInt aPlaceToMove ) 
    {
    TInt slideCount = iSlideArray->Count();
    if ( !slideCount
        || aSlideToMove < 0
        || aSlideToMove >= slideCount
        || aSlideToMove == aPlaceToMove )
        {
        return;
        }
    TInt place = Min( Max( 0, aPlaceToMove ), slideCount - 1 );
    CUniSmilSlide* slide = iSlideArray->At( aSlideToMove );

    iSlideArray->Delete( aSlideToMove );
    iSlideArray->InsertL( place, slide );
    iSizeEstimateUpToDate = EFalse;
    }

// ---------------------------------------------------------
// SortSlidesL
//
// Sorts slides accoring to a separate comparison function
// called SortSlidesCompare().
//
// Insertion sort algorithm.
//
// Notice! Intended to be used with SendAs when there
// are multiple slides - each slide containing only
// one object
// ---------------------------------------------------------
EXPORT_C void CUniSmilModel::SortSlidesL() 
    {
    TInt i = 1; // index 1, i.e. second slide
    TInt count = SlideCount();
    while ( i < count )
        {
        CUniSmilSlide* slide1 = iSlideArray->At( i );
        TInt j = i - 1;
        while ( j >= 0 )
            {
            CUniSmilSlide* slide2 = iSlideArray->At( j );
            if ( SortSlidesCompare( slide1, slide2 ) > 0 )
                {
                break;
                }
            j--;
            }
        j++;
        if ( j < i )
            {
            // slide1 was not in the right place
            iSlideArray->Delete( i );
            iSlideArray->InsertL( j, slide1 );
            }
        i++;
        }
    iSizeEstimateUpToDate = EFalse;
    }


// ---------------------------------------------------------
// SortSlidesCompare
//
// Compares slides according to time. Most recent is greatest.
//
// Empty slides are greater than non-empty slides.
// If aSlide1 "is greater than" aSlide2 returns positive value.
// If aSlide1 "is smaller than" aSlide2 returns negative value.
// If aSlide1 and aSlide2 "are equal" returns zero.
// ---------------------------------------------------------
//
TInt CUniSmilModel::SortSlidesCompare( CUniSmilSlide* aSlide1,
                                      CUniSmilSlide* aSlide2 )
    {
    if ( !aSlide1->ObjectCount() )
        {
        //if both are empty returns 0
        //if slide2 is non-empty -> slide1 "is greater"
        return aSlide2->ObjectCount();
        }
    else if ( !aSlide2->ObjectCount() )
        {
        //empty slide is always "greatest"
        return -1;
        }
    else
        {
        TTime time1;
        TTime time2;
        aSlide1->GetObjectByIndex( 0 )->MediaInfo()->ModificationTime( time1 );
        aSlide2->GetObjectByIndex( 0 )->MediaInfo()->ModificationTime( time2 );
        if ( time1 < time2 )
            {
            return -1;
            }
        else if ( time1 == time2 )
            {
            return 0;
            }
        else
            {
            return 1;
            }
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::IsSlide
//
//
// ---------------------------------------------------------
//
EXPORT_C TBool CUniSmilModel::IsSlide( TInt aSlideNum ) const
    {
    if( aSlideNum >= 0 &&
        aSlideNum < iSlideArray->Count() )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CUniSmilModel::AddTextObjectL
//
// ---------------------------------------------------------
//
EXPORT_C CUniTextObject* CUniSmilModel::AddTextObjectL(
    TInt aSlideNum, 
    CEikRichTextEditor* aText )
    {
    CUniTextObject* obj = CUniTextObject::NewLC( iFs, iMtm, iData, aText );
    iObjectList.AddObjectL( obj );
    CleanupStack::Pop( obj );
    iSlideArray->At(aSlideNum)->AddObjectL( obj );
    iSizeEstimateUpToDate = EFalse;
    return obj;
    }

// ---------------------------------------------------------
// CUniSmilModel::AddTempObjectL
//
// ---------------------------------------------------------
//
/*
EXPORT_C CUniObject* CUniSmilModel::AddTempObjectL( TInt aSlideNum,
                                            CMsgMediaInfo* aMedia )
    {
    if ( !IsSlide( aSlideNum ) )
        {
        User::Leave( KErrArgument );
        }

    CUniObject* obj = CUniObject::NewLC( iFs, iMtm, iData, aMedia );

    // Update model
    iObjectList.AddObjectL( obj );
    CleanupStack::Pop( obj );
    iSlideArray->At( aSlideNum )->AddObjectL( obj );
    SetLayoutFromObjectTypeL( obj->MediaType() );
    iSizeEstimateUpToDate = EFalse;
    return obj;
    }
*/

// ---------------------------------------------------------
// CUniSmilModel::AddObjectL
//
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniSmilModel::AddObjectL( MUniObjectSaveObserver& aObserver,
                                               TInt aSlideNum,
                                               CMsgMediaInfo* aMedia )
    {
    if ( !IsSlide( aSlideNum ) )
        {
        User::Leave( KErrArgument );
        }

    CUniObject* obj = CUniObject::NewLC( iFs, iMtm, iData, aMedia );

    // Update model
    iObjectList.AddObjectL( obj );
    CleanupStack::Pop( obj );
    obj->Save( aObserver, CMsvAttachment::EMsvLinkedFile );
    iSlideArray->At(aSlideNum)->AddObjectL( obj );
    iSizeEstimateUpToDate = EFalse;
    return obj;
    }

// ---------------------------------------------------------
// CUniSmilModel::AddStoredObjectL
//
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniSmilModel::AddStoredObjectL( TInt aSlideNum,
                                                     TMsvAttachmentId aAttachmentId,
                                                     CMsgMediaInfo* aMedia )
    {
    if ( !IsSlide( aSlideNum ) )
        {
        User::Leave( KErrArgument );
        }

    CMsvStore* store = iMtm.Entry().EditStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    MMsvAttachmentManagerSync& managerSync = store->AttachmentManagerExtensionsL();
    CMsvAttachment* attachment = manager.GetAttachmentInfoL( aAttachmentId );
    CleanupStack::PushL( attachment );

    CUniObject* obj = NULL;

    if ( aMedia &&
         aMedia->MediaType() == EMsgMediaText )
        {
        CMsgTextInfo* textInfo = static_cast<CMsgTextInfo*>( aMedia );
        obj = CUniTextObject::NewLC( iFs, iMtm, iData, textInfo, manager, *attachment );
        }
    else
        {
        obj = CUniObject::NewLC( iFs, iMtm, iData, aMedia, manager, *attachment );
        }
    managerSync.ModifyAttachmentInfoL( attachment );
    CleanupStack::Pop( 2, attachment ); //obj, attachment
    CleanupStack::PushL( obj );
    store->CommitL();

    // Update model
    iObjectList.AddObjectL( obj );
    CleanupStack::Pop( obj );
    CleanupStack::PopAndDestroy( store );
    iSlideArray->At(aSlideNum)->AddObjectL( obj );
    iSizeEstimateUpToDate = EFalse;
    return obj;
    }

// ---------------------------------------------------------
// CUniSmilModel::RemoveObjectL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilModel::RemoveObjectL( TInt aSlideNum, 
                                            CUniObject* aObject, 
                                            TBool aLeaveToStore/* = EFalse*/ )
    {
    if ( !IsSlide( aSlideNum ) ||
        !iSlideArray->At( aSlideNum )->IsObject( aObject ) )
        {
        return;
        }
    // TODO: Check that object exists on the slide.

    //if ( !IsObject( aSlideNum, aObjectType ) )
    //    {
    //    return;
    //    }
    DoRemoveObjectL( aSlideNum, aObject, aLeaveToStore );
    }

// ---------------------------------------------------------
// CUniSmilModel::RemoveObjectByIndexL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilModel::RemoveObjectByIndexL( TInt aSlideNum, TInt aObjectNum )
    {
    if ( !IsSlide( aSlideNum ) )
        {
        return;
        }
    CUniObject* obj = iSlideArray->At( aSlideNum )->GetObjectByIndex( aObjectNum );
    DoRemoveObjectL( aSlideNum, obj );
    }

// ---------------------------------------------------------
// CUniSmilModel::DoRemoveObjectL
//
//
// ---------------------------------------------------------
//
void CUniSmilModel::DoRemoveObjectL( TInt aSlideNum, CUniObject* aObject, TBool aLeaveToStore/* = EFalse*/ )
    {
    //MsgDataUTILSLOGGER_ENTERFN( "CUniSmilModel : DoRemoveObjectL()" );

    TBool removeObj = ( aObject->ReferenceCount() - 1 <= 0 );
    //if last reference was (will be) removed, remove from
    //attachment list (and msg store)
    if ( removeObj )
        {
        //MsgDataUTILSLOGGER_WRITE( "CUniSmilModel: Removing from atta list and store." );
        iObjectList.RemoveObjectL( aObject, !aLeaveToStore );
        }

    CUniSmilSlide* slide = iSlideArray->At( aSlideNum );
    slide->RemoveObject( aObject );
    iSizeEstimateUpToDate = EFalse;

    if ( removeObj )
        {
        delete aObject;
        }

    //MsgDataUTILSLOGGER_LEAVEFN( "CUniSmilModel : DoRemoveObjectL()" );
    }

// ---------------------------------------------------------
// CUniSmilModel::GetObject
//
//
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniSmilModel::GetObjectByIndex( TInt aSlideNum, 
                                                     TInt aObjectNum ) const
    {
    if( !IsSlide( aSlideNum ) )
        {
        return NULL;
        }
    return iSlideArray->At( aSlideNum )->GetObjectByIndex( aObjectNum );
    }

// ---------------------------------------------------------
// CUniSmilModel::GetObject
//
//
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniSmilModel::GetObject( TInt aSlideNum, 
                                              TUniRegion aRegion ) const
    {
    if( !IsSlide( aSlideNum ) )
        {
        return NULL;
        }
    return iSlideArray->At( aSlideNum )->GetObject( aRegion );
    }

// ---------------------------------------------------------
// CUniSmilModel::SetLayoutL
//
// Mutator.
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilModel::SetLayoutL( TUniLayout aNewLayout ) 
    {
    iSlideLayout = aNewLayout;
    TUint slideCnt = iSlideArray->Count();
    for (TUint i = 0; i < slideCnt; i++ )
        {
        iSlideArray->At(i)->UpdateLayoutL( iSlideLayout );
        }
    iSizeEstimateUpToDate = EFalse;
    }

// ---------------------------------------------------------
// CUniSmilModel::SmilComposeSize()
//
// Total size
// ---------------------------------------------------------
//
EXPORT_C TInt CUniSmilModel::SmilComposeSize()
    {
    TInt slideCount = iSlideArray->Count();
    if ( !slideCount )
        {
        return 0;
        }
    if ( !iSizeEstimateUpToDate )
        {
        iSizeEstimate = SmilComposeSize(
            slideCount,
            iSlideArray->At( slideCount - 1 )->ObjectCount() );
        iSizeEstimateUpToDate = ETrue;
        }
    return iSizeEstimate;
    }


// ---------------------------------------------------------
// CUniSmilModel::SmilComposeSize()
//
// Size from beginning
// ---------------------------------------------------------
//
EXPORT_C TInt CUniSmilModel::SmilComposeSize( TInt aToSlideNum, TInt aToObjectNum )
    {
    //__ASSERT_DEBUG(
    //    ( aToSlideNum > 0 && aToSlideNum <= iSlideArray->Count() ),
    //    DataUtilsPanic( EMmsIllegalSlide ) );
    //__ASSERT_DEBUG(
    //    ( aToObjectNum <= iSlideArray->At( aToSlideNum - 1 )->ObjectCount() ),
    //    DataUtilsPanic( EMmsIllegalObject ) );

    TInt size( KSizeMandatorySmilTags );
    TBool imageRegion = EFalse;
    TBool textRegion = EFalse;

    for ( TInt i = 0; i < aToSlideNum; ++i )
        {
        CUniSmilSlide* slide = iSlideArray->At( i );
        TInt toObj = slide->ObjectCount();
        if ( i == aToSlideNum - 1 )
            {
            toObj = aToObjectNum;
            }
        if ( !toObj )
            {
            //No objects on the slide
            size += KSizeEmptyPar;
            }
        else
            {
            //One or more objects
            size += KSizeNonEmptyPar;
            }
        for ( TInt ii = 0; ii < toObj; ++ii )
            {
            CUniObject* obj = slide->GetObjectByIndex( ii );
            switch ( obj->MediaType() )
                {
                case EMsgMediaAudio:
                    size += KSizeAudioTag;
                    break;
                case EMsgMediaImage:
                case EMsgMediaSvg:
                    size += KSizeImageTag;
                    imageRegion = ETrue;
                    break;
                case EMsgMediaText:
                case EMsgMediaXhtml:
                    size += KSizeTextTag;
                    textRegion = ETrue;
                    break;
                case EMsgMediaVideo:
                    size += KSizeVideoTag;
                    imageRegion = ETrue;
                    break;
                default:
                    //Should never come here
                    //Don't increase size
                    break;
                }
            }
        }

    TInt regions = 0;
    if ( imageRegion )
        {
        regions++;
        }
    if ( textRegion )
        {
        regions++;
        }
    switch ( regions )
        {
        case 1:
            size += KSizeHeadOneRegion;
            break;
        case 2:
            size += KSizeHeadTwoRegions;
            break;
        default:
            //Should be zero
            //Don't increase size
            break;
        }
    return size;
    }


// ---------------------------------------------------------
// CUniSmilModel::ObjectCount
//
// Accessor
// ---------------------------------------------------------
//
EXPORT_C TInt CUniSmilModel::ObjectCount() const
    {
    TInt objCnt(0);

    for ( TInt i = iSlideArray->Count(); --i >= 0 ;)
        {
        objCnt += iSlideArray->At(i)->ObjectCount();
        }

    return objCnt;
    }


// ---------------------------------------------------------
// CUniSmilModel::SlideObjectCount
//
// Accessor
// ---------------------------------------------------------
//
EXPORT_C TInt CUniSmilModel::SlideObjectCount(TInt aSlideNum) const
    {
    if ( !IsSlide(aSlideNum) )
        {
        return 0;
        }

    return iSlideArray->At( aSlideNum )->ObjectCount();
    }

//**********************************************************
//*
//* CONTRUCTION FUNCTIONS
//*
//**********************************************************


// ---------------------------------------------------------
// CUniSmilModel::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniSmilModel::ConstructL()
    {
    iSlideArray = new ( ELeave ) CSmilSlideArray( KSlideArrayGranularity );
    iTextRegionId.Set( KNullDesC );
    iImageRegionId.Set( KNullDesC );

    // Set default layout to "Text first"
    iSlideLayout = EUniTextFirst;

    // Fix for NSIA-6CKFJA. Sets same screen size that SMIL Player is using. MH
/*
    TRect appWindowRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, appWindowRect );

    TAknLayoutRect mainPane;
    mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 0 ) );

    TAknLayoutRect mainSmilPane;
    mainSmilPane.LayoutRect( mainPane.Rect(), AknLayoutScalable_Apps::main_smil_pane() );

    iScreenSize = mainSmilPane.Rect().Size();
*/
    }

// ---------------------------------------------------------
// CUniSmilModel::Reset
//
//
// ---------------------------------------------------------
//
void CUniSmilModel::Reset()
    {
    iParseResult = 0;
    iSlideLayout = EUniTextFirst;
    iTextRegionId.Set( KNullDesC );
    iImageRegionId.Set( KNullDesC );
    if ( iSlideArray )
        {
        // Reset slide array
        iSlideArray->ResetAndDestroy();
        }
    TInt objCount = iObjectList.Count();
    while ( objCount-- )
        {
        iObjectList.GetByIndex( objCount )->SetReferenceCount( 0 );
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::DoPresentationFromDomL
//
// When this functios is called DOM should be validated as MMS SMIL.
// ---------------------------------------------------------
//
void CUniSmilModel::DoPresentationFromDomL( CMDXMLDocument* aDom )
    {
    RemoveMetaTagsAndComments( aDom );
    // Resolve layout of the slide
    iSlideLayout = GetMmsSmilLayout( aDom );

    CMDXMLNode* node = GetNode( aDom, KSMILWREleBody );

    if ( node && node->FirstChild() )
        {
        // Only one slide in the message since no <par> tag found
        if ( node->FirstChild()->NodeName().Compare( KSMILWRElePar ) != 0 )
            {
            // Add slide - the one and only
            AddSlideFromDomL( node );
            }

        // Other node types than <par> not possible as this is MMS SMIL.
        else
            {
            // Traverse through every page
            CMDXMLNode* page = node->FirstChild();
            while ( page )
                {
                AddSlideFromDomL( page );
                // Go to next page.
                page = page->NextSibling();
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::AddSlideFromDomL
//
// ---------------------------------------------------------
//
void CUniSmilModel::AddSlideFromDomL( CMDXMLNode* aPage )
    {
    CUniObject* obj = NULL;
    CUniSmilSlide* slide = DoAddSlideL();
    CMDXMLNode* node = aPage->FirstChild();

    // Traverse through every object in page.
    while ( node )
        {
        if ( UniSmilUtils::IsMediaElem( node ) )
            {
            obj = iObjectList.GetByNodeL( node );
            if ( obj )
                {
                if ( slide )
                    {
                    obj->SetMediaTypeBySmilTag( UniSmilUtils::MediaElemType( node ) );
                    AddObjectFromDomL(
                        slide,
                        obj,
                        GetRegionType( node ) );
                    }
                else
                    {
                    iObjectList.RemoveObjectL( obj, ETrue );
                    delete obj;
                    iParseResult |= EObjectRemoved;
                    }
                }
            }
        node = node->NextSibling();
        }
    }


// ---------------------------------------------------------
// CUniSmilModel::AddObjectFromDomL
//
// ---------------------------------------------------------
//
void CUniSmilModel::AddObjectFromDomL( CUniSmilSlide* aSlide,
                                       CUniObject* aObj,
                                       TUniRegion aRegion )
    {
    if ( !aObj )
        {
        // Smil presentation refers to attachment that's not in the message
        iParseResult |= EMissingObjects;
        // Not actually error that should prevent message viewing.
        return;
        }

    TMsgMediaType mediaBySmilTag = aObj->MediaTypeBySmilTag();
    TMsgMediaType media = aObj->MediaType();

    if ( media == EMsgMediaSvg )
        {
        // SVG considered an image.
        // (...because "image" tag is used for SVG while composing SMIL.)
        media = EMsgMediaImage;
        }

    if ( media == EMsgMediaXhtml )
        {
        // XHTML is considered as a text.
        // (...because "text" tag is used for xhtml while composing SMIL.)
        media = EMsgMediaText;
        }

    if ( !( media == EMsgMediaText ||
        media == EMsgMediaImage ||
        media == EMsgMediaAudio ||
        media == EMsgMediaVideo ) )
        {
        // Unsupported format
        //MsgDataUTILSLOGGER_WRITE( "CUniSmilModel: Parsing: Message has unsupported objects." );
        iParseResult |= EUnsupportedObjects;
        // Not actually error that should prevent message viewing.
        }

    //  NOTE: Smil standard allows <img src="aa.txt"> references, but conformance
    //        Data forbids these.
    if ( media != EMsgMediaUnknown &&   //Unknown matches anything
         mediaBySmilTag != EMsgMediaRef &&    //<ref> matches anything
         media != mediaBySmilTag )
        {
        // SMIL does not describe content as it should
        //MsgDataUTILSLOGGER_WRITE( "CUniSmilModel: Parsing: Smil object type differs from message object type." );
        iParseResult |= EBadlyReferencedObjects;
        }

    TUniRegion regionBySmilTag = EUniRegionUnresolved;
    switch ( mediaBySmilTag )
        {
        case EMsgMediaAudio:
            {
            regionBySmilTag = EUniRegionAudio;
            break;
            }
        case EMsgMediaImage:
        case EMsgMediaVideo:
            {
            regionBySmilTag = EUniRegionImage;
            break;
            }
        case EMsgMediaText:
            {
            regionBySmilTag = EUniRegionText;
            break;
            }
        default:
            break;
        }

    TUniRegion regionByMediaType = EUniRegionUnresolved;
    switch ( media )
        {
        case EMsgMediaAudio:
            {
            regionByMediaType = EUniRegionAudio;
            break;
            }
        case EMsgMediaImage:
        case EMsgMediaVideo:
            {
            regionByMediaType = EUniRegionImage;
            break;
            }
        case EMsgMediaText:
            {
            regionByMediaType = EUniRegionText;
            break;
            }
        default:
            break;
        }

    // Check that region type matches object media type
    if ( aRegion != EUniRegionUnresolved &&
        regionByMediaType != EUniRegionUnresolved &&
        aRegion != regionByMediaType )
        {
        iParseResult |= ERegionMediaTypeMismatch;
        aRegion = regionByMediaType;
        }
    else if ( regionByMediaType == EUniRegionUnresolved &&
              aRegion == EUniRegionText )
        {
        // do not put every possible content into text reqion
        iParseResult |= ERegionMediaTypeMismatch;
        aRegion = EUniRegionUnresolved;
        }
    else if ( aRegion == EUniRegionUnresolved )
        {
        // If region couldn't be resolved from the region tag
        // follow the media type. If that's not possible follow
        // the smil media tag name. If everything fails
        // "aRegion" will remain "unresolved".
        aRegion = ( regionByMediaType != EUniRegionUnresolved )
            ? regionByMediaType
            : regionBySmilTag;
        }

    CMsgMediaInfo* mediaInfo = aObj->MediaInfo();
    // DRM2 must be put into object view in the viewer
    if ( mediaInfo  &&
         ( /*mediaInfo->HandlerApplicationUid().iUid == 0 ||*/
           aObj->MediaInfo()->Protection() & EFileProtDrm2 ) )
        {
        aRegion = EUniRegionUnresolved;
        }
    // else - should not occur

    if ( aRegion != EUniRegionUnresolved )
        {
        aSlide->AddObjectL( aObj, aRegion );
        iSizeEstimateUpToDate = EFalse;
        }
    else
        {
        iParseResult |= EBadlyReferencedObjects;
        // -> Something unknown in the SMIL.
        // -> Don't add. Will be handled as an attachment.
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::IsMessageSlide
//
// ---------------------------------------------------------
//
TBool CUniSmilModel::IsMessageSlide()
    {
    TInt imageCnt( 0 );
    TInt audioCnt( 0 );
    TInt textCnt( 0 );

    for ( TInt i = iObjectList.Count(); --i >= 0 ; )
        {
        switch( iObjectList.GetByIndex(i)->MediaType() )
            {
            case EMsgMediaSvg:
            case EMsgMediaImage:
                imageCnt++;
                break;

            case EMsgMediaAudio:
                audioCnt++;
                break;

            case EMsgMediaText:
            case EMsgMediaXhtml:
                textCnt++;
                break;

            case EMsgMediaVideo:
                //always consider video as image + audio
                imageCnt++;
                audioCnt++;
                break;

            case EMsgMediaUnknown:
            default:
                //  Unsupported objects will be put into attachment list
                //  in FindAttachmentsL()
                break;
            } // switch
        } // for

    if ( imageCnt > 1 || audioCnt > 1 || textCnt  > 1 )
        {
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// DoNoSmilOneSlideL
//
// NOTE: Message should be a valid slide at this point.
// ---------------------------------------------------------
//
void CUniSmilModel::DoNoSmilOneSlideL()
    {
    CUniSmilSlide* slide = NULL;

    for ( TInt i = 0; i < iObjectList.Count(); ++i )
        {
        CUniObject* obj = iObjectList.GetByIndex(i);
        TMsgMediaType media = obj->MediaType();
        CMsgMediaInfo* mediaInfo = obj->MediaInfo();

        // DRM2 must be put into objects view in the viewer
        if ( ( media == EMsgMediaText ||
               media == EMsgMediaXhtml ||
               media == EMsgMediaImage ||
               media == EMsgMediaSvg ||
               media == EMsgMediaAudio ||
               media == EMsgMediaVideo ) &&
             mediaInfo &&
             /*mediaInfo->HandlerApplicationUid().iUid &&*/
             !( obj->MediaInfo()->Protection() & EFileProtDrm2 ) )
            {
            if ( !slide )
                {
                // Add slide
                slide = DoAddSlideL();
                }
            slide->AddObjectL( obj );
            }
        }
    }

// ---------------------------------------------------------
// DoNoSmilManySlideL
//
//
// ---------------------------------------------------------
//
void CUniSmilModel::DoNoSmilManySlideL()
    {
    for ( TInt i = 0; i < iObjectList.Count(); ++i )
        {
        CUniObject* obj = iObjectList.GetByIndex(i);
        if (obj == NULL)
        	return;
        	
        TMsgMediaType media = obj->MediaType();
        CMsgMediaInfo* mediaInfo = obj->MediaInfo();

        // DRM2 must be put into objects view in the viewer
        if ( ( media == EMsgMediaText ||
               media == EMsgMediaXhtml ||
               media == EMsgMediaImage ||
               media == EMsgMediaSvg ||
               media == EMsgMediaAudio ||
               media == EMsgMediaVideo ) &&
            mediaInfo &&
            /*mediaInfo->HandlerApplicationUid().iUid &&*/
             !( obj->MediaInfo()->Protection() & EFileProtDrm2 ) )
            {
            CUniSmilSlide* slide = DoAddSlideL();
            if ( slide )
                {
                slide->AddObjectL( obj );
                }
            else
                {
                iObjectList.RemoveObjectL( obj, ETrue );
                delete obj;
                iParseResult |= EObjectRemoved;
                i--;
                }
            }
        }
    }

// ---------------------------------------------------------
// DoAddSlideL
//
// ---------------------------------------------------------
//
CUniSmilSlide* CUniSmilModel::DoAddSlideL( TInt aSlideNum /*= -1*/ )
    {
    CUniSmilSlide* newSlide = NULL;
    if ( iMaxSlideCount == 0 ||
         iSlideArray->Count() < iMaxSlideCount )
        {
        newSlide = CUniSmilSlide::NewLC( iSlideLayout );
        if ( aSlideNum == -1 || aSlideNum >= iSlideArray->Count() )
            {
            // Add to end of array
            iSlideArray->AppendL( newSlide );
            }
        else
            {
            iSlideArray->InsertL( aSlideNum, newSlide );
            }
        CleanupStack::Pop( newSlide );  // newSlide
        iSizeEstimateUpToDate = EFalse;
        }
    return newSlide;
    }

// ---------------------------------------------------------
// CUniSmilModel::CreateHeadL
//
// ---------------------------------------------------------
void CUniSmilModel::CreateHeadL( CMDXMLDocument* aDom, CMDXMLElement* aRoot )
    {

    TPtrC name( KSMILWREleHead );
    // Create Head node
    // coverity[size_error][buffer_alloc]
	CMDXMLElement* ele = CMDXMLElement::NewLC( ETrue, aDom, name );
    // Append Head in Root node
    aRoot->AppendChild( ele );

    // Create Layout
    CreateLayoutL( aDom, ele );
    CleanupStack::Pop( ele );
    }

// ---------------------------------------------------------
// CUniSmilModel::ChangeLayoutL
//
// ---------------------------------------------------------
EXPORT_C TBool CUniSmilModel::ChangeLayoutL( CMDXMLDocument* aDom )
    {
    TBool retVal = EFalse;
    if ( aDom )
        {
        CMDXMLElement* layout = (CMDXMLElement*) GetNode( aDom, KSMILWREleLayout );
        if ( !layout )
            {
            // No layout. Nothing to do without composing
            // SMIL from scratch.
            return EFalse;
            }
        CMDXMLElement* head = (CMDXMLElement*) layout->ParentNode();

        // Read default smil parameters
        if( !iSmilParams )
            {
            iSmilParams = CUniSmilParams::NewL( iFs );
            }
        CMDXMLElement* rootLayout = NULL;
        CMDXMLElement* textRegion = NULL;
        CMDXMLElement* imageRegion = NULL;

        GetLayoutElements(
            layout,
            rootLayout,
            textRegion,
            imageRegion );

        if ( rootLayout )
            {
            rootLayout->RemoveAttribute( KSMILWRParaWidth );
            rootLayout->RemoveAttribute( KSMILWRParaHeight );
            }

        if ( textRegion )
            {
            textRegion->RemoveAttribute( KSMILWRParaFit );
            textRegion->RemoveAttribute( KSMILWRParaTop );
            textRegion->RemoveAttribute( KSMILWRParaLeft );
            textRegion->RemoveAttribute( KSMILWRParaBottom );
            textRegion->RemoveAttribute( KSMILWRParaRight );
            textRegion->RemoveAttribute( KSMILWRParaHeight );
            textRegion->RemoveAttribute( KSMILWRParaWidth );
            }
        if ( imageRegion )
            {
            imageRegion->RemoveAttribute( KSMILWRParaFit );
            imageRegion->RemoveAttribute( KSMILWRParaTop );
            imageRegion->RemoveAttribute( KSMILWRParaLeft );
            imageRegion->RemoveAttribute( KSMILWRParaBottom );
            imageRegion->RemoveAttribute( KSMILWRParaRight );
            imageRegion->RemoveAttribute( KSMILWRParaHeight );
            imageRegion->RemoveAttribute( KSMILWRParaWidth );
            }

        CreateLayoutL( aDom, head, rootLayout, layout, textRegion, imageRegion );
        retVal = ETrue;
        }
    return retVal;
    }

// ---------------------------------------------------------
// CUniSmilModel::CorrectTimingL
//
// ---------------------------------------------------------
EXPORT_C TBool CUniSmilModel::CorrectTimingL( CMDXMLDocument* aDom )
    {
    if ( aDom )
        {
        CMDXMLElement* body = static_cast<CMDXMLElement*>( GetNode( aDom, KSMILWREleBody ) );
        if ( !body )
            {
            // No body. Nothing to do without composing
            // SMIL from scratch.
            return EFalse;
            }

        if ( body && body->FirstChild() )
            {
            // Only one slide in the message since no <par> tag found
            if ( body->FirstChild()->NodeName().Compare( KSMILWRElePar ) != 0 &&
                 !body->FirstChild()->NextSibling() )
                {
                // Correct slide timing to the one and only slide
                CorrectSlideTimingL( body );
                }
            // Other node types than <par> not possible as this is MMS SMIL.
            else
                {
                // Traverse through every page
                CMDXMLNode* page = body->FirstChild();

                while ( page != NULL )
                    {
                    if ( page->NodeName().Compare( KSMILWRElePar ) == 0 &&
                         page->NodeType() == CMDXMLNode::EElementNode )
                        {
                        CMDXMLElement* pageElement = static_cast<CMDXMLElement*>( page );
                        if ( !TimingSpecified( pageElement ) )
                            {
                            CorrectSlideTimingL( pageElement );
                            }
                        }
                    // Go to next page.
                    page = page->NextSibling();
                    }
                }
            }
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CUniSmilModel::IsSupportedMediaType
//
// ---------------------------------------------------------
EXPORT_C TBool CUniSmilModel::IsSupportedMediaType( TMsgMediaType aMediaType ) const
    {
    if ( aMediaType == EMsgMediaAudio ||
         aMediaType == EMsgMediaImage ||
         aMediaType == EMsgMediaSvg ||
         aMediaType == EMsgMediaXhtml ||
         aMediaType == EMsgMediaVideo ||
         aMediaType == EMsgMediaText )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CUniSmilModel::CreateLayoutL
//
// ---------------------------------------------------------
void CUniSmilModel::CreateLayoutL( CMDXMLDocument* aDom,
                                  CMDXMLElement* aRoot,
                                  CMDXMLElement* aRootLayout,
                                  CMDXMLElement* aLayout,
                                  CMDXMLElement* aTextRegion,
                                  CMDXMLElement* aImageRegion )
    {

    TPtrC name( KSMILWREleLayout );
    // Create Layout node
    if ( !aLayout )
        {
		// coverity[size_error][buffer_alloc]
        aLayout = CMDXMLElement::NewLC( ETrue, aDom, name );
        // Append Layout in Head
        aRoot->InsertBefore( NULL, aLayout );
        CleanupStack::Pop(); // layout
        }

    // Do we have images and text-objects on a slide with other media objects and
    // do we have any video
    TInt hasVideo( 0 );
    TInt hasImage( 0 );
    TInt hasText( 0 );

    // Do we have media, which is alone on a slide? Also calculate how medias are alone on a slide.
    // Video overrides any image region, it is not needed
    TInt hasImageOneAndOnly( 0 );
    TInt hasTextOneAndOnly( 0 );

    for( TInt i = 0; i < iSlideArray->Count(); ++i )
        {
        CUniSmilSlide* s = iSlideArray->At( i );
        TBool hasVideoOnThisSlide = EFalse;
        TBool hasImageOnThisSlide = EFalse;
        TBool hasTextOnThisSlide = EFalse;

        CUniObject* imageRegionObj = s->GetObject( EUniRegionImage );
        if ( imageRegionObj )
            {
            if ( imageRegionObj->MediaType() == EMsgMediaVideo )
                {
                hasVideoOnThisSlide = ETrue;
                hasVideo++;
                }
            else
                {
                hasImageOnThisSlide = ETrue;
                hasImage++;
                }
            }
        if ( s->GetObject( EUniRegionText ) )
            {
            hasTextOnThisSlide = ETrue;
            hasText++;
            }

        if (    !hasVideoOnThisSlide
            &&  hasImageOnThisSlide
            &&  !hasTextOnThisSlide )
            {
            hasImageOneAndOnly++;
            }
        if (    !hasVideoOnThisSlide
            &&  !hasImageOnThisSlide
            &&  hasTextOnThisSlide )
            {
            hasTextOneAndOnly++;
            }
        }

    // we know:
    // how many media objects of each type there are totally in the message and
    // whether some regions have alone on a slide
    // we conclude:
    // if a media object has been alone on the slide and there is
    // only one media object in the whole message
    // or if there are several images without other content or several texts on several slides
    // without other content then the media object is alone on a slide
    // and is only one of its type in the message
    // but: video is exception. It handling is ignored here. Following big if() statement handles it
    // by handling video before exception case resolved
    if ( hasImageOneAndOnly == hasImage )
        {
        // image does not occur with some other region
        hasImage = 0;
        }
    else
        {
        // image occurs with some other region on some slide
        hasImageOneAndOnly = 0;
        }

    if ( hasTextOneAndOnly ==  hasText )
        {
        // text does not occur with some other region
        hasText = 0;
        }
    else
        {
        // text occurs with some other region on some slide
        hasTextOneAndOnly = 0;
        }

/*
    TBool isLandscape( Layout_Meta_Data::IsLandscapeOrientation() );
    TBool useSubQCIF( EFalse);

    TRect appWin;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, appWin );
    TAknLayoutRect mainPane;

    if ( isLandscape )
        {
        mainPane.LayoutRect(
            appWin,
            AknLayoutScalable_Apps::main_pane( 4 ).LayoutLine() );
        }
    else
        {
        mainPane.LayoutRect(
            appWin,
            AknLayoutScalable_Apps::main_pane( 6 ).LayoutLine() );
        }

    if ( isLandscape &&
         appWin.Height() == KSizeLowResWidth &&
         appWin.Width() == KSizeLowResHeight )
        {
        useSubQCIF = ETrue;
        }

    TAknLayoutRect smilLayout;
    smilLayout.LayoutRect(
        mainPane.Rect(),
        AknLayoutScalable_Apps::main_smil2_pane().LayoutLine() );

    TAknLayoutRect rootLayout;
    TAknLayoutRect textLayout;
    TAknLayoutRect imageLayout;
    TAknLayoutRect videoLayout;

    // For reader about notation:
    //             x + y == x and y are on the different slide
    //             x,y == x and y are on the same slide
    //             { <item> } item can occur 0 or more times
    if ( hasText && hasImage && !hasVideo ) // text, image - no video
        {
        if ( isLandscape ) // landscape text & image
            {
            if ( iSlideLayout == EUniTextFirst )
                {
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 4 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 4 ).LayoutLine() );
                imageLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_image_pane( 3 ).LayoutLine() );
                }
            else
                {
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 2 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 2 ).LayoutLine() );
                imageLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_image_pane( 1 ).LayoutLine() );
                }
            }
        else // portrait text & image
            {
            if ( iSlideLayout == EUniTextFirst )
                {
		        // Option 3 in the layout spec
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 3 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 3 ).LayoutLine() );
                imageLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_image_pane( 2 ).LayoutLine() );
                }
            else
                {
		        // Option 1
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 1 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 1 ).LayoutLine() );
                imageLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_image_pane( 0 ).LayoutLine() );
                }
            }
        }
    else if ( hasText && hasVideo ) // text, video or text, image + video
        {
        if ( isLandscape && useSubQCIF ) // landscape subQCIF text & video
            {
            if ( iSlideLayout == EUniTextFirst )
                {
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 13 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 10 ).LayoutLine() );
                videoLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_video_pane( 7 ).LayoutLine() );
                }
            else
                {
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 12 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 9 ).LayoutLine() );
                videoLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_video_pane( 6 ).LayoutLine() );
                }
            }
        else if ( isLandscape && !useSubQCIF ) // landscape QCIF text & video
            {
            if ( iSlideLayout == EUniTextFirst )
                {
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 10 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 8 ).LayoutLine() );
                videoLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_video_pane( 4 ).LayoutLine() );
                }
            else
                {
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 9 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 7 ).LayoutLine() );
                videoLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_video_pane( 3 ).LayoutLine() );
                }
            }
        else // portrait QCIF text & video
            {
            if ( iSlideLayout == EUniTextFirst )
                {
		        // Option 8
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 8 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 6 ).LayoutLine() );
                videoLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_video_pane( 2 ).LayoutLine() );
                }
            else
                {
		        // Option 6
                rootLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_root_pane( 6 ).LayoutLine() );
                textLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_text_pane( 5 ).LayoutLine() );
                videoLayout.LayoutRect(
                    smilLayout.Rect(),
                    AknLayoutScalable_Apps::smil2_video_pane( 0 ).LayoutLine() );
                }
            }
        }
    // cases left:
    // - 1) text {+ text}
    // - 2) text {+ text} + video
    // - 3) text {+ text} + image { + image }
    // - 4) image {+ image} + video, no text
    // - 5) image {+ image}
    // - 6) video
    // - no visual media content
    else if ( hasVideo )
        {
        // 2) 'text {+ text} + video',  4) 'image {+ image} + video, no text' or 6) 'video'
        if ( isLandscape )
            {
            rootLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_root_pane( 11 ).LayoutLine() );
            videoLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_video_pane( 5 ).LayoutLine() );
            }
        else
            {
            // Option 7
            rootLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_root_pane( 7 ).LayoutLine() );
            videoLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_video_pane( 1 ).LayoutLine() );
            }

        if ( hasTextOneAndOnly ) // + text
            {
            // Option 0
            rootLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_root_pane( 0 ).LayoutLine() );
            textLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_text_pane( 0 ).LayoutLine() );
            }

        }
    else if (   hasTextOneAndOnly
            ||  hasImageOneAndOnly )
        {
        // 1) text {+ text}, 3) text {+ text} + image { + image ), 5) image {+ image}
        if ( hasTextOneAndOnly ) // text
            {
            // Option 0
            rootLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_root_pane( 0 ).LayoutLine() );
            textLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_text_pane( 0 ).LayoutLine() );
            }

        // no else - rootLayouts are same
        if ( hasImageOneAndOnly ) // image
            {
            // Option 5
            rootLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_root_pane( 5 ).LayoutLine() );
            imageLayout.LayoutRect(
                smilLayout.Rect(),
                AknLayoutScalable_Apps::smil2_image_pane( 4 ).LayoutLine() );
            }
        }
    else    // empty
        {
        rootLayout.LayoutRect(
            smilLayout.Rect(),
            AknLayoutScalable_Apps::smil2_root_pane( 0 ).LayoutLine() );
        textLayout.LayoutRect(
            smilLayout.Rect(),
            AknLayoutScalable_Apps::smil2_text_pane( 0 ).LayoutLine() );
        }

    iSmilParams->iComposeParams->iDisplayHeight = rootLayout.Rect().Height();
    iSmilParams->iComposeParams->iDisplayWidth = rootLayout.Rect().Width();

    // it does not matter anymore whether object is alone on the slide.
    // Use one variable only in the if() statements
    if ( hasImage || hasImageOneAndOnly )
        {
        hasImage = 1;
        }
    if ( hasText || hasTextOneAndOnly )
        {
        hasText = 1;
        }

    if ( hasText )
        {
        iSmilParams->iTextRegionParams->iSize = textLayout.Rect().Size();
        iSmilParams->iTextRegionParams->iTopLeft =
            textLayout.Rect().iTl - rootLayout.Rect().iTl;
        }
    if ( hasImage || hasVideo )
        {
        if ( hasVideo )
            {
            iSmilParams->iImageRegionParams->iSize = videoLayout.Rect().Size();
            iSmilParams->iImageRegionParams->iTopLeft =
                videoLayout.Rect().iTl - rootLayout.Rect().iTl;
            }
        else
            {
            iSmilParams->iImageRegionParams->iSize = imageLayout.Rect().Size();
            iSmilParams->iImageRegionParams->iTopLeft =
                imageLayout.Rect().iTl - rootLayout.Rect().iTl;
            }
        }

*/

    // Set Region IDs to defaults
    if ( !iImageRegionId.Length() )
        {
        iImageRegionId.Set( KSMILWRRegIDImage );
        }
    if ( !iTextRegionId.Length() )
        {
        iTextRegionId.Set( KSMILWRRegIDText );
        }

    // Create root layout
    name.Set( KSMILWREleRoot_layout );
    TBool rlPop = EFalse;
    if ( !aRootLayout )
        {
        aRootLayout = CMDXMLElement::NewLC( EFalse, aDom, name );
        rlPop = ETrue;
        }
    // Set root layout parameters (attributes)
    TBuf16<20> val;
    val.Zero();
    val.Format( KSmilIntFormat, iSmilParams->iComposeParams->iDisplayWidth );
    aRootLayout->SetAttributeL( KSMILWRParaWidth,val );
    val.Zero();
    val.Format( KSmilIntFormat, iSmilParams->iComposeParams->iDisplayHeight );
    aRootLayout->SetAttributeL( KSMILWRParaHeight, val );
    if ( rlPop )
        {
        // Append root layout in Layout
        aLayout->AppendChild( aRootLayout );
        CleanupStack::Pop( aRootLayout );
        }

    // Create Image region (if needed)
    name.Set( KSMILWREleRegion );
    if ( hasImage || hasVideo )
        {
        TBool imgPop = EFalse;
        if ( !aImageRegion )
            {
            aImageRegion = CMDXMLElement::NewLC( EFalse, aDom, name );
            imgPop = ETrue;
            }
        // Set Image region attributes
        aImageRegion->SetAttributeL( KSMILWRParaId, iImageRegionId );
        val.Zero();
        val.Format( KSmilIntFormat, iSmilParams->iImageRegionParams->Width() );
        aImageRegion->SetAttributeL( KSMILWRParaWidth, val );
        val.Zero();
        val.Format( KSmilIntFormat, iSmilParams->iImageRegionParams->Height() );
        aImageRegion->SetAttributeL( KSMILWRParaHeight, val );
        val.Zero();
        val.Format( KSmilIntFormat, iSmilParams->iImageRegionParams->Top() );
        aImageRegion->SetAttributeL( KSMILWRParaTop, val );
        val.Zero();
        val.Format( KSmilIntFormat, iSmilParams->iImageRegionParams->Left() );
        aImageRegion->SetAttributeL( KSMILWRParaLeft, val );
        aImageRegion->SetAttributeL( KSMILWRParaFit, iSmilParams->iImageRegionParams->iFit->Des() );
        if ( imgPop )
            {
            // Append region in Layout
            aLayout->AppendChild( aImageRegion ); // Ownership to dom
            CleanupStack::Pop( aImageRegion );
            }
        }

    // Create Text region (if needed)
    if ( hasText )
        {
        TBool txtPop = EFalse;
        if ( !aTextRegion )
            {
            aTextRegion = CMDXMLElement::NewLC( EFalse, aDom, name );
            txtPop = ETrue;
            }
        // Set Text region attributes
        aTextRegion->SetAttributeL( KSMILWRParaId, iTextRegionId );
        val.Zero();
        val.Format( KSmilIntFormat, iSmilParams->iTextRegionParams->Width() );
        aTextRegion->SetAttributeL( KSMILWRParaWidth, val );
        val.Zero();
        val.Format( KSmilIntFormat, iSmilParams->iTextRegionParams->Height() );
        aTextRegion->SetAttributeL( KSMILWRParaHeight, val );
        val.Zero();
        val.Format( KSmilIntFormat, iSmilParams->iTextRegionParams->Top() );
        aTextRegion->SetAttributeL( KSMILWRParaTop, val );
        val.Zero();
        val.Format( KSmilIntFormat, iSmilParams->iTextRegionParams->Left() );
        aTextRegion->SetAttributeL( KSMILWRParaLeft, val );
        aTextRegion->SetAttributeL( KSMILWRParaFit, iSmilParams->iTextRegionParams->iFit->Des() );
        if ( txtPop )
            {
            // Append region in Layout
            aLayout->AppendChild( aTextRegion ); // Ownership to dom
            CleanupStack::Pop( aTextRegion );
            }
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::CreateBodyL
//
// ---------------------------------------------------------
void CUniSmilModel::CreateBodyL( CMDXMLDocument* aDom, CMDXMLElement* aRoot )
    {
    TPtrC name( KSMILWREleBody );
    // Create Body node
	// coverity[size_error][buffer_alloc]
    CMDXMLElement* body = CMDXMLElement::NewLC( ETrue, aDom, name );
    // Append
    aRoot->AppendChild( body );
    CleanupStack::Pop( body );

    // For each slide
    for( TInt slide = 0; slide < iSlideArray->Count(); ++slide )
        {
        // Create par
        CreateParL( aDom, body, slide );
        }

    }

// ---------------------------------------------------------
// CUniSmilModel::CreateParL
//
// ---------------------------------------------------------
void CUniSmilModel::CreateParL( CMDXMLDocument* aDom, CMDXMLElement* aRoot, TInt aSlide )
    {
    TPtrC name( KSMILWRElePar );
    // Create par node
    CMDXMLElement* ele = CMDXMLElement::NewLC( ETrue, aDom, name );
    // Append

    // Get default page duration.
    TInt slideDur = iSmilParams->iComposeParams->iSlideDuration;
    TInt objDur = 0;
    TBool unresolvedDur = EFalse;

    CUniSmilSlide* s = iSlideArray->At( aSlide );

    // For each object in slide
    for ( TInt i = s->ObjectCount(); --i >= 0; )
        {
        // Create Item
        CUniObject* obj = s->GetObjectByIndex( i );
        objDur = CreateItemL( aDom, ele, obj );
        if ( !obj->MediaInfo()->Parsed() &&
            ( obj->MediaType() == EMsgMediaAudio ||
            obj->MediaType() == EMsgMediaVideo ) )
            {
            unresolvedDur = ETrue;
            }
        else
            {
            slideDur = Max( slideDur, objDur );
            }
        }

    if ( !unresolvedDur )
        {
        TBuf16<20> val;
        val.Zero();
        val.Format( KSmilMsecFormat, slideDur );

        // Set par attributes
        ele->SetAttributeL( KSMILWRParaDur, val );
        }
    aRoot->AppendChild( ele ); // Ownership to dom
    CleanupStack::Pop( ele );
    }

// ---------------------------------------------------------
// CUniSmilModel::CreateItemL
//
// ---------------------------------------------------------
TInt CUniSmilModel::CreateItemL( CMDXMLDocument* aDom, CMDXMLElement* aRoot, CUniObject* aObj )
    {
    CMDXMLElement* ele = NULL;
    TPtrC name( KNullDesC );

    // Create mediaobject node
    switch ( aObj->Region() )
        {
        case EUniRegionImage:
            {
            if ( aObj->MediaType() == EMsgMediaVideo )
                {
                name.Set( KSMILWREleVideo );
                ele = CMDXMLElement::NewLC( EFalse, aDom, name );
                ele->SetAttributeL( KSMILWRParaRegion, KSMILWRRegIDImage );
                }
            else
                {
                // Considered an image - includes also SVG
                name.Set( KSMILWREleImg );
                ele = CMDXMLElement::NewLC( EFalse, aDom, name );
                ele->SetAttributeL( KSMILWRParaRegion, KSMILWRRegIDImage );
                }
            }
            break;
        case EUniRegionText:
            {
            name.Set( KSMILWREleText );
            ele = CMDXMLElement::NewLC( EFalse, aDom, name );
            ele->SetAttributeL( KSMILWRParaRegion, KSMILWRRegIDText );
            }
            break;
        case EUniRegionAudio:
            {
            name.Set( KSMILWREleAudio );
            ele = CMDXMLElement::NewLC( EFalse, aDom, name );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }

    // Set MediaObject node attributes
    ele->SetAttributeL( KSMILWRParaSrc, aObj->MimeInfo()->ContentLocation() );

    // Append
    aRoot->AppendChild( ele ); // Ownership to dom
    CleanupStack::Pop( ele );

    return aObj->MediaInfo()->Duration();
    }

// ---------------------------------------------------------
// CUniSmilModel::GetMediaElemRegionIDs
//
// ---------------------------------------------------------
void CUniSmilModel::GetMediaElemRegionIDs( CMDXMLNode* aPage )
    {
    CMDXMLNode* txtEle = NULL;
    CMDXMLNode* imgEle = NULL;
    CMDXMLNode* obj = aPage->FirstChild();

    // Traverse through every object in page.
    while ( obj )
        {
        if( UniSmilUtils::IsMediaElem( obj ) )
            {
            TMsgMediaType type = UniSmilUtils::MediaElemType( obj );
            switch ( type )
                {
                case EMsgMediaImage:
                case EMsgMediaVideo:
                    imgEle = obj;
                    break;

                case EMsgMediaText:
                case EMsgMediaXhtml:
                    txtEle = obj;
                    break;

                // TODO: REF
                default:
                    break;
                }
            }
        obj = obj->NextSibling();
        }

    // Resolve ID:s
    if ( !iTextRegionId.Length() && txtEle )
        {
        UniSmilUtils::GetRegionId( txtEle, iTextRegionId );
        }
    if ( !iImageRegionId.Length() && imgEle )
        {
        UniSmilUtils::GetRegionId( imgEle, iImageRegionId );
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::GetLayoutElements
//
// ---------------------------------------------------------
void CUniSmilModel::GetLayoutElements( CMDXMLNode* aLayout,
                                       CMDXMLElement*& aRlEle,
                                       CMDXMLElement*& aTxtEle,
                                       CMDXMLElement*& aImgEle )
    {
    if ( !aLayout )
        {
        return;
        }

    TPtrC value( KNullDesC );
    CMDXMLNode* node = aLayout->FirstChild();

    // Search REGIONS & RootLayout
    while ( node )
        {
        // REGION PARAMETERS
        if ( node->NodeName().CompareF( KSMILWREleRegion ) == 0 )
            {

            //UniSmilUtils::GetRegionId( node, value );
            if( ((CMDXMLElement*) node)->IsAttributeSpecified( KSMILWRParaId ) )
                {
                ((CMDXMLElement*) node)->GetAttribute( KSMILWRParaId, value );
                }
            else
                {
                value.Set( KNullDesC );
                }

            // IMAGE REGION
            if ( value.CompareF( iImageRegionId ) == 0 )
                {
                aImgEle = (CMDXMLElement*) node;
                }

            // TEXT REGION
            else if ( value.CompareF( iTextRegionId ) == 0 )
                {
                aTxtEle = (CMDXMLElement*) node;
                }

            }
        else if ( node->NodeName().CompareF( KSMILWREleRoot_layout ) == 0 )
            {
            aRlEle = (CMDXMLElement*) node;
            }

        node = node->NextSibling();

        }
    }

// ---------------------------------------------------------
// CUniSmilModel::GetMmsSmilLayout
//
// ---------------------------------------------------------
TUniLayout CUniSmilModel::GetMmsSmilLayout( CMDXMLDocument* aDom )
    {
    TUniLayout rval = EUniUndefinedLayout;

    iTextRegionId.Set( KNullDesC );
    iImageRegionId.Set( KNullDesC );

    // --------------------------------------------
    // Resolve region IDs used in media elements
    // --------------------------------------------
    CMDXMLNode* node = GetNode( aDom, KSMILWREleBody );
    if ( !node )
        {
        return rval;
        }

    // Only one slide in the message since no <par> tag found
    // Other node types than <par> not possible as this is MMS SMIL.
    if ( node->FirstChild() )
        {
        if ( node->FirstChild()->NodeName().CompareF( KSMILWRElePar ) != 0 )
            {
            GetMediaElemRegionIDs( node );
            }
        else
            {
            CMDXMLNode* page = node->FirstChild();
            while ( page )
                {
                GetMediaElemRegionIDs( page );

                // Go to next page.
                page = page->NextSibling();
                }
            }
        }

    // -------------------------------------
    // Search layout elements
    // -------------------------------------
    node = GetNode( aDom, KSMILWREleLayout );
    CMDXMLElement* rootLayout = NULL;
    CMDXMLElement* textRegion = NULL;
    CMDXMLElement* imageRegion = NULL;
    GetLayoutElements( node, rootLayout, textRegion, imageRegion );

    // ------------------------------
    // Resolve Root Layout
    // ------------------------------
    TSize rootSize( 0, 0 );
    GetRootLayoutSize( rootLayout, rootSize );

    // ------------------------------------
    // Resolve region parameters
    // ------------------------------------
    TRect imgRect( 0, 0, 0, 0 );
    TRect txtRect( 0, 0, 0, 0 );
    GetRegionRect( imageRegion, rootSize, imgRect );
    GetRegionRect( textRegion, rootSize, txtRect );

    // ------------------------------------
    // Layout decision
    // ------------------------------------
    if ( textRegion && imageRegion )
        {
        if( imgRect.iTl.iY < txtRect.iTl.iY )
            {
            rval =  EUniImageFirst;
            }
        else if ( imgRect.iTl.iY > txtRect.iTl.iY )
            {
            rval = EUniTextFirst;
            }
        else
            {
            if ( imgRect.iTl.iX <= txtRect.iTl.iX )
                {
                rval = EUniImageFirst;
                }
            else
                {
                rval = EUniTextFirst;
                }
            }
        }
    else if ( textRegion )
        {
        rval = EUniTextFirst;
        }
    else // if ( iImgEle ) or none
        {
        rval = EUniImageFirst;
        }

    return rval;

    }

// ---------------------------------------------------------
// CUniSmilModel::GetRootLayoutSize
//
// ---------------------------------------------------------

void CUniSmilModel::GetRootLayoutSize( CMDXMLElement* aRl, TSize& aSize )
    {
    TPtrC value( KNullDesC );

    if ( aRl )
        {
        if( aRl->IsAttributeSpecified( KSMILWRParaWidth ) )
            {
            aRl->GetAttribute( KSMILWRParaWidth, value );
            aSize.iWidth = UniSmilUtils::StringToIntValue( value, 0 );
            }
        else
            {
            aSize.iWidth = iScreenSize.iWidth;
            }

        if( aRl->IsAttributeSpecified( KSMILWRParaHeight ) )
            {
            aRl->GetAttribute( KSMILWRParaHeight, value );
            aSize.iHeight = UniSmilUtils::StringToIntValue( value, 0 );
            }
        else
            {
            aSize.iHeight = iScreenSize.iHeight;
            }
        }
    else
        {
        aSize = iScreenSize;
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::GetRegionRect
//
// ---------------------------------------------------------

void CUniSmilModel::GetRegionRect( CMDXMLElement* aRegion, TSize aRoot, TRect& aRect )
    {
    TPtrC value;

    TInt width = 0;
    TInt height = 0;

    if ( aRegion )
        {
        if( aRegion->IsAttributeSpecified( KSMILWRParaWidth ) )
            {
            aRegion->GetAttribute( KSMILWRParaWidth, value );
            width = UniSmilUtils::StringToIntValue( value, 0 ) ;
            if ( value.Locate( '%' ) != KErrNotFound )
                {
                width = width * aRoot.iWidth / 100;
                }
            }

        if( aRegion->IsAttributeSpecified( KSMILWRParaHeight ) )
            {
            aRegion->GetAttribute( KSMILWRParaHeight, value );
            height = UniSmilUtils::StringToIntValue( value, 0 ) ;
            if ( value.Locate( '%' ) != KErrNotFound )
                {
                height = height * aRoot.iHeight / 100;
                }
            }

        if( aRegion->IsAttributeSpecified( KSMILWRParaTop ) )
            {
            aRegion->GetAttribute( KSMILWRParaTop, value );
            aRect.iTl.iY = UniSmilUtils::StringToIntValue( value, 0 );
            if ( value.Locate( '%' ) != KErrNotFound )
                {
                aRect.iTl.iY = aRect.iTl.iY * aRoot.iHeight / 100;
                }
            }
        else
            {
            aRect.iTl.iY = 0;
            }

        if( aRegion->IsAttributeSpecified( KSMILWRParaLeft ) )
            {
            aRegion->GetAttribute( KSMILWRParaLeft, value );
            aRect.iTl.iX = UniSmilUtils::StringToIntValue( value, 0 );
            if ( value.Locate( '%' ) != KErrNotFound )
                {
                aRect.iTl.iX = aRect.iTl.iX * aRoot.iWidth / 100;
                }
            }
        else
            {
            aRect.iTl.iX = 0;
            }

        aRect.iBr.iY = aRect.iTl.iY + height;
        aRect.iBr.iX = aRect.iTl.iX + width;

        }
    else
        {
        aRect = TRect( 0, 0, 0, 0 );
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::GetRegionType
// ---------------------------------------------------------
//
TUniRegion CUniSmilModel::GetRegionType( CMDXMLNode* aNodePtr )
    {
    if ( iImageRegionId.CompareF( iTextRegionId ) == 0 )
        {
        // Image and text regions are the same!
        // Literally taken this is agains MMS SMIL, but
        // we should still tolerate it.
        iParseResult |= ETextAndImageRegionsSame;
        return EUniRegionUnresolved;
        }
    TPtrC regId;
    TInt err = UniSmilUtils::GetRegionId( aNodePtr, regId );
    if ( !err )
        {
        if ( regId.CompareF( iImageRegionId ) == 0 )
            {
            return EUniRegionImage;
            }
        else if ( regId.CompareF( iTextRegionId ) == 0 )
            {
            return EUniRegionText;
            }
        else
            {
            return EUniRegionUnresolved;
            }
        }
    else
        {
        return EUniRegionUnresolved;
        }
    }


// ---------------------------------------------------------
// CUniSmilModel::GetNode
//
// ---------------------------------------------------------
CMDXMLNode* CUniSmilModel::GetNode( CMDXMLDocument* aDom, const TDesC& aName )
    {
    CMDXMLNode* node = aDom->DocumentElement()->FirstChild();
    while ( node )
        {
        if ( node->NodeName().CompareF( aName ) == 0 )
            {
            break;
            }
        else
            {
            node = UniSmilUtils::NextNode( node );
            }
        }
    return node;
    }

// ---------------------------------------------------------
// CUniSmilModel::RemoveMetaTagsAndComments
//
// ---------------------------------------------------------
void CUniSmilModel::RemoveMetaTagsAndComments( CMDXMLDocument* aDom )
    {
    CMDXMLNode* node = aDom->DocumentElement()->FirstChild();
    while ( node )
        {
        if ( node->NodeType() == CMDXMLNode::ECommentNode ||
            node->NodeName().CompareF( KSMILWREleMeta ) == 0 )
            {
            CMDXMLNode* removeNode = node;
            node = removeNode->ParentNode();
            /*TInt ignore =*/ node->RemoveChild( removeNode );
            delete removeNode;
            }
        node = UniSmilUtils::NextNode( node );
        }
    }

// ---------------------------------------------------------
// CUniSmilModel::CorrectSlideTimingL
//
// ---------------------------------------------------------
void CUniSmilModel::CorrectSlideTimingL( CMDXMLElement* aPage )
    {
    CMDXMLNode* node = aPage->FirstChild();

    // Traverse through every object in page.
    while ( node != NULL )
        {
        if ( UniSmilUtils::IsMediaElem( node ) )
            {
            CMDXMLElement* nodeElement = static_cast<CMDXMLElement*>( node );

            CUniObject* obj = iObjectList.GetByNodeL( node );

            if( !obj ||
                !obj->MediaInfo() ||
                obj->MediaInfo()->Duration() != 0 ||
                TimingSpecified( nodeElement ) )
                {
                // Return as non-static media was found from the page.
                return;
                }
            else if ( nodeElement->IsAttributeSpecified( KSMILWRParaBegin ) )
                {
                // Remove invalid Begin element since no End or Dur attributes is specified.
                nodeElement->RemoveAttribute( KSMILWRParaBegin );
                }
            else if ( nodeElement->IsAttributeSpecified( KSMILWRParaEnd ) )
                {
                // Remove invalid End element since no Begin or Dur attributes is specified.
                nodeElement->RemoveAttribute( KSMILWRParaEnd );
                }
            }

        node = node->NextSibling();
        }

    // Read default smil parameters
    if ( !iSmilParams )
        {
        iSmilParams = CUniSmilParams::NewL( iFs );
        }

    TBuf16<20> defaultVal;
    defaultVal.Zero();
    defaultVal.Format( KSmilMsecFormat, iSmilParams->iComposeParams->iSlideDuration );

    aPage->SetAttributeL( KSMILWRParaDur, defaultVal );
    }

// ---------------------------------------------------------
// CUniSmilModel::TimingSpecified
//
// ---------------------------------------------------------
TBool CUniSmilModel::TimingSpecified( CMDXMLElement* aNodePrt )
    {
    TBool result( EFalse );

    // Timing is set if dur or begin & end parameters are found
    if ( aNodePrt &&
         ( aNodePrt->IsAttributeSpecified( KSMILWRParaDur ) ||
           ( aNodePrt->IsAttributeSpecified( KSMILWRParaBegin ) &&
             aNodePrt->IsAttributeSpecified( KSMILWRParaEnd ) ) ) )
            {
            result = ETrue;
            }

    return result;
    }

// End of file
