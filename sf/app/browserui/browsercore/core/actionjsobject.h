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

#ifndef _ACTIONJSOBJECT_H_INCLUDED
#define _ACTIONJSOBJECT_H_INCLUDED

#include <QObject>
#include <QAction>
#include <QPointer>

#include "BWFGlobal.h"

// Javascript wrapper for QActions.
class BWF_EXPORT ActionJSObject : public QObject {
    Q_OBJECT
  public:
    ActionJSObject() {}
    ActionJSObject(const ActionJSObject &o);
    ActionJSObject(QObject *parent, QAction *action);
    virtual ~ActionJSObject() {}

    // Initializes meta-types etc. for javascript access to this class.
    static void initClass();

    // Properties --------------------

    void setChecked(bool value);
    bool getChecked() const;
    Q_PROPERTY(bool checked READ getChecked WRITE setChecked)
    
    void setEnabled(bool value);
    bool getEnabled() const;
    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled)

    void setVisible(bool value);
    bool getVisible() const;
    Q_PROPERTY(bool visible READ getVisible WRITE setVisible)

    bool getValid() const;
    // Returns true if the underlying QAction still exists. 
    Q_PROPERTY(bool valid READ getValid)

  signals:
    void triggered ( bool checked = false );
    void toggled ( bool checked );
	void changed();

  public slots:
    QString text() const;
    QString toolTip() const;
	void trigger();
	void toggle();
	void hover();

  protected:
    QPointer<QAction> m_action;
};

// Declare meta-types of objects we expose to javascript.
Q_DECLARE_METATYPE(QObjectList)
Q_DECLARE_METATYPE(ActionJSObject*)

// Inlines ----------------------------

inline ActionJSObject::ActionJSObject(const ActionJSObject &o) : QObject()
    { m_action = o.m_action; }

inline void ActionJSObject::setChecked(bool value) 
    { if (m_action) m_action->setCheckable(value); }
inline bool ActionJSObject::getChecked() const
    { return m_action ? m_action->isCheckable() : false; }

inline void ActionJSObject::setEnabled(bool value) 
    { if (m_action) m_action->setEnabled(value); }
inline bool ActionJSObject::getEnabled() const
    { return m_action ? m_action->isEnabled() : false; }

inline void ActionJSObject::setVisible(bool value) 
    { if (m_action) m_action->setVisible(value); }
inline bool ActionJSObject::getVisible() const
    { return m_action ? m_action->isVisible() : false; }

inline QString ActionJSObject::text() const { return m_action ? m_action->text() : ""; }
inline QString ActionJSObject::toolTip() const { return m_action ? m_action->toolTip() : ""; }
inline void ActionJSObject::toggle() { if(m_action) m_action->toggle(); }
inline void ActionJSObject::hover() { if(m_action) m_action->hover(); }

inline bool ActionJSObject::getValid() const { return !m_action.isNull(); }

#endif // _ACTIONJSOBJECT_H_INCLUDED
