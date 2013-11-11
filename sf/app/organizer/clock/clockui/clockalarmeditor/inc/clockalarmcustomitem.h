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
* Definition for the ClockAlarmCustomItem class.
*
*/

#ifndef CLOCKALARMCUSTOMITEM_H
#define CLOCKALARMCUSTOMITEM_H

// System includes
#include <HbDataFormViewItem>

// Forward declarations
class HbPushButton;

class ClockAlarmCustomItem : public HbDataFormViewItem
{
	Q_OBJECT

public:
	ClockAlarmCustomItem(QGraphicsItem *parent = 0);
	virtual ~ClockAlarmCustomItem();

public:
	virtual HbAbstractViewItem* createItem();
	virtual bool canSetModelIndex(const QModelIndex &index) const;
	void restore();

protected:
	virtual HbWidget* createCustomWidget();
	
private:
	HbPushButton* mCustomButton;
};

#endif // CLOCKALARMCUSTOMITEM_H

// End of file	--Don't remove this.
