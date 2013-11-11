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

#ifndef HELPSTANDARDITEM_H
#define HELPSTANDARDITEM_H

#include <QStandardItem>

class HelpStandardItem : public QStandardItem
{
public:
	HelpStandardItem(const QString& text);
	~HelpStandardItem();
	bool operator<(const QStandardItem & other) const;
};

#endif //HELPSTANDARDITEM_H

