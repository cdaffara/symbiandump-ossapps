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
* Description:  Implementation of HbVolumeSliderPopup
*
*/

// Version : %version:   1 %



#include "mpxvideo_debug.h"
#include "hbvolumesliderpopup.h"

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::HbVolumeSliderPopup
// -------------------------------------------------------------------------------------------------
//
HbVolumeSliderPopup::HbVolumeSliderPopup()
{
    MPX_ENTER_EXIT(_L("HbVolumeSliderPopup::HbVolumeSliderPopup()"));
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::~HbVolumeSliderPopup
// -------------------------------------------------------------------------------------------------
//
HbVolumeSliderPopup::~HbVolumeSliderPopup()
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::HbVolumeSliderPopup") );
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::setEnabled
// -------------------------------------------------------------------------------------------------
//
void HbVolumeSliderPopup::setEnabled( bool enabled )
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::setEnabled %d"), enabled );

    mEnabled = enabled;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::isEnabled
// -------------------------------------------------------------------------------------------------
//
bool HbVolumeSliderPopup::isEnabled()
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::isEnabled %d"), mEnabled );

    return mEnabled;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::setVisible
// -------------------------------------------------------------------------------------------------
//
void HbVolumeSliderPopup::setVisible( bool visible )
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::setVisible %d"), visible );

    mVisible = visible;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::isVisible
// -------------------------------------------------------------------------------------------------
//
bool HbVolumeSliderPopup::isVisible()
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::isVisible %d"), mVisible );

    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::setValue
// -------------------------------------------------------------------------------------------------
//
void HbVolumeSliderPopup::setValue( int value )
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::setValue %d"), value );

    mValue = value;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::setTimeout
// -------------------------------------------------------------------------------------------------
//
void HbVolumeSliderPopup::setTimeout( int timeout )
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::setTimeout %d"), timeout );

    mTimeOut = timeout;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::setTickPosition
// -------------------------------------------------------------------------------------------------
//
void HbVolumeSliderPopup::setTickPosition( Hb::SliderTickPositions position )
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::setTickPosition"));

    mPosition = position;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::setRange
// -------------------------------------------------------------------------------------------------
//
void HbVolumeSliderPopup::setRange( int min, int max )
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::setRange"));

    mMin = min;
    mMax = max;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::value
// -------------------------------------------------------------------------------------------------
//
int HbVolumeSliderPopup::value()
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::value"));

    return mValue;
}

// -------------------------------------------------------------------------------------------------
// HbVolumeSliderPopup::setSingleStep
// -------------------------------------------------------------------------------------------------
//
void HbVolumeSliderPopup::setSingleStep( int steps )
{
    MPX_DEBUG(_L("HbVolumeSliderPopup::setSingleStep"));

    mSteps = steps;
}

// End of file
