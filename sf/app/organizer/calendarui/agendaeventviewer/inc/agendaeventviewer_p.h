/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Header file for AgendaEventViewerPrivate class
*
*/

#ifndef AGENDAEVENTVIEWERPRIVATE_H
#define AGENDAEVENTVIEWERPRIVATE_H

// System includes.
#include <QObject>

// User includes.
#include "agendaeventviewer.h"

// Forward declarations
class QFile;
class AgendaEntry;
class AgendaUtil;
class AgendaEventView;

class AgendaEventViewerPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(AgendaEventViewer)

public:
	explicit AgendaEventViewerPrivate(AgendaUtil *agendaUtil, QObject *parent);
	virtual ~AgendaEventViewerPrivate();

private:
	void view(const ulong id, AgendaEventViewer::Actions action);
	void view(const QFile &fileHandle, AgendaEventViewer::Actions action);
	void view(AgendaEntry entry, AgendaEventViewer::Actions action);
	
    /**
     * close the editor and save the entry
     * should be call if editor is open
     */
	void saveAndCloseEditor();
    
	/**
     * close agenda event view
     * should be call if agenda eevent view is open
     */
	void closeAgendaEventView();

private slots:
	void viewingCompleted(const QDate date);
	void editingStarted();
	void editingCompleted();
	void deletingStarted();
	void deletingCompleted();
	void viewCreationCompleted(int error);

private:
	AgendaEventViewer *q_ptr;
	AgendaUtil *mAgendaUtil;
	AgendaEventView *mAgendaEventView;

	bool mViewerOwnsAgendaUtil;

private:
	friend class AgendaEventView;
	ulong mId;
	AgendaEventViewer::Actions mAction;
	bool mShowEventViewById;
	bool mShowEventViewByFileHandle;
	QString mFileName;
};

#endif // AGENDAEVENTVIEWERPRIVATE_H

// End of file
