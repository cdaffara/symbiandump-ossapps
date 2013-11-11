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
* Description:  CalenMonthGrid class definition.
*
*/

#ifndef CALENMONTHGRID_H
#define CALENMONTHGRID_H

// System includes
#include <hbview.h>
#include <QGraphicsWidget>
#include <QAbstractItemModel>
#include <QtGui>
#include <hbgridviewitem.h>
#include <hbgridview.h>
#include <hbframebackground.h>
#include <hbframedrawer.h>

// Forward declarations
class QStandardItemModel;
class CalenGridItemPrototype;
class CalenMonthData;
class CalenMonthView;

#ifdef  CALENVIEWS_DLL
#define CALENGRID_EXPORT Q_DECL_EXPORT
#else
#define CALENGRID_EXPORT Q_DECL_IMPORT
#endif

// enums
enum scrollDirection{
	up,
	down,
	invalid
};

class CALENGRID_EXPORT CalenMonthGrid : public HbGridView
{
	Q_OBJECT

public:
	CalenMonthGrid(QGraphicsItem *parent = NULL);
	~CalenMonthGrid();
	void setView(CalenMonthView *view);
	void updateMonthGridModel(QList<CalenMonthData> &monthDataArray,
                                int indexToBeScrolled, bool isFirstTime);
	void updateMonthGridWithInActiveMonths(
										QList<CalenMonthData> &monthDataArray);
	void updateMonthGridWithEventIndicators(
										QList<CalenMonthData> &monthDataArray);
	void setCurrentIdex(int index);
	int getCurrentIndex();

protected:
	void gestureEvent(QGestureEvent *event);
		
private:
	void downGesture();
    void upGesture();
	void handlePrependingRows(QList<CalenMonthData > &monthDataList);
	void handleAppendingRows(QList<CalenMonthData > &monthDataList);
	void handlePanGestureFinished();
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void setFocusToProperDay();
	void setActiveDates(QDate activeDate);
	void paint(QPainter* painter, 
	           const QStyleOptionGraphicsItem* option, QWidget* widget);
	bool checkIfWeCanScroll(scrollDirection direction);
	
public slots:
	void scrollingFinished();
	void prependRows();
	void appendRows();
	void itemActivated(const QModelIndex &index);

private slots:

	void handleThemeChange();
	
private:
	QStandardItemModel *mModel;
	scrollDirection mDirection;
	bool mIsPanGesture;
	bool mIsAtomicScroll;
	CalenMonthView *mView;
	int mCurrentRow;
	bool mIsNonActiveDayFocused;
	bool mIgnoreItemActivated;
	QPointF mPressedPos;
	QGraphicsWidget* mContentWidget;
	QList<QString> mLocalisedDates;
	QPointF mStartPos;
	QColor mGridLineColor;
	bool mActiveDatesSet;
	bool mIsGridAdjusting;
	bool mEventIndicatorNotSet;
};

#endif // CALENMONTHGRID_H

// End of file  --Don't remove this.
