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
* Description:   ?Description
*
*/

//Includes
#include <hbcombobox.h>
#include <hbmainwindow.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbpushbutton.h>
#include <QSize>

//User Includes
#include "glxuistd.h"
#include "glxviewids.h"
#include "glxslideshowsettingsview.h"
#include "glxsettinginterface.h"
#include "glxlocalisationstrings.h"
#include "glxcommandhandlers.hrh"

GlxSlideShowSettingsView::GlxSlideShowSettingsView( HbMainWindow *window ) 
    : GlxView( GLX_SLIDESHOWSETTINGSVIEW_ID ), 
      mEffect( NULL ), 
      mDelay( NULL ), 
      mWindow( window ),
      mContextlabel( NULL ), 
      mEffectlabel( NULL ), 
      mDelaylabel( NULL ), 
      mPlaySlideShow( NULL ),
      mSettings( NULL )
{
    mSettings = GlxSettingInterface::instance();
    setContentFullScreen( true );
}

GlxSlideShowSettingsView::~GlxSlideShowSettingsView()
{
    delete mContextlabel;
    delete mEffectlabel;
    delete mDelaylabel;
    delete mEffect;
    delete mDelay;
    delete mPlaySlideShow;
}

void GlxSlideShowSettingsView::setModel( QAbstractItemModel *model )
{
    Q_UNUSED( model )
    return;
}

void GlxSlideShowSettingsView::orientationChanged( Qt::Orientation )
{
    setLayout();
}

void GlxSlideShowSettingsView::playSlideShow()
{
    emit actionTriggered( EGlxCmdSelectSlideshow );
}

void GlxSlideShowSettingsView::activate()
{
    connect( mWindow, SIGNAL( orientationChanged( Qt::Orientation ) ), this, SLOT( orientationChanged( Qt::Orientation ) ) );
    
    if ( mContextlabel == NULL ) {
        mContextlabel = new HbLabel( GLX_MENU_SLIDESHOW, this );
        mContextlabel->setObjectName( "SlideShow" );
    }

    if ( mEffectlabel == NULL ) {
        mEffectlabel = new HbLabel( GLX_LABEL_TRANSITION_EFFECT, this );
        mEffectlabel->setObjectName( "Effect" );
    }

    if ( mEffect == NULL ) {
        mEffect = new HbComboBox( this );
        QStringList effectList = mSettings->slideShowEffectList();
        mEffect->addItems( effectList );
        mEffect->setObjectName( "Effect List" );
    }

    if ( mDelaylabel == NULL ) {
        mDelaylabel = new HbLabel( GLX_LABEL_TRANSITION_DELAY, this );
        mDelaylabel->setObjectName( "Delay" );
    }

    if ( mDelay == NULL ) {
        mDelay = new HbComboBox( this );
        mDelay->setObjectName( "Delay List" );
        QStringList delayList;
        delayList << GLX_VAL_SLOW << GLX_VAL_MEDIUM << GLX_VAL_FAST;
        mDelay->addItems( delayList );
    }
    
    if ( mPlaySlideShow == NULL ) {
        mPlaySlideShow = new HbPushButton( this );
        mPlaySlideShow->setText( GLX_BUTTON_PLAY ) ; 
        connect( mPlaySlideShow, SIGNAL( clicked( bool ) ), this, SLOT( playSlideShow() ) );
    }

    // Read the values from the cenrep    
    mEffect->setCurrentIndex( mSettings->slideShowEffectIndex() );
    mDelay->setCurrentIndex( mSettings->slideShowDelayIndex() );
    setLayout();
}

void GlxSlideShowSettingsView::setLayout()
{
    QSize sz = screenSize();

    mContextlabel->setGeometry( 5, 25, sz.width() - 50, 100 );
    mEffectlabel->setGeometry( 5, 60, sz.width() - 50, 100 );
    mEffect->setGeometry( 15, 130, sz.width() - 50, 100 );
    mDelaylabel->setGeometry( 5, 160, sz.width() - 50, 100 );
    mDelay->setGeometry( 15, 230, sz.width() - 50, 100 );
    if( mWindow->orientation() == Qt::Vertical ) {
        mPlaySlideShow->setGeometry( 120, 300, 120, 60 );
    }
    else {
        mPlaySlideShow->setGeometry( 260, 300, 120, 60 );
    }
}

void GlxSlideShowSettingsView::deActivate()
{
    //Store the current effect and delay before going back to the previous view
    mSettings->setslideShowEffectIndex( mEffect->currentIndex() );
    mSettings->setSlideShowDelayIndex( mDelay->currentIndex() );
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
}

