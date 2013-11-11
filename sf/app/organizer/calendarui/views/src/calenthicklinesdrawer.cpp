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
* Description:  CalenThickLinesDrawer implementation.
*
*/

// System includes
#include <QtGui>
#include <QPainter>
#include <QPen>
#include <hbdeviceprofile.h>
#include <hbcolorscheme.h>
#include <hbtheme.h>

// User includes
#include "calenthicklinesdrawer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenthicklinesdrawerTraces.h"
#endif


// CONSTANTS
#define CALENTHICKLINEWIDTH     0.15 // Units
#define CALENDAYNAMEANDGRIDSEPERATOR    0.75 // Units

/*!
 \class CalenThickLinesDrawer

 Class to draw day names widget a,d week number widget seperators
 */

/*!
 Constructor
 */
CalenThickLinesDrawer::CalenThickLinesDrawer( CalendarNamespace::WidgetType type, 
	QGraphicsItem* parent):
	HbWidget(parent)
{
    OstTraceFunctionEntry0( CALENTHICKLINESDRAWER_CALENTHICKLINESDRAWER_ENTRY );
    
	typeOfWidget = type;
	mGridBorderColor = HbColorScheme::color("qtc_cal_grid_line");
	setFlag(QGraphicsItem::ItemHasNoContents, false);
	
	connect(
				HbTheme::instance(), SIGNAL(changed()),
				this, SLOT(handleThemeChange()));
	
	OstTraceFunctionExit0( CALENTHICKLINESDRAWER_CALENTHICKLINESDRAWER_EXIT );
}

/*!
 To paint grid item border
 */
CalenThickLinesDrawer::~CalenThickLinesDrawer()
{
    OstTraceFunctionEntry0( DUP1_CALENTHICKLINESDRAWER_CALENTHICKLINESDRAWER_ENTRY );

    OstTraceFunctionExit0( DUP1_CALENTHICKLINESDRAWER_CALENTHICKLINESDRAWER_EXIT );
}

/*!
 To paint grid item border
 */
void CalenThickLinesDrawer::paint(QPainter* painter,
								const QStyleOptionGraphicsItem* option,
								QWidget* widget)
{
    OstTraceFunctionEntry0( CALENTHICKLINESDRAWER_PAINT_ENTRY );
    
	Q_UNUSED(option);
	Q_UNUSED(widget);
	QPen pen;
	pen.setStyle(Qt::SolidLine);

	// Get the proper width, layout specificaiton says, width has to be 0.15un
	// Convert 0.15un to pixels
	HbDeviceProfile deviceProf;
	qreal unitValue = deviceProf.unitValue();
	qreal widthInPixels = CALENTHICKLINEWIDTH * unitValue;
	pen.setWidth(widthInPixels);
	pen.setBrush(mGridBorderColor);
	
	// Store the old pen
    QPen oldPen = painter->pen();
	    
	painter->setPen(pen);
	QRectF controlRect = this->boundingRect();

	// Check the type of the widget and paint the necessary things
	if ( typeOfWidget == CalendarNamespace::CalenDayNameWidget ) {
		// Calculate the seperation to be added so that line coincides with 
		// top border of the grid
		qreal seperation = CALENDAYNAMEANDGRIDSEPERATOR * unitValue;
		// Adjust the seperation with the thickness of the line
		seperation = seperation - widthInPixels;
		// Get the start point and end point to draw the line
		QPointF startPoint(controlRect.bottomLeft().x(), controlRect.bottomLeft().y() + seperation);
		QPointF endPoint(controlRect.bottomRight().x(), controlRect.bottomRight().y() + seperation);
		painter->drawLine(startPoint, endPoint);
	} else if ( typeOfWidget == CalendarNamespace::CalenWeekNumWidget ) {
		// Check if this widget is visible
		if(this->isEnabled()) {
			// Get the start point and end point to draw the line
			QPointF startPoint = controlRect.topRight();
			QPointF endPoint = controlRect.bottomRight();
			painter->drawLine(startPoint, endPoint);
		}
	}
	
	// Set the old pen back
    painter->setPen(oldPen);
    
    OstTraceFunctionExit0( CALENTHICKLINESDRAWER_PAINT_EXIT );
}

/*!
 Overriding the base class function to avoid searching for widgetml/css files
 */
void CalenThickLinesDrawer::polish(HbStyleParameters &params)
{
    Q_UNUSED(params);
}

/*!
 Slot to handle the change in theme
 */
void CalenThickLinesDrawer::handleThemeChange()
{
	OstTraceFunctionEntry0(CALENTHICKLINESDRAWER_HANDLETHEMECHANGE_ENTRY);
	
	mGridBorderColor = HbColorScheme::color("qtc_cal_grid_line");
	
	OstTraceFunctionExit0(CALENTHICKLINESDRAWER_HANDLETHEMECHANGE_EXIT);
}

// End of file  --Don't remove this.
