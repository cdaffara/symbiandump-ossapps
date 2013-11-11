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

#include "ViewController.h"
#include <QDebug>

namespace GVA {

  ViewController::ViewController()
    : m_viewMap() {
    m_current = m_viewMap.begin();
  }

  ViewController::~ViewController() {
    foreach(ControllableViewBase *view, m_viewMap) {
      delete view;
    }
  }

  void ViewController::addView(ControllableViewBase *controllableView) {
    assert(controllableView);
    QString key;
    // Set up parent/child link for javascript access to the view.
    if (controllableView->jsObject()) {
      // Use the view's javascript object.
      controllableView->jsObject()->setParent(this);
      key = controllableView->jsObject()->objectName();
    }
    else {
      // Use the view itself.
      controllableView->setParent(this);
      key = controllableView->objectName();
    }
    if (key.isNull()) {
      qWarning("ViewController::addView: missing objectName.");
    }
    m_viewMap.insert(key, controllableView);
    // Set the only view to current view
    if (m_viewMap.size() == 1)
        m_current = m_viewMap.begin();
  }

  QObjectList ViewController::getViews() {
    QObjectList *result = new QObjectList;
    foreach(ControllableViewBase *view, m_viewMap) {
      result->append(view);
    }
    return *result;
  }

  void ViewController::showCurrent() {
      ControllableViewBase *currentView = m_current.value();
      if (!currentView) return;

      if (!currentView->isActive()) {
          emit currentViewChanging();
          // Activate the current view.
          currentView->activate();
          currentView->show();

          // Deactivate all others.
          foreach(ControllableViewBase *view, m_viewMap) {
              if (view && view->isActive() && view != currentView) {
                //If this view has the same widget as the current view,
                //then don't hide this view.
		if(currentView->widget() != view->widget())
		  view->hide();
		view->deactivate();
              }
          }
          emit currentViewChanged();
      }
  }

  void ViewController::showView(const QString &name) {
      ViewMap::iterator it = m_viewMap.find(name);
      if (it != m_viewMap.end()) {
          m_current = it;
          showCurrent();
      }
  }

  void ViewController::freezeView() {
      if (!m_viewMap.isEmpty() ) {
          m_current.value()->freeze();
      }
  }

  void ViewController::unfreezeView() {
      if (!m_viewMap.isEmpty() ) {
          m_current.value()->unfreeze();
      }
  }

  void ViewController::dump() {
    qDebug() << "ViewController::dump:"
        << " count=" << m_viewMap.count()
        << " current=" << m_current.value();
    foreach(ControllableViewBase *view, m_viewMap) {
      qDebug() << "  " << view;
    }
  }

  void ViewController::viewChanged() {
      emit currentViewChanged();
  }

  ControllableViewBase* ViewController::currentView() {
      if (!m_viewMap.isEmpty())
          return m_current.value();
      else
          return NULL;
  }

}

