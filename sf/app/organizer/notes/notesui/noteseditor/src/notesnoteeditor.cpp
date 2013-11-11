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
* Definition file for class NotesNoteEditor class.
*
*/

// System includes
#include <QObject>
#include <QDateTime>
#include <HbTextEdit>
#include <HbView>
#include <HbMainWindow>
#include <HbInstance>
#include <HbAction>
#include <HbLabel>
#include <HbDialog>
#include <HbExtendedLocale>
#include <HbMenu>
#include <CalenEditor>
#include <HbMessageBox>

// User includes
#include "notesnoteeditor.h"
#include "noteseditordocloader.h"
#include "noteseditorcommon.h"
#include "noteseditor.h"
#include "noteseditor_p.h"
#include <agendautil.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesnoteeditorTraces.h"
#endif


/*!
	\class NotesNoteEditor

	This object loads the notesnoteeditor. The view is added to the top
	of the HbMainWindow and returns an object of AgendaEntry when done.

	To check if the note was created or not (i.e cancelled by the user or if
	the contents were empty) can be checked by using isNull() API of AgendaEntry.

	\sa AgendaEntry
 */

/*!
	Constructor.

	\param owner Pointer to the owner object, which is NotesEditorPrivate.
	\param parent The parent object.
 */
NotesNoteEditor::NotesNoteEditor(NotesEditorPrivate *owner, QObject *parent)
:QObject(parent),
 mOwner(owner),
 mEntrySavedInCalendar(false),
 mDiscardChangesActionActive(false),
 mIgnoreFirstContentChange(false)
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_NOTESNOTEEDITOR_ENTRY );
	mDocLoader = new NotesEditorDocLoader;
	Q_ASSERT(mDocLoader);

	bool success;
	// Load the Notes editor docml file.
	mDocLoader->load(NOTES_EDITOR_VIEW_DOCML, &success);
	Q_ASSERT(success);

	// Now load the view and the contents.
	mEditor = static_cast<HbView *> (
			mDocLoader->findWidget(NOTES_EDITOR_VIEW_OBJECT));
	Q_ASSERT(mEditor);

	// Check orientation and load section.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	checkOrientatioAndLoadSection(window->orientation());
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(checkOrientatioAndLoadSection(Qt::Orientation)));

	// Now load the text edit and the list widgets.
	mTextEditor = static_cast<HbTextEdit *> (
			mDocLoader->findWidget(NOTES_EDITOR_TEXT_EDIT));
	if (!mTextEditor) {
		qFatal("Unable to get widget from document");
	}
	connect(
			mTextEditor, SIGNAL(contentsChanged()),
			this, SLOT(updateNoteText()));

	// Take the menu actions.
	mDiscardChangesAction = static_cast<HbAction *> (
			mDocLoader->findObject(NOTES_EDITOR_DISCARD_CHANGES_ACTION));
	if (!mDiscardChangesAction) {
		qFatal("Unable to get object from document");
	}
	connect(
			mDiscardChangesAction, SIGNAL(triggered()),
			this, SLOT(handleDiscardChangesAction()));

	mNewNoteAction = static_cast<HbAction *> (
			mDocLoader->findObject(NOTES_EDITOR_NEWNOTE_ACTION));
	if (!mNewNoteAction) {
		qFatal("Unable to get object from document");
	}
	connect(
			mNewNoteAction, SIGNAL(triggered()),
			this, SLOT(handleNewNoteAction()));

	mMarkToDoAction = static_cast<HbAction *> (
			mDocLoader->findObject(NOTES_EDITOR_MARKTODO_ACTION));
	if (!mMarkToDoAction) {
		qFatal("Unable to get object from document");
	}
	connect(
			mMarkToDoAction, SIGNAL(triggered()),
			this, SLOT(markNoteAsTodo()));

	mAddToCalendarAction = static_cast<HbAction *> (
			mDocLoader->findObject(NOTES_EDITOR_ADD_TO_CALENDAR));
	Q_ASSERT(mAddToCalendarAction);

	connect(
			mAddToCalendarAction, SIGNAL(triggered()),
			this, SLOT(handleAddToCalendarAction()));

	mDeleteAction = static_cast<HbAction *> (
			mDocLoader->findObject(NOTES_EDITOR_DELETE_NOTE));
	if (!mDeleteAction) {
		qFatal("Unable to get widget from document");
	}
	connect(
			mDeleteAction, SIGNAL(triggered()),
			this, SLOT(deleteNote()));

	mMarkFavouriteAction = static_cast<HbAction *> (
			mDocLoader->findObject(NOTES_EDITOR_MARK_FAVOURITE_ACTION));
	if (!mMarkFavouriteAction) {
		qFatal("Unable to get widget from document");
	}
	connect(
			mMarkFavouriteAction, SIGNAL(triggered()),
			this, SLOT(markNoteAsFavourite()));

	// Get the view heading widget.
	mViewHeading =
			static_cast<HbLabel *> (
					mDocLoader->findWidget(NOTES_EDITOR_VIEW_HEADING));
	if (!mViewHeading) {
		qFatal("Unable to get widget from document");
	}

	// Get favorite icon
	mFavouriteIcon = static_cast<HbLabel *> (
			mDocLoader->findWidget(NOTES_EDITOR_FAVOURITE_ICON));
	if (!mFavouriteIcon) {
		qFatal("Unable to get widget from document");
	}
	OstTraceFunctionExit0( NOTESNOTEEDITOR_NOTESNOTEEDITOR_EXIT );
}

/*!
	Destructor.
 */
NotesNoteEditor::~NotesNoteEditor()
{
	OstTraceFunctionEntry0( DUP1_NOTESNOTEEDITOR_NOTESNOTEEDITOR_ENTRY );
	mDocLoader->reset();
	delete mDocLoader;
	OstTraceFunctionExit0( DUP1_NOTESNOTEEDITOR_NOTESNOTEEDITOR_EXIT );
}

/*!
	Shows the editor view. This takes an argument of AgendaEntry.

	\param entry An object of AgendaEntry. Will contain the note details
			depending on whether it is new or it is being edited.
 */
void NotesNoteEditor::execute(AgendaEntry entry)
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_EXECUTE_ENTRY );
	Q_UNUSED(entry)

	HbExtendedLocale locale = HbExtendedLocale::system();

	if (!mOwner->mNewEntry) {
		mTextEditor->setPlainText(mOwner->mModifiedNote.description());

		QString iconName;
		if (mOwner->mModifiedNote.favourite()) {
			iconName = "qtg_small_favorite";

			mMarkFavouriteAction->setText(
					hbTrId("txt_notes_opt_remove_from_favorites"));
		} else {
			iconName = " ";

			mMarkFavouriteAction->setText(
					hbTrId("txt_notes_opt_mark_as_favorite"));
		}

		mFavouriteIcon->setIcon(HbIcon(iconName));

		// Insert new note action.
		HbMenu *viewMenu = mEditor->menu();
		viewMenu->insertAction(mMarkToDoAction, mNewNoteAction);

		// Set the sub heading
		mViewHeading->setPlainText(hbTrId("txt_notes_subhead_note"));
	} else {
		mFavouriteIcon->setIcon(HbIcon(" "));

		// For handling the edit of note when text is given as input
		mTextEditor->setPlainText(mOwner->mModifiedNote.description());

		// Set the sub heading
		mViewHeading->setPlainText(hbTrId("txt_notes_subhead_new_note"));

		// Dim the toolbar icons till text insertion.
		mDeleteAction->setEnabled(false);
		mAddToCalendarAction->setEnabled(false);
	}

	// Remove the discard changes action while launching.
	// This action will be added if user enters some text in editor
	HbMenu *viewMenu = mEditor->menu();
	viewMenu->removeAction(mDiscardChangesAction);

	// Set our view as the current view.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	HbAction *action = new HbAction(Hb::BackNaviAction);
	mEditor->setNavigationAction(action);
	connect(
			action, SIGNAL(triggered()),
			this, SLOT(saveNote()));
	window->addView(mEditor);
	window->setCurrentView(mEditor);
	OstTraceFunctionExit0( NOTESNOTEEDITOR_EXECUTE_EXIT );
}

/*!
	Returns the textedits' current text.

	\return QString Contains the text entered in the text edit.
 */
QString NotesNoteEditor::getDescription()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_GETDESCRIPTION_ENTRY );
	return mTextEditor->toPlainText();
}

/*!
	Marks the currently edited note as a to-do.
 */
void NotesNoteEditor::markNoteAsTodo()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_MARKNOTEASTODO_ENTRY );
	mOwner->markNoteAsTodo();
	OstTraceFunctionExit0( NOTESNOTEEDITOR_MARKNOTEASTODO_EXIT );
}

/*!
	Saves the note. This is called when the Softkey is pressed to save the note.
	No note is saved when the editor is empty.
	It also resets the view back to the previous view.
 */
void NotesNoteEditor::saveNote()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_SAVENOTE_ENTRY );
	bool status = false;

	if (!mOwner->mSaveEntry) {
		// We don't save the note, since the note was modified by an external
		// entity. We just Display a message box alerting the user about this
		// and just close the editor. Note that this happens only when a note is
		// being edited.
		// TODO: Show a note here.
		status = false;
	} else {
		bool status = mOwner->saveNote();
	}
	HbAction *action = qobject_cast<HbAction *>(QObject::sender());

	// Remove the view and reset to the list view.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->removeView(mEditor);

	// Cleanup.
	disconnect(
			action, SIGNAL(triggered()),
			this, SLOT(saveNote()));

	mOwner->editingCompleted(status);
	OstTraceFunctionExit0( NOTESNOTEEDITOR_SAVENOTE_EXIT );
}

/*!
	Deletes the note currently being created/edited and closes the editor.
 */
void NotesNoteEditor::deleteNote()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_DELETENOTE_ENTRY );
	HbMessageBox *confirmationQuery = new HbMessageBox(
			HbMessageBox::MessageTypeQuestion);
	confirmationQuery->setDismissPolicy(HbDialog::NoDismiss);
	confirmationQuery->setTimeout(HbDialog::NoTimeout);
	confirmationQuery->setIconVisible(true);

	QString displayText;
	displayText = displayText.append(hbTrId("txt_notes_info_delete_note"));

	confirmationQuery->setText(displayText);

	// Remove the default actions.
	QList<QAction *> defaultActions = confirmationQuery->actions();
	for (int index=0;index<defaultActions.count();index++) {
		confirmationQuery->removeAction(defaultActions[index]);
	}
	defaultActions.clear();

	// Add delete and cancel actions.
	mDeleteNoteAction = new HbAction(
			hbTrId("txt_common_button_delete"));
	mCancelDeleteAction = new HbAction(
			hbTrId("txt_common_button_cancel"));
	confirmationQuery->addAction(mDeleteNoteAction);
	confirmationQuery->addAction(mCancelDeleteAction);

	confirmationQuery->open(this, SLOT(selectedAction(HbAction*)));
	OstTraceFunctionExit0( NOTESNOTEEDITOR_DELETENOTE_EXIT );
}

/*!
	Closes the notes editor and does nothing else. It removes the view off the
	main window's list of views.
 */
void NotesNoteEditor::close()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_CLOSE_ENTRY );
	HbMainWindow *window = hbInstance->allMainWindows().first();
	// Now close the editor.
	window->removeView(mEditor);
	OstTraceFunctionExit0( NOTESNOTEEDITOR_CLOSE_EXIT );
}

/*!
	Calls the owner to update the note's description.
 */
void NotesNoteEditor::updateNoteText()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_UPDATENOTETEXT_ENTRY );
	// mIgnoreFirstContentChange  is used since updateNoteText
	// slot is called before the cursor is pressed
	if(!mIgnoreFirstContentChange) {
		mIgnoreFirstContentChange = true;
	} else {

		if(mOwner->mNewEntry) {
			// Enable the toolbar icons on text insertion.
			mDeleteAction->setEnabled(true);
			mAddToCalendarAction->setEnabled(true);
		}

		// Insert discardchanges action only while editing the
		// existing entry
		if (!mOwner->mNewEntry && !mDiscardChangesActionActive) {
		HbMenu *viewMenu = mEditor->menu();
		if (mNewNoteAction) {
			viewMenu->insertAction(mNewNoteAction, mDiscardChangesAction);
		} else {
			viewMenu->insertAction(mMarkToDoAction, mDiscardChangesAction);
		}

		mDiscardChangesActionActive = true;
		}
	}

	mOwner->updateNoteText();
	OstTraceFunctionExit0( NOTESNOTEEDITOR_UPDATENOTETEXT_EXIT );
}

/*!
	Checks the orientation and loads the appropriate section from the docml.
 */
void NotesNoteEditor::checkOrientatioAndLoadSection(
		Qt::Orientation screenOrientation)
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_CHECKORIENTATIOANDLOADSECTION_ENTRY );
	bool success;

	// If horizontal, load the landscape section.
	if (Qt::Horizontal == screenOrientation) {
		mDocLoader->load(
				NOTES_EDITOR_VIEW_DOCML, "landscape",
				&success);
		Q_ASSERT_X(
				success, "NotesNoteEditor.cpp",
				"Unable to load landscape section");
	} else if (Qt::Vertical == screenOrientation) {
		mDocLoader->load(
				NOTES_EDITOR_VIEW_DOCML, "portrait",
				&success);
		Q_ASSERT_X(
				success, "NotesNoteEditor.cpp",
				"Unable to load landscape section");
	}
	OstTraceFunctionExit0( NOTESNOTEEDITOR_CHECKORIENTATIOANDLOADSECTION_EXIT );
}

/*!
	Marks or unmarks the note as favourite and updates the favourite icon.
 */
void NotesNoteEditor::markNoteAsFavourite()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_MARKNOTEASFAVOURITE_ENTRY );
	if (mOwner->mModifiedNote.favourite()) {
		mOwner->mModifiedNote.setFavourite(0);

		// Update the icon.
		mFavouriteIcon->setIcon(HbIcon(" "));

		// Change the text on the action.
		mMarkFavouriteAction->setText(
				hbTrId("txt_notes_opt_mark_as_favorite"));
	} else {
		mOwner->mModifiedNote.setFavourite(1);

		// Update the icon.
		mFavouriteIcon->setIcon(HbIcon("qtg_small_favorite"));

		// Change the text on the action.
		mMarkFavouriteAction->setText(
				hbTrId("txt_notes_opt_remove_from_favorites"));
	}
	OstTraceFunctionExit0( NOTESNOTEEDITOR_MARKNOTEASFAVOURITE_EXIT );
}

/*!
	Handles "Discard changes" action when triggered from menu
 */
void NotesNoteEditor::handleDiscardChangesAction()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_HANDLEDISCARDCHANGESACTION_ENTRY );
	// Close the note editor
	close();
	OstTraceFunctionExit0( NOTESNOTEEDITOR_HANDLEDISCARDCHANGESACTION_EXIT );
}

/*!
	Handles "Add to calendar action" from toolbar
 */
void NotesNoteEditor::handleAddToCalendarAction()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_HANDLEADDTOCALENDARACTION_ENTRY );
	mCalenEditor = new CalenEditor(mOwner->mAgendaUtil);

	connect (
			mCalenEditor,SIGNAL(entrySaved()),
			this, SLOT(handleEntrySaveFromCalendar()));

	connect (
			mCalenEditor,SIGNAL(dialogClosed()),
			this, SLOT(handleCalendarEditorClosed()));

	AgendaEntry calendarEntry;
	calendarEntry.setType(AgendaEntry::TypeAppoinment);
	calendarEntry.setSummary(mOwner->mModifiedNote.description().left(40));
	calendarEntry.setDescription(mOwner->mModifiedNote.description());

	QDate currentDate(QDate::currentDate());
	QDateTime startDateTime(currentDate,QTime(8,0,0,0));
	calendarEntry.setStartAndEndTime(startDateTime,startDateTime);

	QString currentDateStr = currentDate.toString("dd/mm/yyyy");

	mCalenEditor->create(
			calendarEntry, 1, CalenEditor::TypeAppointment);

	OstTraceFunctionExit0( NOTESNOTEEDITOR_HANDLEADDTOCALENDARACTION_EXIT );
}

/*!
	Handles entry save signal from calendar editor
 */
void NotesNoteEditor::handleEntrySaveFromCalendar()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_HANDLEENTRYSAVEFROMCALENDAR_ENTRY );
	mEntrySavedInCalendar = true;
	// Delete the old note and go back to the previous view.
	if (!mOwner->mNewEntry) {
		// Delete the note. All the changes are discarded.
		mOwner->mAgendaUtil->deleteEntry(mOwner->mOriginalNote.id());
	}
	OstTraceFunctionExit0( NOTESNOTEEDITOR_HANDLEENTRYSAVEFROMCALENDAR_EXIT );
}

/*!
	Handles calendar editor closed signal
 */
void NotesNoteEditor::handleCalendarEditorClosed()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_HANDLECALENDAREDITORCLOSED_ENTRY );
	if (mEntrySavedInCalendar) {
		mOwner->close(NotesEditor::CloseWithoutSave);

		HbMainWindow *window = hbInstance->allMainWindows().first();
		bool status = false;

		// Now close the notes editor.
		window->removeView(mEditor);

		mOwner->editingCompleted(status);
	}

	disconnect (
			mCalenEditor,SIGNAL(entrySaved()),
			this, SLOT(handleEntrySaveFromCalendar()));

	disconnect (
			mCalenEditor,SIGNAL(dialogClosed()),
			this, SLOT(handleCalendarEditorClosed()));

	// Delete the calendar editor.
	mCalenEditor->deleteLater();
	OstTraceFunctionExit0( NOTESNOTEEDITOR_HANDLECALENDAREDITORCLOSED_EXIT );
}

/*!
	Handles creation of new note signal
 */
void NotesNoteEditor::handleNewNoteAction()
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_HANDLENEWNOTEACTION_ENTRY );
	if (mOwner->isNoteEdited()) {
		mOwner->mAgendaUtil->store(mOwner->mModifiedNote);
	}

	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);

	mOwner->mModifiedNote = mOwner->mOriginalNote = entry;
	mOwner->mNewEntry = true;

	mFavouriteIcon->setIcon(HbIcon(" "));

	// For handling the edit of note when text is given as input
	mTextEditor->setPlainText(mOwner->mModifiedNote.description());

	// Set the sub heading
	mViewHeading->setPlainText(hbTrId("txt_notes_subhead_new_note"));

	// Dim the toolbar icons till text insertion.
	mDeleteAction->setEnabled(false);
	mAddToCalendarAction->setEnabled(false);

	mIgnoreFirstContentChange = false;

	if (mDiscardChangesActionActive) {
		mEditor->menu()->removeAction(mDiscardChangesAction);
		mDiscardChangesActionActive = false;
	}
	mEditor->menu()->removeAction(mNewNoteAction);
	OstTraceFunctionExit0( NOTESNOTEEDITOR_HANDLENEWNOTEACTION_EXIT );
}

/* !
	Slot to handle the selected action from delete message box.
 */
void NotesNoteEditor::selectedAction(HbAction *action)
{
	OstTraceFunctionEntry0( NOTESNOTEEDITOR_SELECTEDACTION_ENTRY );
	if (action == mDeleteNoteAction) {
		mOwner->deleteNote();

		HbMainWindow *window = hbInstance->allMainWindows().first();
		bool status = false;

		// Now close the editor.
		window->removeView(mEditor);

		mOwner->editingCompleted(status);
	}
	OstTraceFunctionExit0( NOTESNOTEEDITOR_SELECTEDACTION_EXIT );
}

// End of file	--Don't remove this.
