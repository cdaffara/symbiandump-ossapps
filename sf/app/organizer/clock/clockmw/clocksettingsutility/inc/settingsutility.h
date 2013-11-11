/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition file for the class SettingsUtility
*
*/



#ifndef SETTINGSUTILITY_H
#define SETTINGSUTILITY_H

#include <QObject>
#include <QStringList>

#include "clockdatatypes.h"

#ifdef  SETTINGSUTILITY_LIBRARY
#define SETTINGSUTILITY_EXPORT Q_DECL_EXPORT
#else
#define SETTINGSUTILITY_EXPORT Q_DECL_IMPORT
#endif

class TimezoneClient;
class XQSettingsKey;
class XQSettingsManager;

class SETTINGSUTILITY_EXPORT SettingsUtility : public QObject
{
	Q_OBJECT

public:
	SettingsUtility(QObject *parent = 0);
	~SettingsUtility();

public:
//	void setTime(const QString &time);
	QString time();

/*	void setTimeZone(const QString &timezone);
	QString timeZone();*/

	void setTimeFormat(const QString &format);
	int timeFormat(QStringList &format);

	void setClockType(const QString &type);
	int clockType(QStringList &list);

	void setTimeSeparator(const QString &separator);
	int timeSeparator(QStringList &list);

/*	void setAutoUpdate(const QString &value);
	int autoUpdate(QStringList &list);*/

//	void setDate(const QString &date);
	QString date();

	void setDateFormat(const QString &format);
	int dateFormat(QStringList &format);

	void setDateSeparator(const QString &format);
	int dateSeparator(QStringList &seprator);

	void setWorkdays(const QString &workdays);
	int workdays(QStringList &workdaysList);

	QString timeFormatString();
	QString dateFormatString();

	void setSnoozeTime(int index);
	int snoozeTime(QStringList &snoozeValueList);

	void setStartOfWeek(int index);

signals:
	void settingsChanged(SettingsItemChanged item, const QString data);

private:
	QStringList mTimeSeparatorList;
	QStringList mClockTypeList;
	QStringList mTimeFormatList;
	QStringList mDisplayDateFormatList;
	QStringList mDateFormatList;
	QStringList mDateSeparatorList;
	QStringList mAutoUpdateValueList;
	QStringList mSnoozeValueList;
	
	XQSettingsManager *mSettingsManager;
	XQSettingsKey *mClockTypeSettingsKey;
};

#endif // SETTINGSUTILITY_H

// End of file
