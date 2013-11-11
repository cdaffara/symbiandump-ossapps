/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Header file for AgendaEventViewer Itemclass
*
*/

#ifndef AGENDAEVENTVIEWERITEM_H
#define AGENDAEVENTVIEWERITEM_H

// System includes
#include <HbWidget>

// Forward declaration
class HbTextItem;
class HbIconItem;
class QGraphicsItem;

class AgendaEventViewerItem : public HbWidget
{
	Q_OBJECT

public:
	AgendaEventViewerItem(QGraphicsItem *parent = 0);
	virtual ~AgendaEventViewerItem();

public:
	void setEventViewerItemData(const QStringList & itemData, int role);
	void setTextItemData(int index, const QString &itemData);
	void setIconItemData(int index, const QString &itemData);

private:

	HbTextItem *mPrimaryText;
	HbTextItem *mSecondaryText;
	HbIconItem *mPrimaryLeftIcon;
	HbIconItem *mPrimaryRightIcon;
	HbIconItem *mSecondaryLeftIcon;
	HbIconItem *mSecondaryRightIcon;

};

#endif // AGENDAEVENTVIEWERITEM_H

// End of file	--Don't remove this.
