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
* Header file for Notes application's common includes.
*
*/

#ifndef NOTESCOMMON_H
#define NOTESCOMMON_H

// Common definitions.
#define NOTES_MAIN_VIEW_XML			":/xml/notesmainview.docml"
#define NOTES_MAIN_VIEW				"notesMainView"
#define NOTES_COLLECTION_VIEW_XML	":/xml/notescollectionsview.docml"
#define NOTES_COLLECTION_VIEW		"notesCollectionView"
#define NOTES_TODO_VIEW_XML			":/xml/notestodoview.docml"
#define NOTES_TODO_VIEW				"notesTodoView"
#define NOTES_FAVORITES_VIEW_XML	":/xml/notesfavoriteview.docml"
#define NOTES_FAVORITES_VIEW		"notesFavoriteView"
#define NOTES_NOTE_VIEW_XML			":/xml/notesnoteview.docml"
#define NOTES_NOTE_VIEW				"notesNoteView"

/*!
	\class NotesNamespace

	This will have all the common enums and declarations needed by the notes
	application.
 */
class NotesNamespace {

public:
	/*!
		\enum DataRole
	 */
	enum DataRole {
		IdRole = Qt::UserRole + 100,
		TypeRole,
		StatusRole,
		FavouriteRole
	};

	/*!
		\enum NotesViewIds
	 */
	enum NotesViewIds {
		NotesMainViewId,
		NotesCollectionViewId,
		NotesTodoViewId,
		NotesFavoritesViewId,
		NotesNoteViewId
	};
};

#endif // NOTESCOMMON_H

// End of file	--Don't remove this.
