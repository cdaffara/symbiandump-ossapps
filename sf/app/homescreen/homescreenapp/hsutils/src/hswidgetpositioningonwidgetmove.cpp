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

#include <QVector2D>
#include "hswidgetpositioningonwidgetmove.h"
#include "hsapp_defs.h"


/*!
    The distance by which the line is extended on each side of reference points
*/
#define SNAP_LINE_EXTEND_VALUE 10.0

/*!
    Sets the snap method instance. The existing instance
    will be deleted.
*/
void HsWidgetPositioningOnWidgetMove::setInstance(HsWidgetPositioningOnWidgetMove *instance)
{
    if (mInstance)
        delete mInstance;
    mInstance = instance;
}

/*!
    Returns the snap method instance.
*/
HsWidgetPositioningOnWidgetMove *HsWidgetPositioningOnWidgetMove::instance()
{
    return mInstance;
}

/*!
    Points to the snap method instance.
*/
HsWidgetPositioningOnWidgetMove *HsWidgetPositioningOnWidgetMove::mInstance = 0;

/*!
    Constructor.
*/
HsSnapToLines::HsSnapToLines() :
    mActiveRectWidth(0.0), mActiveRectHeight(0.0), mMinDistancePosition(0.0),
    mHorizontalSnapPosition(0.0), mVerticalSnapPosition(0.0),
    mHorizontalSnapFound(false), mVerticalSnapFound(false),
    mRectLieAbove(false), mLeftInRange(false), mRightInRange(false), mIsBetterFitHorizontalSnap(false),
    mDistanceVerticalEdges(0.0), mVerticalEdgeToLeftOfInactiveRect(0.0), mVerticalEdgeToRightOfInactiveRect(0.0),
    mMinVerticalEdgesDistance(0.0), mVerticalDistance(0.0),
    mVerticalDistanceFromSelectedRect(0.0), mContainerVerticalEdgeDistance(0.0),
    mRectLieLeft(false), mTopInRange(false), mBottomInRange(false), mIsBetterFitVerticalSnap(false),
    mDistanceHorizontalEdges(0.0), mHorizontalEdgeToTopOfInactiveRect(0.0), mHorizontalEdgeToBottomOfInactiveRect(0.0),
    mMinHorizontalEdgesDistance(0.0), mHorizontalDistance(0.0),
    mHorizontalDistanceFromSelectedRect(0.0), mContainerHorizontalEdgeDistance(0.0),
    mSnapEnabled(false), mSnapForce(0.0), mSnapGap(0.0),
    mRectVerticalEdgeLiesInLineWithVerticalLine(false), mRectLieAboveVerticalLine(false), mRectLieBelowVerticalLine(false),
    mRectHorizontalEdgeLiesInLineWithHorizontalLine(false), mRectLieLeftOfHorizontalLine(false), mRectLiesRightOfHorizontalLine(false)
{

}

/*!
    Configures the snap-to-lines algorithm.
*/
void HsSnapToLines::setConfiguration(const QVariantHash &configuration)
{
    bool canConvert = false;
    mSnapEnabled = configuration[Hs::snapEnabled].toBool();
    //The following values should be in qreal, so the status received in canConvert is ignored
    mSnapForce = configuration[Hs::snapForce].toDouble(&canConvert);
    mSnapGap = configuration[Hs::snapGap].toDouble(&canConvert);
}

/*!
    Set the dimensions of container rect, active rect and position of Inactive Rects
    on the page.
*/
void HsSnapToLines::setPagePresentation(const QRectF &containerRect, 
                                        const QList<QRectF> &inactiveRects,
                                        const QRectF &activeRect)
{
    mContainerRect = containerRect;
    mActiveRectWidth = activeRect.width();
    mActiveRectHeight = activeRect.height();

    createSnappableRectangles(inactiveRects);
}

/*!
    Create the list of rects and flag if their sides are snappable from top or bottom or left or right,
    depending on other rects overlapping with the rect.
*/
void HsSnapToLines::createSnappableRectangles(const QList<QRectF> &inactiveRects)
{
    mInactiveSnapRects.clear();

    int i;
    for (i = 0; i<inactiveRects.count(); ++i) {
        QRectF rect = inactiveRects[i];
        HsSnapRectangle snapRectangle(rect);

        int j;
        for (j = 0; j<inactiveRects.count(); ++j) {
            QRectF rectToCompare = inactiveRects[j];
            if (rect != rectToCompare) {
                //Check if the rectangles being compared intersect each other
                if (rectToCompare.intersects(rect)) {
                    //As the widgets intersect, check which corner is contained,
                    //The corner that is contained is not snappable, when the moving widget is in the same position
                    if (rectToCompare.contains(rect.topLeft())) {
                        snapRectangle.isLeftSnapableForAbove = false;
                        snapRectangle.isTopSnapableForLeft = false;
                    }

                    if (rectToCompare.contains(rect.topRight())) {
                        snapRectangle.isRightSnapableForAbove = false;
                        snapRectangle.isTopSnapableForRight = false;
                    }

                    if (rectToCompare.contains(rect.bottomRight())) {
                        snapRectangle.isRightSnapableForBelow = false;
                        snapRectangle.isBottomSnapableForRight = false;
                    }

                    if (rectToCompare.contains(rect.bottomLeft())) {
                        snapRectangle.isLeftSnapableForBelow = false;
                        snapRectangle.isBottomSnapableForLeft = false;
                    }
                }
            }
        }
        if (snapRectangle.isLeftSnapableForAbove || snapRectangle.isLeftSnapableForBelow ||
            snapRectangle.isRightSnapableForAbove || snapRectangle.isRightSnapableForBelow ||
            snapRectangle.isTopSnapableForLeft || snapRectangle.isTopSnapableForRight ||
            snapRectangle.isBottomSnapableForLeft || snapRectangle.isBottomSnapableForRight) {
                mInactiveSnapRects.append(snapRectangle);
        }
    }
}

/*!
    Run the snap algorithm to with the position of moving rect, to get a snap result.
*/
HsWidgetPositioningOnWidgetMove::Result HsSnapToLines::run(const QRectF &movingRect)
{
    HsWidgetPositioningOnWidgetMove::Result result;

    if (mSnapEnabled) {
        mMovingRect = movingRect;
        mHorizontalSnapPosition = 0.0;
        mVerticalSnapPosition = 0.0;

        mVerticalLine = QLineF();
        mMinVerticalEdgesDistance = mSnapForce;
        mVerticalDistanceFromSelectedRect = 0.0;
        mContainerVerticalEdgeDistance = 0.0;
        mHorizontalSnapFound = false;

        mHorizontalLine = QLineF();
        mMinHorizontalEdgesDistance = mSnapForce;
        mHorizontalDistanceFromSelectedRect = 0.0;
        mContainerHorizontalEdgeDistance = 0.0;
        mVerticalSnapFound = false;

        for (int i = 0; i < mInactiveSnapRects.count(); ++i) {
            mInactiveSnapRectToCompare = mInactiveSnapRects[i];
            mInactiveRectToCompare = mInactiveSnapRectToCompare.rectangle;
            if (!movingRect.intersects(mInactiveRectToCompare)) { //Only compare if Inactive Rect and moving rect do not overlap.
                // Horizontal - Direction Snapping
                compareLeftSideOfMovingRectForSnapping();
                compareRightSideOfMovingRectForSnapping();
                // Vertical - Direction Snapping
                compareTopOfMovingRectForSnapping();
                compareBottomOfMovingRectForSnapping();
            }
        }

        if (mHorizontalSnapFound) {
            result.hasHorizontalSnap = true;
            result.horizontalSnapPosition = mHorizontalSnapPosition;
            extendVerticalLine();
            result.verticalSnapLine = mVerticalLine;
        }

        if (mVerticalSnapFound) {
            result.hasVerticalSnap = true;
            result.verticalSnapPosition = mVerticalSnapPosition;
            extendHorizontalLine();
            result.horizontalSnapLine = mHorizontalLine;
        }
    }

    return result;
}

/*!
    Check if the inactive rect being compared with moving rect lies above or below of moving rect.
*/
void HsSnapToLines::checkInactiveRectLieAboveOrBelowOfMovingRect()
{
    //Check if the inactive rect lies below or above of the moving rect
    mRectLieAbove = false;
    mVerticalDistance = 0.0;
    //Inactive Rect lies above or at the save Y position as the Moving Rect
    if (mInactiveRectToCompare.top() <= mMovingRect.top()) {
        mRectLieAbove = true;
        if (mInactiveRectToCompare.bottom() <= mMovingRect.top()) {
            mVerticalDistance = mMovingRect.top() - mInactiveRectToCompare.bottom();
        }
        else {
            mVerticalDistance = mMovingRect.top() - mInactiveRectToCompare.top();
        }
    }
    //Inactive Rect lies below the Moving Rect
    else {
        mRectLieAbove = false;
        if (mMovingRect.bottom() <= mInactiveRectToCompare.top()) {
            mVerticalDistance = mInactiveRectToCompare.top() - mMovingRect.bottom();
        }
        else {
            mVerticalDistance = mInactiveRectToCompare.bottom() - mMovingRect.bottom();
        }
    }
}

/*!
    Check if the Vertical edges (Left and Right Edges) of the inactive rect being compared
    is in range of the snapping distance of the vertical edge of moving rect
    \param movingRectVerticalEdgePosition Position of the Vertical edge(either left or right) of moving rect.
*/
void HsSnapToLines::checkInactiveRectVerticalEdgesInRange(qreal movingRectVerticalEdgePosition)
{
    mLeftInRange = false;
    mRightInRange = false;

    //calculate the distance of the moving rect's vertical edge to the inactive rect's left and right edges
    mVerticalEdgeToLeftOfInactiveRect = qAbs(mInactiveRectToCompare.left() - movingRectVerticalEdgePosition);
    mVerticalEdgeToRightOfInactiveRect = qAbs(mInactiveRectToCompare.right() - movingRectVerticalEdgePosition);

    if (mVerticalEdgeToLeftOfInactiveRect <= mMinVerticalEdgesDistance 
        && (mRectLieAbove && mInactiveSnapRectToCompare.isLeftSnapableForBelow
        || !mRectLieAbove && mInactiveSnapRectToCompare.isLeftSnapableForAbove)) { 
        mLeftInRange = true;
    }
    if (mVerticalEdgeToRightOfInactiveRect <= mMinVerticalEdgesDistance
        && (mRectLieAbove && mInactiveSnapRectToCompare.isRightSnapableForBelow
        || !mRectLieAbove && mInactiveSnapRectToCompare.isRightSnapableForAbove)) {
        mRightInRange = true;
    }
}

/*!
    Check if this inactive rect is better fit for Horizontal snapping
    \param containerVerticalEdgeToInactiveRectVerticalEdge
           difference between the vertical edge of the container and same vertical edge of inactive rect
    \param containerOtherVerticalEdgeToInactiveRectOtherVerticalEdge
           difference between the opposite vertical edges of continer and the inactive rect
*/
void HsSnapToLines::checkInactiveRectBetterFitForHorizontalSnapping(qreal containerVerticalEdgeToInactiveRectVerticalEdge,
                                                                    qreal containerOtherVerticalEdgeToInactiveRectOtherVerticalEdge)
{
    //Check if the inactive rect is better fit or if it is inline with already selected position and hence is also a better fit
    mIsBetterFitHorizontalSnap = false;
    if (mLeftInRange || mRightInRange) {
        if (mDistanceVerticalEdges < mMinVerticalEdgesDistance) {
            mIsBetterFitHorizontalSnap = true;
        }
        else if (mDistanceVerticalEdges == mMinVerticalEdgesDistance) { //the distance in the vertical edges is same as from the previously selected rect
            //check the position of rect with respect to Vertical line
            checkInactiveRectPositionToVerticalLine();
            //if horizontal snap position was previously found and the rect's edge is in line with Vertical line
            if (mHorizontalSnapFound && mRectVerticalEdgeLiesInLineWithVerticalLine) {
                if (mRectLieAboveVerticalLine || mRectLieBelowVerticalLine) {
                    extendVerticalLineToIncludeInactiveRect();
                }
            }
            //here the case is that moving rect lies exactly in middle of two same sides of two different inactive widgets.
            else {
                //Prioritize first on the fact if the inactive rect is closer to the moving rect in Y - direction.
                if (mVerticalDistance < mVerticalDistanceFromSelectedRect) {
                    mIsBetterFitHorizontalSnap = true;
                }
                else if (mVerticalDistance == mVerticalDistanceFromSelectedRect) {
                    //Prioritize next if this Inactive rect's vertical edge is closer to the same vertical edge of the container rect, then the previously selected rect
                    if (containerVerticalEdgeToInactiveRectVerticalEdge < mContainerVerticalEdgeDistance) {
                        mIsBetterFitHorizontalSnap = true;
                    }
                    //Prioritize next if the Inactive rect's vertical edge lies near to same vertical edge of the container rect than the other pair
                    else if (containerVerticalEdgeToInactiveRectVerticalEdge < containerOtherVerticalEdgeToInactiveRectOtherVerticalEdge) {
                        mIsBetterFitHorizontalSnap = true;
                    }
                    else {
                        //This else will happen if this rectangle being compared is exactly the same as the selected rectangle for snapping, but in opposite Y direction.
                        //In that case it does not matter which is the selected rectangle. Hece we leave the already selected rectangle as the better fit.
                    }
                }
            }
        }
    }
}

/*!
    Check if the left edge of moving rect is snappable to the incative rect's left or right edge.
    The inactive rect's edge is only chosen if it is a better fit for horizontal snapping.
*/
void HsSnapToLines::compareLeftSideOfMovingRectForSnapping()
{
    checkInactiveRectLieAboveOrBelowOfMovingRect();

    checkInactiveRectVerticalEdgesInRange(mMovingRect.left());

    //calculate the distance of inactive rect's left edge and container rect's left edge
    qreal differenceContainerLeftEdgeToInactiveRectLeftEdge = mInactiveRectToCompare.left() - mContainerRect.left();
    //calculate the distance of inactive rect's right edge and container rect's right edge
    qreal differenceContainerRightEdgeToInactiveRectRightEdge = mContainerRect.right() - mInactiveRectToCompare.right();
    qreal xSnapGapAdjustment = 0.0;
    mDistanceVerticalEdges = 0.0;
    mMinDistancePosition = 0.0;

    //If only one edge of inactive rect is in snappable range, save that position
    if ((mLeftInRange && !mRightInRange)
        || !mLeftInRange && mRightInRange) {
        if (mLeftInRange) {
            mMinDistancePosition = mInactiveRectToCompare.left();
            mDistanceVerticalEdges = mVerticalEdgeToLeftOfInactiveRect;
            xSnapGapAdjustment = 0.0;
        }
        else {
            mMinDistancePosition = mInactiveRectToCompare.right();
            mDistanceVerticalEdges = mVerticalEdgeToRightOfInactiveRect;
            xSnapGapAdjustment = mSnapGap;
        }
    }
    //else both edges of inactive rect are in range, check which is a better fit
    else if (mLeftInRange && mRightInRange) {
        //if left edge of moving rect to the left of the inactive rect is closer than the left edge of moving rect to the right of the inactive rect
        if (mVerticalEdgeToLeftOfInactiveRect < mVerticalEdgeToRightOfInactiveRect) {
            mMinDistancePosition = mInactiveRectToCompare.left();
            mDistanceVerticalEdges = mVerticalEdgeToLeftOfInactiveRect;
            xSnapGapAdjustment = 0.0;
            mRightInRange = false;
        }
        //if the left edge of inactive rect to left of moving rect is at the same distance as the right edge of inactive rect to the right of moving rect
        else if (mVerticalEdgeToLeftOfInactiveRect == mVerticalEdgeToRightOfInactiveRect) {
            //if inactive rect lies towards the left or middle of container rect, then the left edge is priortized as the selected edge for outside snapping 
            if (differenceContainerLeftEdgeToInactiveRectLeftEdge <= differenceContainerRightEdgeToInactiveRectRightEdge) { 
                mMinDistancePosition = mInactiveRectToCompare.left();
                mDistanceVerticalEdges = mVerticalEdgeToLeftOfInactiveRect;
                xSnapGapAdjustment = 0.0;
                mRightInRange = false;
            }
            //else right of the inactive rect lies more close to the right of the container rect, and hence prioritize it for snapping.
            else {
                mMinDistancePosition = mInactiveRectToCompare.right();
                mDistanceVerticalEdges = mVerticalEdgeToRightOfInactiveRect;
                xSnapGapAdjustment = mSnapGap;
                mLeftInRange = false;
            }
        }
        //else right edge of inactive rect to the left of the moving rect is closer than the left edge of inactive rect to the left of the moving rect
        else{
            mMinDistancePosition = mInactiveRectToCompare.right();
            mDistanceVerticalEdges = mVerticalEdgeToRightOfInactiveRect;
            xSnapGapAdjustment = mSnapGap;
            mLeftInRange = false;
        }
    }

    //Check if this inactive rect is better fit than the previous selected rect for Horizontal - snapping
    checkInactiveRectBetterFitForHorizontalSnapping(differenceContainerLeftEdgeToInactiveRectLeftEdge,
                                                    differenceContainerRightEdgeToInactiveRectRightEdge);

    if (mIsBetterFitHorizontalSnap) {
        qreal proposedRightOfActiveRect = mMinDistancePosition + xSnapGapAdjustment + mActiveRectWidth;
        if (qBound(mContainerRect.left(), proposedRightOfActiveRect, mContainerRect.right())
            == proposedRightOfActiveRect) {
            mHorizontalSnapFound = true;
            mHorizontalSnapPosition = mMinDistancePosition + xSnapGapAdjustment;
            mMinVerticalEdgesDistance = mDistanceVerticalEdges;
            mVerticalDistanceFromSelectedRect = mVerticalDistance;
            //Save the new distance of the Chosen Rectangle's left edge from Container's left edge
            mContainerVerticalEdgeDistance = differenceContainerLeftEdgeToInactiveRectLeftEdge;
            createVerticalLine();
        }
    }
}

/*!
    Check if the right edge of moving rect is snappable to the incative rect's left or right edge.
    The inactive rect's edge is only chosen if it is a better fit for horizontal snapping.
*/
void HsSnapToLines::compareRightSideOfMovingRectForSnapping()
{
    checkInactiveRectLieAboveOrBelowOfMovingRect();

    checkInactiveRectVerticalEdgesInRange(mMovingRect.right());

    //calculate the distance of inactive rect's left edge and container rect's left edge
    qreal differenceContainerLeftEdgeToInactiveRectLeftEdge = mInactiveRectToCompare.left() - mContainerRect.left();
    //calculate the distance of inactive rect's right edge and container rect's right edge
    qreal differenceContainerRightEdgeToInactiveRectRightEdge = mContainerRect.right() - mInactiveRectToCompare.right();
    qreal xSnapGapAdjustment = 0.0;
    mDistanceVerticalEdges = 0.0;
    mMinDistancePosition = 0.0;

    //If only one edge of inactive rect is in snappable range, save that position
    if ((mLeftInRange && !mRightInRange)
        || !mLeftInRange && mRightInRange) {
        if (mLeftInRange) {
            mMinDistancePosition = mInactiveRectToCompare.left();
            mDistanceVerticalEdges = mVerticalEdgeToLeftOfInactiveRect;
            xSnapGapAdjustment = mSnapGap;
        }
        else {
            mMinDistancePosition = mInactiveRectToCompare.right();
            mDistanceVerticalEdges = mVerticalEdgeToRightOfInactiveRect;
            xSnapGapAdjustment = 0.0;
        }
    }
    //else both edges of inactive rect are in range, check which is a better fit
    else if (mLeftInRange && mRightInRange) {
        //if right edge of moving rect to the right of the inactive rect is closer than the right edge of moving rect to the left of inactive rect
        if (mVerticalEdgeToRightOfInactiveRect < mVerticalEdgeToLeftOfInactiveRect) {
            mMinDistancePosition = mInactiveRectToCompare.right();
            mDistanceVerticalEdges = mVerticalEdgeToRightOfInactiveRect;
            xSnapGapAdjustment = 0.0;
            mLeftInRange = false;
        }
        //if the right edge of moving rect to right of inactive rect is at the same distance as the right edge of moving rect to the left of inactive rect
        else if (mVerticalEdgeToRightOfInactiveRect == mVerticalEdgeToLeftOfInactiveRect) {
            //if inactive rect lies towards the right of container rect, then the right edge is priortized as the selected edge for outside snapping
            if (differenceContainerRightEdgeToInactiveRectRightEdge < differenceContainerLeftEdgeToInactiveRectLeftEdge ) { 
                mMinDistancePosition = mInactiveRectToCompare.right();
                mDistanceVerticalEdges = mVerticalEdgeToRightOfInactiveRect;
                xSnapGapAdjustment = 0.0;
                mLeftInRange = false;
            }
            //else left of the inactive rect lies more close to the left or middle of the container rect, and hence prioritize it
            else {
                mMinDistancePosition = mInactiveRectToCompare.left();
                mDistanceVerticalEdges = mVerticalEdgeToLeftOfInactiveRect;
                xSnapGapAdjustment = mSnapGap;
                mRightInRange = false;
            }
        }
        //else right edge of moving rect to the left of the inactive rect is closer than the right edge of moving rect to the right of the incoming rect
        else{
            mMinDistancePosition = mInactiveRectToCompare.left();
            mDistanceVerticalEdges = mVerticalEdgeToLeftOfInactiveRect;
            xSnapGapAdjustment = mSnapGap;
            mRightInRange = false;
        }
    }

    //Check if this inactive rect is better fit than the previous selected rect 
    checkInactiveRectBetterFitForHorizontalSnapping(differenceContainerRightEdgeToInactiveRectRightEdge,
                                                    differenceContainerLeftEdgeToInactiveRectLeftEdge);

    if (mIsBetterFitHorizontalSnap) {
        qreal proposedLeftOfActiveRect = mMinDistancePosition - mActiveRectWidth - xSnapGapAdjustment;
        if (qBound(mContainerRect.left(), proposedLeftOfActiveRect, mContainerRect.right())
            == proposedLeftOfActiveRect) {
            mHorizontalSnapFound = true;
            mHorizontalSnapPosition = proposedLeftOfActiveRect;
            mMinVerticalEdgesDistance = mDistanceVerticalEdges;
            mVerticalDistanceFromSelectedRect = mVerticalDistance;
            //Save the new distance of the Chosen Rectangle's right edge from Container's right edge
            mContainerVerticalEdgeDistance = differenceContainerRightEdgeToInactiveRectRightEdge;
            createVerticalLine();
        }
    }
}

/*!
    Create the vertical line for horizontal snap guidance
*/
void HsSnapToLines::createVerticalLine()
{
    qreal top = qMin( mInactiveRectToCompare.top(), mMovingRect.top());
    qreal bottom = qMax(mInactiveRectToCompare.bottom(), mMovingRect.bottom()); 
    mVerticalLine.setP1(QPointF(mMinDistancePosition, top));
    mVerticalLine.setP2(QPointF(mMinDistancePosition, bottom));
}

/*!
    Check if the inactive rect being compared with moving rect lies on left or right of moving rect.
*/
void HsSnapToLines::checkInactiveRectLieLeftOrRightOfMovingRect()
{
    mRectLieLeft = false;
    mHorizontalDistance = 0.0;
    //Inactive Rect lies left of the Moving Rect
    if (mInactiveRectToCompare.left() < mMovingRect.left()) {
        mRectLieLeft = true;
        if (mInactiveRectToCompare.right() <= mMovingRect.left()) {
            mHorizontalDistance = mMovingRect.left() - mInactiveRectToCompare.right();
        }
        else {
            mHorizontalDistance = mMovingRect.left() - mInactiveRectToCompare.left();
        }
    }
    //Inactive Rect lies right of the Moving Rect
    else {
        mRectLieLeft = false;
        if (mMovingRect.right() <= mInactiveRectToCompare.left()) {
            mHorizontalDistance = mInactiveRectToCompare.left() - mMovingRect.right();
        }
        else {
            mHorizontalDistance = mInactiveRectToCompare.right() - mMovingRect.right();
        }
    }
}

/*!
    Check if the Horizontal edges (Top and Bottom Edges) of the inactive rect being compared
    is in range of the snapping distance of the horizontal edge of moving rect
    \param movingRectHorizontalEdgePosition Position of the Horizontal edge(either top or bottom) of moving rect.
*/
void HsSnapToLines::checkInactiveRectHorizontalEdgesInRange(qreal movingRectHorizontalEdgePosition)
{
    mTopInRange = false;
    mBottomInRange = false;

    //calculate the distance of the moving rect's horizontal edge to the inactive rect's top and bottom edges
    mHorizontalEdgeToTopOfInactiveRect = qAbs(mInactiveRectToCompare.top() - movingRectHorizontalEdgePosition);
    mHorizontalEdgeToBottomOfInactiveRect = qAbs(mInactiveRectToCompare.bottom() - movingRectHorizontalEdgePosition);

    if (mHorizontalEdgeToTopOfInactiveRect <= mMinHorizontalEdgesDistance) {
        if (mRectLieLeft && mInactiveSnapRectToCompare.isTopSnapableForRight
         || !mRectLieLeft && mInactiveSnapRectToCompare.isTopSnapableForLeft) {
            mTopInRange = true;
        }
    }
    if (mHorizontalEdgeToBottomOfInactiveRect <= mMinHorizontalEdgesDistance) {
        if (mRectLieLeft && mInactiveSnapRectToCompare.isBottomSnapableForRight
         || !mRectLieLeft && mInactiveSnapRectToCompare.isBottomSnapableForLeft) {
            mBottomInRange = true;
        }
    }
}

/*!
    Check if this inactive rect is better fit for Vertical snapping
    \param containerHorizontalEdgeToInactiveRectHorizontalEdge
           difference between the horizontal edge of the container and same horizontal edge of inactive rect
    \param containerOtherHorizontalEdgeToInactiveRectOtherHorizontalEdge
           difference between the opposite horizontal edges of continer and the inactive rect
*/
void HsSnapToLines::checkInactiveRectBetterFitForVerticalSnapping(qreal containerHorizontalEdgeToInactiveRectHorizontalEdge,
                                                       qreal containerOtherHorizontalEdgeToInactiveRectOtherHorizontalEdge)
{
    mIsBetterFitVerticalSnap = false;
    if (mTopInRange || mBottomInRange) {
        if (mDistanceHorizontalEdges < mMinHorizontalEdgesDistance) {
            mIsBetterFitVerticalSnap = true;
        }
        else if (mDistanceHorizontalEdges == mMinHorizontalEdgesDistance) { //the distance in the horizontal edge is same as from the selected rectangle
            //check the position of rect with respect to horizontal line
            checkInactiveRectPositionToHorizontalLine();
            //if vertical snap position was already found and this rect's horizontal edges lies in line with Horizontal snap line
            if (mVerticalSnapFound && mRectHorizontalEdgeLiesInLineWithHorizontalLine) {
                if (mRectLieLeftOfHorizontalLine || mRectLiesRightOfHorizontalLine) {
                    extendHorizontalLineToIncludeInactiveRect();
                }
            }
            else {
                //Prioritize first on the fact if the inactive rect is closer to the moving rect in X - direction.
                if (mHorizontalDistance < mHorizontalDistanceFromSelectedRect) {
                    mIsBetterFitVerticalSnap = true;
                }
                else if (mHorizontalDistance == mHorizontalDistanceFromSelectedRect) {
                    //Prioritize next if this Inactive rect is closer to the top edge of the container rect, then the previously selected rect
                    if (containerHorizontalEdgeToInactiveRectHorizontalEdge < mContainerHorizontalEdgeDistance) {
                        mIsBetterFitVerticalSnap = true;
                    }
                    //Prioritize next if the Inactive widget's top edge lies near to top edge of the container rect
                    else if (containerHorizontalEdgeToInactiveRectHorizontalEdge < containerOtherHorizontalEdgeToInactiveRectOtherHorizontalEdge) {
                        mIsBetterFitVerticalSnap = true;
                    }
                    else {
                        //This else will happen if this rectangle being compared is exactly the same as the selected rectangle for snapping, or in opposite X direction.
                        //In that case it does not matter which is the selected rectangle. Hence we leave the already selected rectangle as the better fit.
                    }
                }
            }
        }
    }

}

/*!
    Check if the top edge of moving rect is snappable to the incative rect's top or bottom edge.
    The inactive rect's edge is only chosen if it is a better fit for vertical snapping.
*/
void HsSnapToLines::compareTopOfMovingRectForSnapping()
{
    //Check if the inactive rect lies to the left or right of the moving rect
    checkInactiveRectLieLeftOrRightOfMovingRect();

    checkInactiveRectHorizontalEdgesInRange(mMovingRect.top());

    //calculate the distance of inactive rect's top edge and container rect's top edge
    qreal differenceContainerTopEdgeToInactiveRectTopEdge = mInactiveRectToCompare.top() - mContainerRect.top();
    //calculate the distance of inactive rect's bottom edge and container rect's bottom edge
    qreal differenceContainerBottomEdgeToInactiveRectBottomEdge = mContainerRect.bottom() - mInactiveRectToCompare.bottom();
    qreal ySnapGapAdjustment = 0.0;
    mDistanceHorizontalEdges = 0.0;
    mMinDistancePosition = 0.0;

    //If only one edge of inactive rect is in snappable range, save that position
    if ((mTopInRange && !mBottomInRange)
        || !mTopInRange && mBottomInRange) {
        if (mTopInRange) {
            mMinDistancePosition = mInactiveRectToCompare.top();
            mDistanceHorizontalEdges = mHorizontalEdgeToTopOfInactiveRect;
            ySnapGapAdjustment = 0.0;
        }
        else {
            mMinDistancePosition = mInactiveRectToCompare.bottom();
            mDistanceHorizontalEdges = mHorizontalEdgeToBottomOfInactiveRect;
            ySnapGapAdjustment = mSnapGap;
        }
    }
    //else both edges of inactive rect are in range, check which is a better fit
    else if (mTopInRange && mBottomInRange) {
        //if top edge of moving rect to the top of the inactive rect is closer than the bottom edge of moving rect to the bottom of the inactive rect
        if (mHorizontalEdgeToTopOfInactiveRect < mHorizontalEdgeToBottomOfInactiveRect) {
            mMinDistancePosition = mInactiveRectToCompare.top();
            mDistanceHorizontalEdges = mHorizontalEdgeToTopOfInactiveRect;
            ySnapGapAdjustment = 0.0;
            mBottomInRange = false;
        }
        //if the top edge of moving rect to top of inactive rect is at the same distance as the top edge of moving rect to the bottom of inactive rect
        else if (mHorizontalEdgeToTopOfInactiveRect == mHorizontalEdgeToBottomOfInactiveRect) {
            //if inactive rect lies towards the top or middle of container rect, then the top edge is priortized as the selected edge for outside snapping
            if (differenceContainerTopEdgeToInactiveRectTopEdge <= differenceContainerBottomEdgeToInactiveRectBottomEdge) { 
                mMinDistancePosition = mInactiveRectToCompare.top();
                mDistanceHorizontalEdges = mHorizontalEdgeToTopOfInactiveRect;
                ySnapGapAdjustment = 0.0;
                mBottomInRange = false;
            }
            //else bottom of the inactive rect lies more close to the bottom of the container rect, and hence prioritize it for snapping.
            else {
                mMinDistancePosition = mInactiveRectToCompare.bottom();
                mDistanceHorizontalEdges = mHorizontalEdgeToBottomOfInactiveRect;
                ySnapGapAdjustment = mSnapGap;
                mTopInRange = false;
            }
        }
        //else top edge of moving rect to the bottom of the inactive rect is closer than the top edge of moving rect to the top of the inactive rect
        else{
            mMinDistancePosition = mInactiveRectToCompare.bottom();
            mDistanceHorizontalEdges = mHorizontalEdgeToBottomOfInactiveRect;
            ySnapGapAdjustment = mSnapGap;
            mTopInRange = false;
        }
    }

    //Check if this inactive rect is better fit than the previous selected rect 
    checkInactiveRectBetterFitForVerticalSnapping(differenceContainerTopEdgeToInactiveRectTopEdge,
                                                  differenceContainerBottomEdgeToInactiveRectBottomEdge);

    if (mIsBetterFitVerticalSnap) {
        qreal proposedBottomOfActiveRect = mMinDistancePosition + mActiveRectHeight + ySnapGapAdjustment;
        if (qBound(mContainerRect.top(), proposedBottomOfActiveRect, mContainerRect.bottom())
            == proposedBottomOfActiveRect) {
            mVerticalSnapFound = true;
            mVerticalSnapPosition = mMinDistancePosition + ySnapGapAdjustment;
            mMinHorizontalEdgesDistance = mDistanceHorizontalEdges;
            mHorizontalDistanceFromSelectedRect = mHorizontalDistance;
            //Save the new distance of the Chosen Rectangle's top edge from Container's top edge
            mContainerHorizontalEdgeDistance = differenceContainerTopEdgeToInactiveRectTopEdge;
            createHorizontalLine();
        }
    }
}

/*!
    Check if the bottom edge of moving rect is snappable to the incative rect's top or bottom edge.
    The inactive rect's edge is only chosen if it is a better fit for vertical snapping.
*/
void HsSnapToLines::compareBottomOfMovingRectForSnapping()
{
    //Check if the inactive rect lies to the left or right of the moving rect
    checkInactiveRectLieLeftOrRightOfMovingRect();

    //calculate the distance of the moving rect's bottom edge to the inactive rect's top and bottom edges
    checkInactiveRectHorizontalEdgesInRange(mMovingRect.bottom());

    //calculate the distance of inactive rect's top edge and container rect's top edge
    qreal differenceContainerTopEdgeToInactiveRectTopEdge = mInactiveRectToCompare.top() - mContainerRect.top();
    //calculate the distance of inactive rect's bottom edge and container rect's bottom edge
    qreal differenceContainerBottomEdgeToInactiveRectBottomEdge = mContainerRect.bottom() - mInactiveRectToCompare.bottom();
    qreal ySnapGapAdjustment = 0.0;
    mDistanceHorizontalEdges = 0.0;
    mMinDistancePosition = 0.0;

    //If only one edge of inactive rect is in snappable range, save that position
    if ((mTopInRange && !mBottomInRange)
        || !mTopInRange && mBottomInRange) {
        if (mTopInRange) {
            mMinDistancePosition = mInactiveRectToCompare.top();
            mDistanceHorizontalEdges = mHorizontalEdgeToTopOfInactiveRect;
            ySnapGapAdjustment = mSnapGap;
        }
        else {
            mMinDistancePosition = mInactiveRectToCompare.bottom();
            mDistanceHorizontalEdges = mHorizontalEdgeToBottomOfInactiveRect;
            ySnapGapAdjustment = 0.0;
        }
    }
    //else both edges of inactive rect are in range, check which is a better fit
    else if (mTopInRange && mBottomInRange) {
        //if bottom edge of moving rect to the bottom of inactive rect is closer than the bottom edge of moving rect to the top of the inactive rect
        if (mHorizontalEdgeToBottomOfInactiveRect < mHorizontalEdgeToTopOfInactiveRect ) {
            mMinDistancePosition = mInactiveRectToCompare.bottom();
            mDistanceHorizontalEdges = mHorizontalEdgeToBottomOfInactiveRect;
            ySnapGapAdjustment = 0.0;
            mTopInRange = false;
        }
        //if bottom edge of moving rect to the bottom of inactive rect is at the same distance as the bottom edge of moving rect to the top of inactive rect
        else if (mHorizontalEdgeToBottomOfInactiveRect == mHorizontalEdgeToTopOfInactiveRect) {
            //if inactive rect lies towards the bottom of container rect, then the bottom edge is priortized as the selected edge for snapping
            //This is done for outside snapping
            if (differenceContainerBottomEdgeToInactiveRectBottomEdge < differenceContainerTopEdgeToInactiveRectTopEdge) {
                mMinDistancePosition = mInactiveRectToCompare.bottom();
                mDistanceHorizontalEdges = mHorizontalEdgeToBottomOfInactiveRect;
                ySnapGapAdjustment = 0.0;
                mTopInRange = false;
            }
            //else top of the inactive rect lies more close to the top of the container rect or at the same distance, and hence prioritize it
            else {
                mMinDistancePosition = mInactiveRectToCompare.top();
                mDistanceHorizontalEdges = mHorizontalEdgeToTopOfInactiveRect;
                ySnapGapAdjustment = mSnapGap;
                mBottomInRange = false;
            }
        }
        //else bottom edge of moving rect to the top of inactive rect is closer than the bottom edge of moving rect to the bottom of the inactive rect
        else{
            mMinDistancePosition = mInactiveRectToCompare.top();
            mDistanceHorizontalEdges = mHorizontalEdgeToTopOfInactiveRect;
            ySnapGapAdjustment = mSnapGap;
            mBottomInRange = false;
        }
    }

    //Check if this inactive rect is better fit than the previous selected rect 
    checkInactiveRectBetterFitForVerticalSnapping(differenceContainerBottomEdgeToInactiveRectBottomEdge,
                                                  differenceContainerTopEdgeToInactiveRectTopEdge);

    if (mIsBetterFitVerticalSnap) {
        qreal proposedTopOfActiveRect = mMinDistancePosition - mActiveRectHeight - ySnapGapAdjustment;
        if (qBound(mContainerRect.top(), proposedTopOfActiveRect, mContainerRect.bottom())
            == proposedTopOfActiveRect) {
            mVerticalSnapFound = true;
            mVerticalSnapPosition = proposedTopOfActiveRect;
            mMinHorizontalEdgesDistance = mDistanceHorizontalEdges;
            mHorizontalDistanceFromSelectedRect = mHorizontalDistance;
            //Save the new distance of the Selected Rectangle's bottom edge from Container's bottom edge
            mContainerHorizontalEdgeDistance = differenceContainerBottomEdgeToInactiveRectBottomEdge;
            createHorizontalLine();
        }
    }
}

/*!
    Create the horizontal line for vertical snap guidance
*/
void HsSnapToLines::createHorizontalLine()
{
    qreal left = qMin( mInactiveRectToCompare.left(), mMovingRect.left());
    qreal right = qMax(mInactiveRectToCompare.right(), mMovingRect.right()); 
    mHorizontalLine.setP1(QPointF(left, mMinDistancePosition));
    mHorizontalLine.setP2(QPointF(right, mMinDistancePosition));
}

/*!
    Extend the Vertical line on both side of reference(snapping) rectancles.
*/
void HsSnapToLines::extendVerticalLine()
{
    if (mVerticalLine.y1() <= mVerticalLine.y2()) {
        mVerticalLine.setP1(QPointF(mVerticalLine.x1(), mVerticalLine.y1() - SNAP_LINE_EXTEND_VALUE));
        mVerticalLine.setP2(QPointF(mVerticalLine.x2(), mVerticalLine.y2() + SNAP_LINE_EXTEND_VALUE));
    }
    else {
        mVerticalLine.setP1(QPointF(mVerticalLine.x1(), mVerticalLine.y1() + SNAP_LINE_EXTEND_VALUE));
        mVerticalLine.setP2(QPointF(mVerticalLine.x2(), mVerticalLine.y2() - SNAP_LINE_EXTEND_VALUE));
    }
}

/*!
    Extend the Horizontal line on both side of reference(snapping) rectancles.
*/
void HsSnapToLines::extendHorizontalLine()
{
    if (mHorizontalLine.x1() <= mHorizontalLine.x2()) {
        mHorizontalLine.setP1(QPointF(mHorizontalLine.x1() - SNAP_LINE_EXTEND_VALUE, mHorizontalLine.y1()));
        mHorizontalLine.setP2(QPointF(mHorizontalLine.x2() + SNAP_LINE_EXTEND_VALUE, mHorizontalLine.y2()));
    }
    else {
        mHorizontalLine.setP1(QPointF(mHorizontalLine.x1() + SNAP_LINE_EXTEND_VALUE, mHorizontalLine.y1()));
        mHorizontalLine.setP2(QPointF(mHorizontalLine.x2() - SNAP_LINE_EXTEND_VALUE, mHorizontalLine.y2()));
    }
}

/*!
    Check if the Vertical edge of the Rectangle lies out of the Vertical line.
    Also check if the rectangle's edge lies out of the line.
*/
void HsSnapToLines::checkInactiveRectPositionToVerticalLine()
{
    mRectVerticalEdgeLiesInLineWithVerticalLine = false;
    mRectLieAboveVerticalLine = false;
    mRectLieBelowVerticalLine = false;

    //if rectangle vertical edge lies inline with Vertical line.
    if ((mLeftInRange && mInactiveRectToCompare.left() == mVerticalLine.x1())
        || (mRightInRange && mInactiveRectToCompare.right() == mVerticalLine.x1())) {
        mRectVerticalEdgeLiesInLineWithVerticalLine = true;
        //if the rectangle lies below the vertical line
        if (mInactiveRectToCompare.bottom() > mVerticalLine.y1() && mInactiveRectToCompare.bottom() > mVerticalLine.y2()) {
            mRectLieBelowVerticalLine = true;
        }
        //if the rectangle lies above the vertical line
        if (mInactiveRectToCompare.top() < mVerticalLine.y1() && mInactiveRectToCompare.top() < mVerticalLine.y2()) {
            mRectLieAboveVerticalLine = true;
        }
    }
}

/*!
    Increase the Vertical line to include the inactive rect whose vertical edge is inline with vertical line
*/
void HsSnapToLines::extendVerticalLineToIncludeInactiveRect()
{
    if (mRectLieAboveVerticalLine) {
        if (mVerticalLine.y1() < mVerticalLine.y2()) {
            mVerticalLine.setP1(QPointF(mVerticalLine.x1(), mInactiveRectToCompare.top()));
        }
        else {
            mVerticalLine.setP2(QPointF(mVerticalLine.x1(), mInactiveRectToCompare.top()));
        }
    }
    if (mRectLieBelowVerticalLine) {
        if (mVerticalLine.y1() < mVerticalLine.y2()) {
            mVerticalLine.setP2(QPointF(mVerticalLine.x1(), mInactiveRectToCompare.bottom()));
        }
        else {
            mVerticalLine.setP1(QPointF(mVerticalLine.x1(), mInactiveRectToCompare.bottom()));
        }
    }
}

/*!
    Check if the Horizontal edge of the Rectangle lies inline with the Horizontal line.
    Also check if the rectangle's edge lies out of the line.
*/
void HsSnapToLines::checkInactiveRectPositionToHorizontalLine()
{
    mRectHorizontalEdgeLiesInLineWithHorizontalLine = false;
    mRectLieLeftOfHorizontalLine = false;
    mRectLiesRightOfHorizontalLine = false;

    //if rectangle horizontal edge lies inline with Horizontal line.
    if ((mTopInRange && mInactiveRectToCompare.top() == mHorizontalLine.y1())
        || (mBottomInRange && mInactiveRectToCompare.bottom() == mHorizontalLine.y1())) {
        mRectHorizontalEdgeLiesInLineWithHorizontalLine = true;
        //if the rectangle lies left of the horizontal line
        if (mInactiveRectToCompare.left() < mHorizontalLine.x1() && mInactiveRectToCompare.left() < mHorizontalLine.x2()) {
            mRectLieLeftOfHorizontalLine = true;
        }
        //if the rectangle lies right of the horizontal line
        if (mInactiveRectToCompare.right() > mHorizontalLine.x1() && mInactiveRectToCompare.right() > mHorizontalLine.x2()) {
            mRectLiesRightOfHorizontalLine = true;
        }
    }
}

/*!
    Increase the Horizontal line to include the inactive rect whose horizontal edge is inline with horizontal line
*/
void HsSnapToLines::extendHorizontalLineToIncludeInactiveRect()
{
    if (mRectLieLeftOfHorizontalLine) {
        if (mHorizontalLine.x1() < mHorizontalLine.x2()) {
            mHorizontalLine.setP1(QPointF(mInactiveRectToCompare.left(), mHorizontalLine.y1()));
        }
        else {
            mHorizontalLine.setP2(QPointF(mInactiveRectToCompare.left(), mHorizontalLine.y1()));
        }
    }
    if (mRectLiesRightOfHorizontalLine) {
        if (mHorizontalLine.x1() < mHorizontalLine.x2()) {
            mHorizontalLine.setP2(QPointF(mInactiveRectToCompare.right(), mHorizontalLine.y1()));
        }
        else {
            mHorizontalLine.setP1(QPointF(mInactiveRectToCompare.right(), mHorizontalLine.y1()));
        }
    }
}
