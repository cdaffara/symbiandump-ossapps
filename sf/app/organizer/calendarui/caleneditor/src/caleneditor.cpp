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
* Definition of CalenEditor class.
*
*/

// System includes.
#include <QDateTime>

// User includes
#include <agendaentry.h>
#include <agendautil.h>
#include <caleneditor.h>

#include "caleneditor_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "caleneditorTraces.h"
#endif

/*!
	\class CalenEditor
 */
/*!
	Constructor.

	\param parent HbView pointer.
 */
CalenEditor::CalenEditor(QObject *parent)
:QObject(parent),d_ptr(new CalenEditorPrivate(0, this))
{
	OstTraceFunctionEntry0( CALENEDITOR_CALENEDITOR_ENTRY );
	
	OstTraceFunctionExit0( CALENEDITOR_CALENEDITOR_EXIT );
}

/*!
	Constructor.

	\param agendaUtil pointer to agenda utility
	\param parent HbView pointer.
 */
CalenEditor::CalenEditor(AgendaUtil *agendaUtil, QObject *parent)
:QObject(parent), d_ptr(new CalenEditorPrivate(agendaUtil, this))
{
	OstTraceFunctionEntry0( DUP1_CALENEDITOR_CALENEDITOR_ENTRY );
	
	OstTraceFunctionExit0( DUP1_CALENEDITOR_CALENEDITOR_EXIT );
}

/*!
	Destructor.
 */
CalenEditor::~CalenEditor()
{
	OstTraceFunctionEntry0( DUP2_CALENEDITOR_CALENEDITOR_ENTRY );
	if (d_ptr) {
		delete d_ptr;
	}
	OstTraceFunctionExit0( DUP2_CALENEDITOR_CALENEDITOR_EXIT );
}

/*!
	Shows the caleneditor by parsing a .vcs file which will be edited.
	\param handle reference to QFile handle.
	\param launchCalendar bool to tell whether calendar has to be launched after 
	 coming back from editor
 */
void CalenEditor::edit(const QFile &handle, bool launchCalendar)
{
	OstTraceFunctionEntry0( CALENEDITOR_EDIT_ENTRY );
	d_ptr->edit(handle, launchCalendar);
	OstTraceFunctionExit0( CALENEDITOR_EDIT_EXIT );
}

/*!
	Shows the caleneditor, by parsing an AgendaEntry.
	\param entry An object of AgendaEntry.
	\param launchCalendar bool to tell whether calendar has to be launched after 
	 coming back from editor
 */
void CalenEditor::edit(AgendaEntry entry, bool launchCalendar)
{
	OstTraceFunctionEntry0( DUP1_CALENEDITOR_EDIT_ENTRY );
	if(entry.isNull()) {
		OstTraceFunctionExit0( DUP1_CALENEDITOR_EDIT_EXIT );
		return;
	}
	d_ptr->edit(entry, launchCalendar);
	OstTraceFunctionExit0( DUP2_CALENEDITOR_EDIT_EXIT );
}

/*!
	Shows the caleneditor, by using the entry id provided.
	\param id entry id.
 */
void CalenEditor::edit(ulong id, bool launchCalendar)
{
	OstTraceFunctionEntry0( DUP2_CALENEDITOR_EDIT_ENTRY );
	d_ptr->edit(id, launchCalendar);
	OstTraceFunctionExit0( DUP3_CALENEDITOR_EDIT_EXIT );
}

/*!
	Creates a new entry based on the create type.It launches the
	editor for editing
	\param QDateTime newEntryDateTime is the event start time.
	\param launchCalendar bool to tell whether calendar has to be launched after 
	 coming back from editor
	\param type type of the editor to be shown for creating new entry.
 */
void CalenEditor::create(QDateTime newEntryDateTime, bool launchCalendar, 
                         CalenEditor::CreateType type)
{
	OstTraceFunctionEntry0( CALENEDITOR_CREATE_ENTRY );
	d_ptr->create(type, newEntryDateTime, launchCalendar);
	OstTraceFunctionExit0( CALENEDITOR_CREATE_EXIT );
}

/*!
	Creates a new entry based on the create type by copying the
	details of passed entry.It launches the editor for editing
	\param entry AgendaEntry from which new entry has to be created
	\param launchCalendar bool to tell whether calendar has to be launched after 
	 coming back from editor
	\param type type of the editor to be shown for creating new entry.
 */
void CalenEditor::create(AgendaEntry entry, bool launchCalendar, 
                         CalenEditor::CreateType type)
{
	OstTraceFunctionEntry0( DUP1_CALENEDITOR_CREATE_ENTRY );
	if(entry.isNull()) {
		OstTraceFunctionExit0( DUP1_CALENEDITOR_CREATE_EXIT );
		return;
	}
	d_ptr->create(type, entry, launchCalendar);
	OstTraceFunctionExit0( DUP2_CALENEDITOR_CREATE_EXIT );
}
/*!
    Save the entry from editor 
    should be called only if  edit or create function of editor has called. 
 */
void CalenEditor::saveAndCloseEditor()
{
    d_ptr->forcedSaveEntry();
}

// End of file	--Don't remove this.
