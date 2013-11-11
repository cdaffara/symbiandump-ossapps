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
*
*/

#include "HelpUtils.h"
#include "HelpCommon.h"
#include "HelpStandardItem.h"

HelpStandardItem::HelpStandardItem(const QString& text) : QStandardItem(text)
{
}

HelpStandardItem::~HelpStandardItem()
{
}

bool HelpStandardItem::operator<(const QStandardItem & other) const
{
	const int role = model() ? model()->sortRole() : Qt::DisplayRole;
	switch(role)
	{
	case Qt::DisplayRole:
		if(HelpUtils::compareStr(text(), other.text()) < 0)
		{
			return true;
		}
		break;
	case UidRole:
	case HrefRole:
	case KeywordRole:
	default:
		return QStandardItem::operator<(other);
	}
	return false;
}
