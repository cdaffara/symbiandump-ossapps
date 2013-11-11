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
* Description:  Implementation of VideoPlaybackWrapper
*
*/

// Version : %version:  9 %


#include "mpxvideo_debug.h"
#include "videoplaybackwrapper.h"
#include "mpxvideoplayerappuiengine.h"


// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::VideoPlaybackWrapper()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackWrapper::VideoPlaybackWrapper()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::VideoPlaybackWrapper()"));

    initializePlugins();
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::~VideoPlaybackWrapper()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackWrapper::~VideoPlaybackWrapper()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::~VideoPlaybackWrapper()"));

    if ( mUiEngine ) 
    {
        delete mUiEngine;
        mUiEngine = NULL;
    }        
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::initializePlugins()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::initializePlugins()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::initializePlugins()"));

    TRAPD( err, mUiEngine = CMpxVideoPlayerAppUiEngine::NewL( this ) );
    MPX_DEBUG(_L("VideoPlaybackWrapper::initializePlugins err = %d"), err);
    Q_UNUSED(err);
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::playMedia()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::playMedia( QString aFileName )
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia( %s )"), aFileName.data() );   
    
    TBuf<KMaxFileName> filename( aFileName.utf16() );
    int error = openFileWithNativePath( filename );

    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia err = %d"), error);

    return error;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::replayMedia()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::replayMedia( QString aFileName )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::replayMedia"));   
    
    TBuf<KMaxFileName> name( aFileName.utf16() );
    
    int error = mUiEngine->ReplayAfterPriorTermination( name );
    
    return error;
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::playMedia()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::playMedia( RFile aFile )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::playMedia( aFile )"));   
    
    TRAPD( error, mUiEngine->OpenFileL( aFile ) );

    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia err = %d"), error);

    return error;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::openPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::openPlaybackView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::openPlaybackView()"));

    emit handlePlaybackView( MpxHbVideoCommon::ActivatePlaybackView );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::postInit()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::lateInit()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::lateInit()"));
    
    TRAP_IGNORE( mUiEngine->LateInitL() );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::openFileWithNativePath()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::openFileWithNativePath(const TDesC& aFileName)
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::openFileWithNativePath()"));    
    
    int err = KErrNone;

    int fwdSlashPos = aFileName.LocateF('/');

    if( fwdSlashPos == KErrNotFound )
    {
        // no fwd slashes found => filepath is already in 
        // preferred format eg. c:\\data\\videos\\test.3gp
        //
        TRAP( err, mUiEngine->OpenFileL( aFileName ) );        
    }
    else
    {        
        HBufC* fileName = NULL;

        TRAP( err,  fileName = aFileName.AllocL() );

        if ( err == KErrNone )
        {
            int count( fileName->Des().Length() );

            for ( int j = fwdSlashPos ; j < count; ++j )
            {
                if ( fileName->Des()[j]== '/' )
                {
                    fileName->Des()[j]='\\';
                }
            }
            
        }

        TRAP( err, mUiEngine->OpenFileL( fileName->Des() ) );
        
        if ( fileName )
        {
            delete fileName;
            fileName = NULL;
        }        
    }
    
    return err;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::resloveErrorString()
// -------------------------------------------------------------------------------------------------
//
const QString VideoPlaybackWrapper::resloveErrorString( int errorCode )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::resloveErrorString()"));
    
    TPtrC text;
    TRAP_IGNORE( text.Set(mUiEngine->ResolveErrorStringL(errorCode)) );
    
    const QString qString( (QChar*)text.Ptr(), text.Length() );
    
    return qString;
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::playMedia()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::playURI( QString aUri )
{
    MPX_ENTER_EXIT(_L("QMpxVideoPlaybackWrapper::playURI"));   
    
    TBuf<KMaxFileName> uri( aUri.utf16() );
    int error = openURI( uri );

    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::playURI err = %d"), error);

    return error;
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::openURI()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::openURI(const TDesC& aUri)
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::openURI()"));    
    
    int err = KErrNone;

    TRAP( err, mUiEngine->OpenFileL( aUri ) );

    return err;
}
// End of File
