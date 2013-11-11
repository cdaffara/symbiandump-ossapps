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

#ifndef QSTMTOUCHGESTURERECOGNISER_H_
#define QSTMTOUCHGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{

/*!
 * QStm_TouchGestureRecogniser handles touch "gesture".  The purpose is to inform the app
 * that a touch has happened.  The gesture is never owned, since most of the gestures start with touch.
 * The app may define the area where to watch the gestures, or it can define empty area:
 * then the touch is recognised inside the window.
 *
 */
class QStm_TouchGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidTouch;

    virtual ~QStm_TouchGestureRecogniser();

    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    void setArea(const QRect& theArea) ;

    QStm_TouchGestureRecogniser(QStm_GestureListenerIf* listener) ;

private:
    QRect m_area ;
};

} // namespace

#endif /* QSTMTOUCHGESTURERECOGNISER_H_ */
