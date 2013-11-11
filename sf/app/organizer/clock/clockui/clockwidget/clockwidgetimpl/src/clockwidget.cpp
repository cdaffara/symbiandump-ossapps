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
* Description:  Clockwidget
*
*/

// System includes
#include <QGraphicsLinearLayout>

// User includes
#include "clockwidget.h"
#include "analogclockwidget.h"
#include "digitalclockwidget.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockwidgetTraces.h"
#endif


/*!
    \class ClockWidget

    This is the generic wrapper for the clockwidget which manages analog and ditial clockwidgets.
 */

/*!
    Constructor.
 */
ClockWidget::ClockWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mClockType(ClockTypeDigital)
{    
    OstTraceFunctionEntry0( CLOCKWIDGET_CLOCKWIDGET_ENTRY );
    loadClockWidget();
OstTraceFunctionExit0( CLOCKWIDGET_CLOCKWIDGET_EXIT );
}

/*!
    Destructor.
*/
ClockWidget::~ClockWidget()
{    
    OstTraceFunctionEntry0( DUP1_CLOCKWIDGET_CLOCKWIDGET_ENTRY );
    
OstTraceFunctionExit0( DUP1_CLOCKWIDGET_CLOCKWIDGET_EXIT );
}

/*!
    Returns the clock type.
*/
ClockWidget::ClockType ClockWidget::clockType() const
{
    OstTraceFunctionEntry0( CLOCKWIDGET_CLOCKTYPE_ENTRY );
    OstTraceFunctionExit0( CLOCKWIDGET_CLOCKTYPE_EXIT );
    return mClockType;
}
  
/*!
    Sets the clock type;
*/
void ClockWidget::setClockType(const ClockType &type)
{
    OstTraceFunctionEntry0( CLOCKWIDGET_SETCLOCKTYPE_ENTRY );
    if (type == ClockTypeAnalog) {
        if(type != mClockType){
	        mClockType = ClockTypeAnalog;
	        updateClockWidget();
        }
    } else {
        if(type != mClockType){
            mClockType = ClockTypeDigital;
            updateClockWidget();
        }      
    }
OstTraceFunctionExit0( CLOCKWIDGET_SETCLOCKTYPE_EXIT );
}

/*!
    Updates the clock time with every second
*/
void ClockWidget::updateTime()
{
    OstTraceFunctionEntry0( CLOCKWIDGET_UPDATETIME_ENTRY );
    if (mClockType == ClockTypeAnalog) {  
        mAnalogClock->tick();    
    } else {
    	mDigitalClock->updatePrimitives();
    }
OstTraceFunctionExit0( CLOCKWIDGET_UPDATETIME_EXIT );
}

/*!
    Constructs the clockwidget based upon its type.
*/
void ClockWidget::loadClockWidget()
{
    OstTraceFunctionEntry0( CLOCKWIDGET_LOADCLOCKWIDGET_ENTRY );
    mLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mLayout->setContentsMargins(0,0,0,0); 

    if (mClockType == ClockTypeAnalog) {
	    mAnalogClock = new AnalogClockWidget(this);        
	    mLayout->addItem(mAnalogClock);
    } else {
	    bool useAmPm = false;
	    mDigitalClock = new DigitalClockWidget(useAmPm, this);
	    mLayout->addItem(mDigitalClock);
    }
    setLayout(mLayout);  
OstTraceFunctionExit0( CLOCKWIDGET_LOADCLOCKWIDGET_EXIT );
}

/*!
    Constructs the clockwidget based upon its type.
*/
void ClockWidget::updateClockWidget()
{
    OstTraceFunctionEntry0( CLOCKWIDGET_UPDATECLOCKWIDGET_ENTRY );
    if (mClockType == ClockTypeAnalog) {        
        mLayout->removeItem(mDigitalClock);
        delete mDigitalClock;
        if (!mAnalogClock) {
        	mAnalogClock = new AnalogClockWidget(this);
        }
        mLayout->addItem(mAnalogClock);
    } else {
        mLayout->removeItem(mAnalogClock);
        delete mAnalogClock;
        if(!mDigitalClock){
        bool useAmPm = false; // find out this fronm the settings utility
        mDigitalClock = new DigitalClockWidget(useAmPm, this);
    }
    mLayout->addItem(mDigitalClock);
    }           
OstTraceFunctionExit0( CLOCKWIDGET_UPDATECLOCKWIDGET_EXIT );
}

ClockWidget::TimeFormat ClockWidget::timeFormat() const
{
    OstTraceFunctionEntry0( CLOCKWIDGET_TIMEFORMAT_ENTRY );
    OstTraceFunctionExit0( CLOCKWIDGET_TIMEFORMAT_EXIT );
    return mTimeFormat;
}

void ClockWidget::setTimeFormat(const TimeFormat &timeFormat)
{
    OstTraceFunctionEntry0( CLOCKWIDGET_SETTIMEFORMAT_ENTRY );
    if(mDigitalClock){
	    mTimeFormat = timeFormat;
		if (timeFormat == ClockWidget::TimeFormat12Hrs) {
			mDigitalClock->setAmPm(true);
		} else {
			mDigitalClock->setAmPm(false);
		}
    }
OstTraceFunctionExit0( CLOCKWIDGET_SETTIMEFORMAT_EXIT );
}
// End of file  --Don't remove this.
