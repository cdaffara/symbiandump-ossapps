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
 * Description:Header file for NotesTodoEditorCustomItem class.
 *
 */

#ifndef NOTESTODOEDITORCUSTOMITEM_H
#define NOTESTODOEDITORCUSTOMITEM_H

// System inlcudes.
#include <QPointer>
#include <HbPushButton>
#include <HbDialog>
#include <hbdataformviewitem.h>

// User includes.
#include "noteseditor_p.h"

class NotesTodoEditorCustomItem : public HbDataFormViewItem
{
Q_OBJECT

public:
	NotesTodoEditorCustomItem(NotesEditorPrivate* noteseditor,
		QGraphicsItem *parent = 0);
	~NotesTodoEditorCustomItem();

public:
	virtual HbAbstractViewItem* createItem();
	virtual HbWidget* createCustomWidget();
	bool canSetModelIndex(const QModelIndex &index) const;
	void restore();

private slots:
	void launchTimePicker();
	void launchDatePicker();
	void selectDueDate();
	void selectedAction(HbAction *action);

private:
	void handleOkAction();

private:
	QPointer<HbDialog> mTimePickerDialog;
	QPointer<HbDialog> mDatePickerDialog;
	QPointer<HbPushButton> mTimeWidget;
	QPointer<HbPushButton> mDateWidget;
	QPointer<HbPushButton> mDueDateItem;

	HbAction *mOkAction;
	HbAction *mCancelAction;

	NotesEditorPrivate* mNotesTodoeditorPvt;
	bool mDueDatePickerIsActive;

};

#endif // NOTESTODOEDITORCUSTOMITEM_H

// End of file	--Don't delete.
