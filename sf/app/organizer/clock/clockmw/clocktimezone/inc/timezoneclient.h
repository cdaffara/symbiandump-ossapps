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

// timezoneclient.h
#ifndef __TIMEZONECLIENT_H__
#define __TIMEZONECLIENT_H__

// System includes
#include <qglobal.h>
#include <QtGui>
#include <QList>
#include <QDateTime>
#include <QMap>

#include <tz.h>

// User includes
#include "clockdatatypes.h"

#ifdef  TIMEZONECLIENT_LIBRARY
#define TIMEZONECLIENT_EXPORT Q_DECL_EXPORT
#else
#define TIMEZONECLIENT_EXPORT Q_DECL_IMPORT
#endif

//class CEnvironmentChangeNotifier;
class QStandardItemModel;
class CTzLocalizer;
class XQSettingsManager;
class XQSettingsKey;
class EnvironmentChangeNotifier;

class TimezoneClient : public QObject
{
	Q_OBJECT

public:
	TIMEZONECLIENT_EXPORT static TimezoneClient* getInstance();
	TIMEZONECLIENT_EXPORT void deleteInstance();
	TIMEZONECLIENT_EXPORT bool isNull();

public:
	TIMEZONECLIENT_EXPORT QList<LocationInfo> &getLocations();
	TIMEZONECLIENT_EXPORT LocationInfo getCurrentZoneInfoL();
	TIMEZONECLIENT_EXPORT void setAsCurrentLocationL(LocationInfo &location);
	TIMEZONECLIENT_EXPORT bool isDSTOnL(int timezoneId);
	TIMEZONECLIENT_EXPORT int getStandardOffset(int timezoneId);
	TIMEZONECLIENT_EXPORT bool timeUpdateOn();
	TIMEZONECLIENT_EXPORT void setTimeUpdateOn(bool timeUpdate = true);
	TIMEZONECLIENT_EXPORT void setDateTime(QDateTime dateTime);
	TIMEZONECLIENT_EXPORT QList<int> getAllTimeZoneOffsets();
	TIMEZONECLIENT_EXPORT QList<LocationInfo> getCountriesForUTCOffset(
			int utcOffset);
	TIMEZONECLIENT_EXPORT LocationInfo addCity(
			int timeZoneId, QString &cityName, int cityGroupId);
	TIMEZONECLIENT_EXPORT bool checkForDstChange(AlarmInfo& alarmInfo);
	TIMEZONECLIENT_EXPORT QList<LocationInfo> getSavedLocations();
	TIMEZONECLIENT_EXPORT void saveLocations(
			const QList<LocationInfo> &locationList);
	
	void notifyTimeChange();

signals:
	void timechanged();
	void listUpdated();
	void autoTimeUpdateChanged(int value);
	void cityUpdated();

private:
	TimezoneClient();
	~TimezoneClient();
	
private:
	bool dstOn(int tzId);	
	QList<int> getAllTimeZoneIds();
	int getDstZoneOffset(int tzId);
	TMonth intToMonth(int month);
	bool getUtcDstOffsetL(int &dstOffset, const CTzId &tzId);
	void getDstRulesL(
			QDateTime &startTime, QDateTime &endTime,int timezoneId);
	void getCitiesForCountry(
			int id, QMap<QString, int>& cities);
	void getLocationInfo(
			int groupId, int cityIndex, LocationInfo& cityInfo);
	int getCityGroupIdByName(const QString& name);
	int getCityOffsetByNameAndId(const QString& name, int tzId);

private slots:
	void populateCities();
	void eventMonitor(const XQSettingsKey& key, const QVariant& value);

private:
	static TimezoneClient *mTimezoneClient;
	static bool mReferenceCount;
	
private:
	EnvironmentChangeNotifier *mNotifier;
	CTzLocalizer *mTzLocalizer;
	QMap<QString, int> mAllCountries;
	QList<LocationInfo> mAllLocations;
	QPointer<QStandardItemModel> mWorldClockModel;
	XQSettingsManager *mSettingsManager;
	XQSettingsKey *mAutoTimeUpdateKey;

	QList<int> mTimeZoneIds;

	int mCountryCount;
	int mAutoTimeUpdateValue;
};

#endif		// __TIMEZONECLIENT_H__

// End of file
