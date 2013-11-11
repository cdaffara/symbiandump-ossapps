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
* Definition file for class ClockCitySelectionListPrivate.
*
*/

// System includes
#include <QStandardItemModel>
#include <QModelIndex>
#include <QGraphicsScene>
#include <QTimer>
#include <HbDocumentLoader>
#include <HbView>
#include <HbListView>
#include <HbListViewItem>
#include <HbMainWindow>
#include <HbInstance>
#include <HbStyleLoader>
#include <HbAction>
#include <HbSearchPanel>
#include <HbLineEdit>
#include <HbDialog>
#include <HbLabel>
#include <HbComboBox>
#include <HbExtendedLocale>
#include <HbAbstractItemView>
#include <HbShrinkingVkbHost>

// User includes
#include "clockcityselectionlist_p.h"
#include "clockcityselectionlist.h"
#include "clockcitylistproxymodel.h"
#include "clockcityselectionlistcommon.h"

#include "timezoneclient.h"
#include "clockdatatypes.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockcityselectionlist_pTraces.h"
#endif


/*!
	\class ClockCitySelectionListPrivate

	This is the private implementation class for ClockCitySelectionList class.
 */

/*!
	Default constructor.

	\param client The pointer to an object of TimezoneClient.
	\param parent of type QObject.
 */
ClockCitySelectionListPrivate::ClockCitySelectionListPrivate(
		TimezoneClient *client, QObject *parent)
:QObject(parent),
 mLoader(0),
 mOwnsClient(false)
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_CLOCKCITYSELECTIONLISTPRIVATE_ENTRY );
	// First get the q-pointer.
	q_ptr = static_cast<ClockCitySelectionList *> (parent);

	mClient = client;
	if (!mClient) {
		mClient = TimezoneClient::getInstance();
		mOwnsClient = true;
	}
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_CLOCKCITYSELECTIONLISTPRIVATE_EXIT );
}

/*!
	Destructor.
 */
ClockCitySelectionListPrivate::~ClockCitySelectionListPrivate()
{
	OstTraceFunctionEntry0( DUP1_CLOCKCITYSELECTIONLISTPRIVATE_CLOCKCITYSELECTIONLISTPRIVATE_ENTRY );
	if (mOwnsClient) {
		mClient->deleteInstance();
	}
	if (mLoader) {
		mLoader->reset();
		delete mLoader;
		mLoader = 0;
	}
	if (mListModel) {
		delete mListModel;
	}
	if (mTimeZoneOffsetList.count()) {
		mTimeZoneOffsetList.clear();
	}
	if(mCountryList.count()) {
		mCountryList.clear();
	}
	if (mVirtualKeyboard) {
	    delete mVirtualKeyboard;
    }

	OstTraceFunctionExit0( DUP1_CLOCKCITYSELECTIONLISTPRIVATE_CLOCKCITYSELECTIONLISTPRIVATE_EXIT );
}

/*!
	Populates the list model by getting the data from timezone client.
 */
void ClockCitySelectionListPrivate::populateListModel()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_POPULATELISTMODEL_ENTRY );
	// First get the data from the timezone client.
	QList<LocationInfo> &infoList = mClient->getLocations();

	// Sanity check.
	if (!mListModel) {
		mListModel = new QStandardItemModel(0, 1, this);
	}
	// Clear the model before starting population.
	mListModel->removeRows(0,infoList.count()-1);
	mListModel->insertRows(0, infoList.count());
	mListModel->setColumnCount(1);

	for (int iter = 0; iter < infoList.count(); iter++) {
		QModelIndex index = mListModel->index(iter, 0);

		// Get the location info and add it to the model.
		LocationInfo info = infoList.at(iter);
		QString displayString;
		displayString += info.cityName;
		displayString += ", ";
		displayString += info.countryName;
		mListModel->setData(index, displayString, Qt::DisplayRole);
		mListModel->setData(index, displayString, Qt::UserRole + 100);

		// Now save the timezone and city group ids.
		mListModel->setData(
				index, QVariant(info.timezoneId), Qt::UserRole + 101);
		mListModel->setData(
				index, QVariant(info.cityGroupId), Qt::UserRole + 102);
		mListModel->setData(
				index, info.cityName, Qt::UserRole + 103);
		mListModel->setData(
				index, info.countryName, Qt::UserRole + 104);
	}

	// Cleanup.
	infoList.clear();
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_POPULATELISTMODEL_EXIT );
}

/*!
	Slot to handle the case when a list item is activated. Here we return the
	city selected by the user. We emit a signal notifying this event and close
	the editor.
 */
void ClockCitySelectionListPrivate::handleItemActivated(
		const QModelIndex &index)
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLEITEMACTIVATED_ENTRY );
	LocationInfo selectedInfo;
	selectedInfo.timezoneId = index.data(Qt::UserRole + 101).value<int>();
	selectedInfo.cityGroupId = index.data(Qt::UserRole + 102).value<int>();
	selectedInfo.cityName = index.data(Qt::UserRole + 103).value<QString>();
	selectedInfo.countryName = index.data(Qt::UserRole + 104).value<QString>();
	selectedInfo.zoneOffset =
			mClient->getStandardOffset(selectedInfo.timezoneId);
	selectedInfo.dstOn = mClient->isDSTOnL(selectedInfo.timezoneId);

	// We have the information. Now emit a signal to notify clients if any.
	emit q_ptr->citySelected(selectedInfo);

	// Close the list.
	closeCityList();
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLEITEMACTIVATED_EXIT );
}

/*!
	Slot to handle back action is pressed.
 */
void ClockCitySelectionListPrivate::handleBackAction()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLEBACKACTION_ENTRY );
	LocationInfo info;
	info.timezoneId = -1;

	// Test code...
	int rowCount = mListModel->rowCount();
	QStandardItem *item = mListModel->item(0);
	QString text = item->data(Qt::DisplayRole).value<QString>();
	int tz = item->data(Qt::UserRole + 100).value<int>();
	int cg = item->data(Qt::UserRole + 101).value<int>();
	QString ci = item->data(Qt::UserRole + 102).value<QString>();
	QString co = item->data(Qt::UserRole + 103).value<QString>();

	// Emit the signal to notify the closure.
	emit q_ptr->citySelected(info);

	// Close the list.
	closeCityList();
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLEBACKACTION_EXIT );
}

/*!
	closes the city selection list.
 */
void ClockCitySelectionListPrivate::closeCityList()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_CLOSECITYLIST_ENTRY );
	// We are done, lets remove the view off the main window.
	HbMainWindow *window = hbInstance->allMainWindows().at(0);
	window->removeView(mView);

	if (mLoader) {
		mLoader->reset();
		delete mLoader;
		mLoader = 0;
	}

	if (mListModel) {
		delete mListModel;
	}
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_CLOSECITYLIST_EXIT );
}

/*!
	Updates the search criteria with the proxy model.

	\param criteria The string entered by the user.
 */
void ClockCitySelectionListPrivate::updateSearchCriteria(
		const QString &criteria)
{
     OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_UPDATESEARCHCRITERIA_ENTRY );
     int originalMask  = mListView->enabledAnimations();
     mListView->setEnabledAnimations(HbAbstractItemView::TouchDown);

	// Create the reg ex so that search is performed for match immediately
	// after word boundary.
	QString searchPattern("\\b");
	searchPattern.append(criteria);

	QRegExp searchExp;
	searchExp.setPattern(searchPattern);

	mProxyModel->setFilterRegExp(searchExp);
	mProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_UPDATESEARCHCRITERIA_EXIT );
}

/*!
	Handles add own city menu action
 */
void ClockCitySelectionListPrivate::handleAddOwnCityAction()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLEADDOWNCITYACTION_ENTRY );
	if (mAddOwnCityDialog) {
		delete mAddOwnCityDialog;
	}
	
	HbMainWindow *window = hbInstance->allMainWindows().first();

	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(loadSection(Qt::Orientation)));
	
	mAddCityDocloader = new HbDocumentLoader();
	bool success = false;
	mAddCityDocloader->load(":/xml/clockaddcitydialog.docml", &success);
	if (!success) {
		qFatal("Unable to load the document.");
	}

	if (Qt::Horizontal == window->orientation()) {
		mAddCityDocloader->load(":/xml/clockaddcitydialog.docml",
		                        "landscape", &success);
	} else if (Qt::Vertical == window->orientation()) {
		mAddCityDocloader->load(":/xml/clockaddcitydialog.docml",
		                        "portrait", &success);
	}
	// Get the dialog.
	mAddOwnCityDialog = static_cast<HbDialog *> (mAddCityDocloader->findWidget("dialog"));
	if (!mAddOwnCityDialog) {
		qFatal("Unable to get the selection view.");
	}
	mAddOwnCityDialog->setTimeout(HbDialog::NoTimeout);
	mAddOwnCityDialog->setDismissPolicy(HbDialog::NoDismiss);
	mAddOwnCityDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	
	// Find the elements from the docml.
	mCityNameEdit = static_cast<HbLineEdit *> (mAddCityDocloader->findWidget("cityNameLineEdit"));
	
	mTimeZoneComboBox = static_cast<HbComboBox *> (mAddCityDocloader->findWidget("timezoneCombobox"));
	if (mTimeZoneComboBox) {
		addTimeZoneField();
	}
	
	mCountryComboBox = static_cast<HbComboBox *> (mAddCityDocloader->findWidget("counrtyCombobox"));
	addCountryListField();
	
	mOkAction = static_cast<HbAction *> (mAddCityDocloader->findObject("okAction"));
	
	mAddOwnCityDialog->open(this, SLOT(selectedAction(HbAction*)));
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLEADDOWNCITYACTION_EXIT );
}

/*!
	Hanldes Ok action of add own city dialog
 */
void ClockCitySelectionListPrivate::handleOkAction()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLEOKACTION_ENTRY );
	QString cityName = mCityNameEdit->text();

	// Add the city if city name is not empty
	if (!cityName.isEmpty()) {
		int selectedTZOffset =
				mTimeZoneOffsetList.at(mTimeZoneComboBox->currentIndex());

		QString selectedCountry = mCountryComboBox->currentText();
		int selectedCityGroupId(-1);
		int selectedTimeZoneId(-1);
		for (int index=0;index<mCountryList.count();index++) {
			LocationInfo selectedInfo = mCountryList.at(index);
			if(!(selectedCountry.compare(selectedInfo.countryName))) {
				selectedCityGroupId = selectedInfo.cityGroupId;
				selectedTimeZoneId = selectedInfo.timezoneId;
			}
		}

		// Add the new city to the city list.
		LocationInfo addedCity =
				mClient->addCity(selectedTimeZoneId,cityName,selectedCityGroupId);

		if (!addedCity.cityName.compare(cityName)) {
			// Refresh the city list.
			populateListModel();
		}
	}
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLEOKACTION_EXIT );
}

/*!
	Handles the selection of time zone offset.
	Populates the country list.
 */
void ClockCitySelectionListPrivate::handleTimeZoneSelection(int index)
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLETIMEZONESELECTION_ENTRY );
	// Get the selected time zone offset.
	int selectedTimeZoneOffset = mTimeZoneOffsetList.at(index);

	if(mCountryList.count()) {
		mCountryList.clear();
	}

	// Get country list for the offset selected
	mCountryList = mClient->getCountriesForUTCOffset(selectedTimeZoneOffset);
	int countryListCount = mCountryList.count();
	QStringList countries;
	for(int index=0;index<countryListCount;index++) {
		countries.append(mCountryList.at(index).countryName);

	qSort(countries);
	mCountryComboBox->setItems(countries);
	}
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_HANDLETIMEZONESELECTION_EXIT );
}

/*!
	Slot to handle the selected action
 */
void ClockCitySelectionListPrivate::selectedAction(HbAction *action)
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_SELECTEDACTION_ENTRY );
	if (action==mOkAction) {
		handleOkAction();
	}
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_SELECTEDACTION_EXIT );
}

/*!
	Slot to handle the orientation change.
 */
void ClockCitySelectionListPrivate::loadSection(Qt::Orientation orientation)
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_LOADSECTION_ENTRY );
	bool loadSuccess;
	if (mAddOwnCityDialog) {
		if (Qt::Horizontal == orientation) {
			mAddCityDocloader->load(":/xml/clockaddcitydialog.docml",
					"landscape", &loadSuccess);
		} else if (Qt::Vertical == orientation) {
			mAddCityDocloader->load(":/xml/clockaddcitydialog.docml",
					"portrait", &loadSuccess);
		}
	}
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_LOADSECTION_EXIT );
}

/*!
	Slot to handle close action of search panel.
	This slot cancels the search and dismisses the vkb.
 */
void ClockCitySelectionListPrivate::cancelSearch()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_CANCELSEARCH_ENTRY );
	// Clear the search criteria.
	mSearchBox->setCriteria(QString(""));
	
	// Hide the virtual keyborad.	
	QInputContext *ic = qApp->inputContext();
	if (ic) {
		QEvent *event = new QEvent(QEvent::CloseSoftwareInputPanel);
		ic->filterEvent(event);
		delete event;
	}
	
	// Set focus to the first city in the list.
	mListView->scrollTo(mProxyModel->index(0, 0));
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_CANCELSEARCH_EXIT );
}

/*!
    Focuses the line edit when the city selection list is opened.
 */
void ClockCitySelectionListPrivate::focusLineEdit()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_FOCUSLINEEDIT_ENTRY );
	if (mSearchBox) {
		// mView->scene()->setFocusItem(mSearchBox);
		HbLineEdit *searchBoxEditor = 0;
		searchBoxEditor = static_cast<HbLineEdit*>(
				mSearchBox->primitive("lineedit"));

		if (searchBoxEditor) {
			searchBoxEditor->setInputMethodHints(Qt::ImhNoPredictiveText);
/*			HbMainWindow *window = hbInstance->allMainWindows().at(0);
			window->scene()->setFocusItem(searchBoxEditor);*/

			// searchBoxEditor->setCursorPosition(0);
			searchBoxEditor->setFocus();
		}
		
		QInputContext *ic = qApp->inputContext();
		if (ic) {
			QEvent *event = new QEvent(QEvent::RequestSoftwareInputPanel);
			ic->filterEvent(event);
			delete event;
		}
	}
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_FOCUSLINEEDIT_EXIT );
}

/*!
	Displays the city selection list.
 */
void ClockCitySelectionListPrivate::showCityList()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_SHOWCITYLIST_ENTRY );
	// Construct the document loader.
	bool success = false;
	mLoader = new HbDocumentLoader;
	mLoader->load(":/xml/clockcityselectionlist.docml", &success);
	if (!success) {
		qFatal("Unable to load the document.");
	}

	// Get the selection view.
	mView = static_cast<HbView *> (mLoader->findWidget(CITYSELECTIONLISTVIEW));
	if (!mView) {
		qFatal("Unable to get the selection view.");
	}
	// Set the shrinking vkb host to prevent pushing of the list.
	mVirtualKeyboard = new HbShrinkingVkbHost(mView);
	
	// Get the list view.
	mListView = static_cast<HbListView *> (mLoader->findWidget(CITYLISTVIEW));
	if (!mListView) {
		qFatal("Unable to get the list view.");
	}

	// Get the searchPanel.
	mSearchBox = static_cast<HbSearchPanel *> (mLoader->findWidget("lineEdit"));
	if (!mSearchBox) {
		qFatal("Unable to get the searchpanel.");
	}
	mSearchBox->setProgressive(true);
	mSearchBox->setSearchOptionsEnabled(false);

	connect(
			mSearchBox, SIGNAL(criteriaChanged(QString)),
			this, SLOT(updateSearchCriteria(QString)));
	connect(mSearchBox, SIGNAL(exitClicked()),
	        this, SLOT(cancelSearch()));
	
	// Construct the source model.
	if (!mListModel) {
		mListModel = new QStandardItemModel(0, 1, this);
	}
	// Construct the proxy model.
	mProxyModel = new ClockCityListProxyModel(this);
	mProxyModel->setDynamicSortFilter(true);
	mProxyModel->setSourceModel(mListModel);
	mProxyModel->setFilterRole(Qt::UserRole + 100);

	// Construct the model for the list.
	QTimer::singleShot(1, this, SLOT(populateListModel()));

	// Set the model to the list.
	mListView->setModel(mProxyModel);
	mListView->setItemPixmapCacheEnabled(true);
	// Listen for list item activated signal.
	connect(
			mListView, SIGNAL(activated(QModelIndex)),
			this, SLOT(handleItemActivated(QModelIndex)));

	// Read the actions for menu
	mAddOwnCityAction = static_cast<HbAction *>(
			mLoader->findObject(ADDOWNCITYACTION));

	// Listen for menu triggered signal
	connect(
			mAddOwnCityAction, SIGNAL(triggered()),
			this, SLOT(handleAddOwnCityAction()));

	// Add the view to the main window and show it.
	HbMainWindow *window = hbInstance->allMainWindows().at(0);
	mBackAction = new HbAction(Hb::BackNaviAction, this);
	mView->setNavigationAction(mBackAction);
	connect(
			mBackAction, SIGNAL(triggered()),
			this, SLOT(handleBackAction()));

	window->addView(mView);
	window->setCurrentView(mView);
	
	// Focus the search box.
	focusLineEdit();

	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_SHOWCITYLIST_EXIT );
}

/*!
	Get all the time zone offset texts.
 */
QStringList ClockCitySelectionListPrivate::getOffsetTexts()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_GETOFFSETTEXTS_ENTRY );
	int offsetCount(mTimeZoneOffsetList.count());

	// Get all the time zone offsets
	if (!offsetCount) {
		mTimeZoneOffsetList = mClient->getAllTimeZoneOffsets();
		offsetCount = mTimeZoneOffsetList.count();
	}

	QStringList offsetTextList;
	int offsetvalue;
	QString offsetText;
	int hours;
	int minutes;
	HbExtendedLocale locale =  HbExtendedLocale::system();
	for(int index=0;index<offsetCount;index++) {
		offsetvalue = mTimeZoneOffsetList.at(index);
		hours = offsetvalue/60;
		minutes = offsetvalue%60;

		if (hours>0) {
			offsetText += "+";
		} else if (hours<0) {
			offsetText += "-";
		}

		offsetText += locale.format(
				QTime(qAbs(hours),qAbs(minutes),0,0),
				r_qtn_time_durat_short_with_zero);

	offsetTextList.append(offsetText);
	offsetText.clear();
	}
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_GETOFFSETTEXTS_EXIT );
	return offsetTextList;
}

/*!
	Add city name edit field to add own city popup.
 */
void ClockCitySelectionListPrivate::addCityNameField()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_ADDCITYNAMEFIELD_ENTRY );
	mCityNameEdit = new HbLineEdit();
	mCityNameEdit->setFocus(Qt::MouseFocusReason);
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_ADDCITYNAMEFIELD_EXIT );
}

/*!
	Add timezone field to add own city popup.
 */
void ClockCitySelectionListPrivate::addTimeZoneField()
{
OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_ADDTIMEZONEFIELD_ENTRY );
//	mTimeZoneComboBox = new HbComboBox();

	QStringList texts = getOffsetTexts();
	mTimeZoneComboBox->addItems(texts);

	// Get the home city information.
	LocationInfo homeCityInfo = mClient->getCurrentZoneInfoL();
	mCurrentTZOffsetIndex = mTimeZoneOffsetList.indexOf(
			homeCityInfo.zoneOffset);
	mTimeZoneComboBox->setCurrentIndex(mCurrentTZOffsetIndex);

	connect(
			mTimeZoneComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(handleTimeZoneSelection(int)));
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_ADDTIMEZONEFIELD_EXIT );
}

/*!
	Add country list field to add own city popup.
 */
void ClockCitySelectionListPrivate::addCountryListField()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLISTPRIVATE_ADDCOUNTRYLISTFIELD_ENTRY );
	// Populate the country list based on the current TZ offset selected.
	handleTimeZoneSelection(mCurrentTZOffsetIndex);
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLISTPRIVATE_ADDCOUNTRYLISTFIELD_EXIT );
}

// End of file	--Don't remove this.
