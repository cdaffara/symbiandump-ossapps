/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*/

// Copied from vcxmyvideosmdsdb.cpp, keep up to date.

//#include <harvesterclient.h>
#include <e32svr.h>
#include <e32def.h>
#include <bautils.h>
#include <mpxmedia.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediavideodefs.h>
#include <mdeconstants.h>
#include <mdequery.h>

#include <vcxmyvideosdefs.h>

#include "VCXTestLog.h"
#include "VCXTestMdsDbModifier.h"
//#include "vcxmyvideoscollectionutil.h"

_LIT( KVcxVideoObjectName, "Video" );
                                                              //1 (ID)
_LIT( KVcxTitlePropertyName, "Title" );                       //2
_LIT( KVcxDescriptionPropertyName, "Description" );           //3
                                                              //4 (file path = URI)
_LIT( KVcxSizePropertyName, "Size" );                         //5

_LIT( KVcxCreationDatePropertyName, "CreationDate" );         //6 creation date is saved to 2 properties
_LIT( KVcxTimeOffsetPropertyName, "TimeOffset" );             //6

_LIT( KVcxFlagsPropertyName, "RecordingFlags" );              //7 flags are saved
_LIT( KVcxDrmPropertyName, "DRM");                            //7 to both of these

_LIT( KVcxCopyrightPropertyName, "Copyright" );               //8
_LIT( KVcxItemTypePropertyName, "ItemType" );                 //9 (mime type)
_LIT( KVcxLastModifiedDatePropertyName, "LastModifiedDate" ); //10
_LIT( KVcxAgeProfilePropertyName, "AgeProfile" );             //11
_LIT( KVcxAudioLanguagePropertyName, "AudioLanguage" );       //12
_LIT( KVcxAuthorPropertyName, "Author" );                     //13
_LIT( KVcxOriginPropertyName, "Origin" );                     //14
_LIT( KVcxDurationPropertyName, "Duration" );                 //15
_LIT( KVcxLastPlayPositionPropertyName, "LastPlayPosition" ); //16
_LIT( KVcxDownloadIdPropertyName, "DownloadID" );             //17
_LIT( KVcxRatingPropertyName, "Rating" );                     //19
_LIT( KVcxBitratePropertyName, "Bitrate" );                   //20
_LIT( KVcxAudioFourCcPropertyName, "AudioFourCC" );           //21

// ---------------------------------------------------------------------------
// This is TCleanupOperation function which clears the db in use flag
// ---------------------------------------------------------------------------
//
static void ClearDbInUseFlag( TAny* aBoolPtr )
    {
    *(static_cast<TBool*>(aBoolPtr)) = EFalse;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVCXTestMdsDbModifier::CVCXTestMdsDbModifier()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::ConstructL()
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::ConstructL" );

    User::LeaveIfError( iRfs.Connect() );

    iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait;

    //  Mark the db to be in use
    iDbInUse = ETrue;

    CleanupStack::PushL( TCleanupItem( ClearDbInUseFlag, &iDbInUse ) );

    VCXLOGLO1( "CVCXTestMdsDbModifier::ConstructL CMdESession::NewL" );

    //  Create session
    iMDSError = KErrNone;

    iMDSSession = CMdESession::NewL( *this );
    if (!iMDSSession)
        {
        //  Failed to create session, leave
        User::Leave( iMDSError );
        }

    //  Wait until session opened
    iActiveSchedulerWait->Start();

    VCXLOGLO1( "CVCXTestMdsDbModifier::ConstructL iActiveSchedulerWait->Start done" );

    if ( iMDSError != KErrNone )
        {
        VCXLOGLO2("Failed to create session to MDS: %d", iMDSError);
        User::LeaveIfError( iMDSError );
        }

    //  Get the schema definitions
    iMDSError = KErrNone;
    GetSchemaDefinitionsL();

    iDbInUse = EFalse;

    //  Is schema ok
    if ( iMDSError != KErrNone )
        {
        //  Schema not loaded, abort
        User::Leave( iMDSError );
        }

    //  The db is not in use anymore, iDbInUse clearer item
    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVCXTestMdsDbModifier* CVCXTestMdsDbModifier::NewL()
    {
    CVCXTestMdsDbModifier* self =
            CVCXTestMdsDbModifier::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVCXTestMdsDbModifier* CVCXTestMdsDbModifier::NewLC()
    {
    CVCXTestMdsDbModifier* self = new( ELeave ) CVCXTestMdsDbModifier();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVCXTestMdsDbModifier::~CVCXTestMdsDbModifier()
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::~CVCXTestMdsDbModifier()" );

    if ( iMDSSession )
        {
        TRAP_IGNORE( iMDSSession->RemoveObjectObserverL( *this ) );
        }

    delete iMDSSession;
    delete iActiveSchedulerWait;

    iRfs.Close();
    }

// ---------------------------------------------------------------------------
// CVCXTestMdsDbModifier::RemoveVideoL
// ---------------------------------------------------------------------------
//
TInt CVCXTestMdsDbModifier::RemoveVideoL(
        TUint32 aMdsId )
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::RemoveVideoL" );

    TInt retValue = KErrNone;

    VCXLOGLO2( "CVCXTestMdsDbModifier:: removing object %d", aMdsId );

    TItemId id = iMDSSession->RemoveObjectL( aMdsId );

    if ( id == KNoId )
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier:: video not found" );
        retValue = KErrNotFound;
        }
    else
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier:: remove successful" );
        }

    return retValue;
    }

// ---------------------------------------------------------------------------
// CVCXTestMdsDbModifier::UpdateVideoL
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::UpdateVideoL(
        CMPXMedia& aVideo )
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::UpdateVideoL" );

    TMPXItemId mpxId = aVideo.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );

    VCXLOGLO2("CVCXTestMdsDbModifier::UpdateVideoL updating object %d ", mpxId.iId1);

    CMdEObject* object =
            iMDSSession->OpenObjectL( mpxId.iId1, *iVideoObjectDef );
    if ( !object )
        {
        // No object with this ID was found!
        VCXLOGLO1("CVCXTestMdsDbModifier::UpdateVideoL no object found");
        User::Leave( KErrNotFound );
        }
    else
        {
        VCXLOGLO1("CVCXTestMdsDbModifier::UpdateVideoL object found");

        if ( object->OpenForModifications() )
            {
            CleanupStack::PushL( object ); // 1->

            Media2ObjectL( aVideo, *object );

            iMDSSession->CommitObjectL(*object);

            CleanupStack::PopAndDestroy(object);
            }
        else
            {
            // Object is already locked!
            VCXLOGLO1("CVCXTestMdsDbModifier::UpdateVideoL object was locked!");
            delete object;
            User::Leave( KErrInUse );
            }
        }
    }

// ---------------------------------------------------------------------------
// CVCXTestMdsDbModifier::DeleteVideoL.
// ---------------------------------------------------------------------------
//
TInt CVCXTestMdsDbModifier::DeleteVideoL(
        TUint32 aFileId,
        TInt& aResultCount,
        TBool aForcedDelete /*= EFalse */ )
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::DeleteVideoL" );

    TInt err = KErrNone;
    aResultCount = 0;

    if (aForcedDelete)
        {
        TItemId id = iMDSSession->RemoveObjectL( aFileId );

        err = (id == KNoId) ? KErrNotFound : KErrNone;
        }

    return err;
    }

// ---------------------------------------------------------------------------
// From class MMdEQueryObserver.
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::HandleQueryNewResults(
        CMdEQuery& /* aQuery */,
        TInt /* aFirstNewItemIndex */,
        TInt /* aNewItemCount */ )
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::HandleQueryNewResults" );
    }

// ---------------------------------------------------------------------------
// From class MMdEQueryObserver.
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::HandleQueryCompleted(
        CMdEQuery& /*aQuery*/,
        TInt /*aError*/)
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::HandleQueryCompleted" );

    if (iActiveSchedulerWait->IsStarted())
        {
        iActiveSchedulerWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// From class MMdESessionObserver.
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::HandleSessionOpened(
        CMdESession& /*aSession*/,
        TInt aError)
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::HandleSessionOpened" );

    iActiveSchedulerWait->AsyncStop();

    if (aError != KErrNone)
        {
        VCXLOGLO2( "CVCXTestMdsDbModifier::HandleSessionOpened: %d", aError );

        iMDSError = aError;
        delete iMDSSession;
        iMDSSession = NULL;
        }
    }

// ---------------------------------------------------------------------------
// From class MMdESessionObserver.
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::HandleSessionError(
        CMdESession& /*aSession*/,
        TInt aError)
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::HandleSessionError" );

    if (iActiveSchedulerWait->IsStarted())
        {
        iActiveSchedulerWait->AsyncStop();
        }

    VCXLOGLO2( "CVCXTestMdsDbModifier::HandleSessionError: %d", aError );

    iMDSError = aError;
    delete iMDSSession;
    iMDSSession = NULL;
    }

// ---------------------------------------------------------------------------
// From class MMdESchemaObserver.
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::HandleSchemaModified()
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::HandleSchemaModified" );
    TRAP( iMDSError, GetSchemaDefinitionsL() );
    }

// ---------------------------------------------------------------------------
// CVCXTestMdsDbModifier::HandleObjectNotification
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::HandleObjectNotification(
        CMdESession& /*aSession*/,
        TObserverNotificationType /* aType */,
        const RArray<TItemId>& /* aObjectIdArray */ )
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::HandleObjectNotification" );
    }

// ---------------------------------------------------------------------------
// CVCXTestMdsDbModifier::Media2ObjectL
// Called by AddVideoL()
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::Media2ObjectL(
        CMPXMedia& aVideo,
        CMdEObject& aObject)
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL" );

    CMdEProperty* property;

    // 1. MDS ID

    // 2. TITLE (NAME)
    if ( aVideo.IsSupported( KMPXMediaGeneralTitle ) )
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralTitle" );
        if ( aObject.Property( *iTitlePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralTitle ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iTitlePropertyDef, aVideo.ValueText( KMPXMediaGeneralTitle ) );
            }
        }

    // 3. DESC
    if ( aVideo.IsSupported( KMPXMediaGeneralComment ) )
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralComment" );
        if ( aObject.Property( *iDescriptionPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralComment ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iDescriptionPropertyDef, aVideo.ValueText( KMPXMediaGeneralComment ) );
            }
        }
#if 0
    // 4. URI ( = file path )
    if ( aVideo.IsSupported( KMPXMediaGeneralUri ) )
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralUri" );
        aObject.SetUriL( aVideo.ValueText( KMPXMediaGeneralUri ) );
        TVolumeInfo volInfo;
        // 4.1 MEDIA ID ( = drive number )
        if ( iRfs.Volume( volInfo, TVcxMyVideosCollectionUtil::DriveLetter2DriveNumber(
                aObject.Uri() ) ) == KErrNone)
            {
            aObject.SetMediaId( volInfo.iUniqueID );
            }
        }
#endif

    // 5. KMPXMediaGeneralSize
    if ( aVideo.IsSupported( KMPXMediaGeneralSize ) )
        {
        TUint32 size = 0;
        TInt sizeInMediaObject = aVideo.ValueTObjectL<TInt>( KMPXMediaGeneralSize );
        VCXLOGLO2( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralSize: %d", sizeInMediaObject );
        size = static_cast<TUint32>( sizeInMediaObject );

        if ( aObject.Property( *iSizePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>( property )->SetValueL( size );
            }
        else
            {
            aObject.AddUint32PropertyL( *iSizePropertyDef, size );
            }
        }

    // KMPXMediaGeneralExtSizeInt64
    if ( aVideo.IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
        {
        TUint32 size = 0;
        TInt64 sizeInMediaObject = aVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 );
        VCXLOGLO2( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralExtSizeInt64: %Ld", sizeInMediaObject );
        size = static_cast<TUint32>( sizeInMediaObject );

        if ( aObject.Property( *iSizePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>( property )->SetValueL( size );
            }
        else
            {
            aObject.AddUint32PropertyL( *iSizePropertyDef, size );
            }
        }

    // 6. KMPXMediaGeneralDate ( creation date )
    if ( aVideo.IsSupported( KMPXMediaGeneralDate ) )
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralDate" );
        TInt64 creationDateInt64 = 0;
        creationDateInt64 = aVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
        TTime creationDate( creationDateInt64 );
        if ( aObject.Property( *iCreationDatePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETimeProperty*>(property)->SetValueL( creationDate );
            }
        else
            {
            aObject.AddTimePropertyL( *iCreationDatePropertyDef, creationDate );
            }
        }

    // 7. KMPXMediaGeneralFlags (including DRM flag)
    if ( aVideo.IsSupported( KMPXMediaGeneralFlags ) )
        {
        TUint32 flags = aVideo.ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
        VCXLOGLO2( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralFlags: %d", KMPXMediaGeneralFlags );
        if ( aObject.Property( *iFlagsPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>(property)->SetValueL( flags );
            }
        else
            {
            aObject.AddUint32PropertyL( *iFlagsPropertyDef, flags );
            }

        TBool drmProtected;
        if ( flags & EVcxMyVideosVideoDrmProtected )
            {
            drmProtected = ETrue;
            }
        else
            {
            drmProtected = EFalse;
            }

        VCXLOGLO2( "CVCXTestMdsDbModifier::Media2ObjectL: Setting drmProtected: %d", drmProtected );

        if ( aObject.Property( *iDrmPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEBoolProperty*>(property)->SetValueL( drmProtected );
            }
        else
            {
            aObject.AddBoolPropertyL( *iDrmPropertyDef, drmProtected );
            }
        }

    // 8. KMPXMediaGeneralCopyright
    if ( aVideo.IsSupported( KMPXMediaGeneralCopyright ))
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralCopyright" );
        if ( aObject.Property( *iCopyrightPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralCopyright ) );
            }
        else
            {
            if ( aVideo.ValueText( KMPXMediaGeneralCopyright ).Length() )
                {
                aObject.AddTextPropertyL(
                        *iCopyrightPropertyDef,
                        aVideo.ValueText( KMPXMediaGeneralCopyright ) );
                }
            }
        }

    // 9. KMPXMediaGeneralMimeType
    if ( aVideo.IsSupported( KMPXMediaGeneralMimeType ))
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralMimeType" );
        if ( aObject.Property( *iItemTypePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralMimeType ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iItemTypePropertyDef,
                    aVideo.ValueText( KMPXMediaGeneralMimeType ) );
            }
        }

    // 10. KVcxMediaMyVideosModifiedDate
    if ( aVideo.IsSupported( KVcxMediaMyVideosModifiedDate ) )
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KVcxMediaMyVideosModifiedDate" );
        TInt64 modifiedDateInt64 = aVideo.ValueTObjectL<TInt64>( KVcxMediaMyVideosModifiedDate );
        TTime modifiedDate( modifiedDateInt64 );
        if ( aObject.Property( *iLastModifiedDatePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETimeProperty*>(property)->SetValueL( modifiedDate );
            }
        else
            {
            aObject.AddTimePropertyL( *iLastModifiedDatePropertyDef, modifiedDate );
            }
        }

    // 11. KVcxMediaMyVideosAgeProfile
    if ( aVideo.IsSupported( KVcxMediaMyVideosAgeProfile ) )
        {
        TInt32 ageProfile = aVideo.ValueTObjectL<TInt32>(KVcxMediaMyVideosAgeProfile );
        VCXLOGLO2( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KVcxMediaMyVideosAgeProfile: %d", ageProfile );
        if ( aObject.Property( *iAgeProfilePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEInt32Property*>(property)->SetValueL( ageProfile );
            }
        else
            {
            aObject.AddInt32PropertyL( *iAgeProfilePropertyDef, ageProfile );
            }
        }

    // 12. KVcxMediaMyVideosAudioLanguage
    if ( aVideo.IsSupported( KVcxMediaMyVideosAudioLanguage ) )
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KVcxMediaMyVideosAudioLanguage" );
        if ( aObject.Property( *iAudioLanguagePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KVcxMediaMyVideosAudioLanguage ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iAudioLanguagePropertyDef,
                    aVideo.ValueText( KVcxMediaMyVideosAudioLanguage ) );
            }
        }

    // 13. KVcxMediaMyVideosAuthor
    if ( aVideo.IsSupported( KVcxMediaMyVideosAuthor ) )
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KVcxMediaMyVideosAuthor" );
        if ( aObject.Property( *iAuthorPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KVcxMediaMyVideosAuthor ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iAuthorPropertyDef,
                    aVideo.ValueText( KVcxMediaMyVideosAuthor ) );
            }
        }

    // 14. KVcxMediaMyVideosOrigin
    if ( aVideo.IsSupported( KVcxMediaMyVideosOrigin ) )
        {
        TUint8 origin = aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosOrigin );
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting xxx" );
        if ( aObject.Property( *iOriginPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint8Property*>(property)->SetValueL( origin );
            }
        else
            {
            aObject.AddUint8PropertyL( *iOriginPropertyDef, origin );
            }
        }

    // 15. KVcxMediaMyVideosDuration
    if ( aVideo.IsSupported( KVcxMediaMyVideosDuration ))
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KVcxMediaMyVideosDuration" );
        TReal32 duration = aVideo.ValueTObjectL<TReal32>( KVcxMediaMyVideosDuration );

        if ( aObject.Property( *iDurationPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEReal32Property*>(property)->SetValueL( duration );
            }
        else
            {
            aObject.AddReal32PropertyL( *iDurationPropertyDef, duration );
            }
        }

    // 16. KVcxMediaMyVideosLastPlaybackPosition
    if ( aVideo.IsSupported( KMPXMediaGeneralLastPlaybackPosition ))
        {
        VCXLOGLO1( "CVCXTestMdsDbModifier::Media2ObjectL: Setting KMPXMediaGeneralLastPlaybackPosition" );
        TInt pos = static_cast<CMdEReal32Property*>(property)->Value();
        aVideo.SetTObjectValueL<TInt>( KMPXMediaGeneralLastPlaybackPosition, pos );
        }

    // 17. DOWNLOAD ID
    if ( aVideo.IsSupported( KVcxMediaMyVideosDownloadId ) )
        {
        TUint32 dlId = aVideo.ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );

        VCXLOGLO2( "CVCXTestMdsDbModifier::Media2ObjectL: DL ID = %d", dlId );

        if ( aObject.Property( *iDownloadIdPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>(property)->SetValueL( dlId );
            }
        else
            {
            aObject.AddUint32PropertyL( *iDownloadIdPropertyDef, dlId );
            }
        }

    // 19. RATING
    if ( aVideo.IsSupported( KVcxMediaMyVideosRating ) )
        {
        TUint8 rating = aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosRating );
        
        if ( aObject.Property( *iRatingPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint8Property*>(property)->SetValueL( rating );
            }
        else
            {
            aObject.AddUint8PropertyL( *iRatingPropertyDef, rating );
            }
        }
    // 20. BITRATE
    if ( aVideo.IsSupported( KMPXMediaVideoBitRate ) )
        {           
        TUint16 bitrate = aVideo.ValueTObjectL<TUint16>( KMPXMediaVideoBitRate );
        
        if ( aObject.Property( *iBitratePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint16Property*>(property)->SetValueL( bitrate );
            }
        else
            {
            aObject.AddUint16PropertyL( *iBitratePropertyDef, bitrate );
            }
        }

    // 21. AUDIOFOURCC
    if ( aVideo.IsSupported( KVcxMediaMyVideosAudioFourCc ) )
        {
        TUint32 audioFourCc = aVideo.ValueTObjectL<TUint32>( KVcxMediaMyVideosAudioFourCc );
        
        if ( aObject.Property( *iAudioFourCcPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>(property)->SetValueL( audioFourCc );
            }
        else
            {
            aObject.AddUint32PropertyL( *iAudioFourCcPropertyDef, audioFourCc );
            }
        }

    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CVCXTestMdsDbModifier::GetSchemaDefinitionsL()
    {
    VCXLOGLO1( "CVCXTestMdsDbModifier::GetSchemaDefinitionsL" );

    //  Namespace
    iNamespaceDef = &(iMDSSession->GetDefaultNamespaceDefL());

    //  Default object definitions
    iVideoObjectDef = &(iNamespaceDef->GetObjectDefL( KVcxVideoObjectName ));

    //  Base object property definitions
    iTitlePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxTitlePropertyName )); //2
    iSizePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxSizePropertyName )); //5
    iCreationDatePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxCreationDatePropertyName )); //6
    iTimeOffsetPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxTimeOffsetPropertyName )); //6
    iItemTypePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxItemTypePropertyName )); //9
    iLastModifiedDatePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxLastModifiedDatePropertyName )); //10

    //  Media object property definitions
    iDescriptionPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxDescriptionPropertyName)); //3
    iCopyrightPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxCopyrightPropertyName )); //8
    iAuthorPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxAuthorPropertyName )); //13
    iOriginPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxOriginPropertyName )); //14
    iDurationPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxDurationPropertyName )); //15
    iDrmPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxDrmPropertyName )); //part of flags (7)
    iRatingPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxRatingPropertyName )); //19
    iBitratePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
                KVcxBitratePropertyName )); //20
    iAudioFourCcPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
                KVcxAudioFourCcPropertyName )); //21

    //  Video object property definitions
    iLastPlayPositionPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxLastPlayPositionPropertyName )); //16

    // remove these when default mds has all fields
    TRAP_IGNORE(

    iFlagsPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxFlagsPropertyName )); //7

    iAgeProfilePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxAgeProfilePropertyName )); //11

    iAudioLanguagePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxAudioLanguagePropertyName )); //12

    iDownloadIdPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxDownloadIdPropertyName )); //17

          );

    }

//  End of File
