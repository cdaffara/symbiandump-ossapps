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
 * Description: Header file for class CalenEditorCustomItem.
 *
 */

#ifndef CALENEDITORCUSTOMITEM_H
#define CALENEDITORCUSTOMITEM_H

// System includes
#include <QObject>
#include <QDateTime>
#include <hbdataformviewitem.h>
#include <hbextendedlocale.h>
#include <hblineedit.h>

class XQApplicationManager;
class XQAiwRequest;

class HbDateTimePicker;
class HbPushButton;
class HbDataFomViewer;
class AgendaEntry;

class CalenEditorCustomItem : public HbDataFormViewItem
{
	Q_OBJECT
	
public:
	CalenEditorCustomItem(QGraphicsItem *parent = 0);
	~CalenEditorCustomItem();
	virtual HbAbstractViewItem* createItem();
	void enableFromTimeFieldAndSetTime(bool, QDateTime);
	void disableFromToDateField();
	void enableToTimeFieldAndSetTime(bool, QDateTime);	
	void populateDateTime(QDateTime defaultDateTime, bool isFromItem);
	void populateLocation( QString location);
	void setDateRange(QDate start, QDate end);
	void setTimeRange(QTime start, QTime end);
	void enableDateButton(bool value);
	bool canSetModelIndex(const QModelIndex &index) const;
	void restore();
	QDateTime getDateTime();
	
protected:
	virtual HbWidget* createCustomWidget();
	
protected slots:
	void handleDate();
	void handleTime();
	void saveTime();
	void saveDate();
	void handleLocationTextChange(QString location);
    void launchLocationPicker();
    void handleEditingFinished();
    void setSelectedLocation( const QVariant &aValue );
	
Q_SIGNALS:
	void dateTimeUpdated(QDateTime& fromDateTime);
	void locationTextChanged(QString location);
	void locationTextChanged(QString location, double latitude, double longitude);
	void locationEditingFinished();	
private:
	AgendaEntry *mEditedEntry;
	QDate mDate;
	QDate mMaxDate;
	QDate mMinDate;
	QTime mTime;
	QTime mMaxTime;
	QTime mMinTime;
	
	XQApplicationManager *mAppManager;
	XQAiwRequest* mRequest;
	
	HbPushButton* mPushButtonTime;
	HbPushButton* mPushButtonDate;
	HbPushButton *mRepeatUntilWidget;
	HbPushButton *mReminderTimeWidget;

	QPointer<HbDateTimePicker> mDatePicker;
	QPointer<HbDateTimePicker> mTimePicker;

	HbExtendedLocale mLocale;
	
	HbLineEdit *mLocationLineEdit; 
	HbPushButton* mLocationPushButton;
	bool mIsFromItem;
	bool mRequestPending;
	
};

#endif // CALENEDITORCUSTOMITEM_H

// End of file	--Don't remove this.
