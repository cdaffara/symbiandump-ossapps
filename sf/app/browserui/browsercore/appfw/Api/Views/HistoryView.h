/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __HISTORYVIEW_H__
#define __HISTORYVIEW_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>
#include <QString>
#include <QWebFrame>
#include "controllableviewimpl.h"

class QWebHistory;
class WebPageController;

namespace WRT {

    class WrtBrowserContainer;
    class FlowInterface;
    class HistoryViewPrivate;

    class BWF_EXPORT HistoryView : public ControllableViewBase
    {
        Q_OBJECT
    public:
        HistoryView(WebPageController * mgr, QWidget* parent);
        HistoryView(WebPageController * mgr, QGraphicsWidget* parent);
        ~HistoryView();
        WebPageController* webPageController();

    public: // public actions available for this view
        QAction * getActionForward();
        QAction * getActionBack();
        QAction * getActionOK();
        QAction * getActionCancel();
        int currentIndex();

    public: // Pure Virtual from ControllableView
        QString title() const;
        bool isActive();
        QList<QAction*> getContext();
        QRect centralRect();
        QGraphicsWidget* widget() const;

    signals:
        void centerIndexChanged(int index);
        void ok(int item);
        void cancel();
        void contextEvent(QObject *context);
        void activated();
        void deactivated();

    public slots:
        void activate();
        void deactivate();
        void setCenterIndex(int i);
        void forward();
        void back();
    
    private slots:
        void updateActions(int centerIndex);
        void updateHistory(bool status=true);

    private:
        bool eventFilter(QObject *obj, QEvent *event);
        bool handleWidgetContextMenuEvent(class QContextMenuEvent *event);
        QString itemTitle(int index) const;

    protected:
        HistoryViewPrivate * const d;
    };

    class BWF_EXPORT HistoryViewJSObject : public ControllableViewJSObject {
        Q_OBJECT
      public:
        HistoryViewJSObject(HistoryView* view, QWebFrame* webFrame, const QString& objectName);
        ~HistoryViewJSObject();

      int getCurrentHistoryIndex() const { return historyViewConst()->currentIndex(); }
      Q_PROPERTY(int currentHistIndex READ getCurrentHistoryIndex)

    public slots:
        void ok(int item);
        void cancel();
        void onContextEvent(QObject *context);
    
    signals:
       void done(int);
       void contextEvent(QObject *context);
       void activated();
       void deactivated();

    protected:
        HistoryView * historyView() { return static_cast<HistoryView *>(m_contentView); }
        HistoryView * historyViewConst() const { return static_cast<HistoryView *>(m_contentView); }
    };
}

#endif // __HISTORYVIEW_H__
