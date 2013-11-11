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
* Header file for class ClockRegionalSettingsView.
*
*/

#ifndef CLOCKREGIONALSETTINGSVIEW_H
#define CLOCKREGIONALSETTINGSVIEW_H

// System includes
#include <QObject>
#include <QPointer>
#include <QModelIndex>
// User includes
#include "settingsutility.h"
#include "clocksettingsdefines.h"

// Forward declarations
class HbView;
class HbAction;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbListWidget;
class HbComboBox;
class HbTranslator;
class ClockSettingsDocLoader;
class SettingsCustomItem;


class ClockRegionalSettingsView : public QObject
{
	Q_OBJECT

public:
	CLOCKSETTINGSVIEW_EXPORT ClockRegionalSettingsView(QObject *parent = 0, bool launchedByClock = 1);
	CLOCKSETTINGSVIEW_EXPORT virtual ~ClockRegionalSettingsView();

public:
	CLOCKSETTINGSVIEW_EXPORT void showView();
public slots:
    void updateWeekStartOn();
    void updateOnLocaleChanges();
private slots:
	void handleBackAction();
	void handleItemDisplayed(const QModelIndex &index);

	void handleTimeFormatChange();
	void handleTimeSeparatorChange();
	void handleDateFormatChange(QString);
	void handleDateSeparatorChange(QString);
	void handleDataChanged(
			const QModelIndex& topLeft, const QModelIndex& bottomRight);

private:
	void createModel();
	void populateFormModel();
	QStringList weekdayList();
	void updateWeekDays();
	void updateTimeFormatField();
	void updateTimeSeparatorField();
	void updateDateFormatField();
	void updateDateSeparatorField();


private:
	QStringList mTimeFormatStringList;
	QStringList mTimeSeparatorStringList;
	QStringList mDateFormatStringList;
	QStringList mDateSeparatorStringList;

	HbView *mView;
	QPointer<HbDataForm> mForm;
	QPointer<HbDataFormModel> mFormModel;

	HbDataFormModelItem *mTimeFormatItem;
	HbDataFormModelItem *mTimeSeparatorItem;
	HbDataFormModelItem *mDateFormatItem;
	HbDataFormModelItem *mDateSeparatorItem;
	QPointer<HbComboBox> mStartOfWeekItem;
	QPointer<HbListWidget> mWorkdaysItem;

	ClockSettingsDocLoader *mLoader;
	SettingsUtility *mSettingsUtility;
	SettingsCustomItem *mCustomPrototype;
    HbTranslator *mTranslator;
    bool mLauncedByClock;
};

#endif // CLOCKREGIONALSETTINGSVIEW_H

// End of file	--Don't remove this.
