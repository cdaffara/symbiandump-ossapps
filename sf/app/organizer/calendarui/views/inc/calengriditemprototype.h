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
 * Description: Header file for class CalenGridItemPrototype.
 *
 */

#ifndef CALENGRIDITEMPROTOTYPE_H
#define CALENGRIDITEMPROTOTYPE_H

// System includes
#include <hblistviewitem.h>
#include <hbgridviewitem.h>

// Forward declarations
class QGraphicsWidget;
class QGraphicsLinearLayout;
class HbIconItem;
class HbTextItem;
class HbFrameItem;
class CalenTodayIndicatorLineDrawer;

// CONSTANTS
const QString focusIconName = "qtg_fr_cal_focused_day_ind";
const QString eventIndname = ":/qtg_graf_cal_event_ind.svg";

class CalenGridItemPrototype : public HbGridViewItem
{
	Q_OBJECT

public:
	CalenGridItemPrototype(QGraphicsWidget *parent = 0);
	virtual ~CalenGridItemPrototype()
	{
	}

public:
	HbAbstractViewItem *createItem();
	void updateChildItems();
	void pressStateChanged(bool pressed,bool animate);
	void pressStateChanged(bool  animate);
	bool canSetModelIndex(const QModelIndex& index);
	void createPrimitives();

public slots:

	void handleThemeChange();

private:
	QColor mActiveTextColor;
	QColor mInActiveTextColor;
	QColor mCurrentDateColor;
	HbIconItem *mEventIndicatorItem;
	HbTextItem *mMonthDayInfoItem;
	HbFrameItem *mFocusIndicatorItem;
	CalenTodayIndicatorLineDrawer *mTodayIndicatorItem;
};

#endif // CALENGRIDITEMPROTOTYPE_H

// End of file  --Don't remove this.
