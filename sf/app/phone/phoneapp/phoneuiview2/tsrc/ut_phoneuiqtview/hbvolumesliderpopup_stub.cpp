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
* Description: 
*
*/

#include "hbvolumesliderpopup.h"

bool m_volumeSliderContructorCalled;
bool m_volumeSliderDesctructorCalled;
int m_sliderValue;
int m_sliderRangeMin;
int m_sliderRangeMax;
bool m_volumeSliderSetValueCalled;
bool m_volumeSliderValueCalled;
bool m_volumeSliderSetRangeCalled;
bool m_volumeSliderSetTimeoutCalled;
int m_sliderTimeout;
bool m_volumeSliderShowCalled;
bool m_isVisible;
bool m_volumeSliderHideCalled;

HbVolumeSliderPopup::HbVolumeSliderPopup ()
{
    m_volumeSliderContructorCalled = true;
    m_volumeSliderDesctructorCalled = false;
    m_sliderValue = -1;
    m_sliderRangeMin = -1;
    m_sliderRangeMax = -1;
    m_volumeSliderSetValueCalled = false;
    m_volumeSliderValueCalled = false;
    m_volumeSliderSetRangeCalled = false;
    m_volumeSliderSetTimeoutCalled = false;
    m_sliderTimeout = -1;
    m_volumeSliderShowCalled = false;
}

HbVolumeSliderPopup::~HbVolumeSliderPopup ()
{
    m_volumeSliderDesctructorCalled = true;
    m_volumeSliderContructorCalled = false;
    m_sliderValue = -1;
    m_sliderRangeMin = -1;
    m_sliderRangeMax = -1;
    m_volumeSliderSetValueCalled = false;
    m_volumeSliderValueCalled = false;
    m_volumeSliderSetRangeCalled = false;
    m_volumeSliderSetTimeoutCalled = false;
    m_sliderTimeout = -1;
    m_volumeSliderShowCalled = false;
    
    emit aboutToClose ();
}

void HbVolumeSliderPopup::setValue (int value)
{
    m_volumeSliderSetValueCalled = true;
    m_sliderValue = value;
}

int HbVolumeSliderPopup::value ()
{
    m_volumeSliderValueCalled = true;
    return m_sliderValue;
}

void HbVolumeSliderPopup::setRange (int min, int max)
{
    m_volumeSliderSetRangeCalled = true;
    m_sliderRangeMin = min;
    m_sliderRangeMax = max;
}

void HbVolumeSliderPopup::setTimeout (int time)
{
    m_volumeSliderSetTimeoutCalled = true;
    m_sliderTimeout = time;	
}

void HbVolumeSliderPopup::show ()
{
    m_volumeSliderShowCalled = true;
    m_isVisible = true;
}

void HbVolumeSliderPopup::hide()
{
    m_volumeSliderHideCalled = true;
    m_isVisible = false;
}

