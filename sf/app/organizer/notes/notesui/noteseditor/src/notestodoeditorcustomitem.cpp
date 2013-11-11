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
 * Description: Source file for NotesTodoEditorCustomItem class which contains
 * 				all custom widgets needed for todo.
 *
 */

// System inlcudes.
#include <QTime>
#include <QGraphicsLinearLayout>
#include <HbExtendedLocale>
#include <HbLabel>
#include <HbDateTimePicker>
#include <HbAction>
#include <HbDataFormModelItem>
#include <HbAbstractViewItem>
#include <HbAbstractItemView>
#include <HbDataFormModel>

// User includes.
#include "notestodoeditorcustomitem.h"
#include "noteseditor_p.h"
#include "noteseditorcommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notestodoeditorcustomitemTraces.h"
#endif


/*!
 \class NotesTodoEditorCustomItem

	 This class is responsible for creating customitem for Alarm Time field in
	 Alarmeditor.
 */

/*!
	 Default Constructor.

	 \param noteseditorPvt pointer to noteseditorprivate class
	 \param parent object of parent for this class.

 */
NotesTodoEditorCustomItem::NotesTodoEditorCustomItem(
	NotesEditorPrivate* noteseditorPvt,QGraphicsItem *parent)
:HbDataFormViewItem(parent),
 mNotesTodoeditorPvt(noteseditorPvt),
 mDueDatePickerIsActive(false)
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_NOTESTODOEDITORCUSTOMITEM_ENTRY );

	OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_NOTESTODOEDITORCUSTOMITEM_EXIT );
}

/*!
	 Destructor.
 */
NotesTodoEditorCustomItem::~NotesTodoEditorCustomItem()
{
	OstTraceFunctionEntry0( DUP1_NOTESTODOEDITORCUSTOMITEM_NOTESTODOEDITORCUSTOMITEM_ENTRY );
	OstTraceFunctionExit0( DUP1_NOTESTODOEDITORCUSTOMITEM_NOTESTODOEDITORCUSTOMITEM_EXIT );
}

/*!
	 Create customitem.

	 \return HbAbstractViewItem* Pointer to NotesTodoEditorCustomItem object.
 */
HbAbstractViewItem* NotesTodoEditorCustomItem::createItem()
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_CREATEITEM_ENTRY );
	return new NotesTodoEditorCustomItem(*this);
}

/*!
	 Create customwidget.

	 \return HbWidget* Holds the custom widget pointer.
 */
HbWidget* NotesTodoEditorCustomItem::createCustomWidget()
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_CREATECUSTOMWIDGET_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
		static_cast<HbDataFormModelItem::DataItemType>(modelIndex()
		.data(HbDataFormModelItem::ItemTypeRole).toInt());

	switch(itemType) {
		case HbDataFormModelItem::CustomItemBase + DueDateItemOffset: {
            setProperty("alarmdateItem",false);
			mDueDateItem = new HbPushButton();
			mDueDateItem->setObjectName("dueDateItem");
			connect(
					mDueDateItem, SIGNAL(clicked()),
					this, SLOT(selectDueDate()));
			OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_CREATECUSTOMWIDGET_EXIT );
			return mDueDateItem;
		}

		case HbDataFormModelItem::CustomItemBase + AlarmDateItemOffset: {
		
            setProperty("alarmdateItem",true);
			// Adding two push buttions.
			HbWidget *widget = new HbWidget();
			QGraphicsLinearLayout *layout =
					new QGraphicsLinearLayout(Qt::Horizontal);
			widget->setLayout(layout);

			mTimeWidget = new HbPushButton();
			mTimeWidget->setObjectName("alarmTime");
			layout->addItem(mTimeWidget);
			connect(
					mTimeWidget, SIGNAL(clicked()),
					this, SLOT(launchTimePicker()));

			mDateWidget = new HbPushButton();
			mDateWidget->setObjectName("alarmDate");
			layout->addItem(mDateWidget);
			connect(
					mDateWidget, SIGNAL(clicked()),
					this, SLOT(launchDatePicker()));
			OstTraceFunctionExit0( DUP1_NOTESTODOEDITORCUSTOMITEM_CREATECUSTOMWIDGET_EXIT );
			return widget;
		}
		case HbDataFormModelItem::CustomItemBase + ItemOffsetUnKnown:
		default:
		return 0;
	}
}

/*!
	Override method to enable the custom items.
 */
bool NotesTodoEditorCustomItem::canSetModelIndex(const QModelIndex &index) const
	{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_CANSETMODELINDEX_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>(
					index.data(HbDataFormModelItem::ItemTypeRole).toInt());

	if( itemType == (HbDataFormModelItem::CustomItemBase + DueDateItemOffset) ||
	itemType == (HbDataFormModelItem::CustomItemBase + AlarmDateItemOffset)) {
		OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_CANSETMODELINDEX_EXIT );
		return true;
	} else {
		OstTraceFunctionExit0( DUP1_NOTESTODOEDITORCUSTOMITEM_CANSETMODELINDEX_EXIT );
		return false;
	}
}

/*!
	Set the text for the custom widgets using the
	model item's content widget data
 */
void NotesTodoEditorCustomItem::restore()
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_RESTORE_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>(
			modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

	HbDataFormModel* model =
			static_cast<HbDataFormModel*>(
			static_cast<HbAbstractViewItem*>(this)->itemView()->model());
	HbDataFormModelItem* modlItem = model->itemFromIndex(modelIndex());

	switch (itemType) {
		case (HbDataFormModelItem::CustomItemBase + DueDateItemOffset):
				mDueDateItem->setText(
						modlItem->contentWidgetData("text").toString());
		break;
		case (HbDataFormModelItem::CustomItemBase + AlarmDateItemOffset):
				mTimeWidget->setText(
						modlItem->contentWidgetData("alarmTime").toString());
				mDateWidget->setText(
						modlItem->contentWidgetData("alarmDate").toString());
		break;

		default:
		break;
	}
	OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_RESTORE_EXIT );
}

/*!
	 Launches the time picker dialog.
 */
void NotesTodoEditorCustomItem::launchTimePicker()
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_LAUNCHTIMEPICKER_ENTRY );
	if (mTimePickerDialog) {
		delete mTimePickerDialog;
	}

	// Create the dialog.
	mTimePickerDialog = new HbDialog;
	mTimePickerDialog->setTimeout(HbDialog::NoTimeout);
	mTimePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mTimePickerDialog->setAttribute(Qt::WA_DeleteOnClose, true);

	// Set the heading for the dialog.
	HbLabel * timeLabel = new HbLabel("Time", mTimePickerDialog);
	mTimePickerDialog->setHeadingWidget(timeLabel);
	// Create the tumbler.
	HbDateTimePicker *timePicker = new HbDateTimePicker(mTimePickerDialog);

	// Set the format for time picker.
	timePicker->setDisplayFormat(mNotesTodoeditorPvt->timeFormatString());
	// Set the time needs to be displayed in time picker.
	timePicker->setTime(QTime::fromString(mTimeWidget->text(),
		mNotesTodoeditorPvt->timeFormatString()));

	// Set the tumbler as the content widget.
	mTimePickerDialog->setContentWidget(timePicker);

	mOkAction = new HbAction(
			hbTrId("txt_common_button_ok"), mTimePickerDialog);
	mTimePickerDialog->addAction(mOkAction);

	mCancelAction = new HbAction(
			hbTrId("txt_common_button_cancel"), mTimePickerDialog);
	mTimePickerDialog->addAction(mCancelAction);

	mTimePickerDialog->open(this, SLOT(selectedAction(HbAction *)));
	OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_LAUNCHTIMEPICKER_EXIT );
}

/*!
	 Launches the date picker dialog.
 */
void NotesTodoEditorCustomItem::launchDatePicker()
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_LAUNCHDATEPICKER_ENTRY );
	if (mDatePickerDialog) {
		delete mDatePickerDialog;
	}

	// Create the dialog.
	mDatePickerDialog = new HbDialog;
	mDatePickerDialog->setTimeout(HbDialog::NoTimeout);
	mDatePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mDatePickerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	
	// Set the heading for the dialog.
	HbLabel * timeLabel = new HbLabel("Date", mDatePickerDialog);
	mDatePickerDialog->setHeadingWidget(timeLabel);

	// Create the tumbler.
	HbDateTimePicker *datePicker = new HbDateTimePicker(mDatePickerDialog);
	// Set the min/max date for the editor.
	datePicker->setDateRange(QDate::fromString("01/01/1900",
		mNotesTodoeditorPvt->dateFormatString()), QDate::fromString("31/12/2100",
			mNotesTodoeditorPvt->dateFormatString()));

	// Set the date format in date picker.
	datePicker->setDisplayFormat(mNotesTodoeditorPvt->dateFormatString());
	// Set the date needs to be in focus in date picker.
	datePicker->setDate(QDate::fromString(mDateWidget->text(),
		mNotesTodoeditorPvt->dateFormatString()));


	// Set the tumbler as the content widget.
	mDatePickerDialog->setContentWidget(datePicker);

	mOkAction = new HbAction(
			hbTrId("txt_common_button_ok"), mDatePickerDialog);
	mDatePickerDialog->addAction(mOkAction);

	mCancelAction = new HbAction(
			hbTrId("txt_common_button_cancel"), mDatePickerDialog);
	mDatePickerDialog->addAction(mCancelAction);

	mDatePickerDialog->open(this, SLOT(selectedAction(HbAction *)));
	OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_LAUNCHDATEPICKER_EXIT );
}

/*
	Launches date picker for selecting the due date
 */
void NotesTodoEditorCustomItem::selectDueDate()
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_SELECTDUEDATE_ENTRY );
	if( mDueDateItem->isDown()) {
		mDueDateItem->setDown(false);
		OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_SELECTDUEDATE_EXIT );
		return;
	}

	mDueDatePickerIsActive = true;

	if (mDatePickerDialog) {
		delete mDatePickerDialog;
	}

	// Create the dialog.
	mDatePickerDialog = new HbDialog;
	mDatePickerDialog->setTimeout(HbDialog::NoTimeout);
	mDatePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mDatePickerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	
	// Create date picker
	HbDateTimePicker *datePicker = new HbDateTimePicker(mDatePickerDialog);
	// Set the min/max date for the editor.
	datePicker->setDateRange(QDate::fromString("01/01/1900",
		mNotesTodoeditorPvt->dateFormatString()), QDate::fromString("31/12/2100",
			mNotesTodoeditorPvt->dateFormatString()));

	// Set the format of date picker.
	datePicker->setDisplayFormat(mNotesTodoeditorPvt->dateFormatString());
	// Set the date needs to be displayed in datepicker.
	datePicker->setDate(QDate::fromString(mDueDateItem->text(),
		mNotesTodoeditorPvt->dateFormatString()));

	// Set the heading text
	HbLabel *label = new HbLabel(hbTrId("txt_notes_formlabel_due_date"));
	mDatePickerDialog->setHeadingWidget(label);

	// Set the tumbler as the content widget.
	mDatePickerDialog->setContentWidget(datePicker);

	mOkAction = new HbAction(
			hbTrId("txt_common_button_ok"), mDatePickerDialog);
	mDatePickerDialog->addAction(mOkAction);

	mCancelAction = new HbAction(
			hbTrId("txt_common_button_cancel"), mDatePickerDialog);
	mDatePickerDialog->addAction(mCancelAction);

	mDatePickerDialog->open(this, SLOT(selectedAction(HbAction *)));
	OstTraceFunctionExit0( DUP1_NOTESTODOEDITORCUSTOMITEM_SELECTDUEDATE_EXIT );
}

/*!
	 Slot to handle the selected action.
 */
void NotesTodoEditorCustomItem::selectedAction(HbAction *action)
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_SELECTEDACTION_ENTRY );
	if (action == mOkAction) {
		handleOkAction();
	}
	OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_SELECTEDACTION_EXIT );
}

/*!
	 Handles the ok action of date/time picker dialog.
 */
void NotesTodoEditorCustomItem::handleOkAction()
{
	OstTraceFunctionEntry0( NOTESTODOEDITORCUSTOMITEM_HANDLEOKACTION_ENTRY );
	HbExtendedLocale locale =  HbExtendedLocale::system();

	HbDataFormModelItem *modelItem =
			static_cast<HbDataFormModel*>(
						itemView()->model())->itemFromIndex(modelIndex());;

	if (mTimePickerDialog) {
		// Get the time from the time picker.
		QTime newTime = static_cast<HbDateTimePicker *>
		(mTimePickerDialog->contentWidget())->time();

		QString timeText =
				newTime.toString(mNotesTodoeditorPvt->timeFormatString());

		// Update the display text on form item.
		mTimeWidget->setText(timeText);

		if (modelItem) {
			modelItem->setContentWidgetData("alarmTime",timeText);
		}
	} else if(mDateWidget) {
		QDate newDate = static_cast<HbDateTimePicker *> (
			mDatePickerDialog->contentWidget())->date();

		QString dateText =
				newDate.toString(mNotesTodoeditorPvt->dateFormatString());

		// Update the display text on form item.
		mDateWidget->setText(dateText);

		if (modelItem) {
			modelItem->setContentWidgetData("alarmDate",dateText);
		}
	} else if(mDueDatePickerIsActive) {
		mDueDatePickerIsActive = false;
		// Save the date from the date picker.
		QDate selectedDate = static_cast<HbDateTimePicker *> (
				mDatePickerDialog->contentWidget())->date();

		// Update the due date in the agenda entry.
		QDateTime dueDateTime=
			QDateTime(selectedDate, QTime::fromString("00:00:00",
				mNotesTodoeditorPvt->timeFormatString()));

		mNotesTodoeditorPvt->mModifiedNote.setStartAndEndTime(
			dueDateTime, dueDateTime);

		QString dueDateText =
				selectedDate.toString(
								mNotesTodoeditorPvt->dateFormatString());

		// Update the text in the due date form item.
		mDueDateItem->setText(dueDateText);

		if (modelItem) {
			modelItem->setContentWidgetData("text",dueDateText);
		}

		// Update the alarm date based on due date change if active
		HbDataFormModelItem *alarmDateItem =
			static_cast<HbDataFormModel*>(
					itemView()->model())->item(3);
		int itemType =
				alarmDateItem->data(HbDataFormModelItem::ItemTypeRole).toInt();
		if(alarmDateItem && (
				(HbDataFormModelItem::CustomItemBase + AlarmDateItemOffset)
					== itemType)) {
			alarmDateItem->setContentWidgetData("alarmDate",dueDateText);
		}
	}
	OstTraceFunctionExit0( NOTESTODOEDITORCUSTOMITEM_HANDLEOKACTION_EXIT );
}

// End of file	--Don't delete.
