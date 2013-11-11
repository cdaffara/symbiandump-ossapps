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

#ifndef HELPPROXYMODEL_H
#define HELPPROXYMODEL_H

#include <QSortFilterProxyModel>

class HelpProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	HelpProxyModel(QObject * parent = 0);
	~HelpProxyModel();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif //HELPPROXYMODEL_H
