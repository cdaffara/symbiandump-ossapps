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
 * Description: Header file for ClockHomeCityItem Itemclass
 *
 */

#ifndef CLOCKHOMECITYITEM_H
#define CLOCKHOMECITYITEM_H

// System includes
#include <QPointer>
#include <HbWidget>

// User include.
#include "clockviewsdefines.h"

// Forward declaration
class QGraphicsItem;
class HbTextItem;
class HbIconItem;

class ClockHomeCityItem : public HbWidget
{
	Q_OBJECT

public:
	enum {
		DayNightIndicator,
		Date,
		City,
		Dst,
		Time
	};

public:
	CLOCKVIEWS_EXPORT ClockHomeCityItem(QGraphicsItem *parent = 0);
	CLOCKVIEWS_EXPORT ~ClockHomeCityItem();

public:
	void setHomeCityItemData(const QVariantMap & itemData);

private:
	QPointer<HbTextItem> mDateText;
	QPointer<HbTextItem> mCityText;
	QPointer<HbTextItem> mTimeText;
	QPointer<HbIconItem> mDayNightIcon;
	QPointer<HbIconItem> mDstIcon;

};

#endif // CLOCKHOMECITYITEM_H

// End of file	--Don't remove this.
