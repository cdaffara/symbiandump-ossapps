/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Music specific memory plugin
*
*/


#include <e32base.h>
#include <f32file.h>
#include <badesca.h>
#include <apgcli.h>
#include <mpxcollectiontype.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediaarray.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxdrmmediautility.h>

#include <mpxmetadataextractor.h>
#include "mpxmusicmemoryplugin.h"

// CONSTANTS
const TInt KIMMusicPluginUid  = 0x10282960;
const TMPXAttributeData KMPXMediaFetched = {KIMMusicPluginUid, 0x01}; // TInt

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default Constructor
// ---------------------------------------------------------------------------
//
CMPXMusicMemoryPlugin::CMPXMusicMemoryPlugin()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phased constructor
// ---------------------------------------------------------------------------
//
void CMPXMusicMemoryPlugin::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iAppArc.Connect() );
    RPointerArray<CMPXCollectionType> dummy;
    CleanupClosePushL( dummy );
    iMetadataExtractor = CMPXMetadataExtractor::NewL( iFs, iAppArc, dummy );
    dummy.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &dummy );
    
    iAttributes.AppendL(KMPXMediaDrmAll);
    
    // Base class construction
    CMPXInMemoryPlugin::ConstructL();
    }


// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CMPXMusicMemoryPlugin* CMPXMusicMemoryPlugin::NewL()
    {
    CMPXMusicMemoryPlugin* self = new(ELeave) CMPXMusicMemoryPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Virtual destructor
// ---------------------------------------------------------------------------
//
CMPXMusicMemoryPlugin::~CMPXMusicMemoryPlugin()
    {
    iFs.Close();
    iAppArc.Close();
    delete iMetadataExtractor;
    iAttributes.Close();
    }


// ----------------------------------------------------------------------------
// Extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
void CMPXMusicMemoryPlugin::MediaL (
    const CMPXCollectionPath& aPath, 
    const TArray<TMPXAttribute>& aAttrs,
    const TArray<TCapability>& /*aCaps*/,
    CMPXAttributeSpecs* /*aSpecs*/)
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdGeneral);
    CMPXMedia* entries=CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    
    // Based on Path, what is the embedded client context id?
    //
    TInt err = KErrNone;
    TInt depth = aPath.Levels();
    switch( depth )
        {
        case 2:  // Playlist / Song level
        case 3:  // Song in a playlist level, fall through
            {
            TInt context = aPath.Id(1);
            TInt contextIndex = iEmbeddedContext.Find( context );
            if( contextIndex >= KErrNone )
                {
                CMPXMedia& media = *iTemporaryData[contextIndex];
                
                TMPXGeneralCategory cat;
                cat = *media.Value<TMPXGeneralCategory>(KMPXMediaGeneralCategory);
                
                // Playlist media
                //
                if( cat == EMPXPlaylist && depth == 2)
                    {
                    // Get
                    const TDesC& title = media.ValueText( KMPXMediaGeneralTitle );
                    const TDesC& uri = media.ValueText( KMPXMediaGeneralUri );
                    // Set
                    entries->SetTextValueL( KMPXMediaGeneralTitle, title );
                    entries->SetTextValueL( KMPXMediaGeneralUri, uri );
                    entries->SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId, 
                                                    TUid::Uid(KIMMusicPluginUid) );
                    }
                // Item in a playlist media
                //
                else if( cat == EMPXPlaylist && depth == 3 )
                    {
                    const CMPXMediaArray* plarray = media.Value<CMPXMediaArray>(KMPXMediaArrayContents);
                    TInt selection = aPath.Id(2);
                    TInt count = plarray->Count();
                    if( selection < count )
                        {
                        if( (*plarray)[selection]->IsSupported(KMPXMediaFetched) )
                            {
                            *entries = *(*plarray)[selection];
                            }
                        else  // not fetched yet
                            {
                            // Extract the data
                            const TDesC& uri = (*plarray)[selection]->ValueText( KMPXMediaGeneralUri );
                            CMPXMedia* metadata = ExtractMetadataL( uri, iAttributes.Array() );
                            CleanupStack::PushL( metadata );                                            
                            
                            // Return to client
                            *entries = *metadata;
                            
                            // Save the data
                            *(*plarray)[selection] = *metadata;         
                            (*plarray)[selection]->SetTObjectValueL(KMPXMediaFetched, ETrue);
                            
                            CleanupStack::PopAndDestroy( metadata );
                            }
                        entries->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, 
                                                         TUid::Uid(KIMMusicPluginUid) );
                        }
                    else
                       {
                       // Bounds check
                       err = KErrArgument; 
                       }
                    }
                // Otherwise, assume it is a song
                //
                else // cat == song/image/video/etc
                    {
                    if( media.IsSupported(KMPXMediaFetched) )
                        {
                        *entries = media;    
                        }
                    else
                        {
                        const TDesC& uri = media.ValueText( KMPXMediaGeneralUri );
                        CMPXMedia* metadata = ExtractMetadataL( uri, iAttributes.Array() );
                        CleanupStack::PushL( metadata );
                        // Return to client
                        *entries = *metadata;
                        
                        //Save the data
                        media = *metadata;
                        media.SetTObjectValueL(KMPXMediaFetched, ETrue);
                        
                        CleanupStack::PopAndDestroy( metadata );    
                        }
                    entries->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, 
                                                     TUid::Uid(KIMMusicPluginUid) );
                    }
                }
            break;    
            }  
        default:
            {
            // Return Nothing because a MediaL at this depth contains nothing
            break;    
            }
        }
    // Copy Path
    TMPXAttribute pathAttr(KMPXMediaGeneralPath);
    for (TInt i=aAttrs.Count();--i>=0;)
        {
        if (aAttrs[i]==pathAttr)
            {
            entries->SetCObjectValueL(pathAttr,
                                         const_cast<CMPXCollectionPath*>(&aPath));
            break;
            }
        }
    // Callback to collection client context
    iObs->HandleMedia( entries, err );
    CleanupStack::PopAndDestroy(entries);
    }

// ----------------------------------------------------------------------------------------------------------
// Set all the attributes in CMPXMedia corresponding to KMPXMediaIdDrm
// ----------------------------------------------------------------------------------------------------------
//
CMPXMedia* CMPXMusicMemoryPlugin::ExtractMetadataL( const TDesC& aUri, 
                                                    const TArray<TMPXAttribute>& aAttrs )
    {
    CMPXMedia* media( NULL );
    iMetadataExtractor->CreateMediaL( aUri, media, ETrue );
    
    // Also set drm 
    if( media )
        {
        CleanupStack::PushL( media );
        DoSetMediaDrmL( *media, aAttrs, aUri );
        CleanupStack::Pop( media );
        }
    return media;  // ownership transferred
    }

// ----------------------------------------------------------------------------------------------------------
// Set all the attributes in CMPXMedia corresponding to KMPXMediaIdDrm
// ----------------------------------------------------------------------------------------------------------
//
void CMPXMusicMemoryPlugin::DoSetMediaDrmL(CMPXMedia& aMedia, 
                                           const TArray<TMPXAttribute>& aAttrs, 
                                           const TDesC& aLocation )
    {
    // Gather all DRM attributes
    TUint aDrmAttributes(0);
    for( TInt i=0; i<aAttrs.Count(); ++i )
        {
        if( aAttrs[i].ContentId() == KMPXMediaIdDrm )
            {
            aDrmAttributes |= aAttrs[i].AttributeId();
            }
        }
    
    iDrmMediaUtility->InitL(aLocation);
    const CMPXMedia* drmMedia( iDrmMediaUtility->GetMediaL( aDrmAttributes ));   
    
    // Only get attributes if it's a DRM file
    if ( drmMedia )
        {
        if ( aDrmAttributes & KMPXMediaDrmType.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmType );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TInt val( *drmMedia->Value<TInt>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmRightsStatus.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmRightsStatus );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TInt val( *drmMedia->Value<TInt>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmRightsType.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmRightsType );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TInt val( *drmMedia->Value<TInt>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmCount.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmCount );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TInt val( *drmMedia->Value<TInt>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmProtected.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmProtected );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TBool val( *drmMedia->Value<TBool>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmSendingAllowed.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmSendingAllowed );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TBool val( *drmMedia->Value<TBool>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmCanSetAutomated.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmCanSetAutomated );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TBool val( *drmMedia->Value<TBool>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmHasInfoUrl.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmHasInfoUrl );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TBool val( *drmMedia->Value<TBool>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmHasPreviewUrl.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmHasPreviewUrl );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TBool val( *drmMedia->Value<TBool>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmAboutToExpire.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmAboutToExpire );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TBool val( *drmMedia->Value<TBool>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmStartTime.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmStartTime );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TInt64 val( *drmMedia->Value<TInt64>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmEndTime.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmEndTime );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TInt64 val( *drmMedia->Value<TInt64>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmIntervalStartTime.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmIntervalStartTime );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TInt64 val( *drmMedia->Value<TInt64>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmAccumulatedTime.iAttributeId )
            {                        
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, 
                                  EMPXMediaDrmAccumulatedTime );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TInt64 val( *drmMedia->Value<TInt64>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        if ( aDrmAttributes & KMPXMediaDrmInterval.iAttributeId )
            {
            TMPXAttribute mpxAtt( KMPXMediaIdDrm, EMPXMediaDrmInterval );
            if ( drmMedia->IsSupported( mpxAtt ))
                {
                TTimeIntervalSeconds val(
                    *drmMedia->Value<TTimeIntervalSeconds>( mpxAtt ));
                aMedia.SetTObjectValueL( mpxAtt, val );
                }
            }
        }
    iDrmMediaUtility->Close();        
    }
// END OF FILE
