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
* Definition file for class ClockAlarmListItemPrototype.
*
*/

// System includes
#include <HbPushButton>

// User includes
#include "clockalarmlistitemprototype.h"
#include "clockdatatypes.h"
#include "clockmainview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockalarmlistitemprototypeTraces.h"
#endif


/*!
	\class ClockAlarmListItemPrototype

	This is the prototype class for the items in the list view. This class is
	responsible for creating any cutom item types.
 */

/*!
	Default constructor.

	\parent of type QGraphicsItem.
 */
ClockAlarmListItemPrototype::ClockAlarmListItemPrototype(QGraphicsItem *parent)
:HbListViewItem(parent)
{
	OstTraceFunctionEntry0( CLOCKALARMLISTITEMPROTOTYPE_CLOCKALARMLISTITEMPROTOTYPE_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( CLOCKALARMLISTITEMPROTOTYPE_CLOCKALARMLISTITEMPROTOTYPE_EXIT );
}

/*!
	Destructor.
 */
ClockAlarmListItemPrototype::~ClockAlarmListItemPrototype()
{
	OstTraceFunctionEntry0( DUP1_CLOCKALARMLISTITEMPROTOTYPE_CLOCKALARMLISTITEMPROTOTYPE_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( DUP1_CLOCKALARMLISTITEMPROTOTYPE_CLOCKALARMLISTITEMPROTOTYPE_EXIT );
}

/*!
	Called for every view item in the list.

	\sa HbAbstractViewItem, HbListViewItem.
 */
void ClockAlarmListItemPrototype::updateChildItems()
{
	OstTraceFunctionEntry0( CLOCKALARMLISTITEMPROTOTYPE_UPDATECHILDITEMS_ENTRY );
	// First allow the base class to update.
	HbListViewItem::updateChildItems();

	QVariant alarmIconRole = modelIndex().data(AlarmIcon).toString();
	if (alarmIconRole.isValid()) {
		if (alarmIconRole.canConvert<QString> ()) {
			QString alarmIconPath = alarmIconRole.toString();
			if (!malarmIconItem) {
				malarmIconItem = new HbPushButton(this);
				HbStyle::setItemName(
						malarmIconItem, QLatin1String("alarmIconItem"));
				connect(
						malarmIconItem, SIGNAL(clicked()),
						this, SLOT(handleAlarmStatusChanged()));
			}
			malarmIconItem->setIcon(alarmIconPath);
		}
	}
	OstTraceFunctionExit0( CLOCKALARMLISTITEMPROTOTYPE_UPDATECHILDITEMS_EXIT );
}

/*!
	Called to construct a HbListViewItem.

	\return AlarmListItemPrototype* Pointer to the constructed view item.
 */
ClockAlarmListItemPrototype *ClockAlarmListItemPrototype::createItem()
{
	OstTraceFunctionEntry0( CLOCKALARMLISTITEMPROTOTYPE_CREATEITEM_ENTRY );
	return new ClockAlarmListItemPrototype(*this);
}

/*!
	SLOT which handles when the alarm push button is pressed.
 */
void ClockAlarmListItemPrototype::handleAlarmStatusChanged()
{
	OstTraceFunctionEntry0( CLOCKALARMLISTITEMPROTOTYPE_HANDLEALARMSTATUSCHANGED_ENTRY );
	int row = modelIndex().row();
	emit static_cast<ClockAlarmListItemPrototype*>(
			prototype())->alarmStatusHasChanged(row);
	OstTraceFunctionExit0( CLOCKALARMLISTITEMPROTOTYPE_HANDLEALARMSTATUSCHANGED_EXIT );
}

// End of file	--Don't remove this.
