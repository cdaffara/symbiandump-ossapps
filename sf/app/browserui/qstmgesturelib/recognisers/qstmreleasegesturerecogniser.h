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
#ifndef QSTMRELEASEGESTURERECOGNISER_H_
#define QSTMRELEASEGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>


namespace qstmGesture
{

/*!
 * QStm_ReleaseGestureRecogniser handles release "gesture"
 * This is countarpart to touch gesture recogniser, just
 * informing the app that ERelease has been seen.
 * This should be placed towards the end of the list of gesture
 * recognisers, all other recognisers which handle ERelease with
 * some other events preceding the ERelease must be before this one.
 *
 */
class QStm_ReleaseGestureRecogniser : public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidRelease;
    virtual ~QStm_ReleaseGestureRecogniser();

    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;

    virtual QStm_GestureUid gestureUid() const  { return KUid; }
    void setArea(const QRect& theArea) ;

    QStm_ReleaseGestureRecogniser(QStm_GestureListenerIf* listener) ;

private:
    QRect m_area ;
};

} // namespace

#endif /* QSTMRELEASEGESTURERECOGNISER_H_ */
