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
* Header file for class NotesModel.
*
*/

#ifndef NOTESMODEL_H
#define NOTESMODEL_H

// System includes
#include <QObject>
#include <QModelIndex>

// User includes
#include "notesmodeldefines.h"
#include "notestest_global.h"

// Forward declarations
class QAbstractItemModel;
class QStandardItemModel;
class AgendaUtil;
class AgendaEntry;

NOTES_TEST_CLASS(TestNotesModel)

class NOTESMODEL_EXPORT NotesModel : public QObject
{
	Q_OBJECT

public:
	NotesModel(AgendaUtil *mAgendaUtil, QObject *parent = 0);
	virtual ~NotesModel();

public:
	QAbstractItemModel *sourceModel();

signals:
	void rowAdded(QModelIndex index);

private slots:
	void populateSourceModel();
	void updateSourceModel(ulong id);
	void populateSourceModel(QList<ulong> ids);
	void addEntryToModel(ulong id);
	void removeEntryFromModel(ulong id);
	void handleInstanceViewCreationCompleted(int status);

private:
	void modifyEntryInModel(ulong id, int row);
	void appendNotesToModel(QList<AgendaEntry> &agendaEntryList);
	void appendInCompTodosToModel(QList<AgendaEntry> &agendaEntryList);
	void appendCompTodosToModel(QList<AgendaEntry> &agendaEntryList);
	bool insertNoteToModel(QModelIndex &index, ulong id);
	bool insertInCompTodoToModel(QModelIndex &index, ulong id);
	bool insertCompTodoToModel(QModelIndex &index, ulong id);
	QString dateFormatString();
	QString timeFormatString();

private:
	QStandardItemModel *mSourceModel;
	AgendaUtil *mAgendaUtil;

	int mNotesCount;
	int mInCompTodoCount;
	int mCompTodoCount;

private:
	NOTES_TEST_FRIEND_CLASS(TestNotesModel)
};

#endif // NOTESMODEL_H

// End of file	--Don't remove this
