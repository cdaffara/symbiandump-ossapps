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
* Description: hbvolumesliderpopup stub for testing mpvolumeslider
*
*/


#include "stub/inc/hbvolumesliderpopup.h"

/*
 * Stub function
 */
HbVolumeSliderPopup::HbVolumeSliderPopup( QObject *parent )
    : QObject( parent ),
      mValue( 0 ),
      mMinRange( 0 ),
      mMaxRange( 0 ),
      mSingleStep( 0 ),
      mVisible( false ),
      mSliderTickPosition( Hb::SliderTicksAbsolute )
{
}

/*
 * Stub function
 */
HbVolumeSliderPopup::~HbVolumeSliderPopup()
{
}

/*
 * Stub function
 */
int HbVolumeSliderPopup::value() const
{
    return mValue;
}

/*
 * Stub function
 */
bool HbVolumeSliderPopup::isVisible() const
{
    return mVisible;
}

/*
 * Stub function
 */
void HbVolumeSliderPopup::setVisible(bool visible)
{
    mVisible = visible;
}

/*
 * Stub function
 */
void HbVolumeSliderPopup::setRange(int min, int max)
{
    mMinRange = min;
    mMaxRange = max;
}

/*
 * Stub function
 */
void HbVolumeSliderPopup::setSingleStep(int step)
{
    mSingleStep = step;
}

/*
 * Stub function
 */
void HbVolumeSliderPopup::setTickPosition(Hb::SliderTickPositions position)
{
    mSliderTickPosition = position;
}

/*
 * Stub function
 */
void HbVolumeSliderPopup::setValue(int value)
{
    mValue = value;
}

//end of file
