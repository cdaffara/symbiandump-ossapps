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
* Header file for NotesTodoEditor class.
*
*/

#ifndef NOTESTODOEDITOR_H
#define NOTESTODOEDITOR_H

// System includes
#include <QObject>
#include <QDateTime>
#include <QPointer>
#include <QModelIndex>
#include <HbView>

// Forward declarations
class QGraphicsWidget;
class HbDataForm;
class HbDataFormModel;
class HbAction;
class HbLineEdit;
class HbComboBox;
class HbPushButton;
class HbCheckBox;
class HbDateTimePicker;
class HbDialog;
class HbGroupBox;
class HbDataFormModelItem;
class AgendaEntry;
class NotesEditorPrivate;
class NotesEditorDocLoader;
class NotesTodoEditorCustomItem;

class NotesTodoEditor: public QObject
{
	Q_OBJECT

public:
	NotesTodoEditor(
			NotesEditorPrivate* owner, QObject *parent = 0);
	virtual ~NotesTodoEditor();

public slots:
	void updateSummaryText(const QString &text);
	void updatePriority(int index);
	void updateDescription(const QString &text);
	void handleReminderItem(int index);

private:
	void execute(AgendaEntry entry);
	void create(AgendaEntry entry);
	void initFormModel();
	void insertSubjectItem();
	void insertDueDateItem();
	void insertReminderToggle();
	void insertPriorityItem();
	void insertDescriptionItem();
	void addMenu();
	bool validAlarmSet();
	void addDiscardChangesAction();

private slots:
	void saveTodo();
	void close();
	void handleAddDescriptionAction();
	void handleRemoveDescriptionAction();
	void handleDeleteAction();
	void handleDiscardChangesAction();
	void selectedAction(HbAction *action);

private:
	NotesEditorPrivate* mOwner;

	HbView *mEditor;
	HbDataForm *mDataForm;
	QPointer<HbDataFormModel> mFormModel;

	HbDataFormModelItem *mSubjectItem;
	HbDataFormModelItem *mDescriptionItem;
	HbDataFormModelItem *mPriorityItem;
	HbDataFormModelItem *mDueDateItem;
	HbDataFormModelItem *mReminderEnabler;
	HbDataFormModelItem *mReminderItem;

	HbAction *mDeleteAction;
	HbAction *mDoneAction;
	HbAction *mDiscardAction;
	QPointer<HbAction> mDescriptionAction;
	HbAction *mDeleteTodoAction;
	HbAction *mCancelDeleteAction;
	HbGroupBox *mSubHeading;

	NotesEditorDocLoader *mDocLoader;

	int mDescriptionItemIndex;
	bool mDiscardChangesActive;
private:
	friend class NotesEditorPrivate;
};

#endif // NOTESTODOEDITOR_H

// End of file	--Don't remove this.
