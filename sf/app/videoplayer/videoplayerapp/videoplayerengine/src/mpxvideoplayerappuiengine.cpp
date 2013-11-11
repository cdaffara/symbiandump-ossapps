/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMpxVideoPlayerAppUiEngine
 *
*/

// Version : %version:  ou1cpsw#14 %



#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxplaybackutility.h>
#include <mpxcollectionmessage.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include <mpxcollectionpath.h>
#include <videoplayeractivationmessage.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionplugin.hrh>
#include <mpxmediageneralextdefs.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>

#include <coeutils.h>
#include <textresolver.h>
#include <videoplaylistutility.h>
#include <mpxvideoplaybackdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>

#include "mpxvideoplayerappuiengine.h"
#include "mpxvideoplayerconstants.h"
#include "videoplaybackwrapper.h"
#include "mpxhbvideocommondefs.h"
#include "videoactivitystate.h" 

#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::CMpxVideoPlayerAppUiEngine
// -------------------------------------------------------------------------------------------------
//
CMpxVideoPlayerAppUiEngine::CMpxVideoPlayerAppUiEngine( VideoPlaybackWrapper* aWrapper )
    : iPlaybackUtility( NULL ),
      iCollectionUtility( NULL ),
      iRecognizer( NULL ),
      iAccessPointId( KUseDefaultIap ),
      iMultilinkPlaylist( EFalse ),
      iSeekable( ETrue ),
      iUpdateSeekInfo( EFalse ),
      iPlaybackWrapper( aWrapper )
{
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::NewL
// -------------------------------------------------------------------------------------------------
//
CMpxVideoPlayerAppUiEngine* CMpxVideoPlayerAppUiEngine::NewL( VideoPlaybackWrapper* aWrapper )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::NewL()"));

    CMpxVideoPlayerAppUiEngine* self = new( ELeave) CMpxVideoPlayerAppUiEngine( aWrapper );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ConstructL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ConstructL()
{    
    TUid collectionMode( KUidMpxVideoPlayerApplication );

    iCollectionUtility = MMPXCollectionUtility::NewL( this, collectionMode ); 
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::PostInitL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::LateInitL()
{	       
    CreatePlaybackUtilityL();
    
    if ( ! iRecognizer )
    {
        iRecognizer = CMediaRecognizer::NewL();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::CreateCollectionUtilityMemberVariablesL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::CreateCollectionUtilityMemberVariablesL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::CreateCollectionUtilityMemberVariablesL()"));

    if ( ! iCollectionUtility )
    {
        //  PLAYLIST default mode
        iCollectionUtility = MMPXCollectionUtility::NewL( this );
    }

    if ( ! iCollectionUiHelper )
    {
        iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::CreatePlaybackUtilityL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::CreatePlaybackUtilityL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::CreatePlaybackUtilityL()"));

    if ( ! iPlaybackUtility )
    {
        //
        //  Create VideoHelix playback plugin
        //
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeNewPlayer );
        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        manager.SelectPlayerL( KVideoHelixPlaybackPluginUid );
        iPlaybackUtility->AddObserverL( *this );
        iPlaybackUtility->CommandL( EPbCmdSetAutoResume, EFalse );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine
// -------------------------------------------------------------------------------------------------
//
CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine()"));

    if ( iRecognizer )
    {
        delete iRecognizer;
        iRecognizer = NULL;
    }

    if ( iCollectionUtility )
    {
        iCollectionUtility->Close();
    }

    if ( iCollectionUiHelper )
    {
        iCollectionUiHelper->Close();
    }

    if ( iPlaybackUtility )
    {
        TRAP_IGNORE( iPlaybackUtility->CommandL( EPbCmdClose ) );
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
    }
}

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandlePlaybackMessage()"));

    if ( aError == KErrNone && aMessage )
    {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
    }
}

// ---------------------------------------------------------------------------
// From MMPXViewActivationObserver.
// Handle view activation.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleViewActivation( const TUid& /*aCurrentViewType*/,
                                                       const TUid& /*aPreviousViewType*/ )
{
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::OpenFileL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::OpenFileL( RFile& aFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::OpenFileL(RFile)"));

    TFileName filename;
    aFile.FullName(filename);
    
    LateInitL();

    CMediaRecognizer::TMediaType mediaType = iRecognizer->IdentifyMediaTypeL( filename, aFile );

    if ( mediaType == CMediaRecognizer::ELocalRamFile ||
         mediaType == CMediaRecognizer::ELocalAsxFile )
    {
        HandleMultiLinksFileL( aFile, mediaType );
    }
    else if ( mediaType == CMediaRecognizer::ELocalSdpFile )
    {
        iPlaybackUtility->InitStreamingL( aFile, iAccessPointId );
    }
    else
    {
        iPlaybackUtility->InitL( aFile );
    }    

    iRecognizer->FreeFilehandle();

    aFile.Close();
}


// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::OpenFileL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::OpenFileL( const TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::OpenFileL()"),
                   _L("aFileName = %S"), &aFileName);    
    
    LateInitL();
    
    CMediaRecognizer::TMediaType mediaType = iRecognizer->IdentifyMediaTypeL(aFileName);

    if ( mediaType == CMediaRecognizer::ELocalVideoFile )
    {
        iPlaybackUtility->InitL( aFileName );
    }
    // check if aFileName is a path to a ram or asx file
    //          eg. c:\\data\\videos\\ramfile.ram
    else if ( mediaType == CMediaRecognizer::ELocalRamFile ||
              mediaType == CMediaRecognizer::ELocalAsxFile )
    {
        HandleMultiLinksFileL( aFileName, mediaType );
    }
    // check if this is a url eg. rtsp://someaddress/file.3gp
    //
    else if ( iRecognizer->IsValidStreamingPrefix(aFileName) )
    {
        HandleUrlDesL( aFileName );
    }
    else if ( mediaType == CMediaRecognizer::ELocalSdpFile )
    {
        iPlaybackUtility->InitStreamingL( aFileName,
                                         (TDesC8*)(&KDATATYPEVIDEOHELIX),
                                         iAccessPointId );
    }
    else
    {
        iPlaybackUtility->InitL( aFileName );
    }

    iRecognizer->FreeFilehandle();
}

// -------------------------------------------------------------------------------------------------
// Open MPX media to player.
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::OpenMediaL( const CMPXMedia& aMedia )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::OpenMediaL()"));

    // Assume MPX medias are always local clips
    CMPXCollectionPath* mediaPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL( mediaPath );

    //
    // Workaround. Need a file like vcxmyvideosuids.h 
    //
    mediaPath->AppendL( 0x20016B97 );
    mediaPath->AppendL( KVcxMvcCategoryIdAll );
        
    mediaPath->AppendL( aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    mediaPath->SelectL( aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

    CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *mediaPath );
    CleanupStack::PushL( playList );
    playList->SetSingleItemPlaylist();
    playList->SetToFirst();
    
    LateInitL();
    
    iPlaybackUtility->InitL( *playList, ETrue );

    CleanupStack::PopAndDestroy( playList );
    CleanupStack::PopAndDestroy( mediaPath );
}

// -------------------------------------------------------------------------------------------------
// Handle playback message.
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoHandlePlaybackMessageL()"));

    TMPXMessageId id( *aMessage.Value<TMPXMessageId>( KMPXMessageGeneralId ) );

    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandlePlaybackMessageL TMPXMessageId = 0x%08x"), id );

    if ( KMPXMessageGeneral == id )
    {
        TInt event( *aMessage.Value<TInt>( KMPXMessageGeneralEvent ) );

        MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandlePlaybackMessageL() event = %d"), event);

        switch ( event )
        {
            case TMPXPlaybackMessage::EPlayerChanged:
            {
                MPX_DEBUG(_L("    EPlayerChanged"));

                if ( iUpdateSeekInfo )
                {
                    // The plugin has been instantiated, update the media
                    UpdatePbPluginMediaL();
                    iUpdateSeekInfo = EFalse;
                }

                HandlePlaybackPlayerChangedL();
                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// Handle playback message for player changed.
// Activate the view which support this player
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandlePlaybackPlayerChangedL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandlePlaybackPlayerChangedL()"));

    ActivateVideoPlaybackView();
}

// -------------------------------------------------------------------------------------------------
// Creates plugin resolver + creates & activates the video view
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ActivateVideoPlaybackView()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ActivateVideoPlaybackView()"));
    
    iPlaybackWrapper->openPlaybackView();
}

// -------------------------------------------------------------------------------------------------
// Closed mpx components and readies to application takedown.
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::PrepareCloseMpxL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::PrepareCloseMpxL()"));

    if ( iPlaybackUtility )
    {
        iPlaybackUtility->CommandL( EPbCmdClose );
        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        TRAP_IGNORE( manager.ClearSelectPlayersL() );
    }
}

// -------------------------------------------------------------------------------------------------
// Handle collection message
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL()"));

    TMPXMessageId id = *(aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ));

    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL() TMPXMessageId = 0x%08x"), id );

    if ( id == KMPXMessageGeneral )
    {
        TInt event( *( aMessage->Value<TInt> ( KMPXMessageGeneralEvent )) );
        TInt type( *( aMessage->Value<TInt> ( KMPXMessageGeneralType )) );
        TInt data( *( aMessage->Value<TInt> ( KMPXMessageGeneralData )) );

        MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL event %d, type %d, data %d"), event, type, data );

        if ( event == TMPXCollectionMessage::EPathChanged && type == EMcPathChangedByOpen )
        {
            if ( data == EMcItemOpened )
            {
                // An item was opened on the collection. Get the media attributes
                // on the item so we can initiatiate playback
                MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL: EMcItemOpened received. Can open video"));

                CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();

                CleanupStack::PushL( cPath );

                if ( cPath->Count() > 0 )
                {
                    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL: Call MediaL() on collection."));

                     RArray<TMPXAttribute> attribs;
                    CleanupClosePushL( attribs );

                    attribs.Append( KMPXMediaGeneralUri );
                    attribs.Append( KVcxMediaMyVideosDownloadId );
                    iCollectionUtility->Collection().MediaL( *cPath, attribs.Array() );

                    CleanupStack::PopAndDestroy( &attribs );
                }

                CleanupStack::PopAndDestroy( cPath );
            }
            else if ( data == EMcContainerOpened && iMultilinkPlaylist )
            {
                // start the playback for streaming playlists
                // once the playlist is opened
                iMultilinkPlaylist = EFalse;

                // The playlist was opened, initiatiate playback
                MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL: EMcContainerOpened received. initiatiate playback"));

                CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL( cPath );

                if ( cPath->Count() > 0 )
                {
                    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL: Call Playback utility with the playlist."));

                    CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL( *cPath );
                    CleanupStack::PushL( playlist );

                    // Set the AutoPlay mode to false
                    playlist->SetAutoPlay(EFalse);
                    // set pre init plugin to false
                    playlist->SetPreInitPlugin(EFalse);

                    iPlaybackUtility->InitL( *playlist, ETrue );
                    CleanupStack::PopAndDestroy( playlist );
                }
                CleanupStack::PopAndDestroy( cPath );
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle media properties.
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::DoHandelCollectionMediaL( const CMPXMedia& aMedia )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoHandelCollectionMediaL()"));

    const TDesC& fileUri = aMedia.ValueText(KMPXMediaGeneralUri);

    MPX_DEBUG(_L("Video URI: %S"), &fileUri );

    //OpenMediaL( aMedia ); // Initialize and initiate playback of a single video

    LateInitL();
    
    TPtrC mediaFile( aMedia.ValueText( KMPXMediaGeneralUri ) );
    CMediaRecognizer::TMediaType mediaType = iRecognizer->IdentifyMediaTypeL(mediaFile);
        
    if ( mediaType == CMediaRecognizer::ELocalRamFile ||
         mediaType == CMediaRecognizer::ELocalAsxFile )
    {   
        HandleMultiLinksFileL( mediaFile, mediaType );
    }    
    else
    {
        OpenMediaL( aMedia ); // Initialize and initiate playback of a single video
    }    
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL( const TDesC& aFileName,
                                                        CMediaRecognizer::TMediaType aMediaType )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL()"),
                   _L("aFileName = %S, type = %d"), &aFileName, aMediaType );

    TBool singleLink;
    TBool localFile;

    CVideoPlaylistUtility* playlistUtil = CVideoPlaylistUtility::NewL();
    CleanupStack::PushL(playlistUtil);

    playlistUtil->GetFileInfoL( aFileName, aMediaType, singleLink, localFile );

    DoHandleMultiLinksFileL( playlistUtil, singleLink, localFile );

    CleanupStack::PopAndDestroy( playlistUtil );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL( RFile& aFile,
                                                        CMediaRecognizer::TMediaType aMediaType )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL(RFile)"));

    // playlist
    TBool singleLink;
    TBool localFile;

    CVideoPlaylistUtility* playlistUtil = CVideoPlaylistUtility::NewL();
    CleanupStack::PushL(playlistUtil);

    playlistUtil->GetFileInfoL( aFile, aMediaType, singleLink, localFile );

    DoHandleMultiLinksFileL( playlistUtil, singleLink, localFile );

    CleanupStack::PopAndDestroy( playlistUtil );
}


void CMpxVideoPlayerAppUiEngine::DoHandleMultiLinksFileL( CVideoPlaylistUtility* aPlaylistUtil,
                                                          TBool aSingleLink,
                                                          TBool aLocalFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoHandleMultiLinksFileL()"),
                   _L("aSingleLink = %d aLocalFile %d "), aSingleLink, aLocalFile);

    if ( aSingleLink )
    {
        TPtrC link;
        link.Set( aPlaylistUtil->GetLinkLC() );
        iUpdateSeekInfo = ETrue;
        iSeekable = aPlaylistUtil->IsSeekable();

        if ( aLocalFile )
        {
            iPlaybackUtility->InitL( link );
        }
        else
        {
            iPlaybackUtility->InitStreamingL( link,
                                             (TDesC8*)(&KDATATYPEVIDEOHELIX),
                                             iAccessPointId );
        }
        CleanupStack::PopAndDestroy();  // link
    }
    else
    {

        CMPXMedia* playlist = aPlaylistUtil->GetPlayListL( iAccessPointId );
        CleanupStack::PushL( playlist );

        //  Create FW utility member variables for playlist embedded use case
        CreateCollectionUtilityMemberVariablesL();

        // Set the flag to true
        iMultilinkPlaylist = ETrue;

        // load the in memory plugin
        iCollectionUiHelper->OpenL( KUidMpxVideoPlayerApplication,
                                    *playlist,
                                    this,
                                    EMPXCollectionPluginGallery );

        CleanupStack::PopAndDestroy( playlist );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleUrlDesL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleUrlDesL( const TDesC& aUrl )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleUrlDesL()"),
                   _L("aUrl = %S"), &aUrl );

    iPlaybackUtility->InitStreamingL( aUrl,
                                      (TDesC8*)(&KDATATYPEVIDEOHELIX),
                                      iAccessPointId );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleMessageL()
// -------------------------------------------------------------------------------------------------
//
TBool CMpxVideoPlayerAppUiEngine::HandleMessageL( TUid /* aMessageUid */,
                                                  const TDesC8& /* aMessageParameters */ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleMessageL()"));

    TBool msgHandled = EFalse;

    return msgHandled;
}

// -------------------------------------------------------------------------------------------------
// Handle Custom messages from the collection
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleCollectionMessage()"));

    if ( aError == KErrNone && aMessage )
    {
        TRAP_IGNORE( DoHandleCollectionMessageL( aMessage ) )
    }
}

// -------------------------------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle media properties.
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleCollectionMediaL()"));

    if ( aError == KErrNone )
    {
        DoHandelCollectionMediaL( aMedia );
    }
}

// -------------------------------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handles the collection entries being opened.
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleOpenL( const CMPXMedia& /* aEntries */,
                                              TInt /* aIndex */,
                                              TBool /* aComplete */,
                                              TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleOpenL(CMPXMedia)"));

    if ( aError != KErrNone )
    {
        //
        //  Clip the collection path to root level and reopen
        //  This could happen after a USB refresh event where the
        //  item no longer exists
        //
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();

        CleanupStack::PushL( path );

        while( path->Levels() > 1 )
        {
            path->Back();
        }

        iCollectionUtility->Collection().OpenL( *path );
        CleanupStack::PopAndDestroy( path );
    }
}

// -------------------------------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handles the item being opened.
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleOpenL(CMPXCollectionPlaylist)"));

    if ( aError == KErrNone )
    {
        iPlaybackUtility->InitL( aPlaylist, EFalse );
    }
}

// -------------------------------------------------------------------------------------------------
// From MMPXCHelperEmbeddedOpenObserver
// Handles OpenL from that occured in embedded mode
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleEmbeddedOpenL( TInt /*aErr*/, TMPXGeneralCategory /*aCategory*/ )
{
    MPX_DEBUG(_L("CMPXVideoAppUi::HandleEmbeddedOpenL()"));
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::UpdatePbPluginMedia()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::UpdatePbPluginMediaL()
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::UpdatePbPluginMediaL()iSeekable %d"), iSeekable);

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );

    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand, 
                                                     EPbCmdUpdateSeekable );

    cmd->SetTObjectValueL<TBool>( KMPXMediaGeneralExtVideoSeekable, iSeekable );

    iPlaybackUtility->CommandL( *cmd );
    
    CleanupStack::PopAndDestroy( cmd );
}


// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ReplayAfterPriorTermination()
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayerAppUiEngine::ReplayAfterPriorTermination(const TDesC& aFileName)
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::ReplayAfterPriorTermination()"));      
    
    TInt error = KErrNone; 
    
    RFile file;
    RFs fs;
    TInt fileError = fs.Connect();
    if ( fileError == KErrNone )
    {
        fileError = file.Open( fs, aFileName, EFileRead | EFileShareReadersOrWriters  );
    
        if ( fileError == KErrNone && file.SubSessionHandle() )
        {
            file.Close();
            
            ReadActivityData();
            TMPXItemId mpxItemId(iLastPlayedItemId);
                               
            MPX_TRAP( error,         
                CMPXMedia* media = CMPXMedia::NewL();
                CleanupStack::PushL(media);
                
                media->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem );  
                media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxItemId );
                media->SetTextValueL( KMPXMediaGeneralTitle, aFileName );
                media->SetTextValueL( KMPXMediaGeneralUri, aFileName );    
                
                OpenMediaL( *media );
                
                CleanupStack::PopAndDestroy( media );
                );        
        }  
        fs.Close();
    }
    
    return error;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ReadActivityData()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ReadActivityData()
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::ReadActivityData()"));   
        
    iLastPlayedItemId = 
            VideoActivityState::instance().getActivityData(KEY_LAST_PLAYED_MEDIA_ID).toUInt();    
    
}


// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ResolveErrorStringL()
// -------------------------------------------------------------------------------------------------
//
const TDesC& CMpxVideoPlayerAppUiEngine::ResolveErrorStringL(TInt aErrorCode)
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::ResolveErrorStringL()"));   
    
    CTextResolver* textresolver = CTextResolver::NewL();
    
    const TDesC& text = textresolver->ResolveErrorString( aErrorCode );
    
    delete textresolver;
    
    return text;       
}

// EOF

