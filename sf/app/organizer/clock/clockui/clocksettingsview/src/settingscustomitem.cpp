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
* Definition file for class ClockSettingsView.
*
*/

// System includes
#include <QGraphicsLinearLayout>
#include <QItemSelectionModel>
#include <HbDataFormModelItem>
#include <HbDataFormModel>
#include <HbPushButton>
#include <HbLabel>
#include <HbDateTimePicker>
#include <HbAction>
#include <HbListWidget>
#include <HbExtendedLocale>
#include <HbListWidgetItem>
#include <HbStyleLoader>

// User includes
#include "settingscustomitem.h"
#include "settingsdatatypes.h"
#include "settingsutility.h"
#include "clockcityselectionlist.h"
#include "timezoneclient.h"
#include "clockregionalsettingsview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "settingscustomitemTraces.h"
#endif


/*!
	\class SettingsCustomItem

	This class is responsible for creating custom items for clock settings.
 */

/*!
	Default Constructor.

	\param parent The parent.
 */

SettingsCustomItem::SettingsCustomItem(QGraphicsItem *parent, bool launchedByClock)
:HbDataFormViewItem(parent), mLaunchedByClock(launchedByClock)
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_SETTINGSCUSTOMITEM_ENTRY );
	// Construct the settignsutility.
	mSettingsUtility = new SettingsUtility();
	// Construct the timezone client.
	mTimezoneClient = TimezoneClient::getInstance();
	// Register for the css.
	HbStyleLoader::registerFilePath(":/style/settingscustomitem.css");
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_SETTINGSCUSTOMITEM_EXIT );
}

/*!
	Destructor.
 */
SettingsCustomItem::~SettingsCustomItem()
{
	OstTraceFunctionEntry0( DUP1_SETTINGSCUSTOMITEM_SETTINGSCUSTOMITEM_ENTRY );
	// Unregister the css.
	HbStyleLoader::unregisterFilePath(":/style/settingscustomitem.css");
	OstTraceFunctionExit0( DUP1_SETTINGSCUSTOMITEM_SETTINGSCUSTOMITEM_EXIT );
}

/*!
	\reimp
	Creates SettingsCustomItem.
 */
HbAbstractViewItem* SettingsCustomItem::createItem()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_CREATEITEM_ENTRY );
	return new SettingsCustomItem(*this);
}

/*!

	\sa
 */
bool SettingsCustomItem::canSetModelIndex(const QModelIndex &index) const
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_CANSETMODELINDEX_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>(
			index.data(HbDataFormModelItem::ItemTypeRole).toInt());

	if(itemType >= HbDataFormModelItem::CustomItemBase &&
			itemType <= (HbDataFormModelItem::CustomItemBase + 50)) {
		OstTraceFunctionExit0( SETTINGSCUSTOMITEM_CANSETMODELINDEX_EXIT );
		return true;
	} else {
		OstTraceFunctionExit0( DUP1_SETTINGSCUSTOMITEM_CANSETMODELINDEX_EXIT );
		return false;
	}

}

void SettingsCustomItem::restore()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_RESTORE_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>(
			modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

	HbDataFormModel* model =
			static_cast<HbDataFormModel*>(
			static_cast<HbAbstractViewItem*>(this)->itemView()->model());
	HbDataFormModelItem* modlItem = model->itemFromIndex(modelIndex());

	switch (itemType) {
		case (TimeItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			mTimeWidget->setProperty(
					"text", modlItem->contentWidgetData("text"));
			mTimeWidget->setProperty(
					"objectName", modlItem->contentWidgetData("objectName"));
			break;
		}
		case (DateItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			mDateWidget->setProperty(
					"text", modlItem->contentWidgetData("text"));
			mDateWidget->setProperty(
					"objectName", modlItem->contentWidgetData("objectName"));
			break;
		}
		case (PlaceItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			mPlaceWidget->setProperty(
					"text", modlItem->contentWidgetData("text"));
			mPlaceWidget->setProperty(
					"objectName", modlItem->contentWidgetData("objectName"));
			break;
		}
		case (RegionalItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			mRegSettingsWidget->setProperty(
					"text", modlItem->contentWidgetData("text"));
			mRegSettingsWidget->setProperty(
					"objectName", modlItem->contentWidgetData("objectName"));
			break;
		}
		case (50 + HbDataFormModelItem::CustomItemBase):
		{
			mWorkdaysWidget->setProperty(
					"objectName", modlItem->contentWidgetData("objectName"));
			break;
		}
		default:
		break;
	}
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_RESTORE_EXIT );
}

void SettingsCustomItem::setWeekdaysList(QStringList list)
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_SETWEEKDAYSLIST_ENTRY );
	mWeekdaysList = list;
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_SETWEEKDAYSLIST_EXIT );
}

/*!
	\reimp
	Creates custom data form item.
 */
HbWidget* SettingsCustomItem::createCustomWidget()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_CREATECUSTOMWIDGET_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
		static_cast<HbDataFormModelItem::DataItemType>(modelIndex()
		.data(HbDataFormModelItem::ItemTypeRole).toInt());

	switch (itemType) {
		case (TimeItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			setProperty("expandItem", false);
			mTimeWidget = new HbPushButton(this);
			connect(
					mTimeWidget, SIGNAL(clicked()),
					this, SLOT(launchTimePicker()));
			OstTraceFunctionExit0( SETTINGSCUSTOMITEM_CREATECUSTOMWIDGET_EXIT );
			return mTimeWidget;
		}

		case (DateItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			setProperty("expandItem", false);
			mDateWidget = new HbPushButton(this);
			connect(
					mDateWidget, SIGNAL(clicked()),
					this, SLOT(launchDatePicker()));
			OstTraceFunctionExit0( DUP1_SETTINGSCUSTOMITEM_CREATECUSTOMWIDGET_EXIT );
			return mDateWidget;
		}

		case (PlaceItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			setProperty("expandItem", false);
			mPlaceWidget = new HbPushButton(this);
			connect(
					mPlaceWidget, SIGNAL(clicked()),
					this, SLOT(launchCitySelectionList()));
			OstTraceFunctionExit0( DUP2_SETTINGSCUSTOMITEM_CREATECUSTOMWIDGET_EXIT );
			return mPlaceWidget;
		}

		case (RegionalItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			setProperty("expandItem", false);
			mRegSettingsWidget = new HbPushButton(this);
			connect(
					mRegSettingsWidget, SIGNAL(clicked()),
					this, SLOT(launchRegSettingsView()));
			OstTraceFunctionExit0( DUP3_SETTINGSCUSTOMITEM_CREATECUSTOMWIDGET_EXIT );
			return mRegSettingsWidget;
		}

		case (WorkdaysItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			// Set the property so that the custom item layout 
			// will be the same for both portrait and landscape 
			setProperty("expandItem", true);
			mWorkdaysWidget = new HbListWidget(this);
			mWorkdaysWidget->setSelectionMode(HbAbstractItemView::MultiSelection);
			mWorkdaysWidget->setScrollDirections(0);
			mWorkdaysWidget->ungrabGesture(Qt::PanGesture);
			
			QItemSelectionModel *model = 0;
			model = mWorkdaysWidget->selectionModel();
			for (int index = 0; index < mWeekdaysList.count(); ++index) {
				mWorkdaysWidget->addItem(mWeekdaysList.at(index));
			}

			QString workdaysString = workdaysSetting();
			for (int i = 0, index = workdaysString.size() - 1;
					index >= 0; ++i, index--) {
				if (workdaysString.at(index) == QChar('0')) {
					// Not a workday.
					model->select(
							model->model()->index(i,0),
							QItemSelectionModel::Deselect);
				} else {
					// Workday.
					model->select(
							model->model()->index(i,0),
							QItemSelectionModel::Select);
				}
			}

			OstTraceFunctionExit0( DUP4_SETTINGSCUSTOMITEM_CREATECUSTOMWIDGET_EXIT );
			return mWorkdaysWidget;
		}

		default:
			return 0;
	}
}

/*!
	Launches the time picker dialog.
 */
void SettingsCustomItem::launchTimePicker()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_LAUNCHTIMEPICKER_ENTRY );
	if (mTimePickerDialog) {
		delete mTimePickerDialog;
	}

	// Create the dialog.
	mTimePickerDialog = new HbDialog;
	mTimePickerDialog->setTimeout(HbDialog::NoTimeout);
	mTimePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mTimePickerDialog->setAttribute(Qt::WA_DeleteOnClose, true );

	// Set the heading for the dialog.
	HbLabel * timeLabel =
		new HbLabel(hbTrId("txt_clock_setlabel_time"), mTimePickerDialog);
	mTimePickerDialog->setHeadingWidget(timeLabel);

	QStringList timeSeparator;
	int index = mSettingsUtility->timeFormat(timeSeparator);
	QString tumblerDisplayFormat = mSettingsUtility->timeFormatString()
			.replace(timeSeparator.at(index), QString("."));

	// Create the tumbler.
	HbDateTimePicker *timePicker = new HbDateTimePicker(mTimePickerDialog);
	timePicker->setDisplayFormat(tumblerDisplayFormat);
	// Set the tumbler as the content widget.
	mTimePickerDialog->setContentWidget(timePicker);
	timePicker->setTime(QTime::currentTime());

	mOkAction = new HbAction(
			QString(hbTrId("txt_common_button_ok")), mTimePickerDialog);
	mCancelAction = new HbAction(
			QString(hbTrId("txt_common_button_cancel")), mTimePickerDialog);

	mTimePickerDialog->addAction(mOkAction);
	mTimePickerDialog->addAction(mCancelAction);

	mTimePickerDialog->open(this, SLOT(selectedAction(HbAction*)));
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_LAUNCHTIMEPICKER_EXIT );
}

/*!
	Handles the ok action of time/date picker dialog.
 */
void SettingsCustomItem::handleOkAction()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_HANDLEOKACTION_ENTRY );
	if (mTimePickerDialog) {
		// Get the time from the time picker.
		QTime newTime = static_cast<HbDateTimePicker *> (
				mTimePickerDialog->contentWidget())->time();

		QString timeFormatString = mSettingsUtility->timeFormatString();

		// If time returned by the picker is valid, then only update.
		if (newTime.isValid()) {
			// Update the display text on form item.
			mTimeWidget->setText(newTime.toString(timeFormatString));
			// Update the system time.
			mTimezoneClient->setDateTime(
					QDateTime(QDate::currentDate(), newTime));
		}
	} else if(mDatePickerDialog) {
		// Get the time from the time picker.
		QDate newDate = static_cast<HbDateTimePicker *> (
				mDatePickerDialog->contentWidget())->date();

		QString dateFormatString = mSettingsUtility->dateFormatString();

		// If date returned by the picker is valid, then only update.
		if (newDate.isValid()) {
			// Update the display text on form item.
			mDateWidget->setText(newDate.toString(dateFormatString));
			// Update the system date.
			mTimezoneClient->setDateTime(
					QDateTime(newDate, QTime::currentTime()));
		}
	}
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_HANDLEOKACTION_EXIT );
}

/*!
	Launches the date picker dialog.
 */
void SettingsCustomItem::launchDatePicker()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_LAUNCHDATEPICKER_ENTRY );
	if (mDatePickerDialog) {
		delete mDatePickerDialog;
	}

	// Create the dialog.
	mDatePickerDialog = new HbDialog;
	mDatePickerDialog->setTimeout(HbDialog::NoTimeout);
	mDatePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mDatePickerDialog->setAttribute(Qt::WA_DeleteOnClose, true );

	// Set the heading for the dialog.
	HbLabel * timeLabel =
		new HbLabel(hbTrId("txt_clock_setlabel_date"), mDatePickerDialog);
	mDatePickerDialog->setHeadingWidget(timeLabel);

	// Create the tumbler.
	HbDateTimePicker *datePicker = new HbDateTimePicker(mDatePickerDialog);
	datePicker->setDisplayFormat(mSettingsUtility->dateFormatString());
	datePicker->setMinimumDate(QDate(2010, 01, 01));
	// Set the tumbler as the content widget.
	mDatePickerDialog->setContentWidget(datePicker);
	datePicker->setDate(QDate::currentDate());

	mOkAction = new HbAction(
			QString(hbTrId("txt_common_button_ok")), mDatePickerDialog);

	mCancelAction = new HbAction(
			QString(hbTrId("txt_common_button_cancel")),mDatePickerDialog);

	mDatePickerDialog->addAction(mOkAction);
	mDatePickerDialog->addAction(mCancelAction);

	mDatePickerDialog->open(this, SLOT(selectedAction(HbAction*)));
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_LAUNCHDATEPICKER_EXIT );
}

/*!
	Launches cityselectionlist for timezone selection.
 */
void SettingsCustomItem::launchCitySelectionList()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_LAUNCHCITYSELECTIONLIST_ENTRY );
	mCitySelectionList = new ClockCitySelectionList(mTimezoneClient, this);
	connect(
			mCitySelectionList, SIGNAL(citySelected(LocationInfo)),
			this, SLOT(updatePlaceItem(LocationInfo)));

	mCitySelectionList->showCityList();
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_LAUNCHCITYSELECTIONLIST_EXIT );
}

/*!
	Launches cityselectionlist for timezone selection.

	\param info Will contain the location info selected by the user.
 */
void SettingsCustomItem::updatePlaceItem(LocationInfo info)
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_UPDATEPLACEITEM_ENTRY );
	if (-1 != info.timezoneId) {
		mTimezoneClient->setAsCurrentLocationL(info);
	}
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_UPDATEPLACEITEM_EXIT );
}

/*!
	Launches the regional settings view.
 */
void SettingsCustomItem::launchRegSettingsView()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_LAUNCHREGSETTINGSVIEW_ENTRY );
	ClockRegionalSettingsView *view =
			new ClockRegionalSettingsView(0, mLaunchedByClock);

	connect(mTimezoneClient, SIGNAL(timechanged()),
          view, SLOT(updateWeekStartOn()));
	connect(mTimezoneClient, SIGNAL(timechanged()),
	          view, SLOT(updateOnLocaleChanges()));
	
	
	view->showView();
	
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_LAUNCHREGSETTINGSVIEW_EXIT );
}

/*!
	Slot to handle the selected action.
 */
void SettingsCustomItem::selectedAction(HbAction *action)
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_SELECTEDACTION_ENTRY );
	if (action==mOkAction) {
		handleOkAction();
	}
	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_SELECTEDACTION_EXIT );
}

/*!
	Returns the workdays setting string based on start of week setting.
 */
QString SettingsCustomItem::workdaysSetting()
{
	OstTraceFunctionEntry0( SETTINGSCUSTOMITEM_WORKDAYSSETTING_ENTRY );
	HbExtendedLocale locale = HbExtendedLocale::system();
	// 0(Sun)0(sat)1(Fri)1(Thu)1(Wed)1(Tue)1(Mon)
	QString workdaysString = locale.workDays();
	QString workdays = workdaysString;
	HbExtendedLocale::WeekDay startOfWeekIndex =
				HbExtendedLocale::system().startOfWeek();
	int max = workdaysString.size() - 1;
	for (int index = max - startOfWeekIndex, i = max; i >= 0; i--) {
		QChar ch = workdaysString.at(index);
		workdays.replace(i, 1, ch);
	    if (index == 0) {
	        index = max;
        } else {
        	index--;
        }
    }
//	workdays.append('\0');

	OstTraceFunctionExit0( SETTINGSCUSTOMITEM_WORKDAYSSETTING_EXIT );
	return workdays;
}

// End of file	--Don't remove this.
