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
 * Description: Definition file for class Clock AlarmEditor.
 *
 */

// System includes
#include <QDateTime>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbDataFormViewItem>
#include <HbInstance>
#include <HbAction>
#include <HbMenu>
#include <HbLabel>
#include <HbNotificationDialog>
#include <HbDialog>
#include <HbDateTimePicker>
#include <HbCheckBox>
#include <HbExtendedLocale>
#include <HbGroupBox>
#include <HbDocumentLoader>
#include <HbTranslator>
#include <HbAbstractVkbHost>
#include <HbLineEdit>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <QLocale>
#include <HbParameterLengthLimiter>

// User includes
#include "clockprivatecrkeys.h"
#include "clockalarmeditor.h"
#include "alarmclient.h"
#include "settingsutility.h"
#include "timezoneclient.h"
#include "clockalarmcustomitem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockalarmeditorTraces.h"
#endif


// Constants
const int KOneHourInMinute(60);
const int KOneMinuteInSecons(60);
const int KOneMinute(1);
const int KOneHour(1);
const int KSecondsInOneDay(24 * 60 * 60);
const int KDaysInWeek(7);

/*!
    \class ClockAlarmEditor

    This object loads the clock alarm editor view. The view is added to the top
    of the HbMainWindow.
 */

/*!
    Constructor.

    \param parentViewIndex Previous view Index.
    \param alarmId Id of the alarm, if new alarm(0) or else id of alarm to be
    edit.
    \param parent The parent object.
 */
ClockAlarmEditor::ClockAlarmEditor(
		AlarmClient &alarmClient, int alarmId,
		QObject *parent)
:QObject(parent),
 mAlarmId(alarmId),
 mStartOfWeek(0),
 mAlarmDayItemInserted(false),
 mIsQuickAlarm(true),
 mAlarmEditorForm(0),
 mAlarmEditorModel(0),
 mAlarmTimeItem(0),
 mAlarmOccurenceItem(0),
 mAlarmDayItem(0),
 mAlarmSoundItem(0),
 mAlarmDescription(0),
 mAlarmClient(alarmClient)
{
 	OstTraceFunctionEntry0( CLOCKALARMEDITOR_CLOCKALARMEDITOR_ENTRY );
 	// Load the translation file and install the alarmeditor specific translator
	mTranslator = new HbTranslator("clockalarmeditor");
	mTranslator->loadCommon();
	
	// create the timezone client object
	mTimezoneClient = TimezoneClient::getInstance();
	// Create the settings manager.
	mSettingsManager = new XQSettingsManager(this);

	mPreviosAlarmTime = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidNitz,
			KPreviousAlarmTime);

	// Create settings utility object. To get the date & time format.
	// TODO: do i need to delete this object ??
	SettingsUtility *settingsUtil = new SettingsUtility(this);
	mTimeFormat = settingsUtil->timeFormatString();
	
	// Get start of week from the locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    mStartOfWeek = locale.startOfWeek();

	// Create the HbDocumentLoader object.
	HbDocumentLoader *loader = new HbDocumentLoader();
	bool success;
	loader->load(":/xml/clockalarmeditor.docml", &success);

	mAlarmEditorView = qobject_cast<HbView *>(
			loader->findWidget("alarmEditorView"));

	// Get the subtitle groupBox.
	HbGroupBox *subtitleGroupBox = qobject_cast<HbGroupBox *>(
			loader->findWidget("subtitleGroupBox"));

	// Set the heading of the subtitle groupBox.
	if (mAlarmId) {
		subtitleGroupBox->setHeading(hbTrId("txt_clock_subhead_alarm"));
	} else {
		subtitleGroupBox->setHeading(hbTrId("txt_clock_subhead_new_alarm"));
	}

	// Get the Alarm Editor DataForm.
	mAlarmEditorForm = qobject_cast<HbDataForm *> (
			loader->findWidget("alarmEditorForm"));

	mAlarmEditorForm->setItemPixmapCacheEnabled(true);
	// Create the alarm info structure with desired values.
	initAlarmInfo();

	// Creates & initializes the DataFormModel for the AlarmEditor DataForm.
	initModel();

	QList <HbAbstractViewItem*> prototypes = mAlarmEditorForm->itemPrototypes();
	ClockAlarmCustomItem *customItem =
			new ClockAlarmCustomItem(mAlarmEditorForm);
	prototypes.append(customItem);
	mAlarmEditorForm->setItemPrototypes(prototypes);

	// Get the menu items for the alarm editor.
	mDeleteAction = qobject_cast<HbAction *> (
			loader->findObject("deleteAction"));

	// Connect the signals for the menu item.
	connect(
			mDeleteAction, SIGNAL(triggered()),
			this, SLOT(handleDeleteAction()));

	// Add the done soft key action.
	mDoneAction = new HbAction(Hb::DoneNaviAction);
	connect(
			mDoneAction, SIGNAL(triggered()),
			this, SLOT(handleDoneAction()));
	mAlarmEditorView->setNavigationAction(mDoneAction);
	
	if (mAlarmEditorView) {
		mVirtualKeyboard = new HbAbstractVkbHost(mAlarmEditorView);
    }
	connect(
			mVirtualKeyboard, SIGNAL(aboutToOpen()),
			this, SLOT(handleKeypadOpen()));
	connect(
			mVirtualKeyboard, SIGNAL(aboutToClose()),
			this, SLOT(handleKeypadClosed()));


	OstTraceFunctionExit0( CLOCKALARMEDITOR_CLOCKALARMEDITOR_EXIT );
}

/*!
    Destructor.
 */
ClockAlarmEditor::~ClockAlarmEditor()
{
	OstTraceFunctionEntry0( DUP1_CLOCKALARMEDITOR_CLOCKALARMEDITOR_ENTRY );
	// Remove the translator
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}

	OstTraceFunctionExit0( DUP1_CLOCKALARMEDITOR_CLOCKALARMEDITOR_EXIT );
}

/*!
    Show the alarm editor.
 */
void ClockAlarmEditor::showAlarmEditor()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_SHOWALARMEDITOR_ENTRY );
	// Store the current view and set alarm editor as current view.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->addView(mAlarmEditorView);
	window->setCurrentView(mAlarmEditorView);
	OstTraceFunctionExit0( CLOCKALARMEDITOR_SHOWALARMEDITOR_EXIT );
}

/*!
    Handles done action on the editor. Saves the alarm
 */
void ClockAlarmEditor::handleDoneAction()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_HANDLEDONEACTION_ENTRY );
	int alarmDayIndex = -1;
	if (mAlarmDayItemInserted) {
		alarmDayIndex = mAlarmDayItem->contentWidgetData("currentIndex").toInt();
		alarmDayIndex += mStartOfWeek;
		if(alarmDayIndex >= KDaysInWeek){
		   alarmDayIndex -= KDaysInWeek;
		}
	}
	
	if(mAlarmId) {
		mAlarmClient.deleteAlarm(mAlarmId);
	}

	setAlarm(
			mAlarmTimeItem->contentWidgetData("text").toString(),
			mAlarmDescription->contentWidgetData("text").toString(),
			mAlarmOccurenceItem->contentWidgetData("currentIndex").toInt(),
			alarmDayIndex);

	mAlarmDayItemInserted = false;

	emit alarmSet();

	closeAlarmEditor();
	OstTraceFunctionExit0( CLOCKALARMEDITOR_HANDLEDONEACTION_EXIT );
}

/*!
	Handles delete action on the editor. Deletes the alarm.
 */
void ClockAlarmEditor::handleDeleteAction()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_HANDLEDELETEACTION_ENTRY );
	if(mAlarmId) {
		mAlarmClient.deleteAlarm(mAlarmId);
	}
	closeAlarmEditor();
	OstTraceFunctionExit0( CLOCKALARMEDITOR_HANDLEDELETEACTION_EXIT );
}

/*!
    Handles change in alarmtime item of the data form.
 */
void ClockAlarmEditor::handleTimeChange(const QString &text)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_HANDLETIMECHANGE_ENTRY );
	// If it is a new alarm.
	if (!mAlarmId) {
		// Decide the day to focus.
		int alarmDayIndex = (QDate::currentDate()).dayOfWeek() - 1;

		QTime alarmTime = QTime::fromString(
				text,
				mTimeFormat);
		if (!alarmTime.isValid()) {
			OstTraceFunctionExit0( CLOCKALARMEDITOR_HANDLETIMECHANGE_EXIT );
			return;
		}

		QTime homeTime = QTime::currentTime();
		if (-1 == alarmDayIndex) {
			OstTraceFunctionExit0( DUP1_CLOCKALARMEDITOR_HANDLETIMECHANGE_EXIT );
			return;
		}
		if (homeTime > alarmTime) {
			alarmDayIndex++;
		}
		if (7 == alarmDayIndex) {
			alarmDayIndex = 0;
		}
		if (mAlarmDayItemInserted) {
  		    alarmDayIndex -= mStartOfWeek;

            if(0 > alarmDayIndex){
               alarmDayIndex += KDaysInWeek;
            }
            mAlarmEditorForm->removeConnection(
            					mAlarmDayItem, SIGNAL(currentIndexChanged(int)),
            					this, SLOT(handleDayChanged(int)));
            mIsQuickAlarm = true;
			mAlarmDayItem->setContentWidgetData(
					"currentIndex",
					alarmDayIndex);
			mAlarmEditorForm->addConnection(
								mAlarmDayItem, SIGNAL(currentIndexChanged(int)),
								this, SLOT(handleDayChanged(int)));
		}
	}
	OstTraceFunctionExit0( DUP2_CLOCKALARMEDITOR_HANDLETIMECHANGE_EXIT );
}

/*!
    Handles change in alarm occurence item of the data form.
 */
void ClockAlarmEditor:: handleOccurenceChanged(int index)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_HANDLEOCCURENCECHANGED_ENTRY );
	Q_UNUSED(index)
	
	mIsQuickAlarm = false;

	int repeatIndex =
			mAlarmOccurenceItem->contentWidgetData("currentIndex").toInt();

	if ((Weekly == repeatIndex || Once == repeatIndex)) {
		// Decide which day to focus.
		int alarmDayIndex = (QDate::currentDate()).dayOfWeek() - 1;
		QTime alarmTime = QTime::fromString(
				mAlarmTimeItem->contentWidgetData("text").toString(),
				mTimeFormat);
		QTime homeTime = QTime::currentTime();
		if (-1 == alarmDayIndex) {
			OstTraceFunctionExit0( CLOCKALARMEDITOR_HANDLEOCCURENCECHANGED_EXIT );
			return;
		}
		if (homeTime > alarmTime) {
			alarmDayIndex++;
		}
		if (7 == alarmDayIndex) {
			alarmDayIndex = 0;
		}
		if (!mAlarmDayItemInserted) {
			// Show the alarm days field to select the day.
			mAlarmDayItem = new HbDataFormModelItem(
					HbDataFormModelItem::ComboBoxItem,
					QString(hbTrId("txt_clk_setlabel_day")));
			mAlarmEditorForm->addConnection(
								mAlarmDayItem, SIGNAL(currentIndexChanged(int)),
								this, SLOT(handleDayChanged(int)));

			// Add the alarm days beginning from the start of week.
			QStringList alarmDays;
			sortAlarmDaysList(alarmDays);
			alarmDayIndex -= mStartOfWeek;

			if(0 > alarmDayIndex){
				alarmDayIndex += KDaysInWeek;
			}
			mAlarmDayItem->setContentWidgetData("items", alarmDays);
			mAlarmDayItem->setContentWidgetData(
					"currentIndex",
					alarmDayIndex);

			int insertIndex = mAlarmEditorModel
					->indexFromItem(mAlarmOccurenceItem).row() + 1;
			mAlarmEditorModel->insertDataFormItem(insertIndex, mAlarmDayItem);

			mAlarmDayItemInserted = true;
		}
	} else {
		// Check if the Alarm days item is added.
		// Remove if already added.
		if (mAlarmDayItemInserted) {
			mAlarmEditorForm->removeConnection(
								mAlarmDayItem, SIGNAL(currentIndexChanged(int)),
								this, SLOT(handleDayChanged(int)));
			mAlarmEditorModel->removeItem(mAlarmDayItem);
			mAlarmDayItemInserted = false;
		}
	}
	OstTraceFunctionExit0( DUP1_CLOCKALARMEDITOR_HANDLEOCCURENCECHANGED_EXIT );
}

/*!
    Called when `OK' is pressed on the Dialog.
 */
void ClockAlarmEditor::handleOkAction()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_HANDLEOKACTION_ENTRY );
	// Get the time from the time picker.
	QTime newAlarmTime = static_cast<HbDateTimePicker *> (
			mTimePickerDialog->contentWidget())->time();

	// Update the selected time value.
	mAlarmTimeItem->setContentWidgetData("text",newAlarmTime.toString(mTimeFormat));

	if (mOldAlarmTime != newAlarmTime && mIsQuickAlarm) {
		handleTimeChange(newAlarmTime.toString(mTimeFormat));
	}
	
	mOldAlarmTime = newAlarmTime;
	OstTraceFunctionExit0( CLOCKALARMEDITOR_HANDLEOKACTION_EXIT );
}

/*!
    Launches datetimepicker when Alarm time button is clicked.
 */
void ClockAlarmEditor::launchTimePicker()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_LAUNCHTIMEPICKER_ENTRY );
	if (mTimePickerDialog) {
		delete mTimePickerDialog;
	}

	// Create the dialog.
	mTimePickerDialog = new HbDialog;
	mTimePickerDialog->setTimeout(HbDialog::NoTimeout);
	mTimePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mTimePickerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	// Set the heading for the dialog.
	HbLabel * timeLabel =
		new HbLabel(hbTrId("txt_tumbler_title_alarm_time"),
			mTimePickerDialog);
	mTimePickerDialog->setHeadingWidget(timeLabel);

	SettingsUtility *settingsUtil = new SettingsUtility(this);
	QStringList timeSeparator;
	int index = settingsUtil->timeFormat(timeSeparator);
	
	QString tumblerDisplayFormat =
			mTimeFormat.replace(timeSeparator.at(index), QString("."));

	// Create the tumbler.
	HbDateTimePicker *timePicker = new HbDateTimePicker(mTimePickerDialog);
	timePicker->setDisplayFormat(tumblerDisplayFormat);

	mTimePickerDialog->setContentWidget(timePicker);

	QString timeString = mAlarmTimeItem->contentWidgetData("text").toString();
	QTime time = QTime::fromString(timeString, mTimeFormat);
	timePicker->setTime(time);

	mOkAction =
		new HbAction(QString(hbTrId("txt_common_button_ok")),
			mTimePickerDialog);

	mCancelAction =
		new HbAction(QString(hbTrId("txt_common_button_cancel")),
			mTimePickerDialog);

	mTimePickerDialog->addAction(mOkAction);
	mTimePickerDialog->addAction(mCancelAction);

	mTimePickerDialog->open(this, SLOT(selectedAction(HbAction*)));
	OstTraceFunctionExit0( CLOCKALARMEDITOR_LAUNCHTIMEPICKER_EXIT );
}

/*!
	Handles the alarm sound change.
 */
void ClockAlarmEditor::handleAlarmSoundChanged(int checkedState)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_HANDLEALARMSOUNDCHANGED_ENTRY );
	Q_UNUSED(checkedState)
	mIsQuickAlarm = false;
/*	if (AlarmVolumeOff == mAlarmInfo.volumeStatus) {
		mAlarmInfo.volumeStatus = AlarmVolumeOn;
	} else {
		mAlarmInfo.volumeStatus = AlarmVolumeOff;
	}*/
	OstTraceFunctionExit0( CLOCKALARMEDITOR_HANDLEALARMSOUNDCHANGED_EXIT );
}

/*!
	Slot to handle the selected action
 */
void ClockAlarmEditor::selectedAction(HbAction *action)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_SELECTEDACTION_ENTRY );
	if (action == mOkAction) {
		handleOkAction();
	}
	OstTraceFunctionExit0( CLOCKALARMEDITOR_SELECTEDACTION_EXIT );
}

/*!
	Slot to handle the change in day item.
 */
void ClockAlarmEditor::handleDayChanged(int index)
{
	Q_UNUSED(index)
	mIsQuickAlarm = false;
}

/*!
	Slot to handle the change in description.
 */
void ClockAlarmEditor::handleDescriptionChanged(const QString &text)
{
	Q_UNUSED(text)
	mIsQuickAlarm = false;
}

/*!
	Slot to handle the open event of vkb.
 */
void ClockAlarmEditor::handleKeypadOpen()
{
	QModelIndex descriptionItemIndex =
			mAlarmEditorModel->indexFromItem(mAlarmDescription); 
	HbDataFormViewItem *viewItem = static_cast<HbDataFormViewItem *> (
			mAlarmEditorForm->itemByIndex(descriptionItemIndex));
	if (viewItem) {
		HbLineEdit *editor = static_cast<HbLineEdit *>(
				viewItem->dataItemContentWidget());
		int temp = editor->text().length();
		bool flag = editor->hasSelectedText();
		if (!flag) {
			editor->setSelection(0, temp);
		}
	}
}

/*!
	Slot to handle the close event of vkb.
 */
void ClockAlarmEditor::handleKeypadClosed()
{
	QModelIndex descriptionItemIndex =
			mAlarmEditorModel->indexFromItem(mAlarmDescription); 
	HbDataFormViewItem *viewItem = static_cast<HbDataFormViewItem *> (
			mAlarmEditorForm->itemByIndex(descriptionItemIndex));
	if (viewItem) {
		HbLineEdit *editor = static_cast<HbLineEdit *>(
				viewItem->dataItemContentWidget());
		
		editor->setSelection(0, 0);
		if (editor->text().isEmpty()) {
	    	mAlarmDescription->setContentWidgetData(
	    			"text", hbTrId("txt_clk_formlabel_val_alarm"));
        }

	}
}

/*!
	Initialises the Model & sets for the AlarmEditorForm.
 */
void ClockAlarmEditor::initModel()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_INITMODEL_ENTRY );
	// Remove the model.
	if (mAlarmEditorForm->model()) {
		delete mAlarmEditorForm->model();
		mAlarmEditorForm->setModel(0);
	}

	// Create a model and set it.
	mAlarmEditorModel = new HbDataFormModel(mAlarmEditorForm);
	// Populates the model items.
	populateModelItems();
	// Set the model.
	mAlarmEditorForm->setModel(mAlarmEditorModel);
	OstTraceFunctionExit0( CLOCKALARMEDITOR_INITMODEL_EXIT );
}

/*!
	Populates the items for the alarm editor.
 */
void ClockAlarmEditor::populateModelItems()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_POPULATEMODELITEMS_ENTRY );
	// Time.
	// Creates a data form model item to set time.
	mAlarmTimeItem = mAlarmEditorModel->appendDataFormItem(
			HbDataFormModelItem::CustomItemBase,
			QString(hbTrId("txt_clk_setlabel_time")),
			mAlarmEditorModel->invisibleRootItem());
	mAlarmTimeItem->setContentWidgetData(
			"text", mAlarmInfo.nextDueTime.toString(mTimeFormat));
	mAlarmTimeItem->setContentWidgetData("objectName", "alarmTime");
	mAlarmEditorForm->addConnection(
			mAlarmTimeItem, SIGNAL(pressed()),
			this, SLOT(launchTimePicker()));

	// Occurence.
	mAlarmOccurenceItem = mAlarmEditorModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem,
			QString(hbTrId("txt_clk_setlabel_occurence")),
			mAlarmEditorModel->invisibleRootItem());
	QStringList repeatTypes;
	repeatTypes << hbTrId("txt_clk_setlabel_val_once")
			<< hbTrId("txt_clk_setlabel_val_repeat_daily")
			<< hbTrId("txt_clk_setlabel_val_repeat_weekly")
			<< hbTrId("txt_clk_setlabel_val_repeat_on_workdays");
	mAlarmOccurenceItem->setContentWidgetData("items", repeatTypes);
	mAlarmOccurenceItem->setContentWidgetData(
			"objectName", "alarmOccurence");
	mAlarmEditorForm->addConnection(
			mAlarmOccurenceItem, SIGNAL(currentIndexChanged(int)),
			this, SLOT(handleOccurenceChanged(int)));

	// Get the repeat type of alarm and set the value accordingly.
	int repeatItemIndex = mAlarmInfo.repeatType;
	mAlarmOccurenceItem->setContentWidgetData("currentIndex", repeatItemIndex);

	// Day.
	// This item is inserted only if the repeat type is
	// "once" or "repeat weekly".
	if (Once == repeatItemIndex || Weekly == repeatItemIndex) {
		if (!mAlarmDayItemInserted) {
			// Show the alarm days item.
			mAlarmDayItem = mAlarmEditorModel->appendDataFormItem(
					HbDataFormModelItem::ComboBoxItem,
					QString(hbTrId("txt_clk_setlabel_day")),
					mAlarmEditorModel->invisibleRootItem());
			mAlarmEditorForm->addConnection(
					mAlarmDayItem, SIGNAL(currentIndexChanged(int)),
					this, SLOT(handleDayChanged(int)));

			// Add the alarm days beginning from the start of week.
			QStringList alarmDays;
			sortAlarmDaysList(alarmDays);

			int currentIndex = mAlarmInfo.alarmDateTime.dayOfWeek() - 1;
			currentIndex -= mStartOfWeek;
			if(0 > currentIndex){
			   currentIndex += KDaysInWeek;
			}
			mAlarmDayItem->setContentWidgetData("items", alarmDays);
			mAlarmDayItem->setContentWidgetData(
					"currentIndex",currentIndex);
			mAlarmDayItem->setContentWidgetData("objectName", "alarmDay");
			mAlarmDayItemInserted = true;
		}/* else { TODO: check and remove this else block.
			HbDataFormModelItem *dayItem = formItem(2);
			dayItem->setData(mAlarmInfo.alarmDateTime.dayOfWeek() - 1,
			                 "selected");
			mDayInserted = false;
		}*/
	}


	// Alarm sound.
	mAlarmSoundItem = mAlarmEditorModel->appendDataFormItem(
			HbDataFormModelItem::CheckBoxItem,
			QString(hbTrId("")),
			mAlarmEditorModel->invisibleRootItem());

	mAlarmSoundItem->setContentWidgetData(
			"text", QString(hbTrId("txt_clk_setlabel_alarm_sound")));

	if (AlarmVolumeOn == mAlarmInfo.volumeStatus) {
		mAlarmSoundItem->setContentWidgetData("checkState",Qt::Checked);
	}
	mAlarmSoundItem->setContentWidgetData("objectName", "alarmSound");
	mAlarmEditorForm->addConnection(
			mAlarmSoundItem, SIGNAL(stateChanged(int)),
			this,SLOT(handleAlarmSoundChanged(int)));

	// Description.
	mAlarmDescription = mAlarmEditorModel->appendDataFormItem(
			HbDataFormModelItem::TextItem,
			QString(hbTrId("txt_clk_formlabel_description")),
			mAlarmEditorModel->invisibleRootItem());
	mAlarmDescription->setContentWidgetData("text", mAlarmInfo.alarmDesc);
	mAlarmDescription->setContentWidgetData("objectName", "alaramDescription");
	mAlarmEditorForm->addConnection(
			mAlarmDescription, SIGNAL(textChanged (const QString &)),
			this, SLOT(handleDescriptionChanged(const QString &)));

	OstTraceFunctionExit0( CLOCKALARMEDITOR_POPULATEMODELITEMS_EXIT );
}

/*!
	Create the alarm info structure with desired values.
	If new alarm: fill with default values
	else: fill with the values of the alram being editted.
 */
void ClockAlarmEditor::initAlarmInfo()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_INITALARMINFO_ENTRY );
	// Fill default values for alarmInfo.
	mAlarmInfo.nextDueTime = getPreviousAlarmTime();
	mAlarmInfo.alarmDesc =
			hbTrId("txt_clk_formlabel_val_alarm");
	mAlarmInfo.repeatType = Once;
	mAlarmInfo.alarmDateTime = QDate::currentDate();

	// Decide the default day.
	QTime alarmTime = QTime::fromString(
			mAlarmInfo.nextDueTime.toString(mTimeFormat), mTimeFormat);
	QTime homeTime = QTime::fromString(
			QTime::currentTime().toString(mTimeFormat), mTimeFormat);
	if (homeTime > alarmTime) {
		mAlarmInfo.alarmDateTime = mAlarmInfo.alarmDateTime.addDays(1);
	}

	if (!mAlarmId) {
		// Set the alarm volume On by default for new alarms..
		mAlarmInfo.volumeStatus = AlarmVolumeOn;
	}

	// If editor state is a reset alarm, then its an already existing alarm
	// get the alarm details and fill it in mAlarmInfo.
	if (mAlarmId) {
		mAlarmClient.getAlarmInfo(mAlarmId, mAlarmInfo);
	}
	
	// Set the value for previous alarm time.
	mOldAlarmTime = mAlarmInfo.nextDueTime;
	
	OstTraceFunctionExit0( CLOCKALARMEDITOR_INITALARMINFO_EXIT );
}

/*!
	Sets the alarm to the alarm server.
 */
void ClockAlarmEditor::setAlarm(
		QString timeInfo, QString descInfo, int repeatType, int alarmDayIndex)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_SETALARM_ENTRY );
	QTime alarmTime;
	AlarmInfo alarmInfo;

	// First the description.
	alarmInfo.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString(timeInfo, mTimeFormat);

	// Construct the alarm info.
	alarmInfo.origAlarmTime = alarmTime;

	// Fill the repeat type.
	alarmInfo.repeatType = (AlarmRepeatType) repeatType;

	// Fill the alarm day selected if it is a weekly alarm,
	// otherwise set to -1.
	if (Weekly == repeatType || Once == repeatType) {
		alarmInfo.alarmDay = alarmDayIndex;
	} else {
		alarmInfo.alarmDay = -1;
	}

	// Fill the alarm volume status
	int state = mAlarmSoundItem->contentWidgetData("checkState").toInt();
	if (Qt::Checked == state) {
	    alarmInfo.volumeStatus = AlarmVolumeOn;
    } else {
    	alarmInfo.volumeStatus = AlarmVolumeOff;
    }

	// Request the listener to set the alarm.
	mAlarmClient.setAlarm(alarmInfo);

	// Check if DST rule gets applied in 24hrs.
	// If so we don't display the remaining time.
	bool displayDSTRolloverNote(false);
	// Check if DST rule gets applied in 24hrs.
	displayDSTRolloverNote = mTimezoneClient->checkForDstChange(alarmInfo);

	if (displayDSTRolloverNote) {
		// display DST rollover note
		displayDSTRollOverNote(alarmInfo);
	}
	else {
		// display remaining time note
		displayRemainingTimeNote(alarmInfo);
	}

	// Save previous alarm time.
	setPreviosAlarmTime(alarmTime);
	OstTraceFunctionExit0( CLOCKALARMEDITOR_SETALARM_EXIT );
}

/*!
	Gets the previous alarm time.
	Fetches the time from cenrep. Default value is 09:00.
	If there is an error in reading the cenrep the default time
	is returned.
 */
QTime ClockAlarmEditor::getPreviousAlarmTime()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_GETPREVIOUSALARMTIME_ENTRY );
	QTime previousTime(9,0,0);

	// Read the initial values from the cenrep.
	QVariant value = mSettingsManager->readItemValue(*mPreviosAlarmTime);

	bool ok(false);
	int alarmTimeMinutes = value.toInt(&ok);

	if (ok) {
		previousTime.setHMS(
				alarmTimeMinutes / 60, alarmTimeMinutes % 60, 0);

		OstTraceFunctionExit0( CLOCKALARMEDITOR_GETPREVIOUSALARMTIME_EXIT );
		return previousTime;
	}

	OstTraceFunctionExit0( DUP1_CLOCKALARMEDITOR_GETPREVIOUSALARMTIME_EXIT );
	return previousTime;
}

/*!
	Saves the previous alarm time in cenrep.
 */
void ClockAlarmEditor::setPreviosAlarmTime(QTime time)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_SETPREVIOSALARMTIME_ENTRY );
	int alarmTimeMinutes = (time.hour() * 60) + (time.minute());

	// Write the value to cenrep.
	mSettingsManager->writeItemValue(*mPreviosAlarmTime, alarmTimeMinutes);
	OstTraceFunctionExit0( CLOCKALARMEDITOR_SETPREVIOSALARMTIME_EXIT );
}

void ClockAlarmEditor::closeAlarmEditor()
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_CLOSEALARMEDITOR_ENTRY );
	// Remove the alarm editor.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->removeView(mAlarmEditorView);
	deleteLater();
	OstTraceFunctionExit0( CLOCKALARMEDITOR_CLOSEALARMEDITOR_EXIT );
}

/*!
	Displays DST rollover note

	\param alarmInfo reference to alarm info
 */
void ClockAlarmEditor::displayDSTRollOverNote(AlarmInfo& alarmInfo)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_DISPLAYDSTROLLOVERNOTE_ENTRY );

	QString alarmTime = alarmInfo.nextDueTime.toString(mTimeFormat);
	QString displayText = HbParameterLengthLimiter(hbTrId("txt_clock_dpopinfo_alarm_will_occur_at_1_after_au"))
			              .arg(alarmTime);
	// show the note
	launchDialog(displayText);
	OstTraceFunctionExit0( CLOCKALARMEDITOR_DISPLAYDSTROLLOVERNOTE_EXIT );
}

/*!
	Displays remaining time note after saving the alarm

	\param alarmInfo reference to alarm info
 */
void ClockAlarmEditor::displayRemainingTimeNote(AlarmInfo& alarmInfo)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_DISPLAYREMAININGTIMENOTE_ENTRY );
	QDateTime currentDateTime= QDateTime::currentDateTime();

	QTime alarmTime = alarmInfo.origAlarmTime;
	QDate alarmDate = alarmInfo.alarmDateTime;
	QDateTime alarmDateTime(alarmDate,alarmTime);

	QString alarmTimeText = alarmTime.toString(mTimeFormat);
	bool oneDayDifference( false );

	if (currentDateTime<alarmDateTime) {
		QDateTime newCurrentDateTime = currentDateTime.addDays(1);
		oneDayDifference = (newCurrentDateTime < alarmDateTime)? true : false;
	}

	int alarmDay(alarmInfo.alarmDay);
	QString dayText;
	getDayText(alarmDay+1,dayText);

	QString displayText;
	QString displayFormattedText;
	bool showDisplayText(false);
	// Choose the appropriate Repeat type.
	switch ( alarmInfo.repeatType ) {
		case Once:{
			if( oneDayDifference ){
				displayText = HbParameterLengthLimiter(hbTrId("txt_clock_note_alarm_occurs_once_only_on_next_1_a"))
						    		 .arg(dayText).arg(alarmTimeText);
				showDisplayText = true;
			}
		}
		break;
		case Daily:	{
			displayText = HbParameterLengthLimiter(hbTrId("txt_clock_dpopinfo_alarm_occurs_every_day_at_1"))
					    		 .arg(alarmTimeText);
		}
		break;
		case Weekly:{
			displayText = HbParameterLengthLimiter(hbTrId("txt_clock_note_alarm_occurs_every_week_on_1_at_2"))
					    		  .arg(dayText).arg(alarmTimeText);
		}
		break;
		case Workday:{
			displayText = HbParameterLengthLimiter(hbTrId("txt_clock_note_alarm_occurs_workdays_at_1"))
					    		 .arg(alarmTimeText);
		}
		break;
		default:
			break;
	}
	if (Enabled == alarmInfo.alarmStatus && !(displayText.isEmpty())) {
		showDisplayText = true;
	}

	// Dislpay the second note.
	if (!oneDayDifference) {
		QDateTime alarmDateTime(alarmInfo.alarmDateTime,alarmInfo.nextDueTime);
		int remainingSeconds;
		if (alarmDateTime<currentDateTime) {
			QDateTime newAlmDateTime = alarmDateTime.addDays(1);
//			remainingSeconds = currentDateTime.secsTo(newAlmDateTime);
			// TODO getRemainingSeconds is used since secsTo api is not giving
			// proper seconds.Once the issue is fixed,this method can be removed
			remainingSeconds = getRemainingSeconds(newAlmDateTime);
		}
		else {
//			remainingSeconds = currentDateTime.secsTo(alarmDateTime);
			// TODO getRemainingSeconds is used since secsTo api is not giving
			// proper seconds.Once the issue is fixed,this method can be removed
			remainingSeconds = getRemainingSeconds(alarmDateTime);
		}

		int remainingMinutes = qAbs(remainingSeconds)/KOneMinuteInSecons;
		int remainingHours = remainingMinutes/KOneHourInMinute;
		remainingMinutes -= remainingHours * KOneHourInMinute;

		QString hourText;
		QString minutesText;
		QString remainingTimeText;
		// Alarm is with in 1 day. Choose the appropriate strings
		// to be displayed
		switch( remainingHours ) {
			case KOneHour:{
				if ( KOneMinute == remainingMinutes ) {
					remainingTimeText =
					HbParameterLengthLimiter(hbTrId("txt_clock_dpopinfo_time_to_alarm_1_hr_and_2_min"))
					.arg(hourText.setNum(remainingHours)).arg(minutesText.setNum(remainingMinutes));
				}
				else {
					remainingTimeText =
						HbParameterLengthLimiter(hbTrId("txt_clock_dpopinfo_time_to_alarm_1_hr_and_2_mins"))
						.arg(hourText.setNum(remainingHours)).arg(minutesText.setNum(remainingMinutes));
				}
			}
			break;
			default:{
				if ( KOneMinute == remainingMinutes ) {
					remainingTimeText =
						HbParameterLengthLimiter(hbTrId("txt_clock_dpopinfo_time_to_alarm_1_hrs_and_2_min"))
						.arg(hourText.setNum(remainingHours)).arg(minutesText.setNum(remainingMinutes));
				}
				else {
					remainingTimeText =
						HbParameterLengthLimiter(hbTrId("txt_clock_dpopinfo_time_to_alarm_1_hrs_and_2_mins"))
						.arg(hourText.setNum(remainingHours)).arg(minutesText.setNum(remainingMinutes));
				}
			}
			break;
		}

		if ( Enabled == alarmInfo.alarmStatus
				&& 	!(remainingTimeText.isEmpty())) {

			if (!(displayText.isEmpty()) ) {
				launchDialog(displayText, remainingTimeText);
				showDisplayText = false;
			}
			else {
				launchDialog(remainingTimeText);
			}
		}
	}
	if (showDisplayText) {
		launchDialog(displayText);
	}
	OstTraceFunctionExit0( CLOCKALARMEDITOR_DISPLAYREMAININGTIMENOTE_EXIT );
}

/*!
	 gets day text for the week day

	 \param alarmDay for which the day text is required
	 \param dayText week day's text in QString
 */
void ClockAlarmEditor::getDayText(int alarmDay,QString& dayText)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_GETDAYTEXT_ENTRY );
	QLocale qLocale;
	switch(alarmDay) {
		case 1:dayText.append(qLocale.dayName(1));
		break;
		case 2:dayText.append(qLocale.dayName(2));
		break;
		case 3:dayText.append(qLocale.dayName(3));
		break;
		case 4:dayText.append(qLocale.dayName(4));
		break;
		case 5:dayText.append(qLocale.dayName(5));
		break;
		case 6:dayText.append(qLocale.dayName(6));
		break;
		case 7:dayText.append(qLocale.dayName(7));
		break;
		default:
			break;
	}
OstTraceFunctionExit0( CLOCKALARMEDITOR_GETDAYTEXT_EXIT );
}
/*!
	gets remaining seconds to the alarm time from current date time

	\param alarmDateTime reference to the alarm date time
 */
int ClockAlarmEditor::getRemainingSeconds(QDateTime& alarmDateTime)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_GETREMAININGSECONDS_ENTRY );
	QDateTime currentDateTime = QDateTime::currentDateTime();

	int currentDateTimeSec =
			(currentDateTime.time().hour() * KOneHourInMinute * KOneMinuteInSecons)
			+ (currentDateTime.time().minute() * KOneMinuteInSecons)
			+ currentDateTime.time().second();

	int alarmDateTimeSec =
			(alarmDateTime.time().hour() * KOneHourInMinute * KOneMinuteInSecons)
			+ alarmDateTime.time().minute() * KOneMinuteInSecons
			+ alarmDateTime.time().second();

	int dayInterval =
			alarmDateTime.date().day() - currentDateTime.date().day();

	if (dayInterval) {
		alarmDateTimeSec += (dayInterval * KSecondsInOneDay);
	}

	int remainingSeconds = alarmDateTimeSec - currentDateTimeSec;
	OstTraceFunctionExit0( CLOCKALARMEDITOR_GETREMAININGSECONDS_EXIT );
	return remainingSeconds;
}

/*!
	Launches the soft notification.
 */
void ClockAlarmEditor::launchDialog(QString title, QString text)
{
	OstTraceFunctionEntry0( CLOCKALARMEDITOR_LAUNCHDIALOG_ENTRY );
	HbNotificationDialog *notificationDialog = new HbNotificationDialog();
	notificationDialog->setTitle(title);

	if (!text.isNull()) {
		notificationDialog->setText(text);
	}

	notificationDialog->setTimeout(HbPopup::ConfirmationNoteTimeout);
	notificationDialog->show();
    OstTraceFunctionExit0( CLOCKALARMEDITOR_LAUNCHDIALOG_EXIT );
}

/*!
    Sorts and appends alarm days on the basis of start of week.
 */
void ClockAlarmEditor::sortAlarmDaysList(QStringList& alarmDaysList)

{
    OstTraceFunctionEntry0( CLOCKALARMEDITOR_SORTALARMDAYSLIST_ENTRY );
    for(int index=(mStartOfWeek + 1),j=KDaysInWeek;j!=0 ;index++,j--){
        if(index > KDaysInWeek){
           index = index - KDaysInWeek;
        }
        QString alarmDayText;
        getDayText(index, alarmDayText );
        alarmDaysList.append(alarmDayText);
    }
    OstTraceFunctionExit0( CLOCKALARMEDITOR_SORTALARMDAYSLIST_EXIT );
}
// End of file	--Don't remove this.
