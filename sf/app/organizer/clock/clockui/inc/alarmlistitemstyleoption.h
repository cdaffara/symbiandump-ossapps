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
* Definition file for class AlarmListItemStyleOption.
*
*/

#ifndef ALARMLISTITEMSTYLEOPTION_H
#define ALARMLISTITEMSTYLEOPTION_H

// System includes
#include <HbStyleOption>

class AlarmListItemStyleOption : public HbStyleOption
{
public:
	enum AlarmListViewItemIndex
	{
		RepeatIconItem,
		AlarmTimeTextItem,
		AlarmTimeAmPmTextItem,
		RemainingTimeTextItem,
		DescriptionTextItem,
		RepeatTypeTextItem,
		AlarmIconButtonButtonItem
	};

	enum StyleOptionType
	{
		Type = HbSO_Widget + 1000
	};

	enum StyleOptionVersion
	{
		Version = 1
	};

public:
	AlarmListItemStyleOption();
	virtual ~AlarmListItemStyleOption();

public:
	QString alarmTimeText;
	QString alarmTimeAmPmText;
	QString remainingTimeText;
	QString descriptionText;
	QString repeatTypeText;
	QString repeatIcon;
	QString alarmIcon;
};

#endif // ALARMLISTITEMSTYLEOPTION_H

// End of file	--Don't remove this.
