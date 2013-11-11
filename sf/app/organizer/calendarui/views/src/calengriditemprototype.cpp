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
 * Description: Definition file for class CalenGridItemPrototype.
 *
 */

// System includes
#include <QPainter>
#include <QPen>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <hblabel.h>
#include <hbtextitem.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbiconitem.h>
#include <hbframebackground.h>
#include <hbcolorscheme.h>
#include <hbtheme.h>

// User includes
#include "calengriditemprototype.h"
#include "calencommon.h"
#include "calentodayindicatorlinedrawer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calengriditemprototypeTraces.h"
#endif


/*!
 \class CalenGridItemPrototype

 Prototype class for HbGridView.
 */

/*!
 Constructor.
 */
CalenGridItemPrototype::CalenGridItemPrototype(QGraphicsWidget *parent) :
    HbGridViewItem(parent),
    mActiveTextColor(HbColorScheme::color("qtc_cal_month_active_dates")),
    mInActiveTextColor(HbColorScheme::color("qtc_cal_month_notactive_dates")),
    mCurrentDateColor(HbColorScheme::color("qtc_cal_month_highlighted_text")),
    mEventIndicatorItem(0),
    mMonthDayInfoItem(0),
    mFocusIndicatorItem(0),
    mTodayIndicatorItem(0)
    {
    OstTraceFunctionEntry0( CALENGRIDITEMPROTOTYPE_CALENGRIDITEMPROTOTYPE_ENTRY );
    
	OstTraceFunctionExit0( CALENGRIDITEMPROTOTYPE_CALENGRIDITEMPROTOTYPE_EXIT );
	}

/*!
	Constructs all the primitives
*/
void CalenGridItemPrototype::createPrimitives()
{
    OstTraceFunctionEntry0( CALENGRIDITEMPROTOTYPE_CREATEPRIMITIVES_ENTRY );
    
	if (!mMonthDayInfoItem) {
		mMonthDayInfoItem = new HbTextItem(this);
		HbStyle::setItemName(mMonthDayInfoItem,
				 QLatin1String("monthDayInfoTextItem"));
		mMonthDayInfoItem->setElideMode(Qt::ElideNone);
	}
		
	if (!mFocusIndicatorItem) {
		mFocusIndicatorItem = new HbFrameItem(this);
		mFocusIndicatorItem->frameDrawer().setFrameType(
				HbFrameDrawer::NinePieces);
		mFocusIndicatorItem->setZValue(-1);
		HbStyle::setItemName(
				mFocusIndicatorItem, QLatin1String("focusIconItem"));
	}
	
	if (!mEventIndicatorItem) {
		mEventIndicatorItem = new HbIconItem(this);
		HbStyle::setItemName(
				mEventIndicatorItem, QLatin1String("eventIconItem"));
	}
	
	if (!mTodayIndicatorItem) {
		mTodayIndicatorItem = new CalenTodayIndicatorLineDrawer(this);
		HbStyle::setItemName(
				mTodayIndicatorItem, QLatin1String("todayIndicatorItem"));
	}

	OstTraceFunctionExit0( CALENGRIDITEMPROTOTYPE_CREATEPRIMITIVES_EXIT );
}

/*!
 From HbAbstractViewItem.

 \return CalenGridItemPrototype* Pointer to the constructed object.
 \sa HbAbstractViewItem, HbGridViewItem
 */
HbAbstractViewItem *CalenGridItemPrototype::createItem()
{
    OstTraceFunctionEntry0( CALENGRIDITEMPROTOTYPE_CREATEITEM_ENTRY );
    
	CalenGridItemPrototype* item = new CalenGridItemPrototype(*this);
	item->createPrimitives();
	
    connect(
            HbTheme::instance(), SIGNAL(changed()),
            item, SLOT(handleThemeChange()));
	// Set the default frame to NULL so that HbGridView does not add its 
	// default frame for items
	HbFrameBackground frame;
	frame.setFrameGraphicsName(QString(""));
	item->setDefaultFrame(frame);
	
	OstTraceFunctionExit0( CALENGRIDITEMPROTOTYPE_CREATEITEM_EXIT );
	return item;
}

/*!
 From HbAbstractViewItem.

 \sa HbAbstractViewItem, HbGridViewItem
 */
void CalenGridItemPrototype::updateChildItems()
{		
    OstTraceFunctionEntry0( CALENGRIDITEMPROTOTYPE_UPDATECHILDITEMS_ENTRY );
    
	// Here update content of each item.
	QVariant monthDayRole;
	bool monthFocusRole;
	bool underlineEnabled = false;
	bool monthEventRole;
	bool monthTextColorRole;
	QVariant itemData = modelIndex().data(Qt::UserRole + 1);
	if (itemData.isValid()) {
		if (itemData.canConvert<QVariantList>()) {
		
			// Get the item list
			QVariantList itemList = itemData.toList();
			// Get the day text
			monthDayRole = itemList.at(CalendarNamespace::CalendarMonthDayRole);
			if (monthDayRole.canConvert<QString>()) {
				mMonthDayInfoItem->setText(monthDayRole.toString());
			}
			
			// Get the focus data
			monthFocusRole = itemList.at(CalendarNamespace::CalendarMonthFocusRole).value<bool>();
			if (monthFocusRole) {
				mFocusIndicatorItem->frameDrawer().setFrameGraphicsName(focusIconName);
			} else {
				mFocusIndicatorItem->frameDrawer().setFrameGraphicsName(QString(""));
			}
			
			// Get the today indicator role
			underlineEnabled = itemList.at(
					CalendarNamespace::CalendarMonthUnderlineRole).
					value<bool>();
			if(underlineEnabled) {
				mTodayIndicatorItem->show();
			} else {
				mTodayIndicatorItem->hide();
			}
			
			// Get the event indicator data
			monthEventRole = itemList.at(CalendarNamespace::CalendarMonthEventRole).value<bool>();
			if (monthEventRole) {
				// Set the event indicator
				//QString iconName(focusIconName);
				mEventIndicatorItem->setIconName(eventIndname);
			} else {
				mEventIndicatorItem->setIconName(QString(""));
			}
			
			// Get the text color
			monthTextColorRole = itemList.at(CalendarNamespace::CalendarMonthTextColorRole).value<bool>();
			if (monthTextColorRole) {
                if (monthFocusRole) {
                    // Set the Highlighted text color
                    mMonthDayInfoItem->setTextColor(mCurrentDateColor);
                } else {
                    // Set the active text color
                    mMonthDayInfoItem->setTextColor(mActiveTextColor);
                }
			} else {
				// Set the inactive text color
				mMonthDayInfoItem->setTextColor(mInActiveTextColor);
			}
		}
	}
	
	// base class implementation
	HbGridViewItem::updateChildItems();
	OstTraceFunctionExit0( CALENGRIDITEMPROTOTYPE_UPDATECHILDITEMS_EXIT );
}

/*!
 Function overwritten to avoid default behavior
 */
void CalenGridItemPrototype::pressStateChanged(bool pressed,bool animate)
{
    OstTraceFunctionEntry0( CALENGRIDITEMPROTOTYPE_PRESSSTATECHANGED_ENTRY );
    
	Q_UNUSED(pressed)
	Q_UNUSED(animate)		
	// Just overrode it as we dont want to have any default behavior
	OstTraceFunctionExit0( CALENGRIDITEMPROTOTYPE_PRESSSTATECHANGED_EXIT );
}

/*!
 Function overwritten to avoid default behavior
 */
void CalenGridItemPrototype::pressStateChanged(bool animate)
{
    OstTraceFunctionEntry0( DUP1_CALENGRIDITEMPROTOTYPE_PRESSSTATECHANGED_ENTRY );
    
	Q_UNUSED(animate)
	// Just overrode it as we dont want to have any default behavior
    OstTraceFunctionExit0( DUP1_CALENGRIDITEMPROTOTYPE_PRESSSTATECHANGED_EXIT );
}

bool CalenGridItemPrototype::canSetModelIndex(const QModelIndex& index)
{
    OstTraceFunctionEntry0( CALENGRIDITEMPROTOTYPE_CANSETMODELINDEX_ENTRY );
    
	Q_UNUSED(index)
    
    OstTraceFunctionExit0( CALENGRIDITEMPROTOTYPE_CANSETMODELINDEX_EXIT );
	return true;
}

/*!
 Slot to handle the change in theme
 */
void CalenGridItemPrototype::handleThemeChange()
{
    OstTraceFunctionEntry0( CALENGRIDITEMPROTOTYPE_HANDLETHEMECHANGE_ENTRY );
    
    mActiveTextColor = HbColorScheme::color("qtc_cal_month_active_dates");
    mInActiveTextColor = HbColorScheme::color("qtc_cal_month_notactive_dates");
    mCurrentDateColor = HbColorScheme::color("qtc_cal_month_highlighted_text");
    
    OstTraceFunctionExit0( CALENGRIDITEMPROTOTYPE_HANDLETHEMECHANGE_EXIT );
    
}
// End of file  --Don't remove this.
