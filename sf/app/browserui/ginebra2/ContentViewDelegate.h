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

#ifndef __GINEBRA_CONTENTVIEWDELEGATE_H__
#define __GINEBRA_CONTENTVIEWDELEGATE_H__

#include <QObject>
#include <QtGui>
#include "controllableviewimpl.h"

namespace GVA {
  class ChromeWidget;

  class ContentViewDelegate : public ControllableViewBase
  {
    Q_OBJECT
  public:
    ContentViewDelegate(ChromeWidget *chrome, QObject * parent);
    virtual ~ContentViewDelegate();
    virtual QGraphicsWidget* view() { return widget(); }
    ChromeWidget *chrome() { return m_chrome; }
    QString name() const { return jsObject()->objectName(); }

//    Q_PROPERTY(QString name READ getName)
//    QString getName() const { return objectName(); }
//
// use isActive() instead.
//    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
//    bool isVisible() { return view()->isVisible(); }
//    void setVisible(bool value) { view()->setVisible(value); }

  public slots:
// use ControllableView version
//    void show() {view()->show();}
//    void hide() {view()->hide();}
    virtual void triggerAction(const QString & action){Q_UNUSED(action);return;}
  protected:
    ChromeWidget * m_chrome;
  };

} // end of namespace GVA

#endif // __GINEBRA_CONTENTVIEWDELEGATE_H__
