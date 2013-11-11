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
* The header file for class ClockCitySelectionListPrivate.
*
*/

#ifndef CLOCKCITYSELECTIONLIST_P_H
#define CLOCKCITYSELECTIONLIST_P_H

// System includes
#include <QObject>
#include <QPointer>
#include <QModelIndex>

#include "clockdatatypes.h"

// Forward declarations
class QStandardItemModel;
class HbDocumentLoader;
class HbView;
class HbListView;
class HbAction;
class HbSearchPanel;
class HbLineEdit;
class HbDialog;
class HbComboBox;
class HbShrinkingVkbHost;
class ClockCitySelectionList;
class ClockCityListProxyModel;
class TimezoneClient;

class ClockCitySelectionListPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(ClockCitySelectionList)

public:
	explicit ClockCitySelectionListPrivate(
			TimezoneClient *client, QObject *parent);
	virtual ~ClockCitySelectionListPrivate();

private slots:
	void populateListModel();
	void handleItemActivated(const QModelIndex &index);
	void handleBackAction();
	void closeCityList();
	void updateSearchCriteria(const QString &criteria);
	void handleAddOwnCityAction();
	void handleOkAction();
	void handleTimeZoneSelection(int index);
	void selectedAction(HbAction *action);
	void loadSection(Qt::Orientation orientation);
	void cancelSearch();

private:
	void showCityList();
	QStringList getOffsetTexts();
	void addCityNameField();
	void addTimeZoneField();
	void addCountryListField();
	void focusLineEdit();
	
private:
	QPointer<QStandardItemModel> mListModel;

	HbDocumentLoader *mLoader;
	HbDocumentLoader *mAddCityDocloader;
	QPointer<HbView> mView;
	QPointer<HbListView> mListView;
	HbAction *mBackAction;
	QPointer<HbSearchPanel> mSearchBox;
	QPointer<HbAction> mAddOwnCityAction;
	QPointer<HbDialog> mAddOwnCityDialog;
	QPointer<HbLineEdit> mCityNameEdit;
	QPointer<HbComboBox> mTimeZoneComboBox;
	QPointer<HbComboBox> mCountryComboBox;
	
	QPointer<HbShrinkingVkbHost> mVirtualKeyboard;

	HbAction *mOkAction;
	HbAction *mCancelAction;

	ClockCitySelectionList *q_ptr;
	TimezoneClient *mClient;
	ClockCityListProxyModel *mProxyModel;

	QList<int> mTimeZoneOffsetList;
	QList<LocationInfo> mCountryList;

	bool mOwnsClient;
	int mCurrentTZOffsetIndex;

};

#endif // CLOCKCITYSELECTIONLIST_P_H

// End of file	--Don't remove this.
