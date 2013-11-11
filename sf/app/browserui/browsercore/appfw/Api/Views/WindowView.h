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

#ifndef __WINDOWVIEW_H__
#define __WINDOWVIEW_H__

#include <QWidget>
#include <QGraphicsWidget>
#include <QAction>
#include "controllableviewimpl.h"

class QWebHistory;
class WebPageController;

namespace WRT {

    class WrtBrowserContainer;
    class WindowViewPrivate;

    class BWF_EXPORT WindowView : public ControllableViewBase
    {
        Q_OBJECT
    public:
    	 enum State {
            WindowViewNotActive,
            WindowViewActive,
            WindowViewAddPage,
            WindowViewDeletePage,
            WindowViewInvalid
        };

        enum Mode {
            WindowViewModeNormal,
            WindowViewModeTransition
        };

        WindowView(WebPageController * mgr, QWidget* parent);
        WindowView(WebPageController * mgr, QGraphicsWidget* parent);
        ~WindowView();

        virtual void show() {
            widget()->show();
        }

        virtual void hide() {
            widget()->hide();
        }

        WebPageController* webPageController();

    public: // public actions available for this view
        QAction * getActionForward();
        QAction * getActionBack();
        QAction * getActionOK();
        QAction * getActionCancel();
        QAction * getActionAddWindow();
        QAction * getActionDelWindow();

        void setSize(QSize& size);

        void setMode(Mode mode);

    public: // Pure Virtual from ControllableView
        QString title() const;
        bool isActive();
        QList<QAction*> getContext();
        void displayModeChanged(QString& newMode);
        QGraphicsWidget* widget() const;

        int pageCount();
        int currentPageIndex();

    signals:
        void centerIndexChanged(int index);
        void ok(WrtBrowserContainer * newPage);
        void cancel();
        void animateComplete();
        void addPageComplete();
        void newWindowTransitionComplete();
        void pageAdded();

    public slots:
        void activate();
        void deactivate();
        void forward();
        void back();
        void updateWindows();
        void onPageCreated(WRT::WrtBrowserContainer*);

        void animate(WrtBrowserContainer *, WrtBrowserContainer*);

    private slots:
        void changedCenterIndex(int);
        void updateActions();    
        void okTriggered(int index);
        void okTriggeredCplt();
        void addPage(WrtBrowserContainer* pg= NULL);
        void addPageCplt(int);
        void addPageCplt();
        void delPage();
        void delPage(int);
        void delPageCplt(int);
        void endAnimation();
        void addNextPage();

    private:
        void init();

        void setCenterIndex();
        void setCenterIndex(int i);
        void setCenterIndex(WrtBrowserContainer *);
        void indexChangeInActiveState(int);
        void updateImages();

    protected:
        WindowViewPrivate * const d;
    };

    class BWF_EXPORT WindowViewJSObject : public ControllableViewJSObject {
        Q_OBJECT
      public:
        WindowViewJSObject(WindowView* view, QWebFrame* webFrame, const QString& objectName);

      Q_SIGNALS:
        void pageAdded();

      public Q_SLOTS:
        int pageCount(){ return windowView()->pageCount();}
        int currentPageIndex(){ return windowView()->currentPageIndex();}
    protected:
        WindowView * windowView() { return static_cast<WindowView *>(m_contentView); }
        WindowView * windowViewConst() const { return static_cast<WindowView *>(m_contentView); }

    
    };

}
#endif // __WINDOWVIEW_H__
