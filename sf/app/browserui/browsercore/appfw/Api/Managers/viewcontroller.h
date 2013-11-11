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

#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QObject>
#include <QMap>

#include "BWFGlobal.h"
#include "factory.h"

class ControllableView;
class QGraphicsScene;
class QWebFrame;

class BWF_EXPORT ViewController : public QObject, public Factory<ControllableView, QWidget, QString> {
    Q_OBJECT
  public:
    ViewController(QWidget *parentWidget,QGraphicsScene *scene = 0)
      : m_scene(scene),
        m_currentViewIndex(-1),
        m_viewParentWidget(parentWidget)
    {
        setObjectName("viewManager");
    }
    
    ~ViewController();

    ControllableView *createView(const QString &name, QWidget *parent = 0);
    ControllableView *getView(const QString &type) const;
    void addView(ControllableView *view,bool destroyOld=true);
    void removeView(ControllableView *view,bool destroyView=true);
    void setCurrent(ControllableView *view);
    ControllableView *currentView() const;
    void setChromeFrame(QWebFrame *frame);

    // temporary cleanup function without deleting of views for bedrock compatibility
    void cleanUp() { m_views.clear(); m_currentViewIndex = -1; }

    QObject *getCurrentView() const;
    Q_PROPERTY(QObject *currentView READ getCurrentView)
    
  public slots:
    void showContent(const QString &type);
    void freezeView();
    void unfreezeView();

  signals:
    void currentViewChanging();
    void currentViewChanged();

  protected:
    void showCurrentView();
    void hideCurrentView();

  protected:
    QList<ControllableView*> m_views;
    QGraphicsScene *m_scene;
    int m_currentViewIndex;
    QWidget* m_viewParentWidget;
};

#endif // VIEWCONTROLLER_H
