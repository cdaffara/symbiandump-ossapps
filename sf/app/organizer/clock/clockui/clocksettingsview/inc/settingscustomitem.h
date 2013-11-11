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

#ifndef SETTINGSCUSTOMITEM_H
#define SETTINGSCUSTOMITEM_H

// System includes
#include <QPointer>
#include <HbDataFormViewItem>
#include <HbDialog>

// User includes
#include "clockdatatypes.h"

// Forward declarations
class SettingsUtility;
class ClockCitySelectionList;
class TimezoneClient;
class HbPushButton;
class HbListWidget;

class SettingsCustomItem : public HbDataFormViewItem
{
	Q_OBJECT

public:
	SettingsCustomItem(QGraphicsItem *parent = 0, bool launchedByClock = 1);
	virtual ~SettingsCustomItem();

public:
	virtual HbAbstractViewItem* createItem();
	virtual bool canSetModelIndex(const QModelIndex &index) const;
	void restore();
	void setWeekdaysList(QStringList list);
	QString workdaysSetting();

protected:
	virtual HbWidget* createCustomWidget();

private slots:
	void launchTimePicker();
	void handleOkAction();
	void launchDatePicker();
	void launchCitySelectionList();
	void updatePlaceItem(LocationInfo info);
	void launchRegSettingsView();
	void selectedAction(HbAction *action);

private:
	QStringList mWeekdaysList;

	QPointer<HbDialog> mTimePickerDialog;
	QPointer<HbDialog> mDatePickerDialog;
	QPointer<HbPushButton> mTimeWidget;
	QPointer<HbPushButton> mDateWidget;
	QPointer<HbPushButton> mPlaceWidget;
	QPointer<HbPushButton> mRegSettingsWidget;
	QPointer<HbListWidget> mWorkdaysWidget;

	HbAction *mOkAction;
	HbAction *mCancelAction;

	SettingsUtility *mSettingsUtility;
	ClockCitySelectionList *mCitySelectionList;
	TimezoneClient *mTimezoneClient;
	bool mLaunchedByClock;
};

#endif // SETTINGSCUSTOMITEM_H

// End of file	--Don't remove this.
