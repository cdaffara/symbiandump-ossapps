/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* The header file for CalenEditorRepeatField class.
*
*/

#ifndef CALENEDITORREPEATFIELD_H
#define CALENEDITORREPEATFIELD_H

// System includes
#include <QObject>
#include <qglobal.h>

// User Includes
#include "caleneditor_p.h"


// FORWARD DECLARATIONS
class QDate;
class HbDataFormModelItem;
class HbComboBox;
class HbDateTimePicker;
class CalenEditorPrivate;

class CalenEditorRepeatField : public QObject
{
	Q_OBJECT
	
public:
	CalenEditorRepeatField(CalenEditorPrivate* calenEditor,
								HbDataForm* form, 
								HbDataFormModel* model,
								QObject *parent = NULL);
	virtual ~CalenEditorRepeatField();
	
public:
	void addItemToModel();
	void removeItemFromModel();
	void populateRepeatItem(int index);
	QModelIndex modelIndex();
	void updateRepeatChoices();
	void saveRepeatRule();
	bool isRepeatUntilItemAdded();
	QDate repeatUntilDate();
	void refreshRepeatUntilDate();

private:
	void insertRepeatUntilItem();
	void removeRepeatUntilItem();
	
private slots:
	void handleRepeatIndexChanged(int index);
	void launchRepeatUntilDatePicker();
	void setRepeatUntilDate();
	
private:
	enum RepeatTypes {
		RepeatOnce,
		RepeatDaily,
		RepeatWorkdays,
		RepeatWeekly,
		RepeatBiWeekly,
		RepeatMonthly,
		RepeatYearly
	};
	enum RepeatRoles {
		DailyRole = 1,
		WorkdaysRole,
		WeeklyRole,
		BiWeeklyRole,
		MonthlyRole,
		YearlyRole
	};
	CalenEditorPrivate* mCalenEditor;
	HbDataForm* mEditorForm;
	HbDataFormModel* mCalenEditorModel;
	HbDataFormModelItem* mRepeatItem;
	HbComboBox *mRepeatComboBox;
	HbDataFormModelItem *mCustomRepeatUntilItem;
	QPointer<HbDateTimePicker> mDatePicker;
	QDate mRepeatUntilDate;
	AgendaRepeatRule::RuleType mRepeatRuleType;
	int mRepeatRoleValue;
	bool mIsBiWeekly;
	bool mIsWorkdays;
	bool mRepeatUntilItemAdded;
};

#endif // CALENEDITORREPEATFIELD_H

// End of file	--Don't remove this.

