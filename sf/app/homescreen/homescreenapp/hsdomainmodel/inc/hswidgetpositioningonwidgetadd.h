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
*
*/

#ifndef HSWIDGETPOSITIONINGONWIDGETADD_H
#define HSWIDGETPOSITIONINGONWIDGETADD_H

#include <QList>
#include <QRectF>
#include <QPointF>

#include "hsconfiguration.h"
#include "hstest_global.h"
#include "hsdomainmodel_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HsWidgetHost;

class HSDOMAINMODEL_EXPORT HsWidgetPositioningOnWidgetAdd
{
public:
    class Result
    {
    public:
        Result() { }
        QList<QRectF> calculatedRects;
        QList<int> rectOrder;
    };

    virtual ~HsWidgetPositioningOnWidgetAdd() {}
    
    virtual Result convert(const QRectF &contentArea,
                                  const QList<QRectF> &existingRects,
                                  const QList<QRectF> &newRects,
                                  const QPointF &startPoint) = 0;
  
    static void setInstance(HsWidgetPositioningOnWidgetAdd *instance);
    static HsWidgetPositioningOnWidgetAdd *instance();

private:
    static HsWidgetPositioningOnWidgetAdd *mInstance;
};

class HSDOMAINMODEL_EXPORT HsAnchorPointInBottomRight : public HsWidgetPositioningOnWidgetAdd
{
public:
    Result convert(const QRectF &contentArea,
                          const QList<QRectF> &existingRects,
                          const QList<QRectF> &newRects,
                          const QPointF &startPoint);
 
};

class HSDOMAINMODEL_EXPORT HsAnchorPointInCenter : public HsWidgetPositioningOnWidgetAdd
{
public:
    Result convert(const QRectF &contentArea,
                          const QList<QRectF> &existingRects,
                          const QList<QRectF> &newRects,
                          const QPointF &startPoint);
 };

class HSDOMAINMODEL_EXPORT HsWidgetOrganizer : public HsWidgetPositioningOnWidgetAdd
{
public:
    HsWidgetOrganizer(int anchorDistance, HsConfiguration::WidgetOrganizerSearchSequence sequence);
    ~HsWidgetOrganizer();

public:
    Result convert(const QRectF &contentArea,
                          const QList<QRectF> &existingRects,
                          const QList<QRectF> &newRects,
                          const QPointF &startPoint);

private:
    enum SortMode {SortByHeight, SortByWidth, SortByArea};
    enum SearchMode {SearchColumn, SearchRow};

private:
    void initAnchors();
    bool checkExistingRects(const QList<QRectF> &existingRects);
    int convertToAnchors(int length);
    bool markAnchors(const QRectF &rect);
    int getAnchorListIndex(const QPointF &position);
    QPointF getPosition(const QSizeF &size);
    QPointF searchPositionRowByRow(QPointF startPoint, int width, int height);
    QPointF searchPositionColumnByColumn(QPointF startPoint, int width, int height);
    QPointF searchSpace(SearchMode mode, QPointF startPoint, int contentLength);
    QPointF getAnchorCoordinates(int index);
    QList<int> sortRects(SortMode mode, const QMap<int, QRectF> &rects);

private:
    int mAnchorDistance;
    HsConfiguration::WidgetOrganizerSearchSequence mSequence;
    QList<bool> mAnchors;
    QRectF mContentArea;
    int mAnchorColumns;
    int mAnchorRows;
    HsAnchorPointInCenter *mCenterAlgorithm;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};


#endif // HSWIDGETPOSITIONINGONWIDGETADD_H
