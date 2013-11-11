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
* Description:
* Header file for class ClockAlarmListItemPrototype.
*
*/

#ifndef CLOCKALARMLISTITEMPROTOTYPE_H
#define CLOCKALARMLISTITEMPROTOTYPE_H

// System includes
#include <QGraphicsWidget>
#include <QPointer>
#include <hblistviewitem.h>

// Forward declarations
class QGraphicsItem;
class HbPushButton;

class ClockAlarmListItemPrototype : public HbListViewItem
{
	Q_OBJECT

public:
	explicit ClockAlarmListItemPrototype(QGraphicsItem *parent = 0);
	virtual ~ClockAlarmListItemPrototype();

public:
	virtual void updateChildItems();
	virtual ClockAlarmListItemPrototype* createItem();

signals:
	void alarmStatusHasChanged(int row);

private slots:
	void handleAlarmStatusChanged();

private:
	QGraphicsItem *mParent;

	QPointer<HbPushButton> malarmIconItem;
};

#endif // CLOCKALARMLISTITEMPROTOTYPE_H

// End of file	--Don't remove this.
