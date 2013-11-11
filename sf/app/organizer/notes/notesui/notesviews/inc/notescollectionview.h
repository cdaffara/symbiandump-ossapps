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
* Header file for NotesCollectionView class.
*
*/

#ifndef NOTESCOLLECTIONVIEW_H
#define NOTESCOLLECTIONVIEW_H

// System includes
#include <HbView>

// User includes
#include "notesviewsdefines.h"
#include "notesappcontrollerif.h"

// Forward declarations
class QGraphicsWidget;
class HbListView;
class HbLabel;
class AgendaUtil;
class NotesModel;
class NotesDocLoader;
class NotesEditor;
class NotesSortFilterProxyModel;

class NotesCollectionView : public HbView
{
	Q_OBJECT

public:
	NOTESVIEWS_EXPORT NotesCollectionView(QGraphicsWidget *parent = 0);
	NOTESVIEWS_EXPORT virtual ~NotesCollectionView();

public:
	NOTESVIEWS_EXPORT void setupView(
			NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader);
	NOTESVIEWS_EXPORT void populateListView();

private slots:
	void displayAllNotesView();
	void resetCollectionView();
	void createNewNote();
	void handleEditingCompleted(bool status);
	void updateData(ulong id);
	void updateData(QList<ulong> ids);
	void handleActivated(const QModelIndex &index);
	void updateFavouritesCount(const QModelIndex &index, int start, int end);
	void handleActionStateChanged();

private:

	int todosCount();
	int recentNotesCount();

private:
	HbListView *mListView;
	HbAction *mAllNotesAction;
	HbAction *mViewCollectionAction;
	HbAction *mAddNoteAction;

	AgendaUtil *mAgendaUtil;

	NotesAppControllerIf *mAppControllerIf;
	NotesDocLoader *mDocLoader;
	NotesModel *mNotesModel;
	NotesEditor *mNotesEditor;
	NotesSortFilterProxyModel *mFavouriteModel;
};

#endif // NOTESCOLLECTIONVIEW_H

// End of file	--Don't remove this.
