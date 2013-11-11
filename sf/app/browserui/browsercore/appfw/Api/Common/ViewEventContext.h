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

#ifndef VIEWEVENTCONTEXT_H_
#define VIEWEVENTCONTEXT_H_

#include <QPoint>
#include <QObject>

#include "BWFGlobal.h"

#include "scriptobjects.h"


    /*!
     * \brief Describes the context of a UI event in a ControllableView.  
     * 
     * The ViewEventContext class contains parameters that describe the context of a UI event 
     * that occurs in a ControllableView.  In the case of a long-press event, for example, it
     * would detail what link or image the user pressed.  These objects are visible to javascript.
     * \sa ControllableView
     */
    class BWF_EXPORT ViewEventContext : public QObject {
        Q_OBJECT
      public:
        ViewEventContext() {}
        ViewEventContext(const QString &viewType, const QPoint pos);
        ViewEventContext(const ViewEventContext &o);
        ~ViewEventContext() {}
                              
        // The geometric position of the event (relative to the parent widget). 
        QObject *getPos() { return static_cast<QObject*>(&m_pos); }
        Q_PROPERTY(QObject * pos READ getPos)
                
        // The type of the view in which the event occured.
        // \sa ControllableView::type()
        QString getViewType() const { return m_viewType; }
        Q_PROPERTY(QString viewType READ getViewType)
        
      protected:
        QPoint getPosition() const { return m_pos; }
        
      protected:
        QString m_viewType;
        ScriptPoint m_pos;        
    };

#endif /* VIEWEVENTCONTEXT_H_ */

