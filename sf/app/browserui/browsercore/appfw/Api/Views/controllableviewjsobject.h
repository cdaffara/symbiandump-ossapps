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

#ifndef __CONTOROLLABLEVIEWJSOBJECT_H__
#define __CONTOROLLABLEVIEWJSOBJECT_H__

#include "brtglobal.h"
#include <QWebFrame>
#include <QPointer>

class ControllableView;

class WRT_BROWSER_EXPORT ControllableViewJSObject : public QObject
{
    Q_OBJECT
  public:
    ControllableViewJSObject(ControllableView *contentView, QWebFrame *chromeFrame, const QString &objectName);
    virtual ~ControllableViewJSObject();

    QObject *getGeometry() const;
    Q_PROPERTY(QObject *geometry READ getGeometry)

    QObject *getPosition() const;
    Q_PROPERTY(QObject *position READ getPosition)

    QString getType() const;
    Q_PROPERTY(QString type READ getType)

    QString getTitle() const;
    Q_PROPERTY(QString title READ getTitle)

    void activate();
    void deactivate();

    QWebFrame *chromeFrame() { return m_chromeFrame; }

    // Injects this object into the javascript engine attached to 'frame'.
    void setChromeFrame(QWebFrame* frame);

  public slots:
    // It would be nice to have these as part of the Q_PROPERTY definition for geometry and position but
    // if we want to pass ScriptRects we need access to the chrome's QScriptEngine which we don't have since
    // the engine is deep inside QWebView.
    void setGeometry(int x, int y, int w, int h);
    void setPosition(int x, int y);

    void show();
    void hide();

  signals:
    /*!
      \brief Sent when the view has finished its task, ex: the user clicks OK or Cancel.
     */
    void done();

  private slots:
    void updateActions();

  protected:
    ControllableView *m_contentView;
    QPointer<QWebFrame> m_chromeFrame;

    // Parent of all actions and child of this object.  Allows access to the actions from javascript.
    // Example: "window.webView.actions.back.trigger()" will trigger the Back action on the web view.
    QObject *m_actionsParent;

    friend class ControllableView;
};

#endif // __CONTOROLLABLEVIEWJSOBJECT_H__

