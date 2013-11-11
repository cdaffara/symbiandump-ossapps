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
* Header file for class NotesSortFilterProxyModel.
*
*/

#ifndef NOTESSORTFILTERPROXYMODEL_H
#define NOTESSORTFILTERPROXYMODEL_H

// System includes
#include <QSortFilterProxyModel>

// User includes
#include "notesmodeldefines.h"
#include <agendautil.h>

class NOTESMODEL_EXPORT NotesSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	NotesSortFilterProxyModel(AgendaUtil &agendaUtil, QObject *parent = 0);
	virtual ~NotesSortFilterProxyModel();

protected:
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	bool filterAcceptsRow(int sourceRow,
	                      const QModelIndex &sourceParent) const;

private:
	AgendaUtil &mAgendaUtil;
};

#endif // NOTESSORTFILTERPROXYMODEL_H

// End of file	--Don't remove this.
