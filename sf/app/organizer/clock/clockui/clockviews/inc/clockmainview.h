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
* Header file for ClockMainView class.
*
*/

#ifndef CLOCKMAINVIEW_H
#define CLOCKMAINVIEW_H

// System includes
#include <QObject>
#include <HbView>

// User includes
#include "clockviewsdefines.h"
#include "clockdatatypes.h"

// Forward declarations
class QGraphicsItem;
class QTimer;
class HbLabel;
class HbListView;
class HbAbstractViewItem;
class ClockDocLoader;
class ClockAppControllerIf;
class AlarmClient;
class TimezoneClient;
class SettingsUtility;
class ClockAlarmListModel;
class ClockWidget;
class XQSettingsManager;
class XQSettingsKey;
class AfActivityStorage;

class ClockMainView : public HbView
{
	Q_OBJECT

public:
	CLOCKVIEWS_EXPORT ClockMainView(QGraphicsItem *parent = 0);
	CLOCKVIEWS_EXPORT virtual ~ClockMainView();

public:
	CLOCKVIEWS_EXPORT void setupView(
			ClockAppControllerIf &controllerIf, ClockDocLoader *docLoader);
	CLOCKVIEWS_EXPORT void setupAfterViewReady();
	CLOCKVIEWS_EXPORT void captureScreenShot(bool captureScreenShot = false);

private slots:
	void handleAlarmStatusChanged(int row);
	void refreshMainView();
	void displayWorldClockView();
	void addNewAlarm();
	void openSettings();
	void handleActivated(const QModelIndex &index);
	void handleLongPress(HbAbstractViewItem *item, const QPointF &coords);
	void deleteAlarm();
	void updateView();
	void updatePlaceLabel(int autoTimeUpdate = -1);
	void handleAlarmListDisplay();
	void checkOrientationAndLoadSection(Qt::Orientation orientation);
	void selectedMenuAction(HbAction *action);
	void handleMenuClosed();
    void saveActivity();
    void eventMonitor(const XQSettingsKey& key, const QVariant& value);

private:
	void setmodel();
	void hideAlarmList(bool show);
	void removeSnoozedAlarm();
	void updateDateLabel();
	void updateClockWidget();
	bool removeActivity();

private:
	QTimer *mTickTimer;
	QColor mLabelColorGroup;

	HbAction *mRefreshMainView;
	HbAction *mDisplayWorldClockView;
	HbAction *mAddNewAlarm;
	HbAction *mSettingsAction;
	HbAction *mDeleteAction;

	HbLabel *mDayLabel;
	HbLabel *mPlaceLabel;
	HbLabel *mDstIcon;
	HbLabel *mHorizontalDivider;
	HbLabel *mVerticalDivider;
	ClockWidget *mClockWidget;
	HbLabel *mNoAlarmLabel;
	HbListView *mAlarmList;

	TimezoneClient *mTimezoneClient;
	AlarmClient *mAlarmClient;
	SettingsUtility *mSettingsUtility;
	ClockDocLoader *mDocLoader;
	ClockAppControllerIf *mAppControllerIf;
	ClockAlarmListModel *mAlarmListModel;
	
	XQSettingsManager *mSettingsManager;
	XQSettingsKey *mClockTypeSettingsKey;
	
	AfActivityStorage *mActivityStorage;

	int            mSelectedItem;
	bool           mHideAlarmList;
	bool           mIsLongTop;
	bool           mIsScreenShotCapruted; // check if the screenshot captured is valid
	QVariantHash   mScreenShot; // screenshot
};

#endif // CLOCKMAINVIEW_H

// End of file	--Don't remove this.
