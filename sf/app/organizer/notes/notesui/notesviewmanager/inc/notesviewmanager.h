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
* Description: Header file for class NotesViewManager.
*
*/

#ifndef NOTESVIEWMANAGER_H
#define NOTESVIEWMANAGER_H

// System includes
#include <QObject>

// User includes
#include "notesviewmanagerdefines.h"
#include "notesappcontrollerif.h"

// Forward declarations
class HbMainWindow;
class HbAction;
class NotesMainView;
class NotesModel;
class AgendaUtil;
class NotesCollectionView;
class NotesTodoView;
class NotesFavoriteView;
class NotesNoteView;

class NotesViewManager : public QObject
{
	Q_OBJECT

public:
	NOTESVIEWMANAGER_EXPORT NotesViewManager(
			NotesAppControllerIf &controller, QObject *parent = 0);
	NOTESVIEWMANAGER_EXPORT virtual ~NotesViewManager();

public:
	NOTESVIEWMANAGER_EXPORT void switchToView(
			NotesNamespace::NotesViewIds viewId);

signals:
	void appReady();
	
private:
	void loadNotesMainView();
	void loadNotesCollectionView();
	void loadTodoView();
	void loadFavoritesView();
	void loadNoteView();
	void updateOtherViews();

private slots:
	void loadOtherViews();
	void deleteEntryFromView(ulong entryId);
	void selectedAction(HbAction *action);
	void handleInstanceViewCreationCompleted(int status);

private:
	NotesAppControllerIf &mAppControllerIf;
	AgendaUtil *mAgendaUtil;

	NotesMainView *mMainView;
	NotesCollectionView *mCollectionView;
	NotesTodoView *mTodoView;
	NotesFavoriteView *mFavoriteView;
	NotesNoteView *mNoteView;

	HbAction *mDeleteAction;
	HbAction *mCancelAction;
	ulong mEntryId;
	
	bool instanceViewCreated;

private:
	Q_DISABLE_COPY(NotesViewManager)
};

#endif // NOTESVIEWMANAGER_H

// End of file	--Don't remove this.
