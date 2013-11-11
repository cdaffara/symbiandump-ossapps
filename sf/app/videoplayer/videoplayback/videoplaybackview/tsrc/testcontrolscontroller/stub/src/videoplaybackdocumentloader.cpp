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
* Description:  Implementation of VideoPlaybackDocumentLoader
*
*/

// Version : %version:  5 %



#include <hblistwidget.h>
#include <hblabel.h>
#include <hbtransparentwindow.h>
#include <hbratingslider.h>

#include "mpxvideo_debug.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackcontrolscontroller.h"

#include "videoplaybackcontrolbar.h"
#include "videoplaybackfiledetailswidget.h"
#include "videoplaybackcontrolconfiguration.h"
#include "videoplaybackdetailsplaybackwindow.h"


// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader( 
        VideoPlaybackControlsController *controller )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader()"));
    mController = controller;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::~VideoPlaybackDocumentLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader::~VideoPlaybackDocumentLoader()
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader : mWidgets.count(%d)"), 
            mWidgets.count() );

    for ( int i = 0 ; i < mWidgets.count() ; i++ )
    {
        mWidgets.removeAt( 0 );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::findWidget()
// -------------------------------------------------------------------------------------------------
//
QGraphicsWidget *VideoPlaybackDocumentLoader::findWidget( const QString &name )
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::findWidget(%s)"), name.data() );

    QGraphicsWidget *object = NULL;

    //int index = exist( name );
    mIndex = exist( name );

    if ( mIndex == -1 )
    {
        object = createWidget( name );
    }
    else
    {
        object = mWidgets[ mIndex ];
    }
    
    return object;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::createWidget()
// -------------------------------------------------------------------------------------------------
//
QGraphicsWidget *VideoPlaybackDocumentLoader::createWidget( const QString &name )
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::createWidget(%s)"), name.data() );

    QGraphicsWidget *object = NULL;
    
    if ( name == "transparentWindow" ||
         name == "small_transparentWindow" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating transparentWindow") );
        
        object = new HbTransparentWindow();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[transparentWindow] to object list") );
        mWidgets.append( object );                
    }
    else if ( name == "content" || name == "bitmapLayout" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating HbWidget") );
        
        object = new HbWidget();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[HbWidget] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "title" || name == "bufferingIcon" || 
              name == "tvOutBitmap" || name == "realAudioOnlyBitmap" ||
              name == "partialAudioOnlyBitmap" || name == "audioOnlyBitmap" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating HbLabel") );
        
        object = new HbLabel();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[HbLabel] to object list") );                
        mWidgets.append( object );
    }
    else if ( name == "controlBarLayout" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating controlBarLayout") );
    
        object = new VideoPlaybackControlBar( mController );
        object->setObjectName( name );
        
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[controlBarLayout] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "fileDetailsLayout" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating fileDetailsLayout") );
    
        object = new VideoPlaybackFileDetailsWidget( mController );
        object->setObjectName( name );
        
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[fileDetailsLayout] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "detailsPlaybackWindow" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating detailsPlaybackWindow") );
    
        object = new VideoPlaybackDetailsPlaybackWindow( mController );
        object->setObjectName( name );
        
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[detailsPlaybackWindow] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "fileDetails" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating HbListWidget") );
        
        object = new HbListWidget();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[HbListWidget] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "nonLocalPlayback" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating HbRatingSlider") );
        
        object = new HbRatingSlider();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[HbRatingSlider] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "rnLogoBitmap" )
    {
        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::creating rnLogoBitmap") );
        
        object = new HbWidget();
        object->setObjectName( name );

        MPX_DEBUG(_L("VideoPlaybackDocumentLoader::appending[rnLogoBitmap] to object list") );                        
        mWidgets.append( object );
    }

    return object;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::exist()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackDocumentLoader::exist( const QString &name )
{    
    int i = 0;

    for ( ; i < mWidgets.count() ; i++ )
    {
        if( mWidgets[i]->objectName() == name )
        {
            break;
        }
    }

    if ( i == mWidgets.count() )
    {
        i = -1;
    }

    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::exist(%s) returns %d"), 
            name.data(), i );

    return i;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::load()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackDocumentLoader::load( 
        const QString &fileName, const QString &section , bool *ok )
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::load(fileName=%s, section=%s)"), 
            fileName.data(), section.data() );
    
    Q_UNUSED( fileName );
    Q_UNUSED( section );
    *ok = true;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::load()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackDocumentLoader::load( 
        const QString &fileName, bool *ok )
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::load(fileName=%s)"), fileName.data() );
    Q_UNUSED( fileName );
    *ok = true;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackDocumentLoader::isVisible( int index )
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::isVisible(%d)"), index );
    
    return mWidgets[ index ]->isVisible();
}

// End of file
