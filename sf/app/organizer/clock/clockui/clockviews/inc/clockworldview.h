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
*
*/

#ifndef CLOCKWORLDVIEW_H
#define CLOCKWORLDVIEW_H

// System includes
#include <QObject>
#include <QPointer>
#include <QModelIndex>
#include <HbView>

// User includes
#include "clockviewsdefines.h"
#include "clockdatatypes.h"

// Forward declarations
class QGraphicsItem;
class QStandardItemModel;
class QTimer;
class HbListView;
class HbListWidgetItem;
class HbAbstractViewItem;
class ClockDocLoader;
class ClockAppControllerIf;
class TimezoneClient;
class SettingsUtility;
class ClockCitySelectionList;
class HbGroupBox;
class ClockHomeCityItem;

class ClockWorldView : public HbView
{
	Q_OBJECT

public:
	CLOCKVIEWS_EXPORT ClockWorldView(QGraphicsItem *parent = 0);
	CLOCKVIEWS_EXPORT ~ClockWorldView();

public:
	CLOCKVIEWS_EXPORT void setupView(
			ClockAppControllerIf &controllerIf, ClockDocLoader *docLoader);

private slots:
	void refreshCityList();
	void updateCurrentLocationInfo(int networkTime);
	void handleItemLongPressed(HbAbstractViewItem *item, const QPointF &coord);
	void handleAddLocation();
	void handleDeleteAction();
	void handleSetAsCurrentLocationAction();
	void handleCitySelected(LocationInfo info);
	void showAlarmsView();
	void refreshWorldView();
	void loadSection(Qt::Orientation orientation);
	void updateAllLocationInfo();
	void selectedMenuAction(HbAction *action);

private:
	QModelIndex addCityToList(const LocationInfo& locationInfo);
	bool isDay(QDateTime dateTime);
	QVariantList getCityListDisplayString(const LocationInfo& locationInfo);
	QVariantList getCityListDecorationString(const LocationInfo& locationInfo);
	void updateCityList();

private:
	QTimer *mRefreshTimer;
	QList<LocationInfo> mCityInfoList;
	QStandardItemModel *mCityListModel;

	QPointer<HbListView> mCityListView;
	ClockHomeCityItem *mHomeCityWidget;
	HbAction *mAddCityAction;
	HbAction *mSetCurrentLocationAction;
	HbAction *mRemoveCityAction;
	HbAction *mBackAction;
	HbAction *mShowAlarmsViewAction;
	HbAction *mDisplayWorldClockView;
	HbAction *mAddCityMenuAction;

	ClockDocLoader *mDocLoader;
	ClockAppControllerIf *mAppControllerIf;
	SettingsUtility *mSettingsUtility;
	TimezoneClient *mTimezoneClient;
	ClockCitySelectionList *mCitySelectionList;

	int mSelectedItem;
};

#endif // CLOCKWORLDVIEW_H

// End of file
