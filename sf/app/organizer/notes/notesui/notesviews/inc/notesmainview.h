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
* Header file for NotesMainView class.
*
*/

#ifndef NOTESMAINVIEW_H
#define NOTESMAINVIEW_H

// System includes
#include <HbView>

// User includes
#include "notesviewsdefines.h"
#include "notesappcontrollerif.h"

// Forward declarations
class QGraphicsWidget;
class HbListView;
class HbTextEdit;
class HbAction;
class HbAbstractViewItem;
class HbDialog;
class HbLabel;
class HbGroupBox;
class NotesModel;
class NotesDocLoader;
class AgendaUtil;
class NotesSortFilterProxyModel;
class NotesEditor;
class AgendaEventViewer;
class AfActivityStorage;

class NotesMainView : public HbView
{
	Q_OBJECT

public:
	NOTESVIEWS_EXPORT NotesMainView(QGraphicsWidget *parent = 0);
	NOTESVIEWS_EXPORT virtual ~NotesMainView();

public:
	NOTESVIEWS_EXPORT void setupView(
			NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader);
	NOTESVIEWS_EXPORT void setupAfterViewReady();
	NOTESVIEWS_EXPORT void updateTitle();
	NOTESVIEWS_EXPORT void captureScreenShot(bool captureScreenShot = false);

signals:
	void deleteEntry(ulong entryId);
private :
	/*
	 * remove the activity
	 */
	bool removeActivity();
private slots:
	void createNewNote();
	void handleItemReleased(const QModelIndex &index);
	void handleItemLongPressed(
			HbAbstractViewItem *item, const QPointF &coords);
	void deleteNote();
	void markTodoStatus();
	void markNoteAsFavourite();
	void handleEditingCompleted(bool status);
	void displayCollectionView();
	void scrollTo(QModelIndex index);
	void handleViewingCompleted();
	void handleActionStateChanged();
	void editTodo();
	void handleOrientationChanged(Qt::Orientation);
	void updateSubTitle(ulong id=0);
	void markNoteAsTodo();
	void openNote();
	void selectedMenuAction(HbAction *action);
	void handleMenuClosed();
	void saveActivity();
	
private:
	HbListView *mListView;
	HbAbstractViewItem *mSelectedItem;
	HbLabel* mViewHeading;

	HbDialog *mFilterPopup;

	HbAction *mAddNoteAction;
	HbAction *mDeleteAction;
	HbAction *mAllNotesAction;
	HbAction *mViewCollectionAction;
	HbAction *mMakeFavouriteAction;
	HbAction *mTodoStatusAction;
	HbAction *mEditTodoAction;
	HbAction *mMarkTodoAction;
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
	bool 			mIsLongTop;
	bool			mIsScreenShotCapruted; // check if the screenshot captured is valid
	QVariantHash	mScreenShot; // screenshot
	AfActivityStorage *mActivityStorage;;
};

#endif // NOTESMAINVIEW_H

// End of file	--Don't remove this.
