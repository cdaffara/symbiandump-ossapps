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

#include <QLineF>
#include <QtGlobal>
#include <QPointF>
#include <math.h>

#include "hswidgetpositioningonwidgetadd.h"
#include "hsconfiguration.h"
#include "hsgui.h"

const qreal offset = 20; //TODO: Implement this as configurable parameter


/*!
    \class HsWidgetPositioningOnWidgetAdd
    \ingroup group_hsutils
    \brief 
*/

/*!
    \class HsWidgetPositioningOnWidgetAdd
    \brief Defines widget positioning on widget add.

    Widget positioning on widget add sets positions for
    a set of home screen widgets added from application library.
*/

/*!
    Sets the positioning \a instance as the current one.
    Deletes the existing instance if present.
*/
void HsWidgetPositioningOnWidgetAdd::setInstance(
    HsWidgetPositioningOnWidgetAdd *instance)
{
    if (mInstance)
        delete mInstance;
    mInstance = instance;
}
 
/*!
    Returns the current positioning instance.
*/
HsWidgetPositioningOnWidgetAdd *HsWidgetPositioningOnWidgetAdd::instance()
{
    return mInstance;
}

/*!
    Stores the current positioning instance.
*/
HsWidgetPositioningOnWidgetAdd *HsWidgetPositioningOnWidgetAdd::mInstance = 0;

/*!
    \class HsAnchorPointInBottomRight
    \brief Diagonal widget positioning algorithm.
    
    Sets widget's lower right corner to follow content area's diagonal.
    Widgets are positioned to certain offset to each other.
*/
HsWidgetPositioningOnWidgetAdd::Result HsAnchorPointInBottomRight::convert(
    const QRectF &contentArea,
    const QList<QRectF> &existingRects,
    const QList<QRectF> &newRects,
    const QPointF &startPoint)
{
    Q_UNUSED(existingRects);

    HsWidgetPositioningOnWidgetAdd::Result result;
    QList<QRectF> toGeometries;

    //Offset for widgets' bottom right position to each other
    qreal k = contentArea.height()/contentArea.width(); //slope of the diagonal
    qreal offset_x = offset/(sqrt(k + 1));
    qreal offset_y = k*offset_x;
    QPointF offsetPoint(offset_x, offset_y);
    
    QPointF anchorPoint;
   
    if(startPoint.isNull()){

        QLineF diagonal(contentArea.topLeft(), contentArea.bottomRight());
        QLineF widgetRightSide(contentArea.center().x()+ newRects.at(0).width()/2,
                           contentArea.top(),
                           contentArea.center().x()+ newRects.at(0).width()/2,
                           contentArea.bottom());

        // right side line intersection with diagonal will be bottom right position
        // for the first rect
        if(QLineF::BoundedIntersection != 
            diagonal.intersect(widgetRightSide, &anchorPoint)) {
            result.calculatedRects = newRects;
            return result; //Return original since undefined error.
                            //In this case widget's must be wider than the content area.
        }
    } else {
        anchorPoint = startPoint - offsetPoint;
    }

    QRectF widgetRect;
    for(int i=0;i<newRects.count();++i) {
        widgetRect = newRects.at(i);
        widgetRect.moveBottomRight(anchorPoint);
        //if widget rect doesn't fit, try to move it
        if(!contentArea.contains(widgetRect)) {
            /*! precondition is that
             widget's max height < content area height
             widget's max widht < content area width
            */
            widgetRect.moveBottomRight(contentArea.bottomRight());
            // anchorPoin is always previous bottom right
            anchorPoint = widgetRect.bottomRight();
        }
        toGeometries << widgetRect;
        anchorPoint -= offsetPoint;
        
    }
    result.calculatedRects = toGeometries;
    return result;
}

/*!
    \class HsAnchorPointInCenter
    \brief Diagonal widget positioning algorithm.
    
    Sets widget's center point to follow content area's diagonal.
    Widgets are positioned to certain offset to each other.
*/
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
HsWidgetPositioningOnWidgetAdd::Result HsAnchorPointInCenter::convert(
    const QRectF &contentArea,
    const QList<QRectF> &existingRects,
    const QList<QRectF> &newRects,
    const QPointF &startPoint )
{
    Q_UNUSED(existingRects);
    Q_UNUSED(startPoint)

    HsWidgetPositioningOnWidgetAdd::Result result;
    QList<QRectF> toGeometries;

    //Offset for widgets' centers position to each other
    qreal k = contentArea.height()/contentArea.width(); //slope of the diagonal
    qreal offset_x = offset/(sqrt(k + 1));
    qreal offset_y = k*offset_x;
    QPointF offsetPoint(offset_x, offset_y);

    //First widget to the center of the content area
    QPointF anchorPoint = contentArea.center();
    foreach (QRectF g, newRects) {
        g.moveCenter(anchorPoint);
        toGeometries << g;
        anchorPoint -= offsetPoint;
        if(!contentArea.contains(anchorPoint)) {
            anchorPoint = contentArea.bottomRight();
        }
    }
    result.calculatedRects = toGeometries;
    return result;
}

HsWidgetOrganizer::HsWidgetOrganizer(int anchorDistance,
                                     HsConfiguration::WidgetOrganizerSearchSequence sequence)
: mAnchorDistance(anchorDistance), mSequence(sequence), mAnchorColumns(0), mAnchorRows(0),
  mCenterAlgorithm(new HsAnchorPointInCenter())
{

}

HsWidgetOrganizer::~HsWidgetOrganizer()
{
    delete mCenterAlgorithm;
}

/*!
    \class HsWidgetOrganizer
    \brief Advanced widget positioning algorithm.
    
    Organizes widget's starting from upper left corner towards right,
    and then continues the on the next line.
*/
HsWidgetPositioningOnWidgetAdd::Result HsWidgetOrganizer::convert(
    const QRectF &contentArea,
    const QList<QRectF> &existingRects,
    const QList<QRectF> &newRects,
    const QPointF &startPoint)
{
    Q_UNUSED(startPoint)

    HsWidgetPositioningOnWidgetAdd::Result result;

    // mandatory check ups
    if (mAnchorDistance <= 0 || contentArea == QRectF() ||
        newRects == QList<QRectF>()) {
            return result;
    }

    // calculate anchor limits based on anchor distance
    mAnchorColumns = convertToAnchors(contentArea.width());
    mAnchorRows = convertToAnchors(contentArea.height());
    mContentArea = contentArea;

    // map rects so that we can later return them in original order
    QMap<int, QRectF> newRectsMap;
    for (int id = 0; id < newRects.count(); id++) {
        newRectsMap.insert(id, newRects.at(id));
    }

    // get orientation
    Qt::Orientation orientation(HsGui::instance()->orientation());

    SortMode mode;
    // select sorting mode based on orientation and search sequence
    if((orientation == Qt::Vertical && mSequence == HsConfiguration::SearchRowByRow) ||
        (orientation == Qt::Horizontal && mSequence == HsConfiguration::SearchColumnByColumn)) {
        mode = SortByHeight;
    } else {
        mode = SortByWidth;
    }

    // sort rects into order
    QList<int> newRectsSorted = sortRects(mode, newRectsMap);

    // initialize anchor points
    initAnchors();

    // go through existing rects
    bool ok = checkExistingRects(existingRects);
    if (!ok) {
            return result;
    }

    QList<int> newRectsNotCalculated;
    QList<int> newRectsCalculated;
    QList<QRectF> newExistingRects;
    newExistingRects += existingRects;

    // get positions for all new rects
    for (int i = 0; i < newRectsMap.count(); i++) {
        // proceed in sorted order with the rects
        QRectF newRect = newRectsMap.value(newRectsSorted.at(i));
        // find first free anchor point for rect
        QPointF position = getPosition(newRect.size());
        if (position != QPointF(-1,-1)) {
            QRectF calculatedGeometry = QRectF(position.x() + mContentArea.x(),
                                               position.y() + mContentArea.y(),
                                               newRect.width(), newRect.height());
            // update new rect instead of old one based on id map
            newRectsMap.insert(newRectsSorted.at(i), calculatedGeometry);
            // update existing rects
            newExistingRects << calculatedGeometry;
            newRectsCalculated << newRectsSorted.at(i);
            // mark new rect reserved
            bool marked = markAnchors(QRectF(position, newRect.size()));
            if (!marked) {
                return result;
            }

        } else {
            // collect rect that do not fit
            newRectsNotCalculated << newRectsSorted.at(i);
        }
    }

    // use center algorithm with offset for the rest rects that did not fit to content area
    if (newRectsNotCalculated.count() > 0 ) {
        // collect not organized rects
        QList<QRectF> undoneRects;
        for (int i = 0; i < newRectsNotCalculated.count(); i++) {
            undoneRects << newRectsMap.value(newRectsNotCalculated.at(i));
        }

        HsWidgetPositioningOnWidgetAdd::Result centerResult;
        centerResult = mCenterAlgorithm->convert(mContentArea, newExistingRects,
                                           undoneRects, QPointF());
        // update the rest rects instead of old ones
        for (int i = 0; i < centerResult.calculatedRects.count(); i++) {
            newRectsMap.insert(newRectsNotCalculated.at(i), centerResult.calculatedRects.at(i));
        }
    }

    result.calculatedRects = newRectsMap.values();
    result.rectOrder = newRectsCalculated + newRectsNotCalculated;

    return result;
}

/*!    
    Initializes anchor point network for area size
*/
void HsWidgetOrganizer::initAnchors()
{
    // need to zero just in case (if algorithm is called twice)
    mAnchors = QList<bool>();
    // create anchor point network
    for (int i = 0; i < (mAnchorColumns * mAnchorRows); i++) {
        mAnchors.append(false);
    }
}

/*!    
    Check existing rects and marks them reserved
*/
bool HsWidgetOrganizer::checkExistingRects(const QList<QRectF> &existingRects)
{
    foreach (QRectF rect, existingRects) {
        /* if existing rect is on the edges of content area
           need to drop one pixels because there is no anchors on the edge */
        int rightmostPoint = rect.x() + rect.width();
        if (rightmostPoint == mContentArea.width()) {
            rect.setWidth(rect.width() - 1);
        }
        int undermostPoint = rect.y() + rect.height();
        if (undermostPoint == mContentArea.height()) {
            rect.setHeight(rect.height() - 1);
        }
        // decrease content area size in case it does not start from (0,0)
        rect = QRectF(
            QPointF(rect.x() - mContentArea.x(), rect.y() - mContentArea.y()),
            rect.size());
        bool marked = markAnchors(rect);
        if (!marked) {
            return false;
        }
    }
    return true;
}

/*!    
    Calculates pixel length as anchor points
*/
int HsWidgetOrganizer::convertToAnchors(int length)
{
    // calculate remainder
    int remainder = length % mAnchorDistance;
    // calculate anchor points (only pixel integrals are counted, decimals are cut away)
    int anchorPoints = (length - remainder) / mAnchorDistance;
    return anchorPoints;
}

/*!    
    Marks reserved anchor points based on given rects
*/
bool HsWidgetOrganizer::markAnchors(const QRectF &rect)
{
    // in case content does not start from zero, need take contentArea into calculations
    int startWidth = convertToAnchors(rect.x());
    int endWidth = convertToAnchors(rect.x() + rect.width());
    int startHeight = convertToAnchors(rect.y());
    int endHeight = convertToAnchors(rect.y() + rect.height());

    // mark reserved anchors row by row from left to right
    for (int i = startWidth; i <= endWidth; i++) {
        for (int j = startHeight; j <= endHeight; j++) {
            int index = getAnchorListIndex(QPointF(i,j));
            if (index < 0) {
                return false;
            }
            mAnchors[index] = true;
        }
    }
    return true;
}

/*!    
    Returns anchor's list index based on given position
*/
int HsWidgetOrganizer::getAnchorListIndex(const QPointF &position)
{
    int index = (position.y() * mAnchorColumns) + position.x();
    if (index < mAnchors.count()) {
        return index;
    } else {
        return -1;
    }
}

/*!    
    Finds anchor points for content size
*/
QPointF HsWidgetOrganizer::getPosition(const QSizeF &size)
{
    QPointF startPoint(0,0);
    // convert units from pixels to anchors
    int width = convertToAnchors(size.width());
    int height = convertToAnchors(size.height());

    // based on search sequence, select position searching method
    if (mSequence == HsConfiguration::SearchRowByRow) {
        startPoint = searchPositionRowByRow(startPoint, width, height);
    } else {
        startPoint = searchPositionColumnByColumn(startPoint, width, height);
    }

    if (startPoint == QPointF(-1,-1)) {
        return startPoint;
    } else {
        // return the actual pixel coordinate
        return QPointF(startPoint.x() * mAnchorDistance, startPoint.y() * mAnchorDistance);
    }
}

/*!    
    Search sequence that finds anchor position by looking first for width on x-axis and
    then secondarily on height from y-axis
*/
QPointF HsWidgetOrganizer::searchPositionRowByRow(QPointF startPoint, int width, int height)
{
    bool anchorFound = false;
    QPointF candidatePoint(0,0);
    // loop until anchor point is found
    while (anchorFound == false) {
        // search for width on specified row
        candidatePoint = searchSpace(SearchRow, startPoint, width);
        if (candidatePoint != QPointF(-1,-1)) {
            // update start point to where found width starts
            startPoint.setX(candidatePoint.x());
            // check all anchor height points corresponding the found free width points
            for(int i = startPoint.x(); i <= startPoint.x() + width; i++) {
                // save current start point to be checked
                QPointF point = QPointF(i, startPoint.y());
                // search for height on specified column
                candidatePoint = searchSpace(SearchColumn, point, height);
                if (candidatePoint == QPointF(-1,-1)) {
                    // update x anchor index
                    startPoint.setX(startPoint.x() + 1);
                    // set i to max to stop searching
                    i = startPoint.x() + width;
                }
            }
            // if all height searches were successfull
            if (candidatePoint != QPointF(-1,-1)) {
                anchorFound = true;
            }
        } else {
            // update x and y start positions when row has been checked
            startPoint.setX(0);
            startPoint.setY(startPoint.y() + 1);
            // check that enough height left
            if (startPoint.y() >= mAnchorRows) {
                return QPointF(-1,-1);
            }
        }
    }

    return startPoint;
}

/*!    
    Search sequence that finds anchor position by looking first for height on y-axis and
    then secondarily on width from x-axis
*/
QPointF HsWidgetOrganizer::searchPositionColumnByColumn(QPointF startPoint,
                                                        int width, int height)
{
    bool anchorFound = false;
    QPointF candidatePoint(0,0);

    while (anchorFound == false) {
        candidatePoint = searchSpace(SearchColumn, startPoint, height);
        if (candidatePoint != QPointF(-1,-1)) {
            startPoint.setY(candidatePoint.y());
            for(int i = startPoint.y(); i <= startPoint.y() + height; i++) {
                QPointF point = QPointF(startPoint.x(), i);
                candidatePoint = searchSpace(SearchRow, point, width);
                if (candidatePoint == QPointF(-1,-1)) {
                    startPoint.setY(startPoint.y() + 1);
                    i = startPoint.y() + height;
                }
            }
            if (candidatePoint != QPointF(-1,-1)) {
                anchorFound = true;
            }
        } else {
            startPoint.setY(0);
            startPoint.setX(startPoint.x() + 1);
            if (startPoint.x() >= mAnchorColumns) {
                return QPointF(-1,-1);
            }
        }
    }

    return startPoint;
}

/*!    
    Searches anchor point space for given length
*/
QPointF HsWidgetOrganizer::searchSpace(SearchMode mode, QPointF startPoint, int length)
{
    int availableLength = 0;
    // convert start point to an index in anchor list
    int startIndex = getAnchorListIndex(startPoint);
    int increment = 0;
    int endIndex = 0;

    // set end anchor index depending on checked axis
    if (mode == SearchRow) {
        // save the last index of the checked row
        endIndex = getAnchorListIndex(QPointF((mAnchorColumns - 1), startPoint.y()));

    } else {
        // save the last index of the checked column
        endIndex = getAnchorListIndex(QPointF(startPoint.x(), (mAnchorRows - 1)));
        // we need to add increment due to anchors are listed row by row
        increment = mAnchorColumns - 1;
    }

    // safety checks
    if (startIndex == -1 || endIndex == -1) {
        return QPointF(-1,-1);
    }

    // loop through anchor indexes, increment is added only when going through height
    for (int i = startIndex; i <= endIndex; i = i + 1 + increment) {
        // if anchor reserved
        if (mAnchors.at(i) == true) {
            availableLength = 0;
            // if going through the first part of sequence (width/height)
            if ((mSequence == HsConfiguration::SearchRowByRow && mode == SearchRow) ||
                (mSequence == HsConfiguration::SearchColumnByColumn && mode == SearchColumn)) {
                // update start index
                startIndex = i + 1 + increment;
            } else {
                // exit immediately if second part of sequence fails
                return QPointF(-1,-1);                
            }
        } else {
            // if enough space found
            if (availableLength == length) {
                // return the actual anchor position
                return getAnchorCoordinates(startIndex);
            }
            // update available length
            availableLength++;
        }
    }

    return QPointF(-1,-1);
}

/*!    
    Returns pixel coordinate based on anchor coordinate
*/
QPointF HsWidgetOrganizer::getAnchorCoordinates(int index)
{
    if (index < mAnchors.count()) {
        int x = index % mAnchorColumns;
        int y = (index - x) / mAnchorColumns;
        return QPointF(x,y);
    } else {
        return QPointF();
    }
}

/*!
    Sorts rects based on sort mode and given map of rects
*/
QList<int> HsWidgetOrganizer::sortRects(SortMode mode, const QMap<int, QRectF> &rects)
{
    QList<int> sortedRects;
    int i = 0;
    // loop through all rects
    QMapIterator<int, QRectF> id(rects);
    while (id.hasNext()) {
        id.next();
        int index = 0;
        // add first rect id to sorted list
        if (i == 0) {
            sortedRects << id.key();
        } else {
            // go through existing rects in the sorted list
            for ( int j = 0; j < sortedRects.count(); j++) {
                // calculations for sortByArea
                qreal existingArea = rects.value(sortedRects.at(j)).width() *
                                                 rects.value(sortedRects.at(j)).height();
                qreal newArea = id.value().width() * id.value().height();
                // sort rects in height order
                switch (mode) {
                case SortByHeight:
                    /* if rect heigth is smaller on already
                       existing ones in the list -> increment index
                    */
                    if (id.value().height() <= rects.value(sortedRects.at(j)).height()) {
                        index++;
                    }
                    break;
                // sort rects in width order
                case SortByWidth:
                    // if rect width is smaller -> increment index
                    if (id.value().width() <= rects.value(sortedRects.at(j)).width()) {
                        index++;
                    }
                    break;
                case SortByArea:
                    // if rect area is smaller -> increment index
                    if (newArea <= existingArea) {
                        index++;
                    }
                // otherwise return in original order
                default:
                    index++;
                    break;
                }
            }
            // add rect id in the sorted list
            sortedRects.insert(index, id.key());
        }
        i++;
    }
    return sortedRects;
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

