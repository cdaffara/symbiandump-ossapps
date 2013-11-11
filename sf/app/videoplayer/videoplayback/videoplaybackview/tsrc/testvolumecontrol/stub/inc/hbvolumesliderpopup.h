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



#ifndef HBVOLUMESLIDERPOPUP_H
#define HBVOLUMESLIDERPOPUP_H

#include <hbwidget.h>


class HbVolumeSliderPopup : public HbWidget
{
    Q_OBJECT

    public:
        HbVolumeSliderPopup();
        virtual ~HbVolumeSliderPopup();

    public:
        void setEnabled( bool enabled );
        void setVisible( bool visible );
        bool isEnabled();
        bool isVisible();
        void setValue( int value );
        void setTimeout( int timeout );
        void setTickPosition( Hb::SliderTickPositions position );
        void setRange( int min, int max );
        int value();
        void setSingleStep( int steps );

    signals:
        void valueChanged( int );
        void iconClicked();
        void sliderPressed();
        void sliderReleased();

    public:
        bool mVisible;
        bool mEnabled;
        int  mValue;
        int  mTimeOut;
        int  mMin;
        int  mMax;
        int  mSteps;

        Hb::SliderTickPositions mPosition;
};

#endif /*HBVOLUMESLIDERPOPUP_H*/

