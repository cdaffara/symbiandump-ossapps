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

#ifndef QSTMEDGESCROLLGESTURERECOGNISER_H_
#define QSTMEDGESCROLLGESTURERECOGNISER_H_


#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{

/*!
 * QStm_EdgeScrollGestureRecogniser handles edge scroll gesture: Hold event near the edges of the window
 *
 */
class QStm_EdgeScrollGestureRecogniser : public QObject, public QStm_GestureRecogniser
{
public:
    static const QStm_GestureUid KUid = EGestureUidEdgeScroll;
    
    QStm_EdgeScrollGestureRecogniser(QStm_GestureListenerIf* listener) ;
    virtual ~QStm_EdgeScrollGestureRecogniser();

    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;
    virtual void enable(bool enabled) ;
    virtual bool isEnabled() ;
    virtual void setOwner(void* owner) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    void setArea(const QRect& theArea)  ;

    /*!
     * Define how close to the border the Hold UI event must be
     * \param rangeInPixels
     */
    void setScrollRange(int rangeInPixels) ;
    /*!
     * for testing purposes we need to log the stuff somewhere...
     */
    virtual void enableLogging(bool loggingOn) ;
    /*!
     * The timer function to handle timeout for generating scroll events
     */

private:
    bool      m_loggingenabled ;
    void*     m_powner ; // The owning control for this gesture
    QStm_GestureListenerIf* m_listener ;
    bool      m_waitingforsecondtap ;
    QRect     m_area ;
    bool      m_gestureEnabled ;
    int       m_rangesizeInPixels ;
};

} // namespace


#endif /* QSTMEDGESCROLLGESTURERECOGNISER_H_ */
