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

#ifndef QSTMGENERICSIMPLEGESTURE_H_
#define QSTMGENERICSIMPLEGESTURE_H_
#include "qstmgesture_if.h"
#include "qstmuievent_if.h"


namespace qstmGesture
{

/*!
 * Class for creating a simple gesture for notifying the listener.
 * If the basic features are enough for the gesture, then this class
 * is enough to be instantiated; there is the location and speed
 * determined from the UI event; also type can be specified as well
 * as integer detail data.
 */
class QStm_GenericSimpleGesture : public QStm_GestureIf
{
public:
    QStm_GenericSimpleGesture(
        QStm_GestureUid uid, const QPoint& loc, int type = 0,
        const qstmUiEventEngine::QStm_UiEventSpeedIf* speedIf = NULL);

public: // implementation of QStm_GestureIf

    QStm_GestureUid gestureUid() const { return m_uid; }
    void setDetails(void* details) { m_details = details;}  // additional info can be passed here.
    virtual QPoint getLocation() const ;  // Location where the gesture happened (if applicable)
    virtual int getType() const ;         // If the gesture can have different types
    
    virtual float getSpeed() const /*__SOFTFP*/
    {
        return m_speed ? m_speed->speed() : 0.f;
    }
    
    virtual QPointF getSpeedVec() const 
    {
        return m_speed ? m_speed->speedVec() : QPointF(0.0, 0.0);	
    }
    
    virtual void* getDetails() const ;    // Other possible details....

    virtual void setName(const QString& name) ;
    virtual QString getGestureName() ;       // String name for gesture

    virtual void setTarget(void* target) { m_target = target; }
    virtual void* target() { return m_target; }
    
public:
    void setType(int type) { m_type = type; }

protected:
    QPoint              m_location ;
    QStm_GestureUid     m_uid;
    int                 m_type ;
    void*               m_details ;
    const qstmUiEventEngine::QStm_UiEventSpeedIf* m_speed ;
    QString             m_name ;
    void*               m_target;
};

class QStm_DirectionalGesture : public QStm_GenericSimpleGesture
{
public:
    QStm_DirectionalGesture(
        QStm_GestureUid uid, const QPoint& loc, const QPoint& prevLoc,
        const qstmUiEventEngine::QStm_UiEventSpeedIf* speedIf = NULL, bool logging = false);

    void setVector(const QPoint& last, const QPoint& previous) ;
    void setLogging(bool enabled) { m_loggingEnabled = enabled; }

    QPoint getLengthAndDirection() const {return m_vector;}
    QStm_GestureDirection getDirection() const ;
    int getLength() const
    {
        return ((m_vector.x() * m_vector.x()) + (m_vector.y() * m_vector.y())) ;
    }

protected:
    QPoint m_vector ;
    bool m_loggingEnabled ;
};

class QStm_TwoPointGesture : public QStm_DirectionalGesture
{
public:
    QStm_TwoPointGesture(QStm_GestureUid uid, const QPoint& pos1, const QPoint& pos2);

private:
    QPoint m_location2 ;
};

} // namespace



#endif /* QSTMGENERICSIMPLEGESTURE_H_ */
