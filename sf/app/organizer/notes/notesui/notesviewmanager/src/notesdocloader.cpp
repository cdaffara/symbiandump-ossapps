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
* Description: Definition file for class NotesDocLoader.
*
*/


// User includes
#include "notesdocloader.h"
#include "notesmainview.h"
#include "notescollectionview.h"
#include "notestodoview.h"
#include "notesfavoriteview.h"
#include "notesnoteview.h"
#include "notescommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesdocloaderTraces.h"
#endif


/*!
	\class NotesDocLoader

	Custom document loader class for notes view manager.
 */
/*!
	From HbDocumentLoader.
	Creates and returns an object of type and assigns name as its object name.

	\param type The type name as mentioned in docml.
	\param name The name of the object as mentioned in docml.
	\return QObject* Pointer to the constructed QObject.
	\sa HbDocumentLoader
 */
QObject* NotesDocLoader::createObject(const QString &type, const QString &name)
{
	OstTraceFunctionEntry0( NOTESDOCLOADER_CREATEOBJECT_ENTRY );
	if (NOTES_MAIN_VIEW == name) {
		QObject *object = new NotesMainView();
		object->setObjectName(name);
		OstTraceFunctionExit0( NOTESDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	} else if (NOTES_COLLECTION_VIEW == name) {
		QObject *object = new NotesCollectionView();
		object->setObjectName(name);
		OstTraceFunctionExit0( DUP1_NOTESDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	} else if (NOTES_TODO_VIEW == name) {
		QObject *object = new NotesTodoView();
		object->setObjectName(name);
		OstTraceFunctionExit0( DUP2_NOTESDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	} else if (NOTES_FAVORITES_VIEW == name) {
		QObject *object = new NotesFavoriteView();
		object->setObjectName(name);
		OstTraceFunctionExit0( DUP3_NOTESDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	} else if (NOTES_NOTE_VIEW == name) {
		QObject *object = new NotesNoteView();
		object->setObjectName(name);
		OstTraceFunctionExit0( DUP4_NOTESDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	}

	return HbDocumentLoader::createObject(type, name);
}

// End of file	--Don't remove this.
