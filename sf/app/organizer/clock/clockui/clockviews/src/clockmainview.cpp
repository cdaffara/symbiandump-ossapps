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
* Definition file for class ClockMainView.
*
*/

// System includes
#include <QGraphicsItem>
#include <HbInstance>
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbMenu>
#include <HbAction>
#include <HbListView>
#include <HbNotificationDialog>
#include <HbStyleLoader>
#include <HbColorScheme>
#include <HbApplication> // hbapplication
#include <HbActivityManager> // activity manager
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <clockdomaincrkeys.h>
#include <AfActivityStorage.h>//activity

// User includes
#include "clockmainview.h"
#include "clockdocloader.h"
#include "clockappcontrollerif.h"
#include "alarmclient.h"
#include "clocksettingsview.h"
#include "clockcommon.h"
#include "clockalarmeditor.h"
#include "settingsutility.h"
#include "timezoneclient.h"
#include "clockalarmlistitemprototype.h"
#include "clockalarmlistmodel.h"
#include "clockwidget.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockmainviewTraces.h"
#endif


/*!
	\class ClockMainView

	The main view of the clock application.
 */

/*!
	Constructor.

	\param parent The parent of type QGraphicsWidget.
 */
ClockMainView::ClockMainView(QGraphicsItem *parent)
:HbView(parent),
 mAlarmList(0),
 mSelectedItem(-1),
 mIsLongTop(false),
 mIsScreenShotCapruted(false)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_CLOCKMAINVIEW_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( CLOCKMAINVIEW_CLOCKMAINVIEW_EXIT );
}

/*!
	Destructor.
 */
ClockMainView::~ClockMainView()
{
	OstTraceFunctionEntry0( DUP1_CLOCKMAINVIEW_CLOCKMAINVIEW_ENTRY );
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}
	if (mAlarmListModel) {
		delete mAlarmListModel;
		mAlarmListModel = 0;
	}
	if (mSettingsManager) {
		delete mSettingsManager;
	}
	// Delete cenrep key
	delete mClockTypeSettingsKey;
	
	HbStyleLoader::unregisterFilePath(
			":/style/clockalarmlistitemprototype.css");
	HbStyleLoader::unregisterFilePath(
			":/style/clockalarmlistitemprototype.widgetml");
	HbStyleLoader::unregisterFilePath(
			":/style/clockalarmlistitemprototype_color.css");

	OstTraceFunctionExit0( DUP1_CLOCKMAINVIEW_CLOCKMAINVIEW_EXIT );
}

/*!
	Called by the ClockViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The ClockAppController object.
	\param docLoader Pointer to ClockDocLoader object.
 */
void ClockMainView::setupView(
		ClockAppControllerIf &controllerIf, ClockDocLoader *docLoader)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_SETUPVIEW_ENTRY );
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;
    //initialize the activity 
	mActivityStorage = new AfActivityStorage(this);
	mTimezoneClient = controllerIf.timezoneClient();
	mSettingsUtility = controllerIf.settingsUtility();
	mAlarmClient = controllerIf.alarmClient();
	
	// Fetch the color group of the labels from the theme.
	mLabelColorGroup = HbColorScheme::color("qtc_view_normal");

	// Create the model.
	mAlarmListModel =  new ClockAlarmListModel(*mAppControllerIf, this);
	connect(
			mAlarmListModel, SIGNAL(changeAlarmListDisplay()),
			this, SLOT(handleAlarmListDisplay()));

	QList<AlarmInfo> alarmInfoList;
	// Get the list of pending clock alarms.
	mAlarmClient->getAlarmList(alarmInfoList);
	QList<AlarmInfo> displayInfoList;
	AlarmInfo alarmInfo;
	for (int index = 0; index < alarmInfoList.count(); ++index) {
		alarmInfo = alarmInfoList[index];
		if (Notified == alarmInfo.alarmState) {
			continue;
		}
		displayInfoList.append(alarmInfo);
	}
	int alarmCount = displayInfoList.count();

	HbMainWindow *window = hbInstance->allMainWindows().first();

	bool loadSuccess = false;
	Qt::Orientation currentOrienation = window->orientation();
	
	// Get the dividers.
	mHorizontalDivider = static_cast<HbLabel *> (
			mDocLoader->findObject("horizontalDivider"));
	mVerticalDivider = static_cast<HbLabel *> (
				mDocLoader->findObject("verticalDivider"));

	// Get the "No alarm set" label.
	mNoAlarmLabel = qobject_cast<HbLabel *> (
			mDocLoader->findWidget(CLOCK_NOALARMLABEL));
	mNoAlarmLabel->setTextColor(mLabelColorGroup);

	// Get the alarm's list.
	mAlarmList = qobject_cast<HbListView *> (
			mDocLoader->findWidget(CLOCK_ALARMLIST));
	mAlarmList->setItemPixmapCacheEnabled(true);
	connect(
			mAlarmList, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(handleActivated(const QModelIndex &)));

	connect(
			mAlarmList,
			SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
			this,
			SLOT(handleLongPress(HbAbstractViewItem*, const QPointF&)));

	HbStyleLoader::registerFilePath(":/style/clockalarmlistitemprototype.css");
	HbStyleLoader::registerFilePath(
			":/style/clockalarmlistitemprototype.widgetml");
	HbStyleLoader::registerFilePath(
			":/style/clockalarmlistitemprototype_color.css");
	setmodel();

	// Load the correct section based on orientation.
	if (Qt::Vertical == currentOrienation) {
		mDocLoader->load(
				CLOCK_MAIN_VIEW_DOCML,
				CLOCK_MAIN_VIEW_PORTRAIT_SECTION,
				&loadSuccess);
		mHorizontalDivider->setVisible(true);
		mVerticalDivider->setVisible(false);
	} else {
		mDocLoader->load(
				CLOCK_MAIN_VIEW_DOCML,
				CLOCK_MAIN_VIEW_LANDSCAPE_SECTION,
				&loadSuccess);
		mHorizontalDivider->setVisible(false);
		mVerticalDivider->setVisible(true);
	}
	if (loadSuccess) {
		if (0 == alarmCount) {
			hideAlarmList(true);
		} else {
			hideAlarmList(false);
			mAlarmListModel->populateModel();
		}
	}

	mDayLabel = static_cast<HbLabel *> (mDocLoader->findObject("dateLabel"));
	mDayLabel->setTextColor(mLabelColorGroup);
	mPlaceLabel = static_cast<HbLabel *> (mDocLoader->findObject("placetext"));
	mPlaceLabel->setTextColor(mLabelColorGroup);
	mPlaceLabel->setAlignment(Qt::AlignLeft);
	mPlaceLabel->setTextWrapping(Hb::TextNoWrap);
	mDstIcon = static_cast<HbLabel *> (mDocLoader->findObject("dstIcon"));
	mClockWidget = static_cast<ClockWidget*> (
			mDocLoader->findObject(CLOCK_WIDGET));

	// Update the date info.
	updateDateLabel();
	// Update the place info.
	updatePlaceLabel(mTimezoneClient->timeUpdateOn());
	// Update clock widget display.
	updateClockWidget();

	// Connect to orientation change and load appropriate section.
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(checkOrientationAndLoadSection(Qt::Orientation)));
	

  
    // clean up any previous versions of this activity.
    // ignore return value as the first boot would always return a false
    // bool declared on for debugging purpose
    bool ok = removeActivity();

	// connect for the aboutToQuit events on application Exit as to call saveActivity
    connect(qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));

    OstTraceFunctionExit0( CLOCKMAINVIEW_SETUPVIEW_EXIT );
}

/*!
	To defer the connects and initialization. To be done after the view is drawn.
	Should be called in the slot of view ready.
 */
void ClockMainView::setupAfterViewReady()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_SETUPAFTERVIEWREADY_ENTRY );
	// Get the toolbar/menu actions.
	mRefreshMainView = static_cast<HbAction *> (
			mDocLoader->findObject("alarmsAction"));

	mRefreshMainView->setCheckable(true);
	mRefreshMainView->setChecked(true);

	connect(
			mRefreshMainView, SIGNAL(changed()),
			this, SLOT(refreshMainView()));

	mDisplayWorldClockView = static_cast<HbAction *> (
			mDocLoader->findObject("worldClockAction"));
	connect(
			mDisplayWorldClockView, SIGNAL(triggered()),
			this, SLOT(displayWorldClockView()));

	mAddNewAlarm = static_cast<HbAction *> (
			mDocLoader->findObject("newAlarmAction"));
	connect(
			mAddNewAlarm, SIGNAL(triggered()),
			this, SLOT(addNewAlarm()));

	mSettingsAction = static_cast<HbAction *> (
			mDocLoader->findObject("settingsAction"));
	connect(
			mSettingsAction, SIGNAL(triggered()),
			this, SLOT(openSettings()));

	// Connect the necessary timezone client call backs.
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateView()));
	connect(
			mTimezoneClient, SIGNAL(autoTimeUpdateChanged(int)),
			this, SLOT(updatePlaceLabel(int)));
	connect(
			mTimezoneClient, SIGNAL(cityUpdated()),
			this, SLOT(updatePlaceLabel()));
	
	// Connect for the clock type changes to refresh the view
	mSettingsManager = new XQSettingsManager(this);
	mClockTypeSettingsKey = new XQSettingsKey(
							XQSettingsKey::TargetCentralRepository,
							KCRUidClockApp,
							KClockType);
	mSettingsManager->startMonitoring(*mClockTypeSettingsKey);
	connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
				this, SLOT(eventMonitor(XQSettingsKey, QVariant)));
	
	OstTraceFunctionExit0( CLOCKMAINVIEW_SETUPAFTERVIEWREADY_EXIT );
}

/*!
	Slot which gets called for the status change of the alarm i.e for
	active/inactive.

	\param row contains the row number of the alarm in the alarm list
 */
void ClockMainView::handleAlarmStatusChanged(int row)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_HANDLEALARMSTATUSCHANGED_ENTRY );
	AlarmInfo alarmInfo;

	// Get the data for the alarm.
	QList<QVariant> alarmData =
			mAlarmListModel->sourceModel()->index(row, 0).data(
					AlarmDetails).toList();
	int alarmId = alarmData.at(0).value<int>();

	mAlarmClient->getAlarmInfo(alarmId, alarmInfo);

	mSelectedItem = row;

	if (-1 < mSelectedItem) {
		QList<QVariant> alarmData =
				mAlarmListModel->sourceModel()->index(
						mSelectedItem, 0).data(AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();
		QString displayNote;
		// Activate or deactivate the alarm depending on the alarm status.
		// Display the NotificationDialog with appropriate message.
		if (Snoozed == alarmInfo.alarmState && Enabled == alarmInfo.alarmStatus
				&& Once != alarmInfo.repeatType) {
			removeSnoozedAlarm();
			displayNote.append(tr("Snoozed alarm removed"));
		}else if (Enabled == alarmInfo.alarmStatus) {
			mAlarmClient->toggleAlarmStatus(alarmId, Disabled);
			displayNote.append(
			    hbTrId("txt_clock_main_view_dpopinfo_alarm_deactivated"));
		} else {
			mAlarmClient->toggleAlarmStatus(alarmId, Enabled);
			displayNote.append(
			    hbTrId("txt_clock_main_view_dpopinfo_alarm_activated"));
		}

		HbNotificationDialog *dialog = new HbNotificationDialog();
		dialog->setTitle(displayNote);
		dialog->setTimeout(HbPopup::ConfirmationNoteTimeout);
		dialog->show();

		mSelectedItem = -1;
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_HANDLEALARMSTATUSCHANGED_EXIT );
}

/*!
	Slot which gets called when `Alarm' action is triggered from the view
	toolbar. This is responsible for reloading the content of the main view.
 */
void ClockMainView::refreshMainView()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_REFRESHMAINVIEW_ENTRY );
	mRefreshMainView->setChecked(true);

	OstTraceFunctionExit0( CLOCKMAINVIEW_REFRESHMAINVIEW_EXIT );
}

/*!
	Slot which gets called when `World clock' action is triggered from the view
	toolbar. This is responsible for launching the world clock.
 */
void ClockMainView::displayWorldClockView()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_DISPLAYWORLDCLOCKVIEW_ENTRY );
	mAppControllerIf->switchToView(WorldClock);
	// no need to capture the screenshot here as it's done in ClockViewManager::showView

	OstTraceFunctionExit0( CLOCKMAINVIEW_DISPLAYWORLDCLOCKVIEW_EXIT );
}

/*!
	Slot which gets called when `New alarm' action is triggered from the view
	toolbar. This is responsible for launching the editor to create a new alarm.
 */
void ClockMainView::addNewAlarm()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_ADDNEWALARM_ENTRY );
	ClockAlarmEditor *alarmEditor = new ClockAlarmEditor(*mAlarmClient);
	alarmEditor->showAlarmEditor();
	// capture screenshot for future use, if application
	// is exited/Quit from alarmEditor
	captureScreenShot(true);
	OstTraceFunctionExit0( CLOCKMAINVIEW_ADDNEWALARM_EXIT );
}

/*!
	Slot which gets called when `Settings' action is triggered from the view
	menu. This is responsible for launching the settings view.
 */
void ClockMainView::openSettings()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_OPENSETTINGS_ENTRY );
	// Create the settings view.
	ClockSettingsView *settingsView = new ClockSettingsView(this);
	settingsView->loadSettingsView();
	// capture screenshot for future use, if application
	// is exited/Quit from alarmEditor
	captureScreenShot(true);
	OstTraceFunctionExit0( CLOCKMAINVIEW_OPENSETTINGS_EXIT );
}

/*!
	This slot handles the 'activated' signal of the alarm list. Opens the
	alarm editor to edit the alarm.
	\param index the modelIndex in the list which has been tapped.
 */
void ClockMainView::handleActivated(const QModelIndex &index)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_HANDLEACTIVATED_ENTRY );
	if(!mIsLongTop) {// Get the data for the alarm.
		int row = index.row();
		QList<QVariant> alarmData =
				mAlarmListModel->sourceModel()->index(row, 0).data(
						AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();

		// Construct the alarm editor.
		ClockAlarmEditor *alarmEditor = new ClockAlarmEditor(
				*mAlarmClient, alarmId);
		alarmEditor->showAlarmEditor();
		// capture screenshot for future use, if application 
		// is exited/Quit from alarmEditor
		captureScreenShot(true);
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_HANDLEACTIVATED_EXIT );
}

/*!
	This slot handles the 'longpressed' signal of the alarm list. Opens the
	context menu.

	\param item The item in the list which has been long pressed.
	\param coords The position where mouse was pressed.
 */
void ClockMainView::handleLongPress(
		HbAbstractViewItem *item, const QPointF &coords)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_HANDLELONGPRESS_ENTRY );
	mIsLongTop = true;
	AlarmInfo alarmInfo;

	// Save the item row number where the long press was made.
	mSelectedItem = item->modelIndex().row();

	// Get the data for the alarm.
	QList<QVariant> alarmData =
			mAlarmListModel->sourceModel()->
			index(mSelectedItem, 0).data(AlarmDetails).toList();
	int alarmId = alarmData.at(0).value<int>();

	mAlarmClient->getAlarmInfo(alarmId, alarmInfo);

	// On long press we display item specific context menu.
	HbMenu *itemContextMenu = new HbMenu();
	connect(
			itemContextMenu,SIGNAL(aboutToClose()),
			this, SLOT(handleMenuClosed()));

	// Add the delete action to the context menu.
	mDeleteAction = itemContextMenu->addAction(
			hbTrId("txt_clk_main_view_menu_delete_alarm"));

	// Show the menu.
	itemContextMenu->open(this, SLOT(selectedMenuAction(HbAction*)));
	itemContextMenu->setPreferredPos(coords);
	itemContextMenu->setAttribute(Qt::WA_DeleteOnClose, true );
	
	OstTraceFunctionExit0( CLOCKMAINVIEW_HANDLELONGPRESS_EXIT );
}

/*!
	This slot handles the deletion of the alarm from the server.

 */
void ClockMainView::deleteAlarm()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_DELETEALARM_ENTRY );
	if (-1 < mSelectedItem) {
		// Get the data for the alarm.
		QList<QVariant> alarmData = mAlarmListModel->sourceModel()->
				index(mSelectedItem, 0).data(AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();
		mAlarmClient->deleteAlarm(alarmId);
		mSelectedItem = -1;
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_DELETEALARM_EXIT );
}

/*!
	Deltes the snoozed alarm.
 */
void ClockMainView::removeSnoozedAlarm()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_REMOVESNOOZEDALARM_ENTRY );
	if (-1 < mSelectedItem) {
		// Get the data for the alarm.
		QList<QVariant> alarmData = mAlarmListModel->sourceModel()->
		index(mSelectedItem, 0).data(AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();
		mAlarmClient->deleteSnoozedAlarm(alarmId);
		mSelectedItem = -1;
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_REMOVESNOOZEDALARM_EXIT );
}

/*!
	Slot which is called when the value changes in cenrep.

	\param key The key which got changed in cenrep.
	\param value The new value of that key.
 */
void ClockMainView::eventMonitor(
		const XQSettingsKey& key, const QVariant& value)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_EVENTMONITOR_ENTRY );
	if (key.uid() == KCRUidClockApp && key.key() == KClockType) {
		// Update view
		updateView();
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_EVENTMONITOR_EXIT );
}


void ClockMainView::updateView()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_UPDATEVIEW_ENTRY );
	// Update the place label.
	updatePlaceLabel(mTimezoneClient->timeUpdateOn());
	// Update date label.
	updateDateLabel();
	// Update clock widget.
	updateClockWidget();
	captureScreenShot(false);
	OstTraceFunctionExit0( CLOCKMAINVIEW_UPDATEVIEW_EXIT );
}

/*!
	Slot which gets called for value change in auto time update in cenrep.

	\param autoTimeUpdate Value of auto time update.
 */
void ClockMainView::updatePlaceLabel(int autoTimeUpdate)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_UPDATEPLACELABEL_ENTRY );
	if (-1 == autoTimeUpdate) {
		autoTimeUpdate = mTimezoneClient->timeUpdateOn();
	}
	
	// Get the current zone info.
	LocationInfo currentZoneInfo = mTimezoneClient->getCurrentZoneInfoL();

	// Construct the GMT +/- X string.
	QString gmtOffset;

	int utcOffset = currentZoneInfo.zoneOffset;
	int offsetInHours (utcOffset/60);
	int offsetInMinutes (utcOffset%60);

	// Check wether the offset is +ve or -ve.
	if (0 < utcOffset) {
		// We have a positive offset. Append the '+' character.
		gmtOffset += tr(" +");
	} else if (0 > utcOffset) {
		// We have a negative offset. Append the '-' character.
		gmtOffset += tr(" -");
		offsetInHours = -offsetInHours;
	} else {
		// We dont have an offset. We are at GMT zone.
	}

	// Append the hour component.
	gmtOffset += QString::number(offsetInHours);

	// Get the time separator from settings and append it.
	QStringList timeSeparatorList;
	int index = mSettingsUtility->timeSeparator(timeSeparatorList);
	gmtOffset += timeSeparatorList.at(index);

	// Append the minute component.
	// If minute component is less less than 10, append a '00'
	if (0 <= offsetInMinutes && offsetInMinutes < 10) {
		gmtOffset += tr("00");
	} else {
		gmtOffset += QString::number(offsetInMinutes);
	}

	// Append space.
	gmtOffset += tr(" ");

	// Append GMT sting.
	gmtOffset += hbTrId("txt_clock_main_view_setlabel_gmt");

	// Append space.
	gmtOffset += tr(" ");

	// Append DST info.
	HbIcon *dstIcon = new HbIcon("");
	if (currentZoneInfo.dstOn) {
		dstIcon->setIconName("qtg_mono_day_light_saving_time");
		dstIcon->setColor(mLabelColorGroup);
		
	}
	mDstIcon->setIcon(*dstIcon);

	// Update the labels with the correct info.
	mPlaceLabel->clear();
	if(autoTimeUpdate) {
		mPlaceLabel->setPlainText(
				currentZoneInfo.countryName + tr(" ") + gmtOffset);
	} else {
		mPlaceLabel->setPlainText(
				currentZoneInfo.cityName + tr(", ")
				+ currentZoneInfo.countryName + tr(" ") + gmtOffset);
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_UPDATEPLACELABEL_EXIT );
}

/*!
	Slot to either hide or show the alarmlist.
 */
void ClockMainView::handleAlarmListDisplay()
{
    OstTraceFunctionEntry0( CLOCKMAINVIEW_HANDLEALARMLISTDISPLAY_ENTRY );
    // alarmEditor closed reset the captured screenshot, current view is main view now
    captureScreenShot(false);
	// Get the list of pending clock alarms from server.
	QList<AlarmInfo> alarmInfoList;
	QList<AlarmInfo> displayInfoList;
	AlarmInfo alarmInfo;
	mAlarmClient->getAlarmList(alarmInfoList);
	for (int index = 0; index < alarmInfoList.count(); ++index) {
		alarmInfo = alarmInfoList[index];
		if (Notified == alarmInfo.alarmState) {
			continue;
		}
		displayInfoList.append(alarmInfo);
	}
	if (mHideAlarmList) {
		hideAlarmList(false);
	} else {
		if (0 == displayInfoList.count() &&
				0 == mAlarmListModel->sourceModel()->rowCount()) {
			hideAlarmList(true);
		}
	}

	OstTraceFunctionExit0( CLOCKMAINVIEW_HANDLEALARMLISTDISPLAY_EXIT );
}

/*!
	Slot to check the orientation & load the appropriate section.

	\param orientation contains the current orientation of the window.
 */
void ClockMainView::checkOrientationAndLoadSection(
		Qt::Orientation orientation)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_CHECKORIENTATIONANDLOADSECTION_ENTRY );
	bool success;
	// If horizontal, load the landscape section.
	if (Qt::Horizontal == orientation) {
		mDocLoader->load(
				CLOCK_MAIN_VIEW_DOCML, CLOCK_MAIN_VIEW_LANDSCAPE_SECTION,
				&success);
		mHorizontalDivider->setVisible(false);
		mVerticalDivider->setVisible(true);
	} else {
		mDocLoader->load(
				CLOCK_MAIN_VIEW_DOCML, CLOCK_MAIN_VIEW_PORTRAIT_SECTION,
				&success);
		mHorizontalDivider->setVisible(true);
		mVerticalDivider->setVisible(false);
	}

	if(success) {
		QList<AlarmInfo> alarmInfoList;
		// Get the list of all clock alarms.
		mAlarmClient->getAlarmList(alarmInfoList);
		if (0 == alarmInfoList.count()) {
			hideAlarmList(true);
		} else {
			hideAlarmList(false);
		}
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_CHECKORIENTATIONANDLOADSECTION_EXIT );
}

/*!
	Slot to handle context menu actions.
 */
void ClockMainView::selectedMenuAction(HbAction *action)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_SELECTEDMENUACTION_ENTRY );
	if (action == mDeleteAction) {
		deleteAlarm();
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_SELECTEDMENUACTION_EXIT );
}

/*!
	Slot to handle the context menu closed.
 */
void ClockMainView::handleMenuClosed()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_HANDLEMENUCLOSED_ENTRY );
	mIsLongTop = false;
	OstTraceFunctionExit0( CLOCKMAINVIEW_HANDLEMENUCLOSED_EXIT );
}
/*!
	Sets the model to the alarm list.
 */
void ClockMainView::setmodel()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_SETMODEL_ENTRY );
	// Set the model.
	if (mAlarmList) {
		mAlarmList->setModel(mAlarmListModel->sourceModel());
		ClockAlarmListItemPrototype *listItemPrototype =
				new ClockAlarmListItemPrototype();
		connect(
				listItemPrototype, SIGNAL(alarmStatusHasChanged(int)),
				this, SLOT(handleAlarmStatusChanged(int)));
		mAlarmList->setItemPrototype(listItemPrototype);
		mAlarmList->setLayoutName("layout-alarmlist");
	}

	OstTraceFunctionExit0( CLOCKMAINVIEW_SETMODEL_EXIT );
}

/*!
	Hides the alarmlist in the main view.

	\param hide 'true' if alarm list is to be hidden.
 */
void ClockMainView::hideAlarmList(bool hide)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_HIDEALARMLIST_ENTRY );
	if (hide) {
		mNoAlarmLabel->show();
		mAlarmList->hide();
		mHideAlarmList = true;
	} else {
		mAlarmList->show();
		mNoAlarmLabel->hide();
		mHideAlarmList = false;
	}
	OstTraceFunctionExit0( CLOCKMAINVIEW_HIDEALARMLIST_EXIT );
}

/*!
	Updates the day and date in the day label.
 */
void ClockMainView::updateDateLabel()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_UPDATEDATELABEL_ENTRY );
	// Get the current datetime.
	QDateTime dateTime = QDateTime::currentDateTime();
	// Get the day name.
	QString dayName = dateTime.toString("ddd");
	// Get the date in correct format.
	QString currentDate = mSettingsUtility->date();
	// Construct the day + date string.
	QString dayDateString;
	dayDateString+= dayName;
	dayDateString += " ";
	dayDateString += currentDate;

	mDayLabel->setPlainText(dayDateString);
	OstTraceFunctionExit0( CLOCKMAINVIEW_UPDATEDATELABEL_EXIT );
}

/*!
	Updates the zone info in the place label.
 */
/*void ClockMainView::updatePlaceLabel()
{
	updatePlaceLabel(mTimezoneClient->timeUpdateOn());
}*/

/*!
	Updates the clock widget display.
 */
void ClockMainView::updateClockWidget()
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_UPDATECLOCKWIDGET_ENTRY );
	QStringList clockType;
    int index = mSettingsUtility->clockType(clockType);
    int zeroIndex(0);
    if(zeroIndex == index){
    	mClockWidget->setClockType(ClockWidget::ClockTypeAnalog);
    } else {
    	mClockWidget->setClockType(ClockWidget::ClockTypeDigital);
    }
    
    QStringList timeFormat;

    if (zeroIndex == mSettingsUtility->timeFormat(timeFormat)) {
    	mClockWidget->setTimeFormat(ClockWidget::TimeFormat24Hrs);
    } else {
    	mClockWidget->setTimeFormat(ClockWidget::TimeFormat12Hrs);
    }

	mClockWidget->updateTime();
	OstTraceFunctionExit0( CLOCKMAINVIEW_UPDATECLOCKWIDGET_EXIT );
}

/*!
	CaptureScreenShot captures screen shot 
	\param captureScreenShot bool to indicate if screenshot needs to be captured
*/ 
void ClockMainView::captureScreenShot(bool captureScreenShot)
{
	OstTraceFunctionEntry0( CLOCKMAINVIEW_CAPTURESCREENSHOT_ENTRY );
	// check if screen shot needs to be captured
    if (captureScreenShot) {
        mScreenShot.clear();
        mScreenShot.insert("screenshot", QPixmap::grabWidget(mainWindow(), mainWindow()->rect()));
    }
    mIsScreenShotCapruted = captureScreenShot; // set mIsScreenShotCapruted set validity of screenshot
    OstTraceFunctionExit0( CLOCKMAINVIEW_CAPTURESCREENSHOT_EXIT );
}

/*!    
	saveActivity saves main view as an activity 
*/ 
void ClockMainView::saveActivity()
{
   OstTraceFunctionEntry0( CLOCKMAINVIEW_SAVEACTIVITY_ENTRY );
 	// check if a valid screenshot is already captured
   if (!mIsScreenShotCapruted)  {
       mScreenShot.clear();
       mScreenShot.insert("screenshot", QPixmap::grabWidget(mainWindow(), mainWindow()->rect()));
   }
 
   // save any data necessary to save the state
   QByteArray serializedActivity;
   QDataStream stream(&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);
   stream << MainView;
 
   // add the activity to the activity manager
   bool ok = mActivityStorage->saveActivity(clockMainView, serializedActivity, mScreenShot);
   if ( !ok ) {
       qFatal("Add failed" );
   }
   OstTraceFunctionExit0( CLOCKMAINVIEW_SAVEACTIVITY_EXIT );
}

/*!
 Function to remove the activity 
 */
bool ClockMainView::removeActivity()
    {
    OstTraceFunctionEntry0( CLOCKMAINVIEW_REMOVEACTIVITY_ENTRY );
    OstTraceFunctionExit0( CLOCKMAINVIEW_REMOVEACTIVITY_EXIT );
    return mActivityStorage->removeActivity(clockMainView);
    }
// End of file	--Don't remove.
