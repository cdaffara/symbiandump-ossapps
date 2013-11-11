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

#ifndef ANALOGCLOCKWIDGET_H
#define ANALOGCLOCKWIDGET_H

// User includes 
#include <hbwidget.h>
#include <QPointer>

// Forward declarations
class HbIconItem;
class QTimer;

class AnalogClockWidget : public HbWidget
{
	Q_OBJECT

public:
	AnalogClockWidget(QGraphicsItem *parent = 0);
	virtual ~AnalogClockWidget();

public slots:
	void tick();

public:
	void setGeometry(const QRectF &rect);

private:
	void constructPrimitives();
	void updatePrimitives();

private:
	QPointer<QTimer> mTimer;
	QPointer<HbIconItem> mClockBackground;
	QPointer<HbIconItem> mClockHourHand;
	QPointer<HbIconItem> mClockMinuteHand;
	QPointer<HbIconItem> mClockSecondHand;
};

#endif

// End of file  --Don't remove this.
