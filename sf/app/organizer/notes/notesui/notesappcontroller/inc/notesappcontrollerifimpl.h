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
* This file contains the NotesAppControllerIfImpl class declaration.
*
*/

#ifndef NOTESAPPCONTROLLERIFIMPL_H
#define NOTESAPPCONTROLLERIFIMPL_H

// System includes
#include <QObject>

// User includes
#include "notesappcontrollerif.h"
#include "notescommon.h"

// Forward declarations
class AgendaUtil;
class NotesModel;
class NotesAppController;

class NotesAppControllerIfImpl : public NotesAppControllerIf, public QObject
{
public:
	NotesAppControllerIfImpl(
			NotesAppController *controller, QObject *parent = 0);
	virtual ~NotesAppControllerIfImpl();

public:
	virtual AgendaUtil *agendaUtil();
	virtual NotesModel *notesModel();
	virtual void switchToView(NotesNamespace::NotesViewIds viewId);

private:
	NotesAppController *mAppController;
};

#endif // NOTESAPPCONTROLLERIFIMPL_H

// End of file	--Don't remove this.
