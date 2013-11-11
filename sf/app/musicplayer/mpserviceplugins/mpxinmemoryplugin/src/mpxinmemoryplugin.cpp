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
* Description:  Simple plugin to cache media objects
*
*/


// INCLUDE FILES
#include <e32cmn.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcmn.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxdrmmediautility.h>
#include <mpxmediadrmdefs.h>
#include <mpxlog.h>

#include "mpxinmemoryplugin.h"

// CONSTANTS
const TInt KIMPluginUid  = 0x101FFCD8;

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXInMemoryPlugin* CMPXInMemoryPlugin::NewL()
    {
    CMPXInMemoryPlugin* p = new (ELeave) CMPXInMemoryPlugin();
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop(p);
    return p;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXInMemoryPlugin::~CMPXInMemoryPlugin()
    {
    delete iDrmMediaUtility;

    // Cleanup Arrays
    iTemporaryData.ResetAndDestroy();
    iTemporaryData.Close();
    iEmbeddedContext.Reset();
    iEmbeddedContext.Close();
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXInMemoryPlugin::CMPXInMemoryPlugin()
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::ConstructL()
    {
    iDrmMediaUtility = CMPXDrmMediaUtility::NewL();
    }

// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::OpenL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& /*aAttrs*/,
    CMPXFilter* /*aFilter*/)
    {
    // Media object to return
    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdContainer);
    supportedIds.AppendL(KMPXMediaIdGeneral);
    CMPXMedia* entries=CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PushL(entries);

    // Media array for items
    CMPXMediaArray* array=CMPXMediaArray::NewL();
    CleanupStack::PushL(array);

    // Based on Path, what is the embedded client context id?
    TInt depth = aPath.Levels();
    TBool oneSong = EFalse;
    TInt err = KErrNone;
    switch( depth )
        {
        case 1: // should not have anything
            {
            User::Leave( KErrArgument );
            break;
            }
        case 2: // an item or a playlist
            {
            TInt context = aPath.Id(1);
            TInt contextIndex = iEmbeddedContext.Find( context );
            if( contextIndex >= KErrNone )
                {
                CMPXMedia& media = *iTemporaryData[contextIndex];

                TMPXGeneralCategory cat = *media.Value<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
                if( cat == EMPXPlaylist )
                    {
                    const CMPXMediaArray* plarray = media.Value<CMPXMediaArray>( KMPXMediaArrayContents );
                    for( TInt i=0; i<plarray->Count(); ++i )
                        {
                        CMPXMedia* item = (*plarray)[i];
                        CMPXMedia* copy = CMPXMedia::NewL(*item);

                        // item id is the index in the array
                        copy->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, i );
                        array->AppendL( copy ); // ownership x-fer
                        }


                    // Insert dummy media objects if the "specified" count is > actual
                    // number of media objects
                    TInt count(0);
                    if( media.IsSupported(KMPXMediaArrayCount) )
                        {
                        count = *media.Value<TInt>(KMPXMediaArrayCount);
                        }
                    if( count > 0 && count > plarray->Count() )
                        {
                        for( TInt i=plarray->Count(); i<count; ++i )
                            {
                            // Insert dummies
                            CMPXMedia* copy = CMPXMedia::NewL();
                            copy->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, i );
                            copy->SetTObjectValueL( KMPXMediaGeneralType,
                                                    EMPXItem );
                            copy->SetTObjectValueL( KMPXMediaGeneralCategory,
                                                    EMPXSong );
                            array->AppendL( copy ); // ownership x-fer
                            }
                        }

                    // Set the title for the playlist
                    //
                    if( media.IsSupported( KMPXMediaGeneralTitle ) )
                        {
                        const TDesC& title = media.ValueText( KMPXMediaGeneralTitle );
                        entries->SetTextValueL( KMPXMediaGeneralTitle,
                                                title );
                        }
                    }
                else // assume it is an item then
                    {
                    // Set one song to true
                    oneSong = ETrue;
                    }
                }
            else
                {
                err = KErrNotFound;
                }
            break;
            }
        case 3:  // item in a playlist
            {
            oneSong = ETrue;
            break;
            }
        default:
        break;
        }

    // Set array if not one song
    if( !oneSong )
        {
        entries->SetCObjectValueL(KMPXMediaArrayContents,array);
        entries->SetTObjectValueL(KMPXMediaArrayCount,
                                  array->Count());
        entries->SetTObjectValueL( KMPXMediaGeneralType,
                                   EMPXGroup );
        entries->SetTObjectValueL( KMPXMediaGeneralCategory,
                                   EMPXPlaylist );
        }
    else
        {
        entries->SetTObjectValueL( KMPXMediaGeneralType,
                                   EMPXItem );
        entries->SetTObjectValueL( KMPXMediaGeneralCategory,
                                   EMPXSong );
        }

    if(oneSong)
        {
        iObs->HandleOpen(const_cast<CMPXCollectionPath*>(&aPath), err );
        }
    else
        {
        entries->SetCObjectValueL( KMPXMediaGeneralContainerPath,
                                   const_cast<CMPXCollectionPath*>(&aPath) );
        iObs->HandleOpen(entries, err );
        }
    CleanupStack::PopAndDestroy(array);
    CleanupStack::PopAndDestroy(entries);
    CleanupStack::PopAndDestroy(&supportedIds);
    }

// ----------------------------------------------------------------------------
// Extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::MediaL (
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
                cat = *media.Value<TMPXGeneralCategory>( KMPXMediaGeneralCategory );

                // Playlist media
                //
                if( cat == EMPXPlaylist && depth == 2)
                    {
                    // Get
                    const TDesC& title = media.ValueText( KMPXMediaGeneralTitle );
                    const TDesC& uri = media.ValueText( KMPXMediaGeneralUri );
                    // Set
                    entries->SetTextValueL( KMPXMediaGeneralTitle,
                                            title );
                    entries->SetTextValueL( KMPXMediaGeneralUri,
                                            uri );
                    entries->SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId,
                                                    TUid::Uid(KIMPluginUid) );
                    }
                // Item in a playlist media
                //
                else if( cat == EMPXPlaylist && depth == 3 )
                    {
                    const CMPXMediaArray* plarray = media.Value<CMPXMediaArray>(
                                                        KMPXMediaArrayContents );
                    TInt selection = aPath.Id( 2 );
                    TInt count = plarray->Count();
                    if( selection < count )
                        {
                        *entries = *(*plarray)[selection];

                        entries->SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId,
                                                        TUid::Uid(KIMPluginUid) );
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
                    *entries = media;

                    entries->SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId,
                                                    TUid::Uid(KIMPluginUid) );
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
    //
    for (TInt i=aAttrs.Count();--i>=0;)
        {
        if (aAttrs[i]==KMPXMediaGeneralPath)
            {
            entries->SetCObjectValueL(KMPXMediaGeneralPath,
                                      const_cast<CMPXCollectionPath*>(&aPath));
            break;
            }
        }

    // Full metadata is not available from this plugin
    entries->SetTObjectValueL( KMPXMediaColDetailMediaNotAvailable,
                               ETrue );
    //
    iObs->HandleMedia(entries, err );
    CleanupStack::PopAndDestroy(entries);
    }

// ----------------------------------------------------------------------------
// Cancel outstanding request
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::CancelRequest()
    {
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected collection
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::CommandL(TMPXCollectionCommand /*aCmd*/, TInt /*aArg*/)
    {
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected collection
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::CommandL(CMPXCommand& aCmd)
    {
    if (!aCmd.IsSupported(TMPXAttribute(KMPXCommandContentIdGeneral, EMPXCommandGeneralId)))
        {
        User::Leave(KErrArgument);
        }

    TMPXCommandId commandId =
        *aCmd.Value<TMPXCommandId>(
            TMPXAttribute(KMPXCommandContentIdGeneral, EMPXCommandGeneralId));

    TBool syncOp(EFalse);
    if( aCmd.IsSupported(KMPXCommandGeneralDoSync) )
        {
        syncOp = *aCmd.Value<TBool>(KMPXCommandGeneralDoSync);
        }

    // Handle each operation
    //
    switch( commandId )
        {
        case KMPXCommandIdCollectionAdd:
            {
            DoAddL( *aCmd.Value<CMPXMedia>(KMPXCommandColAddMedia) );
            break;
            }
        case KMPXCommandIdCollectionSet:
            {
            DoSetL( *aCmd.Value<CMPXMedia>(KMPXCommandColSetMedia) );
            break;
            }
        default:
            {
            User::Leave(KErrNotSupported);
            }
        }

    // Complete Async operations
    //
    if( !syncOp )
        {
        iObs->HandleCommandComplete( NULL, KErrNone );
        }
    }

// ----------------------------------------------------------------------------
// Adds item(s) to the collection
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::AddL(
    const CMPXMedia& aNewMedia)
    {
    DoAddL( aNewMedia );
    }

// ----------------------------------------------------------------------------
// Remove by collection path
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::RemoveL(const CMPXCollectionPath& aPath )
    {
    (void) aPath;
    }

// ----------------------------------------------------------------------------
// Remove an item or items under a group from the collection
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::RemoveL(const CMPXMedia& aMedia)
    {
    (void)aMedia;
    }

// ----------------------------------------------------------------------------
// Sets/updates the media for the item
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::SetL(
    const CMPXMedia& aMedia)
    {
    DoSetL( aMedia );
    }

// ----------------------------------------------------------------------------
// Find based on media properties
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::FindAllL(const CMPXMedia& aMedia,
                                      const TArray<TMPXAttribute>& aAttrs )
    {
    (void)aMedia;
    (void)aAttrs;
    }

// ----------------------------------------------------------------------------
// Find based on media properties
// ----------------------------------------------------------------------------
//
CMPXMedia* CMPXInMemoryPlugin::FindAllSyncL(const CMPXMedia& aMedia,
                                            const TArray<TMPXAttribute>& aAttrs )
    {
    (void)aMedia;
    (void)aAttrs;
    User::Leave(KErrNotSupported);
    return NULL;
    }

// ----------------------------------------------------------------------------
// Find the capabilities of this plugin
// ----------------------------------------------------------------------------
//
TCollectionCapability CMPXInMemoryPlugin::GetCapabilities()
    {
    return 0; // nothing special supported
    }

// ----------------------------------------------------------------------------------------------------------
// Handle change
// ----------------------------------------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::HandleChangeL(const TMPXItemId& aId, TMPXChangeEventType aChange )
    {
    // Construct the message
    //
    CMPXMessage* message = CMPXMedia::NewL();
    CleanupStack::PushL( message );

    // Set attributes
    //
    message->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);

    message->SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(KIMPluginUid));

    message->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType, aChange);

    message->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, aId);

    // Callback to observer and destroy
    //
    iObs->HandleMessage( *message );
    CleanupStack::PopAndDestroy( message );
    }

// ----------------------------------------------------------------------------
// Adds item(s) to the collection
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::DoAddL(
    const CMPXMedia& aNewMedia)
    {
    TInt context = *aNewMedia.Value<TInt>( TMPXAttribute(KMPXMediaIdGeneral,
                                                         EMPXMediaGeneralId ) );

    // Only 1 set of data per embedded context
    TInt index = iEmbeddedContext.Find( context );
    if( index != KErrNotFound )
        {
        iEmbeddedContext.Remove( index );
        delete iTemporaryData[index];
        iTemporaryData.Remove( index );
        iEmbeddedContext.Compress();
        iTemporaryData.Compress();
        }
    // Push onto list
    iEmbeddedContext.AppendL( context );

    CMPXMedia* copy = CMPXMedia::NewL();
    *copy = aNewMedia;
    iTemporaryData.AppendL( copy );  // ownership transferred.
    }

// ----------------------------------------------------------------------------
// Sets/updates the media for the item
// ----------------------------------------------------------------------------
//
void CMPXInMemoryPlugin::DoSetL(
    const CMPXMedia& aMedia)
    {
    MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL <---");
    if( !aMedia.IsSupported(KMPXMediaGeneralId) )
        {
        User::Leave( KErrArgument );
        }

    TInt context = *aMedia.Value<TMPXItemId>(KMPXMediaGeneralId);
    TInt index = iEmbeddedContext.Find( context );

    MPX_DEBUG2("CMPXInMemoryPlugin::DoSetL index %i", index);
    if( index != KErrNotFound )
        {
        TMPXAttribute att( KMPXMediaArrayContents );

        // Make sure it has a media array
        //
        if( aMedia.IsSupported( att ) )
            {
            MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL 1");

            // Grab the 2 media arrays
            //
            CMPXMediaArray* currentMedia =
                           const_cast<CMPXMediaArray*>(iTemporaryData[index]->Value<CMPXMediaArray>(att));
            const CMPXMediaArray* newMedia = aMedia.Value<CMPXMediaArray>(att);

            MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL 2");
            // Copy all entries from newMedia into currentMedia
            //
            TInt newCount = newMedia->Count();
            for( TInt i=0; i<newCount; ++i )
                {
                MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL 3");
                CMPXMedia* copy = CMPXMedia::NewL(*(*newMedia)[i]);
                CleanupStack::PushL( copy );

                // If the item contains an item ID, we use it as an index to insert
                // into the array, otherwise, append item to the end
                //
                MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL 4");
                if( copy->IsSupported(KMPXMediaGeneralId) )
                    {
                    TInt index = *copy->Value<TMPXItemId>(KMPXMediaGeneralId);
                    MPX_DEBUG2("CMPXInMemoryPlugin::DoSetL index count %i", index);
                    // index is 0th based, like an array
                    //
                    TInt currentCount = currentMedia->Count();
                    if( index > currentCount )
                        {
                        // Fill in blanks then append the item we want
                        //
                        for( TInt j=currentCount; j<index; ++j )
                            {
                            CMPXMedia* blank = CMPXMedia::NewL();
                            CleanupStack::PushL( blank );
                            currentMedia->AppendL( blank ); // ownership xfer
                            CleanupStack::Pop( blank );
                            }
                        currentMedia->AppendL( copy ); // ownership xfer
                        }
                    else
                        {
                        // Replace item at index with this new version
                        //
                        currentMedia->Remove(index);
                        currentMedia->Insert( copy, index ); // ownership xfer
                        }
                    }
                else
                    {
                    currentMedia->AppendL( copy );  // ownership xfer
                    }
                MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL 5");
                CleanupStack::Pop(copy);
                }

            // Set the new array into the media
            //
            MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL 6");
            iTemporaryData[index]->SetCObjectValueL(att, currentMedia );

            // Tell collection client context that something was added
            //
            MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL 7");
            HandleChangeL( context, EMPXItemInserted );
            MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL 8");
            }
        }
    else
        {
        MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL KErrArgument");
        User::Leave( KErrArgument );
        }
    MPX_DEBUG1("CMPXInMemoryPlugin::DoSetL --->");
    }

// End of file
