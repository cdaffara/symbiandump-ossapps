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

#ifndef CALENSETTINGS_H
#define CALENSETTINGS_H

#include <QObject>
#include <QHash>

class QPersistentModelIndex;
class QModelIndex;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbComboBox;
class HbPushButton;
class HbAbstractButton;
class HbLineEdit;
class XQSettingsManager;
class XQSettingsKey;
class MCalenServices;


#ifdef  CALENSETTINGS_DLL
#define CALENSETTINGS_EXPORT Q_DECL_EXPORT
#else
#define CALENSETTINGS_EXPORT Q_DECL_IMPORT
#endif

class CALENSETTINGS_EXPORT CalenSettings : public QObject
{
	Q_OBJECT

public:
    CalenSettings(MCalenServices& services, HbDataForm *form, QObject *parent = 0);
    ~CalenSettings();

    void createModel();
    void populateSettingList();
    void addWeekNumberItem();
    bool isPluginEnabled();
    
private:
    void updateShowWeekItem();
    void addRegionalInfoItem();
    void removeRegionalInfoItem();
    void populateRegionalInfo();

public slots:
    void handleAlarmSnoozeTimeChange(int index);
    void handleWeekNumberChange();
    void handleRegionalInfoChange();
    //set the start day of the week to the locale 
    void setStartDayOfWeek(const int index);

private:
    MCalenServices  &mServices;
	HbDataForm *mSettingsForm;
	HbDataFormModel *mSettingsModel;
	HbDataFormModelItem *mAlarmSnoozeTimeItem;
	HbDataFormModelItem* mShowWeekNumberItem;
	HbDataFormModelItem* mShowRegionalInfoItem;
	HbDataFormModelItem* mShowWeekStartOnInfoItem;
	HbComboBox* mStartOfWeekItem;
	QHash<int, int> mAlarmSnoozeTimeHash;
	XQSettingsManager *mSettingsManager;
	XQSettingsKey *mAlarmSnoozeCenrepKey;
	XQSettingsKey *mWeekNumberCenrepKey;
	XQSettingsKey *mShowRegionalInfoKey;
	int mStartOfWeek;
	bool mWeekNumberItemAdded;
	bool mRegionalpluginActive;
};

#endif // CALENSETTINGS_H

// End of file
