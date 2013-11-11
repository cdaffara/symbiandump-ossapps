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

// Version : %version:   2 %



#ifndef HBPROGRESSSLIDER_H_
#define HBPROGRESSSLIDER_H_

#include <hbwidget.h>
#include <hbstyle.h>

class QGraphicsItem;

class HbProgressSlider : public HbWidget
{
    Q_OBJECT

    public:
        HbProgressSlider();
        virtual ~HbProgressSlider();

    public:
        void setRange( int minimum, int maximum );

        void setMaxText( const QString &text );
        QString maxText();

        void setMinText( const QString &text );
        QString minText();

        void setProgressValue( int value );
        void setSliderValue( int value );

        int sliderValue();
        int progressValue();

        int maximum();

    public:
        void press();
        void release();
        void move( int value );

    signals:
        void sliderPressed();
        void sliderReleased();
        void sliderMoved( int value );
        void trackPressed();
        void trackReleased();

    public:
        int mSliderValue;
        int mProgressValue;
        int mMax;
        int mMin;

        QString mMaxText;
        QString mMinText;
};

#endif /*HBPROGRESSSLIDER_H_*/

