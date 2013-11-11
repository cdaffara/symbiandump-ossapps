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
* Description:  Implementation of HbProgressSlider
*
*/

// Version : %version:   1 %

#include <qgraphicswidget>

#include "mpxvideo_debug.h"
#include "hbprogressslider.h"

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::HbProgressSlider
// -------------------------------------------------------------------------------------------------
//
HbProgressSlider::HbProgressSlider()
{
    MPX_ENTER_EXIT(_L("HbProgressSlider::HbProgressSlider()"));
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::~HbProgressSlider
// -------------------------------------------------------------------------------------------------
//
HbProgressSlider::~HbProgressSlider()
{
    MPX_DEBUG(_L("HbProgressSlider::HbProgressSlider") );
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::setRange
// -------------------------------------------------------------------------------------------------
//
void HbProgressSlider::setRange( int minimum, int maximum )
{
    MPX_DEBUG(_L("HbProgressSlider::setRange"));

    mMax = maximum;
    mMin = minimum;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::setMaxText
// -------------------------------------------------------------------------------------------------
//
void HbProgressSlider::setMaxText( const QString &text )
{
    MPX_DEBUG(_L("HbProgressSlider::setMaxText") );

    mMaxText = text;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::maxText
// -------------------------------------------------------------------------------------------------
//
QString HbProgressSlider::maxText()
{
    MPX_DEBUG(_L("HbProgressSlider::maxText") );

    return mMaxText;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::setMinText
// -------------------------------------------------------------------------------------------------
//
void HbProgressSlider::setMinText( const QString &text )
{
    MPX_DEBUG(_L("HbProgressSlider::setMinText") );

    mMinText = text;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::minText
// -------------------------------------------------------------------------------------------------
//
QString HbProgressSlider::minText()
{
    MPX_DEBUG(_L("HbProgressSlider::minText") );

    return mMinText;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::setProgressValue
// -------------------------------------------------------------------------------------------------
//
void HbProgressSlider::setProgressValue( int value )
{
    MPX_DEBUG(_L("HbProgressSlider::setProgressValue") );

    mProgressValue = value;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::setSliderValue
// -------------------------------------------------------------------------------------------------
//
void HbProgressSlider::setSliderValue( int value )
{
    MPX_DEBUG(_L("HbProgressSlider::setSliderValue") );

    mSliderValue = value;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::sliderValue
// -------------------------------------------------------------------------------------------------
//
int HbProgressSlider::sliderValue()
{
    MPX_DEBUG(_L("HbProgressSlider::sliderValue") );

    return mSliderValue;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::progressValue
// -------------------------------------------------------------------------------------------------
//
int HbProgressSlider::progressValue()
{
    MPX_DEBUG(_L("HbProgressSlider::progressValue") );

    return mProgressValue;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::maximum
// -------------------------------------------------------------------------------------------------
//
int HbProgressSlider::maximum()
{
    MPX_DEBUG(_L("HbProgressSlider::maximum") );

    return mMax;
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::press
// -------------------------------------------------------------------------------------------------
//
void HbProgressSlider::press()
{
    MPX_DEBUG(_L("HbProgressSlider::press") );

    emit sliderPressed();
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::release
// -------------------------------------------------------------------------------------------------
//
void HbProgressSlider::release()
{
    MPX_DEBUG(_L("HbProgressSlider::release") );

    emit sliderReleased();
}

// -------------------------------------------------------------------------------------------------
// HbProgressSlider::move
// -------------------------------------------------------------------------------------------------
//
void HbProgressSlider::move( int value )
{
    MPX_DEBUG(_L("HbProgressSlider::move") );

    emit sliderMoved( value );
}

// End of file
