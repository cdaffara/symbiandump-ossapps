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
* This file contains the NotesAppControllerIf abstract class declaration.
*
*/

#ifndef NOTESVIEWINTERFACE_H
#define NOTESVIEWINTERFACE_H

// User includes
#include "notescommon.h"

// Forward declarations
class NotesModel;
class AgendaUtil;

class NotesAppControllerIf
{
public:
	virtual AgendaUtil *agendaUtil() = 0;
	virtual NotesModel *notesModel() = 0;
	virtual void switchToView(NotesNamespace::NotesViewIds viewId) = 0;
};

#endif // NOTESVIEWINTERFACE_H

// End of file	--Don't remove this.
