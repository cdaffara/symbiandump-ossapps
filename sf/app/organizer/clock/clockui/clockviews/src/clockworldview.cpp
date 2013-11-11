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
* Definition file for class ClockWorldView.
*
*/

// System includes
#include <QGraphicsItem>
#include <QStandardItem>
#include <QStandardItemModel>
#include <HbInstance>
#include <HbAbstractViewItem>
#include <HbMenu>
#include <HbAction>
#include <HbStyleLoader>
#include <HbListView>
#include <HbParameterLengthLimiter>

// User includes
#include "clockworldview.h"
#include "clockdocloader.h"
#include "clockappcontrollerif.h"
#include "clockcommon.h"
#include "settingsutility.h"
#include "timezoneclient.h"
#include "clockcityselectionlist.h"
#include "clockhomecityitem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockworldviewTraces.h"
#endif


/*!
	\class ClockWorldView

	The world clock view of the clock application.
 */

/*!
	Constructor.

	\param parent The parent of type QGraphicsWidget.
 */
ClockWorldView::ClockWorldView(QGraphicsItem *parent)
:HbView(parent),
 mSelectedItem(-1)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_CLOCKWORLDVIEW_ENTRY );
	// Timer for updating list data upon time change/update.
	mRefreshTimer = new QTimer();
	connect(
			mRefreshTimer, SIGNAL(timeout()),
			this, SLOT(refreshCityList()));

	// Create the model.
	mCityListModel = new QStandardItemModel();
	OstTraceFunctionExit0( CLOCKWORLDVIEW_CLOCKWORLDVIEW_EXIT );
}

/*!
	Destructor.
 */
ClockWorldView::~ClockWorldView()
{
	OstTraceFunctionEntry0( DUP1_CLOCKWORLDVIEW_CLOCKWORLDVIEW_ENTRY );
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}
	if(mCityListModel){
		delete mCityListModel;
	}
	
	HbStyleLoader::unregisterFilePath(":/style/hblistviewitem.css");
	HbStyleLoader::unregisterFilePath(":/style/hblistviewitem.widgetml");
	HbStyleLoader::unregisterFilePath(":/style/hblistviewitem_color.css");
	OstTraceFunctionExit0( DUP1_CLOCKWORLDVIEW_CLOCKWORLDVIEW_EXIT );
}

/*!
	Called by the ClockViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The ClockAppController object.
	\param docLoader Pointer to ClockDocLoader object.
 */
void ClockWorldView::setupView(
		ClockAppControllerIf &controllerIf,
		ClockDocLoader *docLoader)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_SETUPVIEW_ENTRY );
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;

	mTimezoneClient = controllerIf.timezoneClient();
	mSettingsUtility = controllerIf.settingsUtility();

	// Establish required connections.
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateAllLocationInfo()));
	connect(
			mTimezoneClient, SIGNAL(autoTimeUpdateChanged(int)),
			this, SLOT(updateCurrentLocationInfo(int)));
	connect(
			mTimezoneClient, SIGNAL(cityUpdated()),
			this, SLOT(updateAllLocationInfo()));

	mDisplayWorldClockView = static_cast<HbAction *> (
			mDocLoader->findObject("worldClockAction"));

	mDisplayWorldClockView->setCheckable(true);
	mDisplayWorldClockView->setChecked(true);

	connect(
			mDisplayWorldClockView, SIGNAL(changed()),
			this, SLOT(refreshWorldView()));

	// Get the currently added locations to the list.
	mCityInfoList = mTimezoneClient->getSavedLocations();

	// Get the homecity widget.
	mHomeCityWidget = qobject_cast<ClockHomeCityItem *>(
			mDocLoader->findWidget(CLOCK_WORLD_HOMECITY));
	
	// Get the city list item.
	mCityListView = qobject_cast<HbListView *> (
			mDocLoader->findWidget(CLOCK_WORLD_CITYLIST));
	HbStyleLoader::registerFilePath(":/style/hblistviewitem.css");
	HbStyleLoader::registerFilePath(":/style/hblistviewitem.widgetml");
	HbStyleLoader::registerFilePath(":/style/hblistviewitem_color.css");
	mCityListView->setLayoutName("citylist");
	mCityListView->setModel(mCityListModel);
	mCityListView->setItemPixmapCacheEnabled(true);
	// Get the toolbar/menu actions.
	mAddCityAction = static_cast<HbAction *> (
			mDocLoader->findObject("addCityAction"));
	connect(
			mAddCityAction, SIGNAL(triggered()),
			this, SLOT(handleAddLocation()));

	mShowAlarmsViewAction = static_cast<HbAction *> (
			mDocLoader->findObject("alarmsAction"));
	connect(
			mShowAlarmsViewAction, SIGNAL(triggered()),
			this, SLOT(showAlarmsView()));

	HbMainWindow *window = hbInstance->allMainWindows().first();

	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(loadSection(Qt::Orientation)));

	if (mCityInfoList.count() > 0) {
		// There is atleast one city. Refresh needed.
		// Calculate after how much time the refresh has to happen.
		QTime currentTime = QDateTime::currentDateTime().time();
		int after = 60 - currentTime.second();
		mRefreshTimer->start(after * 1000);
	}

	connect(
			mCityListView,
			SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
			this,
			SLOT(handleItemLongPressed(HbAbstractViewItem *, const QPointF &)));

	// Update current location info.
	updateCurrentLocationInfo(mTimezoneClient->timeUpdateOn());

	// Populate the list.
	for (int index = 0; index < mCityInfoList.count(); index++) {
		addCityToList(mCityInfoList.at(index));
	}
	
	// Adding the "Add city" in options menu.
	mAddCityMenuAction = new HbAction(
			QString(hbTrId("txt_clock_opt_add_city")),this);
	menu()->addAction(mAddCityMenuAction);
	mAddCityMenuAction->setVisible(true);
	connect(
			mAddCityMenuAction, SIGNAL(triggered()),
			this, SLOT(handleAddLocation()));

	// Check for disabling of addCityAction in view toolbar.
	if ((KMaximumCityListCount-1) == mCityInfoList.count()) {
		mAddCityAction->setEnabled(false);
		mAddCityMenuAction->setVisible(false);
	}
	OstTraceFunctionExit0( CLOCKWORLDVIEW_SETUPVIEW_EXIT );
}

/*!
	Refreshes the data in the city list.
 */
void ClockWorldView::refreshCityList()
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_REFRESHCITYLIST_ENTRY );
	updateCurrentLocationInfo(mTimezoneClient->timeUpdateOn());
	int cityInfoCount = mCityInfoList.count();

	if (cityInfoCount) {
		QStandardItem *item = 0;
		for (int infoIndex = 0; infoIndex < cityInfoCount; infoIndex++) {
			item = mCityListModel->item(infoIndex);
			item->setData(
					getCityListDisplayString(mCityInfoList.at(infoIndex)),
					Qt::DisplayRole);
			item->setData(
					getCityListDecorationString(mCityInfoList.at(infoIndex)),
					Qt::DecorationRole);
		}

		// Start the timer again for one minute.
		QTimer::singleShot(60 * 1000, this, SLOT(refreshCityList()));
	}
	OstTraceFunctionExit0( CLOCKWORLDVIEW_REFRESHCITYLIST_EXIT );
}

/*!
	Updates the current location info.
 */
void ClockWorldView::updateCurrentLocationInfo(int networkTime)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_UPDATECURRENTLOCATIONINFO_ENTRY );
	HbMainWindow *window = hbInstance->allMainWindows().first();
	Qt::Orientation currentOrienation = window->orientation();
	loadSection(currentOrienation);

	if (!networkTime) {
		// Get the updated home city.
		LocationInfo homeCity = mTimezoneClient->getCurrentZoneInfoL();

		QMap<QString, QVariant> itemList;
		QString value;
		QDateTime dateTime = QDateTime::currentDateTime();

		// Show the date.
		QString dateInfo = dateTime.toString(
				mSettingsUtility->dateFormatString());
		itemList.insert(value.setNum(ClockHomeCityItem::Date), dateInfo);

		// Show the city and country name.
		QString placeInfo =
				homeCity.cityName + tr(", ") + homeCity.countryName;
		itemList.insert(value.setNum(ClockHomeCityItem::City), placeInfo);

		// Show the time at that location.
		QString timeInfo = dateTime.toString(
				mSettingsUtility->timeFormatString());
		itemList.insert(value.setNum(ClockHomeCityItem::Time), timeInfo);

		//Use the home city specific icons.
		// Display day/night indicators.
		QString dayNightIconPath = "";
		if (isDay(dateTime)) {
			dayNightIconPath = "qtg_large_clock_home";
		} else {
			dayNightIconPath = "qtg_large_clock_night_home";
		}
		itemList.insert(value.setNum(
				ClockHomeCityItem::DayNightIndicator), dayNightIconPath);

		// Show dst icon when needed.
		QString dstIconPath = " ";
		if (homeCity.dstOn) {
			dstIconPath = "qtg_mono_day_light_saving_time";
		}
		itemList.insert(value.setNum(ClockHomeCityItem::Dst), dstIconPath);
		
		mHomeCityWidget->setHomeCityItemData(itemList);
	}
	OstTraceFunctionExit0( CLOCKWORLDVIEW_UPDATECURRENTLOCATIONINFO_EXIT );
}

/*!
	Handles the long press on each item in the citylist.
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.
 */
void ClockWorldView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_HANDLEITEMLONGPRESSED_ENTRY );
	// Get the ndex of the selected item.
	mSelectedItem = item->modelIndex().row();

	// On long press we display item specific context menu.
	HbMenu *itemContextMenu = new HbMenu();

	// Add the actions to the context menu.
	mSetCurrentLocationAction = itemContextMenu->addAction(
			hbTrId("txt_clk_menu_set_as_current_location"));
	mRemoveCityAction = itemContextMenu->addAction(
			hbTrId("txt_clk_menu_delete"));

	// Show the menu.
	itemContextMenu->open(this, SLOT(selectedMenuAction(HbAction*)));
	itemContextMenu->setPreferredPos(coords);
	itemContextMenu->setAttribute( Qt::WA_DeleteOnClose,true);
	OstTraceFunctionExit0( CLOCKWORLDVIEW_HANDLEITEMLONGPRESSED_EXIT );
}

/*!
	This slot is called when Add location action is triggered. It is responsible
	for launching the city selection list.
 */
void ClockWorldView::handleAddLocation()
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_HANDLEADDLOCATION_ENTRY );
	// Construct the city selection list and show the same.
	mCitySelectionList = new ClockCitySelectionList(mTimezoneClient, this);
	connect(
			mCitySelectionList, SIGNAL(citySelected(LocationInfo)),
			this, SLOT(handleCitySelected(LocationInfo)));

	// Show the city list.
	mCitySelectionList->showCityList();
	OstTraceFunctionExit0( CLOCKWORLDVIEW_HANDLEADDLOCATION_EXIT );
}

/*!
	This slot handles delete action on the city list. It removes the item from
	the view.
 */
void ClockWorldView::handleDeleteAction()
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_HANDLEDELETEACTION_ENTRY );
	if (-1 != mSelectedItem) {
		QStandardItem *item = mCityListModel->takeItem(mSelectedItem);
		mCityListModel->removeRow(mSelectedItem);
		delete item;

		mCityInfoList.removeAt(mSelectedItem);
		mSelectedItem = -1;

		// Update the data file.
		mTimezoneClient->saveLocations(mCityInfoList);

		if (mCityInfoList.count() <= 0) {
			// No need to refresh the list anymore.
			mRefreshTimer->stop();
		} else if (mCityInfoList.count() < (KMaximumCityListCount-1) &&
				!mAddCityAction->isEnabled()) {
			mAddCityAction->setEnabled(true);
			mAddCityMenuAction->setVisible(true);
		}
	}
	OstTraceFunctionExit0( CLOCKWORLDVIEW_HANDLEDELETEACTION_EXIT );
}

/*!
	This slot sets the selected city as the current location.
 */
void ClockWorldView::handleSetAsCurrentLocationAction()
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_HANDLESETASCURRENTLOCATIONACTION_ENTRY );
	// Get the info of the selected item.
	LocationInfo newHomeCity = mCityInfoList[mSelectedItem];

	// Check if time update is set to ON.
	// If yes, reset it to OFF and change the home location.
	if (mTimezoneClient->timeUpdateOn()) {
		mTimezoneClient->setTimeUpdateOn(false);

		QStandardItem *item = mCityListModel->takeItem(mSelectedItem);
		mCityListModel->removeRow(mSelectedItem);
		delete item;

		mCityInfoList.removeAt(mSelectedItem);
	} else {
		// The current location and home city should be swapped.
		// Store the info of current home city.
		LocationInfo oldHomeCity = mTimezoneClient->getCurrentZoneInfoL();

		// Update the info list.
		mCityInfoList.removeAt(mSelectedItem);
		mCityInfoList.insert(mSelectedItem, oldHomeCity);

	}
	// Update the home city with the timezone server.
	mTimezoneClient->setAsCurrentLocationL(newHomeCity);

	// Update the current location info.
	updateCurrentLocationInfo(mTimezoneClient->timeUpdateOn());

	// Update the offset difference in each list item.
	refreshCityList();

	// Update the data file.
	mTimezoneClient->saveLocations(mCityInfoList);
	mSelectedItem = -1;
	OstTraceFunctionExit0( CLOCKWORLDVIEW_HANDLESETASCURRENTLOCATIONACTION_EXIT );
}

/*!
	Slot to handle the case when a city has been selected in from the city
	selection list.

	\param info of type LocationInfo which contains the city selected.
 */
void ClockWorldView::handleCitySelected(LocationInfo info)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_HANDLECITYSELECTED_ENTRY );
	// Info is invalid if the timezoneId is set to -1. We don't do anything in
	// that case.
	if (-1 != info.timezoneId) {
		// Now we check if the city is already added in the list.
		bool proceed = true;
		for (int i = 0; i < mCityInfoList.count(); i++) {
			if (info.timezoneId == mCityInfoList.at(i).timezoneId
					&& (info.cityName == mCityInfoList.at(i).cityName)
					&& (info.countryName == mCityInfoList.at(i).countryName) ) {
				proceed = false;
				break;
			}
		}

		LocationInfo currentCity = mTimezoneClient->getCurrentZoneInfoL();
		// Check if the selected city is not the home city.
		if (
				(info.timezoneId == currentCity.timezoneId)
				&& (info.cityName == currentCity.cityName)
				&& (info.countryName == currentCity.countryName)) {
			proceed = false;
		}

		if (proceed) {
			// This is new city totally. So add it.
			// Add the city information to the list
			mCityInfoList.append(info);
			QModelIndex index = addCityToList(info);
			mCityListView->scrollTo(index);
			mTimezoneClient->saveLocations(mCityInfoList);

			// Start the refresh timer if it was stopped previously.
			if (!mRefreshTimer->isActive()) {
				// Calculate after how much time the refresh has to happen.
				QTime currentTime = QDateTime::currentDateTime().time();
				int after = 60 - currentTime.second();
				mRefreshTimer->start(after * 1000);
			}
		}
	}
	if ((KMaximumCityListCount-1) == mCityInfoList.count() &&
			mAddCityAction->isEnabled()) {
		mAddCityAction->setEnabled(false);
		mAddCityMenuAction->setVisible(false);
	}
	// Cleanup.
	mCitySelectionList->deleteLater();
	OstTraceFunctionExit0( CLOCKWORLDVIEW_HANDLECITYSELECTED_EXIT );
}

/*!
	Navigates to the clock alarms view.
 */
void ClockWorldView::showAlarmsView()
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_SHOWALARMSVIEW_ENTRY );
	mAppControllerIf->switchToView(MainView);
	OstTraceFunctionExit0( CLOCKWORLDVIEW_SHOWALARMSVIEW_EXIT );
}

/*!
	Slot which gets called when `World Clock' action is triggered from the view
	toolbar. This is responsible for reloading the content of worldclock view.
 */
void ClockWorldView::refreshWorldView()
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_REFRESHWORLDVIEW_ENTRY );
	mDisplayWorldClockView->setChecked(true);
	OstTraceFunctionExit0( CLOCKWORLDVIEW_REFRESHWORLDVIEW_EXIT );
}

/*!
	Loads the appropriate section based on window orientaion.
 */
void ClockWorldView::loadSection(Qt::Orientation orientation)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_LOADSECTION_ENTRY );
	bool networkTime = mTimezoneClient->timeUpdateOn();
	bool loadSuccess;
	if (Qt::Horizontal == orientation) {
		if (networkTime) {
			// Do not show home city.
			mDocLoader->load(
					CLOCK_WORLD_VIEW_DOCML,
					CLOCK_WORLD_VIEW_LANDSCAPE_NOHOMECITY_SECTION,
					&loadSuccess);
			mHomeCityWidget->hide();
		} else {
			mDocLoader->load(
					CLOCK_WORLD_VIEW_DOCML,
					CLOCK_WORLD_VIEW_LANDSCAPE_HOMECITY_SECTION,
					&loadSuccess);
			mHomeCityWidget->show();
		}
	} else if (Qt::Vertical == orientation) {
		if (networkTime) {
			// Do not show home city.
			mDocLoader->load(
					CLOCK_WORLD_VIEW_DOCML,
					CLOCK_WORLD_VIEW_PORTRAIT_NOHOMECITY_SECTION,
					&loadSuccess);
			mHomeCityWidget->hide();
		} else {
			// Show home city.
			mDocLoader->load(
					CLOCK_WORLD_VIEW_DOCML,
					CLOCK_WORLD_VIEW_PORTRAIT_HOMECITY_SECTION,
					&loadSuccess);
			mHomeCityWidget->show();
		}
	}
	mCityListView->update();
	OstTraceFunctionExit0( CLOCKWORLDVIEW_LOADSECTION_EXIT );
}

/*!
	Slot which gets called for timeChanged signal of timezone client.
	Refreshes both homecity & city list.
 */
void ClockWorldView::updateAllLocationInfo()
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_UPDATEALLLOCATIONINFO_ENTRY );
	updateCurrentLocationInfo(mTimezoneClient->timeUpdateOn());
	updateCityList();
	refreshCityList();
	OstTraceFunctionExit0( CLOCKWORLDVIEW_UPDATEALLLOCATIONINFO_EXIT );
}

/*!
	Slot to handle the selected context menu actions
 */
void ClockWorldView::selectedMenuAction(HbAction *action)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_SELECTEDMENUACTION_ENTRY );
	if (action == mSetCurrentLocationAction) {
		handleSetAsCurrentLocationAction();
	} else if (action == mRemoveCityAction) {
		handleDeleteAction();
	}
	OstTraceFunctionExit0( CLOCKWORLDVIEW_SELECTEDMENUACTION_EXIT );
}

/*!
	Adds the location info to the city list.

	\param locationInfo Details of the city to be added to the list.
 */
QModelIndex ClockWorldView::addCityToList(const LocationInfo& locationInfo)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_ADDCITYTOLIST_ENTRY );
	// Here we construct a model item and add it to the list model.
	QStandardItem *modelItem = new QStandardItem();

	// Add the item to the model.
	mCityListModel->appendRow(modelItem);

	QModelIndex index = mCityListModel->indexFromItem(modelItem);
	mCityListModel->setData(
			index, getCityListDisplayString(locationInfo), Qt::DisplayRole);
	mCityListModel->setData(
			index, getCityListDecorationString(locationInfo),
			Qt::DecorationRole);

	OstTraceFunctionExit0( CLOCKWORLDVIEW_ADDCITYTOLIST_EXIT );
	return index;
}

/*!
	Checks if the given time is day or night.
	6:00 AM and 6:00 PM is considered as day. Otherwise night.

	\param dateTime Time for which check has to be performed.
	\return bool True if it is day otherwise false.
 */
bool ClockWorldView::isDay(QDateTime dateTime)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_ISDAY_ENTRY );
	// It is day between 6:00 AM and 6:00 PM. Otherwise night.
	if (17 < dateTime.time().hour() || 6 > dateTime.time().hour()) {
		OstTraceFunctionExit0( CLOCKWORLDVIEW_ISDAY_EXIT );
		return false;
	}
	OstTraceFunctionExit0( DUP1_CLOCKWORLDVIEW_ISDAY_EXIT );
	return true;
}

/*!
	Returns the QVariantList for citylist to be set for DisplayRole.
	
	/param locationInfo Details of the city to be added to the list.
 */
QVariantList ClockWorldView::getCityListDisplayString(
		const LocationInfo& locationInfo)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_GETCITYLISTDISPLAYSTRING_ENTRY );
	QVariantList displayString;
	QDateTime dateTime = QDateTime::currentDateTime();
	dateTime = dateTime.toUTC();
	dateTime = dateTime.addSecs(locationInfo.zoneOffset * 60);

	// Show the date. If date is current date then show 'today'.
	QString dateInfo = dateTime.toString(mSettingsUtility->dateFormatString());
	if (dateTime.date() == QDate::currentDate()) {
		dateInfo = hbTrId("txt_clock_main_view_dblist_daily_val_today");
	}
	displayString.append(dateInfo);

	// Show the city and country name.
	QString placeInfo =
			locationInfo.cityName + tr(", ") + locationInfo.countryName;
	displayString.append(placeInfo);

	// Get the homecity information.
	LocationInfo homeCityInfo = mTimezoneClient->getCurrentZoneInfoL();
	// Find out if the city being added has an offset greater than or less than
	// the homecity offset.
	QString offsetDifference;
	if (locationInfo.zoneOffset < homeCityInfo.zoneOffset) {
		offsetDifference += "-";
	} else if (locationInfo.zoneOffset > homeCityInfo.zoneOffset) {
		offsetDifference += "+";
	}
	// Now get the hours and minutes.
	int difference = qAbs(homeCityInfo.zoneOffset - locationInfo.zoneOffset);
	int hours = difference / 60;
	int minutes = difference % 60;
	QString displayFormat("");
	QString offsetString("");

	if ( hours && minutes ) {
		if (hours == 1) {
			displayFormat =
					HbParameterLengthLimiter(hbTrId("txt_clock_dblist_daily_val_1_hr_2_mins"))
					.arg(QString::number(hours)).arg(QString::number(minutes));
			offsetDifference += displayFormat;
		}
		else {
			displayFormat =
					HbParameterLengthLimiter(hbTrId("txt_clock_dblist_daily_val_1_hrs_2_mins"))
					.arg(QString::number(hours)).arg(QString::number(minutes));
			offsetDifference += displayFormat;
		}
	}
	else if ( hours ){
		if(hours == 1) {
			displayFormat = hbTrId("txt_clock_dblist_daily_val_ln_hr",hours);
			offsetDifference += displayFormat;
		}
		else {
			displayFormat = hbTrId("txt_clock_dblist_daily_val_ln_hr",hours);
			offsetDifference += displayFormat;
		}
	}
	else if (minutes){
		displayFormat = hbTrId("txt_clock_dblist_daily_val_ln_mins",minutes);
		offsetDifference += displayFormat;
	} else {
		displayFormat = hbTrId("txt_clock_dblist_daily_val_ln_hr",0);
		offsetDifference += displayFormat;
	}
	displayString.append(offsetDifference);

	// Show the time at that location.
	QString timeInfo = dateTime.toString(mSettingsUtility->timeFormatString());
	displayString.append(timeInfo);

	OstTraceFunctionExit0( CLOCKWORLDVIEW_GETCITYLISTDISPLAYSTRING_EXIT );
	return displayString;
}

/*!
	Returns the QVariantList for citylist to be set for DecorationRole.

	/param locationInfo Details of the city to be added to the list.
 */
QVariantList ClockWorldView::getCityListDecorationString(
		const LocationInfo& locationInfo)
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_GETCITYLISTDECORATIONSTRING_ENTRY );
	QVariantList decorationString;
	QDateTime dateTime = QDateTime::currentDateTime();
	dateTime = dateTime.toUTC();
	dateTime = dateTime.addSecs(locationInfo.zoneOffset * 60);

	// Display day/night indicators.
	QString dayNightIconPath = "";
	if (isDay(dateTime)) {
		dayNightIconPath = "qtg_large_clock";
	} else {
		dayNightIconPath = "qtg_large_clock_night";
	}
	decorationString.append(HbIcon(dayNightIconPath));

	// Show dst icon when needed.
	QString dstIconPath = "";
	if (locationInfo.dstOn) {
		dstIconPath = "qtg_mono_day_light_saving_time";
	}
	decorationString.append(HbIcon(dstIconPath));
	
	OstTraceFunctionExit0( CLOCKWORLDVIEW_GETCITYLISTDECORATIONSTRING_EXIT );
	return decorationString;
	
}

/*!
	Updates the city list according to the home city.
 */
void ClockWorldView::updateCityList()
{
	OstTraceFunctionEntry0( CLOCKWORLDVIEW_UPDATECITYLIST_ENTRY );
	int cityInfoCount = mCityInfoList.count();

	if (cityInfoCount) {
		bool deletion = false;
		bool valueUpdated = false;
		int index;
		LocationInfo currentCity = mTimezoneClient->getCurrentZoneInfoL();
		// Remove the new home city if it is already added to the list.
		for (index = 0; index < cityInfoCount; index++) {
			if (currentCity.timezoneId == mCityInfoList.at(index).timezoneId
					&& (currentCity.cityName ==
							mCityInfoList.at(index).cityName)
					&& (currentCity.countryName ==
							mCityInfoList.at(index).countryName) ) {
				deletion = true;
				break;
			}
		}
		if (deletion) {
			QStandardItem *item = mCityListModel->takeItem(index);
			mCityListModel->removeRow(index);
			delete item;

			mCityInfoList.removeAt(index);
			index = -1;

			// Update the data file.
			mTimezoneClient->saveLocations(mCityInfoList);

			if (mCityInfoList.count() <= 0) {
				// No need to refresh the list anymore.
				mRefreshTimer->stop();
			} else if (mCityInfoList.count() < (KMaximumCityListCount-1) &&
					!mAddCityAction->isEnabled()) {
				mAddCityAction->setEnabled(true);
				mAddCityMenuAction->setVisible(true);
			}
			
		}
		
		for (int index = 0; index < cityInfoCount; ++index) {
			int tzid = mCityInfoList[index].timezoneId;
			bool dst = mTimezoneClient->isDSTOnL(tzid);
			if (dst != mCityInfoList[index].dstOn) {
				mCityInfoList[index].dstOn = dst;
				valueUpdated = true;
			}

		}
		
		if (valueUpdated) {
			// Update the data file.
			mTimezoneClient->saveLocations(mCityInfoList);
        }
	}
	OstTraceFunctionExit0( CLOCKWORLDVIEW_UPDATECITYLIST_EXIT );
}

// End of file-- Don't delete.
