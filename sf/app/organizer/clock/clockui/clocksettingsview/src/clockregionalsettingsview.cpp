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
* Definition file for class ClockRegionalSettingsView.
*
*/

// System includes
#include <HbView>
#include <HbAction>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbExtendedLocale>
#include <HbDataFormModelItem>
#include <HbDataFormViewItem>
#include <HbMainWindow>
#include <HbInstance>
#include <HbPushButton>
#include <HbListWidget>
#include <HbComboBox>
#include <HbListWidgetItem>
#include <HbTranslator>
#include <QLocale>

// User includes
#include "clockregionalsettingsview.h"
#include "clocksettingsdocloader.h"
#include "clocksettingsdefines.h"
#include "settingsdatatypes.h"
#include "settingscustomitem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockregionalsettingsviewTraces.h"
#endif


/*!
	\class ClockRegionalSettingsView

	The view for regional setting items in clock applications settings.
 */

/*!
	Default constructor.
 */
ClockRegionalSettingsView::ClockRegionalSettingsView(QObject *parent, bool launchedByClock)
:QObject(parent),
 mView(0),
 mLoader(0),
 mLauncedByClock(launchedByClock)
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_CLOCKREGIONALSETTINGSVIEW_ENTRY );
	// Construct the document loader.
	mLoader = new ClockSettingsDocLoader;
	
	// Construct the settignsutility.
	mSettingsUtility = new SettingsUtility();
	
	// Load the translation file and install the editor specific translator
    mTranslator = new HbTranslator("clocksettingsview");
    mTranslator->loadCommon();
    
	// Create the custom prototype.
	mCustomPrototype = new SettingsCustomItem();
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_CLOCKREGIONALSETTINGSVIEW_EXIT );
}

/*!
	Destructor.
 */
ClockRegionalSettingsView::~ClockRegionalSettingsView()
{
	OstTraceFunctionEntry0( DUP1_CLOCKREGIONALSETTINGSVIEW_CLOCKREGIONALSETTINGSVIEW_ENTRY );
	if (mLoader) {
		delete mLoader;
		mLoader = 0;
	}
	
	if (mSettingsUtility) {
        delete mSettingsUtility;
        mSettingsUtility = 0;
	}
	if(mFormModel){
		delete mFormModel;
	}
// Remove the translator
    if (mTranslator) {
        delete mTranslator;
        mTranslator = 0;
    }
    OstTraceFunctionExit0( DUP1_CLOCKREGIONALSETTINGSVIEW_CLOCKREGIONALSETTINGSVIEW_EXIT );
}

/*!
	Shows the regional settings view.
 */
void ClockRegionalSettingsView::showView()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_SHOWVIEW_ENTRY );
	bool success;

	// Load the application xml.
	mLoader->load(CLOCK_REG_SETTINGS_VIEW_DOCML, &success);
	if (!success) {
		qFatal("Unable to load the docml.");
	}

	// Find the main view.
	mView = static_cast<HbView *> (
			mLoader->findWidget(CLOCK_REG_SETTINGS_VIEW));
	if (!mView) {
		qFatal("Unable to find view");
	}

	// Find the data form.
	mForm = static_cast<HbDataForm *> (
			mLoader->findWidget(CLOCK_REG_SETTINGS_DATA_FORM));
	if (!mForm) {
		qFatal("Unable to find the form");
	}
	mForm->setItemPixmapCacheEnabled(true);
	connect(
			mForm, SIGNAL(itemShown(QModelIndex)),
			this, SLOT(handleItemDisplayed(QModelIndex)));
	

	QList <HbAbstractViewItem*> prototypes = mForm->itemPrototypes();
	prototypes.append(mCustomPrototype);
	mForm->setItemPrototypes(prototypes);
	mCustomPrototype->setWeekdaysList(weekdayList());

	// Create the form model.
	createModel();

	// Set the view as the current view.
	if(!mLauncedByClock) {
	    mView->setTitle(hbTrId("txt_clock_title_control_panel"));
	}
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->addView(mView);
	window->setCurrentView(mView);

	// Add the back softkey.
	HbAction *backAction = new HbAction(Hb::BackNaviAction);
	mView->setNavigationAction(backAction);
	connect(
			backAction, SIGNAL(triggered()),
			this, SLOT(handleBackAction()));
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_SHOWVIEW_EXIT );
}

/*!
	The view is removed from main window and a deleteLater is called on `this'.
 */
void ClockRegionalSettingsView::handleBackAction()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_HANDLEBACKACTION_ENTRY );
	HbExtendedLocale locale = HbExtendedLocale::system();
	HbExtendedLocale::WeekDay startOfWeekIndex =
				HbExtendedLocale::system().startOfWeek();
	// Save workdays settings.
	QItemSelectionModel *model = 0;
	model = mWorkdaysItem->selectionModel();
	QModelIndexList selectedModelIndex = model->selectedIndexes();
	int count = selectedModelIndex.count();
	QModelIndex index;
	for(int i = 0 ; i < count ; i++){
		index = selectedModelIndex[i];
	}
	
	QString workdayString("0000000");
	for (int i = 0; i < workdayString.count(); ++i) {
		if (model->isSelected(model->model()->index(i, 0))) {
			workdayString.replace(i, 1, '1');
		}
	}
	
	QString workdaysSetting = workdayString;
	int max = workdayString.size() - 1;
	for (int index = max - startOfWeekIndex, i = 0; i <= max; i++) {
		QChar ch = workdayString.at(index);
		workdaysSetting.replace(i, 1, ch);
		if (index == 0) {
			index = max;
		} else {
			index--;
		}
	}
	locale.setWorkDays(workdaysSetting);
	
	HbMainWindow *window = hbInstance->allMainWindows().first();
	// Cleanup.
	window->removeView(mView);
	deleteLater();
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_HANDLEBACKACTION_EXIT );
}

/*!
	Called when each of the form items are displayed. Using this slot, we
	connect to SIGNALS of the items that have been added to the form.

	\param index QModelIndex of the row that was just displayed.
 */
void ClockRegionalSettingsView::handleItemDisplayed(const QModelIndex &index)
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_HANDLEITEMDISPLAYED_ENTRY );
	if (!index.isValid()) {
		OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_HANDLEITEMDISPLAYED_EXIT );
		return;
	}

	HbDataFormViewItem *item =
			static_cast<HbDataFormViewItem*>(mForm->itemByIndex(index));
	HbWidget *widget = item->dataItemContentWidget();

	switch (index.row()) {
		case 0:

			break;
		case 1:

			break;

		case 2:

			break;

		case 3:

			break;

		case 4:
			mWorkdaysItem = static_cast<HbListWidget *> (widget);
		case 5:
			mStartOfWeekItem = static_cast<HbComboBox *> (widget);
			break;
		default:
			break;
	}
	OstTraceFunctionExit0( DUP1_CLOCKREGIONALSETTINGSVIEW_HANDLEITEMDISPLAYED_EXIT );
}

/*!
	Called when the time format toggle item is clicked. Here we update the
	value in the locale.
 */
void ClockRegionalSettingsView::handleTimeFormatChange()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_HANDLETIMEFORMATCHANGE_ENTRY );
	mSettingsUtility->setTimeFormat(
			mTimeFormatItem->contentWidgetData("text").toString());
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_HANDLETIMEFORMATCHANGE_EXIT );
}

/*!
	Called when the time separator toggle item is clicked. Here we update the
	value in the locale.
 */
void ClockRegionalSettingsView::handleTimeSeparatorChange()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_HANDLETIMESEPARATORCHANGE_ENTRY );
	mSettingsUtility->setTimeSeparator(
			mTimeSeparatorItem->contentWidgetData("text").toString());
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_HANDLETIMESEPARATORCHANGE_EXIT );
}

/*!
 */
void ClockRegionalSettingsView::handleDateFormatChange(QString text)
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_HANDLEDATEFORMATCHANGE_ENTRY );
	mSettingsUtility->setDateFormat(text);
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_HANDLEDATEFORMATCHANGE_EXIT );
}

/*!
    Called when the date separator item is changed. Here we update the
    value in the locale.
 */
void ClockRegionalSettingsView::handleDateSeparatorChange(QString text)
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_HANDLEDATESEPARATORCHANGE_ENTRY );
	mSettingsUtility->setDateSeparator(text);
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_HANDLEDATESEPARATORCHANGE_EXIT );
}

/*!
	This slot is called any item in the data form is changed.
 */
void ClockRegionalSettingsView::handleDataChanged(
		const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_HANDLEDATACHANGED_ENTRY );
	Q_UNUSED(bottomRight)


	switch (topLeft.row()) {
		case 5:
		{
		// The Start of week item.
		if (mStartOfWeekItem != 0)
		    {
			int index = mStartOfWeekItem->currentIndex();
			mSettingsUtility->setStartOfWeek(index);
			//update the week days
			updateWeekDays();
		    }
		}
		break;

		default:
		break;
	}
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_HANDLEDATACHANGED_EXIT );
}

/*!
	Here we create the form model.
 */
void ClockRegionalSettingsView::createModel()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_CREATEMODEL_ENTRY );
	if (mForm->model()) {
		delete mForm->model();
		mForm->setModel(0);
	}

	mFormModel = new HbDataFormModel(this);
	
	// Populate the form model.
	populateFormModel();
	
	mForm->setModel(mFormModel);

	connect(
			mFormModel,
			SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
			this,
			SLOT(handleDataChanged(const QModelIndex, const QModelIndex)));
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_CREATEMODEL_EXIT );
}

/*!
	Function in which we populate the form model.
 */
void ClockRegionalSettingsView::populateFormModel()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_POPULATEFORMMODEL_ENTRY );
	if (!mFormModel) {
		createModel();
	}

	// Get the locale.
	HbExtendedLocale locale  = HbExtendedLocale::system();

	// Time format item.
	 mTimeFormatItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ToggleValueItem,
			hbTrId("txt_clock_setlabel_time_format"));
	int index = mSettingsUtility->timeFormat(mTimeFormatStringList);
	if (0 == index) {
		mTimeFormatItem->setContentWidgetData("text", mTimeFormatStringList[0]);
		mTimeFormatItem->setContentWidgetData("additionalText", mTimeFormatStringList[1]);
	} else {
		mTimeFormatItem->setContentWidgetData("text", mTimeFormatStringList[1]);
		mTimeFormatItem->setContentWidgetData("additionalText", mTimeFormatStringList[0]);
	}
	mTimeFormatItem->setContentWidgetData("objectName", "timeFormat");
	mForm->addConnection(
			mTimeFormatItem, SIGNAL(clicked()),
			this, SLOT(handleTimeFormatChange()));

	// Time separator item.
	mTimeSeparatorItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ToggleValueItem,
			hbTrId("txt_clock_setlabel_time_separator"));
	index = mSettingsUtility->timeSeparator(mTimeSeparatorStringList);
	if (0 == index) {
		mTimeSeparatorItem->setContentWidgetData("text", mTimeSeparatorStringList[0]);
		mTimeSeparatorItem->setContentWidgetData(
				"additionalText", mTimeSeparatorStringList[1]);
	} else {
		mTimeSeparatorItem->setContentWidgetData("text", mTimeSeparatorStringList[1]);
		mTimeSeparatorItem->setContentWidgetData(
				"additionalText", mTimeSeparatorStringList[0]);
	}
	mTimeSeparatorItem->setContentWidgetData("objectName", "timeSeparator");
	mForm->addConnection(
			mTimeSeparatorItem, SIGNAL(clicked()),
			this, SLOT(handleTimeSeparatorChange()));

	// Date format.
	mDateFormatItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem,
			hbTrId("txt_clock_setlabel_date_format"));
	index = mSettingsUtility->dateFormat(mDateFormatStringList);

	mDateFormatItem->setContentWidgetData("items", mDateFormatStringList);
	mDateFormatItem->setContentWidgetData("currentIndex",index);
	mDateFormatItem->setContentWidgetData("objectName", "dateFormat");
	mForm->addConnection(
			mDateFormatItem, SIGNAL(currentIndexChanged(QString)),
	 		this, SLOT(handleDateFormatChange(QString)));

	// Date separator.
	mDateSeparatorItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem,
			hbTrId("txt_clock_setlabel_date_separator"));
	index = mSettingsUtility->dateSeparator(mDateSeparatorStringList);

	mDateSeparatorItem->setContentWidgetData("items", mDateSeparatorStringList);
	mDateSeparatorItem->setContentWidgetData("currentIndex",index);
	mDateSeparatorItem->setContentWidgetData("objectName", "dateSeparator");
	mForm->addConnection(
			mDateSeparatorItem, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(handleDateSeparatorChange(QString)));

	// Workdays.
	HbDataFormModelItem *item = 0;
	// Create the weekday list based on start of week.
	QStringList weekdaysList;
	QLocale qLocale;
	weekdaysList
			<< qLocale.dayName(1)
			<< qLocale.dayName(2)
			<< qLocale.dayName(3)
			<< qLocale.dayName(4)
			<< qLocale.dayName(5)
			<< qLocale.dayName(6)
			<< qLocale.dayName(7);
	
	HbDataFormModelItem::DataItemType workdaysItemType =
			static_cast<HbDataFormModelItem::DataItemType>
			(HbDataFormModelItem::CustomItemBase + WorkdaysItemOffset);
	item = new HbDataFormModelItem(workdaysItemType,
			hbTrId("txt_clock_setlabel_workdays"));
	mFormModel->appendDataFormItem(item);
	item->setContentWidgetData("objectName", "workdays");

	// Start of week item.
	item = 0;
	item = mFormModel->appendDataFormItem(HbDataFormModelItem::ComboBoxItem,
		hbTrId("txt_clock_setlabel_week_starts_on"));
	HbExtendedLocale::WeekDay startOfWeek = locale.startOfWeek();
	item->setContentWidgetData("items", weekdaysList);
	item->setContentWidgetData("currentIndex", startOfWeek);
	item->setContentWidgetData("objectName", "startOfWeek");
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_POPULATEFORMMODEL_EXIT );
}

/*!
	Returns the weekday list ordered based on start of week.
 */

QStringList ClockRegionalSettingsView::weekdayList()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_WEEKDAYLIST_ENTRY );
	QStringList weekDays;
	QStringList daysList;
	QLocale qLocale;
	daysList
			<< qLocale.dayName(1)
			<< qLocale.dayName(2)
			<< qLocale.dayName(3)
			<< qLocale.dayName(4)
			<< qLocale.dayName(5)
			<< qLocale.dayName(6)
			<< qLocale.dayName(7);
	
	HbExtendedLocale::WeekDay startOfWeekIndex =
			HbExtendedLocale::system().startOfWeek();
	
	for (int i = 0, index = startOfWeekIndex; i < daysList.count(); ++i) {
		weekDays.append(daysList.at(index));
		if (6 == index) {
			index = 0;
		} else {
			index++;
		}
	}
	
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_WEEKDAYLIST_EXIT );
	return weekDays;
}


/*!
    update the start week on .
 */

void ClockRegionalSettingsView::updateWeekStartOn()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATEWEEKSTARTON_ENTRY );
	if (mStartOfWeekItem != 0)
	{
		HbExtendedLocale locale;
		HbExtendedLocale::WeekDay weekdDayStart = locale.startOfWeek();
		int currentDay = mStartOfWeekItem->currentIndex();
		if(currentDay == weekdDayStart )
		{
			OstTraceFunctionExit0(
					CLOCKREGIONALSETTINGSVIEW_UPDATEWEEKSTARTON_EXIT );
			return;
		}
		else
		{
			mStartOfWeekItem->setCurrentIndex(weekdDayStart);
			updateWeekDays();
		}
	}
	OstTraceFunctionExit0(
			DUP1_CLOCKREGIONALSETTINGSVIEW_UPDATEWEEKSTARTON_EXIT );
}

/*!
    update on locale changes.
 */

void ClockRegionalSettingsView::updateOnLocaleChanges()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATEONLOCALECHANGES_ENTRY );
	
	updateTimeFormatField();
	updateTimeSeparatorField();
	updateDateFormatField();
	updateDateSeparatorField();
	
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATEONLOCALECHANGES_EXIT );
}

/*!
    update the  week days .
 */
void ClockRegionalSettingsView::updateWeekDays()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATEWEEKDAYS_ENTRY );
	QStringList weekdays = weekdayList();
	QString workdays = mCustomPrototype->workdaysSetting();
	QItemSelectionModel *model = 0;
	model = mWorkdaysItem->selectionModel();

	for (int i = 0, index = workdays.size() - 1;
			i < mWorkdaysItem->count(); ++i, index--)
	{
		QString str = weekdays[i];
		mWorkdaysItem->item(i)->setText(str);

		QChar ch = workdays.at(index);
		if ( ch == QChar('0')) 
		{
			// Not a workday.
			model->select(
					model->model()->index(i,0),
					QItemSelectionModel::Deselect);
		}
		else
		{
			// Workday.
			model->select(
					model->model()->index(i,0),
					QItemSelectionModel::Select);}
	}
	OstTraceFunctionExit0( CLOCKREGIONALSETTINGSVIEW_UPDATEWEEKDAYS_EXIT );
}

/*!
    update the  time format field.
 */
void ClockRegionalSettingsView::updateTimeFormatField()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATETIMEFORMATFIELD_ENTRY );
	//for time format change
	int index = mSettingsUtility->timeFormat(mTimeFormatStringList);
	if (0 == index) {
		mTimeFormatItem->setContentWidgetData("text", mTimeFormatStringList[0]);
		mTimeFormatItem->setContentWidgetData("additionalText", mTimeFormatStringList[1]);
	} else {
		mTimeFormatItem->setContentWidgetData("text", mTimeFormatStringList[1]);
		mTimeFormatItem->setContentWidgetData("additionalText", mTimeFormatStringList[0]);
	}
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATETIMEFORMATFIELD_EXIT );
}

/*!
    update the time separator field.
 */
void ClockRegionalSettingsView::updateTimeSeparatorField()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATETIMESEPARATORFIELD_ENTRY );
	// for time separator change
	int index = mSettingsUtility->timeSeparator(mTimeSeparatorStringList);
	if (0 == index) {
		mTimeSeparatorItem->setContentWidgetData("text", mTimeSeparatorStringList[0]);
		mTimeSeparatorItem->setContentWidgetData(
				"additionalText", mTimeSeparatorStringList[1]);
	} else {
		mTimeSeparatorItem->setContentWidgetData("text", mTimeSeparatorStringList[1]);
		mTimeSeparatorItem->setContentWidgetData(
				"additionalText", mTimeSeparatorStringList[0]);
	}
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATETIMESEPARATORFIELD_EXIT );
}

/*!
    update the date format field.
 */
void ClockRegionalSettingsView::updateDateFormatField()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATEDATEFORMATFIELD_ENTRY );
	
	int index = mSettingsUtility->dateFormat(mDateFormatStringList);
	mDateFormatItem->setContentWidgetData("currentIndex",index);
	
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATEDATEFORMATFIELD_EXIT );
}

/*!
    update the date separator field.
 */
void ClockRegionalSettingsView::updateDateSeparatorField()
{
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATEDATESEPARATORFIELD_ENTRY );
	
	int index = mSettingsUtility->dateSeparator(mDateSeparatorStringList);
	mDateSeparatorItem->setContentWidgetData("currentIndex",index);
	
	OstTraceFunctionEntry0( CLOCKREGIONALSETTINGSVIEW_UPDATEDATESEPARATORFIELD_EXIT );
}

// End of file	--Don't remove this.
