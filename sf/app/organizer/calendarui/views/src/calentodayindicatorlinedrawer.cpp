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
* Description:  CalenTodayIndicatorLineDrawer implementation.
*
*/

// System includes
#include <QtGui>
#include <QPainter>
#include <QPen>
#include <hbcolorscheme.h>
#include <hbtheme.h>

// User includes
#include "CalenTodayIndicatorLineDrawer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calentodayindicatorlinedrawerTraces.h"
#endif


/*!
 \class CalenTodayIndicatorLineDrawer.

 Class to draw the today indicator line.
 */

/*!
 Constructor.
 */
CalenTodayIndicatorLineDrawer::CalenTodayIndicatorLineDrawer(
		QGraphicsItem* parent): HbWidgetBase(parent)
{
    OstTraceFunctionEntry0( CALENTODAYINDICATORLINEDRAWER_CALENTODAYINDICATORLINEDRAWER_ENTRY );
    
	connect(
			HbTheme::instance(), SIGNAL(changed()),
			this, SLOT(handleThemeChange()));
	setFlag(QGraphicsItem::ItemHasNoContents, false);
	
	OstTraceFunctionExit0( CALENTODAYINDICATORLINEDRAWER_CALENTODAYINDICATORLINEDRAWER_EXIT );
}

/*!
 Destructor.
 */
CalenTodayIndicatorLineDrawer::~CalenTodayIndicatorLineDrawer()
{
    OstTraceFunctionEntry0( DUP1_CALENTODAYINDICATORLINEDRAWER_CALENTODAYINDICATORLINEDRAWER_ENTRY );

    OstTraceFunctionExit0( DUP1_CALENTODAYINDICATORLINEDRAWER_CALENTODAYINDICATORLINEDRAWER_EXIT );
}

/*!
 To paint the today indicator line.
 */
void CalenTodayIndicatorLineDrawer::paint(
		QPainter* painter, const QStyleOptionGraphicsItem* option,
		QWidget* widget)
{
    OstTraceFunctionEntry0( CALENTODAYINDICATORLINEDRAWER_PAINT_ENTRY );
    
	Q_UNUSED(option);
	Q_UNUSED(widget);
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setBrush(HbColorScheme::color("qtc_cal_month_current_day"));
	
	// Store the old pen
    QPen oldPen = painter->pen();
	    
	painter->setPen(pen);
	QRectF controlRect = this->boundingRect();
	painter->fillRect(
			controlRect, HbColorScheme::color("qtc_cal_month_current_day"));
	
	// Set the old pen back
    painter->setPen(oldPen);
    
    OstTraceFunctionExit0( CALENTODAYINDICATORLINEDRAWER_PAINT_EXIT );
}

/*!
 To handle the theme change for the today indicator line.
 */
void CalenTodayIndicatorLineDrawer::handleThemeChange()
{
    OstTraceFunctionEntry0( CALENTODAYINDICATORLINEDRAWER_HANDLETHEMECHANGE_ENTRY );
    
	update();
	
	OstTraceFunctionExit0( CALENTODAYINDICATORLINEDRAWER_HANDLETHEMECHANGE_EXIT );
}

// End of file  --Don't remove this.
