/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef QSTMGESTURE_IF_H_
#define QSTMGESTURE_IF_H_

#include <QtCore>
namespace qstmGesture
{

enum QStm_GestureUid
{
    EGestureUidUnknown     = 0,
    EGestureUidTouch,
    EGestureUidTap,
    EGestureUidDoubleTap   = EGestureUidTap,
    EGestureUidLongPress,
    EGestureUidHover,
    EGestureUidPan,
    EGestureUidRelease,
    EGestureUidLeftRight,
    EGestureUidUpDown,
    EGestureUidFlick,
    EGestureUidEdgeScroll,
    EGestureUidPinch,
    EGestureUidCornerZoom,
    //
    EStmGestureUid_Count
};

enum QStm_GestureAreaSettings
{
    ETouchTimeArea,
    ETouchArea,
    EHoldArea,
    //
    EGestureAreaSettingsCount
};

enum QStm_GestureSettings
{
    //ETouchTimeout,
    EDoubleTapTimeout,
    ESuppressTimeout,
    EMoveSuppressTimeout,
    EHoverSpeed,
    EPanSpeedLow,
    EPanSpeedHigh,
    EFlickSpeed, 
    EPinchSpeed,
    EZoomCornerSize,
    EEdgeScrollRange,
    ECapacitiveUpUsed,
    EPanDisabledWhileHovering,
    EHoverDisabledWhilePanning,
    EAdjustYPos,
    EEnableFiltering,
    EWServMessageInterception, // TBool
    EMoveTolerance,
    //
    EGestureSettingsCount
};

struct QStm_GestureArea
{
public:
    enum QStm_Shape
    {
        // Use GestureArea settings from globally defined default
        EDefaultSettings = -1,
        ERectangle  = 1, // == stmUiEventEngine::ERectangle,
        ECircle     = 2, // == stmUiEventEngine::ECircle,
        EEllipse    = 3  // == stmUiEventEngine::EEllipse
    };

    QStm_Shape m_shape;
    int        m_timeout;
    QSize      m_size;
};

enum QStm_EdgeScroll
{
    EEdgeScrollUp = 1,
    EEdgeScrollDown,
    EEdgeScrollLeft,
    EEdgeScrollRight,
    EEdgeScrollUnknown
};

enum QStm_TapType
{
    ETapTypeSingle = 1,
    ETapTypeDouble
};

enum QStm_ZoomType
{
    EZoomUndefined  = 0,
    EZoomIn         = 1,
    EZoomOut        = 2
};

enum QStm_GestureDirection
{
    ENoDirection = 0,
    //
    ENorth = 1,
    ENorthEast,
    EEast,
    ESouthEast,
    ESouth,
    ESouthWest,
    EWest,
    ENorthWest,
};

/*!
 * Generic gesture interface.  The gestures
 * contain a generic part and a gesture specific part
 * is obtained using the getDetails() method.
 *
 */
class QStm_GestureIf
{
public:
    virtual QStm_GestureUid gestureUid() const = 0; // Identifier of recognizer type that issues the gesture
    virtual QPoint getLocation() const = 0 ;    // Location where the gesture happened (if applicable)
    virtual int getType() const = 0 ;           // If the gesture can have different types (like tap or double tap)
    virtual float getSpeed() const = 0 ;  // px/msec, as calculated in the UI state machine, not valid in all cases
    virtual void* getDetails() const = 0 ;        // Other possible details.... (can be a pointer to a larger struct)
    virtual QString getGestureName() = 0 ;       // String name for gesture, makes possible to use single callback for gestures
    virtual QPoint getLengthAndDirection() const { return QPoint(0,0); }  // very simple vector
    virtual QStm_GestureDirection getDirection() const { return qstmGesture::ENoDirection; }
    virtual QPointF getSpeedVec() const = 0;
    virtual void* target() = 0;
};

} // namespace

#endif /* QSTMGESTURE_IF_H_ */
