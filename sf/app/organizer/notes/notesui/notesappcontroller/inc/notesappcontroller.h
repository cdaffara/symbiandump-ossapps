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
* Description: This is the header file for NotesAppController class.
*
*/

#ifndef NOTESAPPCONTROLLER_H
#define NOTESAPPCONTROLLER_H

// System includes
#include <QObject>

// User inlcudes
#include "notesappcontrollerdefines.h"

// Forward declarations
class NotesViewManager;
class NotesModelHandler;
class NotesAppControllerIfImpl;

class NOTESAPPCONTROLLER_EXPORT NotesAppController : public QObject
{
	Q_OBJECT
	
public:
	explicit NotesAppController(QObject *parent = 0);
	virtual ~NotesAppController();

signals:
	void appReady();

private slots:
	void handleAppReady();
	
private:
	NotesViewManager *mViewManager;
	NotesModelHandler *mNotesModelHandler;
	NotesAppControllerIfImpl *mIfImpl;

	friend class NotesAppControllerIfImpl;
};

#endif // NOTESAPPCONTROLLER_H

// End of file	--Don't remove this.
