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
* Description: This is the Definition file for NotesModelHandler class.
*
*/

// System includes
#include <QtGui>
// User includes
#include "notesmodelhandler.h"
#include <agendautil.h>
#include "notesmodel.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notesmodelhandlerTraces.h"
#endif


/*!
	\class NotesModelHandler

	This is responsible for holding the notesmodel and agendautil objects.
 */

/*!
	Default constructor.
 */
NotesModelHandler::NotesModelHandler(QObject *parent)
:QObject(parent),
 mAgendaUtil(0),
 mNotesModel(0)
{
	OstTraceFunctionEntry0( NOTESMODELHANDLER_NOTESMODELHANDLER_ENTRY );
	QT_TRAP_THROWING(mAgendaUtil = new AgendaUtil(this));
	Q_ASSERT_X(mAgendaUtil, "notesviewmanager.cpp", "mAgendaUtil is 0");

	// Construct the source model here.
	mNotesModel = new NotesModel(mAgendaUtil, this);
	OstTraceFunctionExit0( NOTESMODELHANDLER_NOTESMODELHANDLER_EXIT );
}

/*!
	Destructor
 */
NotesModelHandler::~NotesModelHandler()
{
	OstTraceFunctionEntry0( DUP1_NOTESMODELHANDLER_NOTESMODELHANDLER_ENTRY );
	if (mAgendaUtil) {
		delete mAgendaUtil;
		mAgendaUtil = 0;
	}
	if (mNotesModel) {
		delete mNotesModel;
		mNotesModel = 0;
	}
	OstTraceFunctionExit0( DUP1_NOTESMODELHANDLER_NOTESMODELHANDLER_EXIT );
}

/*!
	Returns a pointer to the agenda interface.

	\return AgendaUtil pointer.
 */
AgendaUtil *NotesModelHandler::agendaInterface()
{
	OstTraceFunctionEntry0( NOTESMODELHANDLER_AGENDAINTERFACE_ENTRY );
	Q_ASSERT(mAgendaUtil);

	OstTraceFunctionExit0( NOTESMODELHANDLER_AGENDAINTERFACE_EXIT );
	return mAgendaUtil;
}

/*!
	Returns a pointer to the notes application model.

	\return NotesModel pointer.
 */
NotesModel *NotesModelHandler::notesModel()
{
	OstTraceFunctionEntry0( NOTESMODELHANDLER_NOTESMODEL_ENTRY );
	Q_ASSERT(mNotesModel);

	OstTraceFunctionExit0( NOTESMODELHANDLER_NOTESMODEL_EXIT );
	return mNotesModel;
}

// End of file	--Don't remove this.
