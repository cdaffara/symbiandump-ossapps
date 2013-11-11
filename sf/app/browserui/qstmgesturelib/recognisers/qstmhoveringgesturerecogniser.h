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
#ifndef QSTMHOVERINGGESTURERECOGNISER_H_
#define QSTMHOVERINGGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{
/*!
 * Recognise hovering gesture.
 * If the EMove distance from previous is short enough, then
 * we assume hovering gesture.  If the previous UI event is ETouch, EHold or EMove
 * and the distance is short enough the gesture takes control.
 */
class QStm_HoveringGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidHover;

    virtual ~QStm_HoveringGestureRecogniser();
    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    void setHoveringSpeed(float aSpeed) /*__SOFTFP */;

    QStm_HoveringGestureRecogniser(QStm_GestureListenerIf* listener) ;
private:
    bool m_hovering ;
    float m_hoveringspeed ;
};

}

#endif /* QSTMHOVERINGGESTURERECOGNISER_H_ */
