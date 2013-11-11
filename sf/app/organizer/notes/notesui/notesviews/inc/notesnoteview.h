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
* Header file for NotesNoteView class.
*
*/

#ifndef NOTESNOTEVIEW_H
#define NOTESNOTEVIEW_H

// System includes
#include <QObject>
#include <HbView>

// User includes
#include "notesviewsdefines.h"
#include "notesappcontrollerif.h"

// Forward declarations
class QGraphicsWidget;
class HbListView;
class HbAction;
class HbAbstractViewItem;
class HbLabel;
class NotesModel;
class NotesDocLoader;
class AgendaUtil;
class NotesSortFilterProxyModel;
class NotesEditor;

class NotesNoteView : public HbView
{
	Q_OBJECT

public:
	NOTESVIEWS_EXPORT NotesNoteView(QGraphicsWidget *parent = 0);
	NOTESVIEWS_EXPORT virtual ~NotesNoteView();

public:
	NOTESVIEWS_EXPORT void setupView(
			NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader);
	NOTESVIEWS_EXPORT void updateNoteView();


signals:
	void deleteEntry(ulong entryId);

private slots:
	void createNewNote();
	void handleItemReleased(const QModelIndex &index);
	void handleItemLongPressed(
			HbAbstractViewItem *item, const QPointF &coords);
	void deleteNote();
	void markNoteAsFavourite();
	void markNoteAsTodo();
	void handleEditingCompleted(bool status);
	void displayCollectionView();
	void displayAllNotesView();
	void handleActionStateChanged();
	void handleOrientationChanged(Qt::Orientation);
	void openNote();
	void selectedMenuAction(HbAction *action);
	void handleMenuClosed();
	void updateView(ulong id=0);

private:
	HbListView *mListView;
	HbAbstractViewItem *mSelectedItem;

	HbAction *mAllNotesAction;
	HbAction *mViewCollectionAction;
	HbAction *mAddNoteAction;
	HbAction *mEditAction;
	HbAction *mDeleteAction;
	HbAction *mMakeFavouriteAction;
	HbAction *mMarkTodoAction;
	HbAction *mOpenAction;

	HbLabel *mEmptyListLabel;
	
	AgendaUtil *mAgendaUtil;

	NotesAppControllerIf *mAppControllerIf;
	NotesDocLoader *mDocLoader;
	NotesModel *mNotesModel;
	NotesSortFilterProxyModel *mProxyModel;
	NotesEditor *mNotesEditor;
	bool mIsLongTop;
};

#endif // NOTESNOTEVIEW_H

// End of file	--Don't remove this.
