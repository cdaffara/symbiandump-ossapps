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
* Description:  Clock widget
*
*/

#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

// System includes
#include <hbwidget.h>
#include <QPointer>
// User includes
#include "clockwidgetdefines.h"

//Forward declarations

class QGraphicsLinearLayout;
class AnalogClockWidget;
class DigitalClockWidget;

class CLOCKWIDGET_EXPORT  ClockWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(ClockType clockType READ clockType WRITE setClockType)
	Q_PROPERTY(TimeFormat timeFormat READ timeFormat WRITE setTimeFormat)

public:
    enum ClockType
    {
        ClockTypeAnalog,
        ClockTypeDigital
    };
	enum TimeFormat
	{
		TimeFormat12Hrs,
		TimeFormat24Hrs
	};

    ClockWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~ClockWidget();
    ClockType clockType() const;
    void setClockType(const ClockType &type);
    void loadClockWidget();
    TimeFormat timeFormat() const;
	void setTimeFormat(const TimeFormat &timeFormat);

public slots:
    void updateTime();
    
private:
    void updateClockWidget();
private:
    ClockType                       mClockType;
    TimeFormat                      mTimeFormat;
    QPointer<AnalogClockWidget>     mAnalogClock;
    QPointer<DigitalClockWidget>    mDigitalClock;
    QGraphicsLinearLayout           *mLayout;
};

#endif // CLOCKWIDGET_H

// End of file  --Don't remove this.
