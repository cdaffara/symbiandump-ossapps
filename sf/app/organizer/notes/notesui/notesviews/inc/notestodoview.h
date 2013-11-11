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
* Description: Header file for NotesTodoView class.
*
*/

#ifndef NOTESTODOVIEW_H
#define NOTESTODOVIEW_H

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
class HbGroupBox;
class HbLabel;
class NotesModel;
class NotesDocLoader;
class AgendaUtil;
class NotesSortFilterProxyModel;
class NotesEditor;
class AgendaEventViewer;

class NotesTodoView : public HbView
{
	Q_OBJECT

public:
	NOTESVIEWS_EXPORT NotesTodoView(QGraphicsWidget *parent = 0);
	NOTESVIEWS_EXPORT virtual ~NotesTodoView();

public:
	NOTESVIEWS_EXPORT void setupView(
			NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader);
	NOTESVIEWS_EXPORT void updateTitle();

signals:
	void deleteEntry(ulong entryId);

private slots:
	void createNewTodo();
	void handleItemReleased(const QModelIndex &index);
	void handleItemLongPressed(
			HbAbstractViewItem *item, const QPointF &coords);
	void deleteTodo();
	void markTodoStatus();
	void editTodo();
	void handleEditingCompleted(bool status);
	void displayCollectionView();
	void displayAllNotesView();
	void handleEditingCompleted();
	void handleViewingCompleted();
	void handleActionStateChanged();
	void handleOrientationChanged(Qt::Orientation);
	void updateSubTitle(ulong id=0);
	void openTodo();
	void selectedMenuAction(HbAction *action);
	void handleMenuClosed();

private:
	HbListView *mListView;
	HbAbstractViewItem *mSelectedItem;

	HbAction *mAllNotesAction;
	HbAction *mViewCollectionAction;
	HbAction *mAddTodoAction;
	HbAction *mEditAction;
	HbAction *mDeleteAction;
	HbAction *mTodoStatusAction;
	HbAction *mOpenAction;

	HbGroupBox *mSubTitle;
	HbLabel *mEmptyListLabel;
	
	AgendaUtil *mAgendaUtil;

	NotesAppControllerIf *mAppControllerIf;
	NotesDocLoader *mDocLoader;
	NotesModel *mNotesModel;
	NotesSortFilterProxyModel *mProxyModel;
	NotesEditor *mNotesEditor;
	AgendaEventViewer *mAgendaEventViewer;
	bool mIsLongTop;
};

#endif // NOTESTODOVIEW_H

// End of file	--Don't remove this.
