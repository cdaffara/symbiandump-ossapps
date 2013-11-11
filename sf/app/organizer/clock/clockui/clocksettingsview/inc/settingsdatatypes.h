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
* Defines some common data structures used by the settings view.
*
*/

#ifndef CLOCKSETTINGSDATATYPES_H
#define CLOCKSETTINGSDATATYPES_H

enum FormItemIndex {
	NetworkTimeItem = 0,
	TimeItem,
	DateItem,
	PlaceItem,
	RegionalSettingsItem,
	ClockTypeItem
};

enum RegionalFormItemIndex {
	TimeFormat = 0,
	TimeSeparator,
	DateFormat,
	DateSeparator,
	Workdays,
	StartOfWeek
};

enum CustomItemOffset {
	TimeItemOffset = 1,
	DateItemOffset,
	PlaceItemOffset,
	RegionalItemOffset,
	WorkdaysItemOffset
};

#endif // CLOCKSETTINGSDATATYPES_H

// End of file	--Don't remove this.
