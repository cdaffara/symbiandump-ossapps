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
* Description:  CalenMonthView class definition.
*
*/

#ifndef CALENMONTHVIEW_H  
#define CALENMONTHVIEW_H

// System includes
#include <QGraphicsLinearLayout>
#include <qdatetime.h>
#include <hblabel.h>
#include <hbextendedlocale.h>
#include <hbframeitem.h>

// User includes
#include "calennativeview.h"

// Forward declarations
class QStandardItemModel;
class HbAction;
class HbGridView;
class HbExtendedLocale;
class XQSettingsManager;
class XQSettingsKey;
class AgendaUtil;
class AgendaEntry;
class MCalenContext;
class CalenMonthData;
class MCalenServices;
class CalenPreviewPane;
class CalenDocLoader;
class CalenThickLinesDrawer;
class CalenMonthGrid;
class CalenPluginLabel;

// Constants
const int KNumOfVisibleRows = 6;

#ifdef  CALENVIEWS_DLL
#define CALENMONTHVIEW_EXPORT Q_DECL_EXPORT
#else
#define CALENMONTHVIEW_EXPORT Q_DECL_IMPORT
#endif

class CALENMONTHVIEW_EXPORT CalenMonthView : public CalenNativeView
{
	Q_OBJECT
	
public:
	CalenMonthView( MCalenServices &services );
	virtual ~CalenMonthView();
	
public:  // From CCalenView
	virtual void doPopulation();
	void setupView(CalenDocLoader *docLoader);
	void doLazyLoading();
	void handleGridItemActivated();
	void setContextForActiveDay(int index);
	QDateTime getActiveDay();
	int rowsInPrevMonth();
	int rowsInFutMonth();
	int getCurrGridIndex();
	void setCurrGridIndex(int index);
	void populatePrevMonth();
	void populateNextMonth();
	void onLocaleChanged(int reason);
	QList<CalenMonthData>& monthDataList();
	void updateModelWithPrevMonth();
	void updateModelWithFutureMonth();
	void populatePreviewPane(QDateTime &dateTime);
	void handlePreviewPaneGesture(bool rightGesture);
	QDateTime firstDayOfGrid();
	void fetchEntriesAndUpdateModel();
	void launchDayView();
	void disconnectAboutToQuitEvent();
	
private:
	void createGrid();
	void populateWithInstanceView();
	void completePopulation();
	void prepareForPopulation();
	QDateTime dateFromContext( const MCalenContext &context );
	void setActiveDay(QDateTime day);
	void setDate();
	void getInstanceList(QList<QDate> &list,
                         QDateTime rangeStart, QDateTime rangeEnd);
	void handleChangeOrientation();
	void setDateToLabel();
	void updateWeekNumGridModel();
	void addWeekNumbers();
	void removeWeekNumbers();
	void addBackgroundFrame();
	void refreshViewOnGoToDate();
	void showHideRegionalInformation();
	void onContextChanged();
	void updateDayLabel();
	
private slots:
	void createEditor();
	void goToToday();
	void updateMonthDataArrayWithActiveDates();
	void handleLeftEffectCompleted(const HbEffect::EffectStatus &status);
	void handleRightEffectCompleted(const HbEffect::EffectStatus &status);
	void addRemoveActionsInMenu();
	void changeOrientation(Qt::Orientation orientation);
	void handleThemeChange();
	
private:
	bool mIsFirstTimeLoad;
	CalenDocLoader *mDocLoader;
	CalenThickLinesDrawer *mDayNameWidget;
	CalenMonthGrid *mMonthGrid;
	HbWidget* mPrevPaneParent;
	HbWidget* mPrevPaneLayoutWidget;
	QGraphicsLinearLayout* mPrevPaneLayout;
	HbWidget* mCurrPaneParent;
	HbWidget* mCurrPaneLayoutWidget;
	QGraphicsLinearLayout* mCurrPaneLayout;
	HbWidget* mNextPaneParent;
	HbWidget* mNextPaneLayoutWidget;
	QGraphicsLinearLayout* mNextPaneLayout;
	CalenPreviewPane* mCurrPreviewPane;
	CalenPreviewPane* mPrevPreviewPane;
	CalenPreviewPane* mNextPreviewPane;
	CalenThickLinesDrawer *mWeekNumberWidget;
	HbWidget *mMonthGridPlusWeekNumWidget;
	HbExtendedLocale mLocale;
	int mIndexToBeScrolled;
	QDateTime mActiveMonth;
	int mNumOfRowsInPrevMonth;
	int mNumOfRowsInFutureMonth;
	HbLabel *mFirstDayLabel;
	HbLabel *mSecondDayLabel;
	HbLabel *mThirdDayLabel;
	HbLabel *mFourthDayLabel;
	HbLabel *mFifthDayLabel;
	HbLabel *mSixthDayLabel;
	HbLabel *mSeventhDayLabel;
	HbLabel *mFirstWeekLabel;
	HbLabel *mSecondWeekLabel;
	HbLabel *mThirdWeekLabel;
	HbLabel *mFourthWeekLabel;
	HbLabel *mFifthWeekLabel;
	HbLabel *mSixthWeekLabel;
	QList<CalenMonthData> mMonthDataArray;
	QDateTime mDate;
	QDateTime mFirstDayOfGrid;
	QDateTime mLastDayOfGrid;
	int mTotalNumOfGridItems;
	QList<int> mWeekNumbers;
	HbLabel *mTitleLabel;
	Qt::Orientation mOrientation;
	XQSettingsManager *mSettingsManager;
	XQSettingsKey *mWeekNumberCenrepKey;
	uint mIsWeekNumbersShown;
	AgendaUtil *mAgendaUtil;
	bool mIsPrevPaneGesture;
	HbAction *mGoToTodayAction;
	CalenPluginLabel *mPrevRegionalInfo;
	CalenPluginLabel *mCurrRegionalInfo;
	CalenPluginLabel *mNextRegionalInfo;
	HbMenu *mDeleteSubMenu;
	QColor mWeekDaysColor;
	bool   mIsAboutToQuitEventConnected; // bool to check if month view is registered to get aboutToQuit signals
	QColor mPreviewHeadingColor;
};

#endif //CALENMONTHVIEW_H

// End of file  --Don't remove this.
