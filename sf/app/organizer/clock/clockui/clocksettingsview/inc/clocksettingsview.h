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

#ifndef CLOCKSETTINGSVIEW_H
#define CLOCKSETTINGSVIEW_H

// System includes
#include <QObject>
#include <QPointer>
#include <HbView>

// User includes
#include "clocksettingsdefines.h"

// Forward declarations
class HbDataForm;
class HbAction;
class HbDataFormModel;
class HbDataFormModelItem;
class HbLabel;
class TimezoneClient;
class SettingsUtility;
class ClockSettingsDocLoader;
class ClockSettingsView;
class HbTranslator;
class XQSettingsManager;
class XQSettingsKey;

class ClockSettingsView : public QObject
{
	Q_OBJECT

public:
	CLOCKSETTINGSVIEW_EXPORT ClockSettingsView(QObject *parent = 0, HbTranslator *translator = 0,
	        bool launchedByClock = 1);
	CLOCKSETTINGSVIEW_EXPORT virtual ~ClockSettingsView();

public:
	CLOCKSETTINGSVIEW_EXPORT void loadSettingsView();

private slots:
	void handleBackAction();
	void updatePlaceItem();
	void updateDateItem();
	void updateTimeItem();
	void handleOrientationChanged(Qt::Orientation orientation);
	void handleNetworkTimeStateChange(int state);
	void handleAutoTimeUpdateChange(int value);
	void handleAlarmSnoozeTimeChanged(int index);
	void handleClockTypeChanged();
	void eventMonitor(const XQSettingsKey& key, const QVariant& value);
	void updateClockType();

private:
	void setupView();
	void createModel();
	void populateModel();

private:
	QTimer *mTickTimer;

	HbDataForm *mSettingsForm;
	HbAction *mBackAction;

	HbView *mSettingsView;

	QPointer<HbDataFormModel> mSettingsModel;

	HbDataFormModelItem *mNetworkTimeItem;
	HbDataFormModelItem *mTimeDataFormItem;
	HbDataFormModelItem *mDateDataFormItem;
	HbDataFormModelItem *mPlaceDataFormItem;
	HbDataFormModelItem *mRegionalDataFormItem;
	HbDataFormModelItem *mClockTypeItem;
	HbDataFormModelItem *mAlarmSnoozeItem;
	ClockSettingsDocLoader *mDocLoader;
	SettingsUtility *mSettingsUtility;
	TimezoneClient *mTimezoneClient;	
	XQSettingsManager *mSettingsManager;
	XQSettingsKey *mAlarmSnoozeTimeKey;
	XQSettingsKey *mClockTypeKey;
	QHash<int, int> mAlarmSnoozeTimeHash;
	HbTranslator *mTranslator;
	bool mLaunchedByClock;
};

#endif // CLOCKSETTINGSVIEW_H

// End of file	--Don't remove this.
