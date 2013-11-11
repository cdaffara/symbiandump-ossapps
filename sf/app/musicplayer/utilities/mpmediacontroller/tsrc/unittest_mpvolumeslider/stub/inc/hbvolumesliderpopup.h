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
* Description: hbvolumesliderpopup stub for testing mpvolumeslider
*
*/


#ifndef HBVOLUMESLIDERPOPUP_H
#define HBVOLUMESLIDERPOPUP_H

#include <QObject>

namespace Hb
{
    enum SliderTickPosition {
        NoSliderTicks = 0,
        SliderTicksAbove = 1,
        SliderTicksLeft = SliderTicksAbove,
        SliderTicksBelow = 2,
        SliderTicksRight = SliderTicksBelow,
        SliderTicksBothSides = 3,
        SliderTicksAbsolute
    };

    Q_DECLARE_FLAGS(SliderTickPositions , SliderTickPosition)
}

class HbVolumeSliderPopup : public QObject
{
    Q_OBJECT

public:
    explicit HbVolumeSliderPopup( QObject *parent = 0 );
    virtual ~HbVolumeSliderPopup();

    int value() const;
    bool isVisible() const;
    void setVisible(bool visible);
    void setRange(int min, int max);
    void setSingleStep(int step);
    void setTickPosition(Hb::SliderTickPositions position);

signals :
    void sliderPressed();
    void sliderReleased();
    void iconClicked();
    void valueChanged(int value);

public slots:
    void setValue(int value);

public:
    int                         mValue;
    int                         mMinRange;
    int                         mMaxRange;
    int                         mSingleStep;
    bool                        mVisible;
    Hb::SliderTickPositions     mSliderTickPosition;
};

#endif      // HBVOLUMESLIDERPOPUP_H

// End of File
