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

#ifndef HBVOLUMESLIDERPOPUP_H
#define HBVOLUMESLIDERPOPUP_H

#include <QObject>
#include <hbpopup.h>

extern bool m_volumeSliderContructorCalled;
extern bool m_volumeSliderDesctructorCalled;
extern bool m_volumeSliderSetValueCalled;
extern int m_sliderValue;
extern bool m_volumeSliderValueCalled;
extern bool m_volumeSliderSetRangeCalled;
extern int m_sliderRangeMin;
extern int m_sliderRangeMax;
extern bool m_volumeSliderSetTimeoutCalled;
extern int m_sliderTimeout;
extern bool m_volumeSliderShowCalled;
extern bool m_isVisible;
extern bool m_volumeSliderHideCalled;

class HbVolumeSliderPopup : public QObject
{
    Q_OBJECT
    
public:
    HbVolumeSliderPopup ();
    ~HbVolumeSliderPopup ();
    
    void setValue (int value);
    int value ();
    void setRange (int min, int max);
    void setTimeout (int time);
    void show();
    void hide();
    void setDismissPolicy( HbPopup::DismissPolicy value ) {;};
    int minimum() {return m_sliderRangeMin;};
    int maximum() {return m_sliderRangeMax;};
    bool isVisible() {return m_isVisible;};
    
    
signals:
    void aboutToClose ();
    void valueChanged (int);

};

#endif // HBVOLUMESLIDERPOPUP_H