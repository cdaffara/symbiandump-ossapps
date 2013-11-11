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
* Description:  DigitalClockWidget
*
*/

// System includes
#include <HbStyleLoader>
#include <HbLabel>
#include <HbTextItem>
#include <hbextendedlocale>
#include <QGraphicsSceneMouseEvent>
#include <QSizePolicy>
#include <QTimer>
#include <QTime>

// User includes
#include "digitalclockwidget.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "digitalclockwidgetTraces.h"
#endif


// Constants
const int clockUpdateInterval  (60000); // msec

/*!
    \class DigitalClockWidget

    This class implements the digitalclock widget which gets displayed
    in the clockmainview when the clocktype is set to digital type.
*/

/*!
    Constructor.
    \param useAmPm bool for setting 12 hour format
    \param parent The parent of type QGraphicsItem.
*/
DigitalClockWidget::DigitalClockWidget(bool useAmPm, QGraphicsItem *parent)
  : HbWidget(parent),
    mUseAmPm(useAmPm)
{
    OstTraceFunctionEntry0( DIGITALCLOCKWIDGET_DIGITALCLOCKWIDGET_ENTRY );
    bool result = HbStyleLoader::registerFilePath(":/resource/digitalclockwidget.widgetml");
    result = HbStyleLoader::registerFilePath(":/resource/digitalclockwidget.css");
    result = HbStyleLoader::registerFilePath(":/resource/digitalclockwidget_color.css");

    createPrimitives();
    
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(tick()));
    
    QTime time = QTime::currentTime();
    int initialIntervalTime = (clockUpdateInterval - ( time.msec() + (time.second() * 1000)));
    mTimer->start(initialIntervalTime);
    
OstTraceFunctionExit0( DIGITALCLOCKWIDGET_DIGITALCLOCKWIDGET_EXIT );
}

/*!
    Destructor.
*/
DigitalClockWidget::~DigitalClockWidget()
{   
    OstTraceFunctionEntry0( DUP1_DIGITALCLOCKWIDGET_DIGITALCLOCKWIDGET_ENTRY );
    mTimer->stop();
    HbStyleLoader::unregisterFilePath(":/resource/digitalclockwidget.widgetml");
    HbStyleLoader::unregisterFilePath(":/resource/digitalclockwidget.css");
    HbStyleLoader::unregisterFilePath(":/resource/digitalclockwidget_color.css");

OstTraceFunctionExit0( DUP1_DIGITALCLOCKWIDGET_DIGITALCLOCKWIDGET_EXIT );
}

/*!
    Updates clock visualization according to current time
 */
void DigitalClockWidget::tick()
{
    OstTraceFunctionEntry0( DIGITALCLOCKWIDGET_TICK_ENTRY );
    mTimer->setInterval(clockUpdateInterval);
    updatePrimitives();
    update();
OstTraceFunctionExit0( DIGITALCLOCKWIDGET_TICK_EXIT );
}

/*!
    Toggles time format
 */
void DigitalClockWidget::setAmPm(bool useAmPm)
{
    OstTraceFunctionEntry0( DIGITALCLOCKWIDGET_SETAMPM_ENTRY );
    
    bool old = mUseAmPm;
    mUseAmPm = useAmPm;
    
    if (mUseAmPm != old) { // change am/pm label
	    QTime time = QTime::currentTime();
	    QString timeString;
	    if (mUseAmPm) {
	    	mAmPmLabel->setText(time.toString("ap")); // set am/pm label
	    } else {
	    	mAmPmLabel->setText(""); // set am/pm label as null
	    }
    }
    
OstTraceFunctionExit0( DIGITALCLOCKWIDGET_SETAMPM_EXIT );
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void DigitalClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    OstTraceFunctionEntry0( DIGITALCLOCKWIDGET_RESIZEEVENT_ENTRY );
    HbWidget::resizeEvent(event);
OstTraceFunctionExit0( DIGITALCLOCKWIDGET_RESIZEEVENT_EXIT );
}

/*!
    Creates all widget primitives.
 */
void DigitalClockWidget::createPrimitives()
{
    OstTraceFunctionEntry0( DIGITALCLOCKWIDGET_CREATEPRIMITIVES_ENTRY );
    mClockLabel = new HbTextItem(this);    
    mClockLabel->setTextWrapping( Hb::TextNoWrap );
    HbStyle::setItemName(mClockLabel, QLatin1String("clockLabel"));
    
    mAmPmLabel = new HbTextItem(this);
    mClockLabel->setTextWrapping( Hb::TextNoWrap );
    HbStyle::setItemName(mAmPmLabel, QLatin1String("amPmLabel"));
OstTraceFunctionExit0( DIGITALCLOCKWIDGET_CREATEPRIMITIVES_EXIT );
}

/*!
    @copydoc HbWidget::updatePrimitives()
    updates all widget primitives
 */
void DigitalClockWidget::updatePrimitives()
{
OstTraceFunctionEntry0( DIGITALCLOCKWIDGET_UPDATEPRIMITIVES_ENTRY );

    QTime time = QTime::currentTime();
    QString timeString;
    
    if (mUseAmPm) {
	    timeString = time.toString("hh:ap"); // covert time in 12 hours format
	    timeString = timeString.section(":", 0, 0); // it would remove :ap section from timeString
	    mAmPmLabel->setText(time.toString("ap")); // set am/pm label
	} else {
	    mAmPmLabel->setText(""); // set am/pm label as null
	    timeString = time.toString("hh"); // append hour part
    }

    int index(1);
    HbExtendedLocale locale;
    QChar timeSeparator(locale.timeSeparator(index)) ;
    timeString.append(timeSeparator); // append time separator
    timeString.append(time.toString("mm")); //append minute part

    mClockLabel->setText(timeString);

OstTraceFunctionExit0( DIGITALCLOCKWIDGET_UPDATEPRIMITIVES_EXIT );
}
