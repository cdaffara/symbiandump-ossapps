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
* Description: Music Player volume slider.
*
*/

#include <hbvolumesliderpopup.h>

#include "mpvolumeslider.h"
#include "mpenginefactory.h"
#include "mptrace.h"

const int KVolumeMin = 0;
const int KVolumeMuted = 1;
const int KDefaultSteps = 10;
const int KDataCompleted = 0x07; // 0x01 volume max, 0x02 volume level, 0x04 volume mute

/*!
 Constructs MpVolumeSlider.
 */
MpVolumeSlider::MpVolumeSlider( QObject *parent )
    : QObject( parent),
      mMpEngine( 0 ),
      mVolumeSliderPopup( 0 ),
      mEngineVolumeMax( 0 ),
      mEngineVolumeLevel( 0 ),
      mEngineMuted( false ),
      mEngineDataCompleted( 0 ),
      mSliderPressed( false ),
      mSliderStep( 0 )
{
    TX_ENTRY
    mMpEngine = MpEngineFactory::sharedEngine();
    connect( mMpEngine, SIGNAL( volumePropertyChanged( MpCommon::MpVolumeProperty, int ) ), 
             this, SLOT( handleVolumeProperty( MpCommon::MpVolumeProperty, int ) ) );

    //Request Volume properties
    mMpEngine->getMaxVolume();
    mMpEngine->getVolume();
    mMpEngine->getMuteState();
    TX_EXIT
}

/*!
 Destructs MpVolumeSlider.
 */
MpVolumeSlider::~MpVolumeSlider()
{
    TX_ENTRY
    delete mVolumeSliderPopup;
    TX_EXIT
}

/*!
 Make volume slider visible with latest engine data.
 */
void MpVolumeSlider::showVolumeSlider()
{
    TX_ENTRY

    if ( !mSliderPressed ) {

        // Initialize popup until needed, in order to save time during app start up.
        if ( !mVolumeSliderPopup ) {
            initializeVolumeSliderPopup();
        }

        // Update VolumeSlider with Engine values
        if ( mEngineMuted ) {
            if ( mVolumeSliderPopup->value() != KVolumeMin ) {
                // Muted from outside (possibly from Headset or MediaKeys), update volume slider as "muted"
                // Todo: Replace with a more appropriate method to mute slider, when provided by Orbit.
                mVolumeSliderPopup->setValue( KVolumeMin );
            }
        }
        else if ( mVolumeSliderPopup->value() != ( mEngineVolumeLevel / mSliderStep ) ){
            mVolumeSliderPopup->setValue( mEngineVolumeLevel / mSliderStep );
        }

        // Show VolumeSlider
        if ( !mVolumeSliderPopup->isVisible() ) {
            mVolumeSliderPopup->setVisible( true );
        }
    }

    TX_EXIT
}

/*!
 Slot to handle volume property notifications
 */
void MpVolumeSlider::handleVolumeProperty( MpCommon::MpVolumeProperty property, int value )
{
    TX_ENTRY_ARGS("Property: " << property << ", Value: " << value );
    switch( property )
    {
        case MpCommon::MaxVolume :
            mEngineVolumeMax = value;
            mEngineDataCompleted |= 0x01;
            break;
        case MpCommon::Volume :
            mEngineVolumeLevel = value;
            // Framework automatically mutes if volume == KVolumeMin and unmutes when volume > KVolumeMin
            mEngineMuted = mEngineVolumeLevel == KVolumeMin; 
            if ( mEngineDataCompleted == KDataCompleted ) {
                showVolumeSlider();
            }
            else {
                mEngineDataCompleted |= 0x02;
            }
            break;
        case MpCommon::MuteState :
            mEngineMuted = ( value == KVolumeMuted );
            if ( mEngineDataCompleted == KDataCompleted ) {
                showVolumeSlider();
            }
            else {
                mEngineDataCompleted |= 0x04;
            }
            break;
        default:
            TX_LOG_ARGS( "We should never be here" );
            break;
    }
    TX_EXIT
}

/*!
 Slot called when slider is pressed.
 */
void MpVolumeSlider::sliderPressed()
{
    TX_ENTRY
    mSliderPressed = true;
    TX_EXIT
}

/*!
 Slot called when slider is released.
 */
void MpVolumeSlider::sliderReleased()
{
    TX_ENTRY
    mSliderPressed = false;
    TX_EXIT
}

/*!
 Slot called when mute icon is clicekd.
 */
void MpVolumeSlider::muteIconClicked()
{
    TX_ENTRY
    if ( mVolumeSliderPopup->value() ) {
        mMpEngine->unmute();
        mEngineMuted = false;
    }
    else {
        mMpEngine->mute();
        mEngineMuted = true;
    }
    TX_EXIT
}

/*!
 Slot called when volume slider value changes.
 */
void MpVolumeSlider::volumeSliderChanged( int value )
{
    TX_ENTRY

    if ( mEngineVolumeLevel != ( value * mSliderStep ) ) {
        // When Mute icon is clicked, slider value changes automatically to zero,
        // don't send that change in order to avoid reset the engine previous volume.
        if ( !( mEngineMuted && value == KVolumeMin ) ) {
            mMpEngine->setVolume( value * mSliderStep );
        }
    }

    TX_EXIT
}

/*!
 Inernal
 */
void MpVolumeSlider::initializeVolumeSliderPopup()
{
    TX_ENTRY

    mVolumeSliderPopup = new HbVolumeSliderPopup();
    mVolumeSliderPopup->setRange( KVolumeMin, KDefaultSteps );
    mVolumeSliderPopup->setSingleStep( 1 );
    mVolumeSliderPopup->setTickPosition( Hb::NoSliderTicks );
    mSliderStep = mEngineVolumeMax / KDefaultSteps ;

    connect( mVolumeSliderPopup, SIGNAL( sliderPressed() ), 
             this, SLOT( sliderPressed() ) );
    connect( mVolumeSliderPopup, SIGNAL( sliderReleased() ), 
             this, SLOT( sliderReleased() ) );
    connect( mVolumeSliderPopup, SIGNAL( iconClicked() ), 
             this, SLOT( muteIconClicked() ) );
    // volumeChanged event is also sent when VolumeSlider is Muted/Unmuted
    // QueuedConnection used in order to handle Mute events first,
    // and then be able to know whether to apply the slider volume changes to the engine
    connect( mVolumeSliderPopup, SIGNAL( valueChanged( int ) ), 
             this, SLOT( volumeSliderChanged( int ) ),
             Qt::QueuedConnection );

    TX_EXIT
}

