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
* Description: This is the Header file for NotesModelHandler class.
*
*/

#ifndef NOTESMODELHANDLER_H
#define NOTESMODELHANDLER_H

// System includes
#include <QObject>

// User includes
#include "notesmodelhandlerdefines.h"

// Forward declarations
class AgendaUtil;
class NotesModel;

class NOTESMODELHANDLER_EXPORT NotesModelHandler : public QObject
{
public:
	explicit NotesModelHandler(QObject *parent = 0);
	virtual ~NotesModelHandler();

public:
	AgendaUtil *agendaInterface();
	NotesModel *notesModel();

private:
	AgendaUtil *mAgendaUtil;
	NotesModel *mNotesModel;
};

#endif // NOTESMODELHANDLER_H

// End of file	--Don't remove this.
