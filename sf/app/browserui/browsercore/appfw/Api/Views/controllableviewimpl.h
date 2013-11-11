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


#ifndef __CONTROLLABLEVIEWIMPL_H__
#define __CONTROLLABLEVIEWIMPL_H__

#include <assert.h>
#include <QAction>
#include <QEvent>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "BWFGlobal.h"

#include "controllableview.h"
#include "controllableviewjsobject.h"

/*!
 * Controllable view basic implementation template
 */
template <
    class PARENT,
    class JSObject = ControllableViewJSObject
>
class BWF_EXPORT ControllableViewImpl : public PARENT, public ControllableView
{
public:
    ControllableViewImpl(QObject* parent = 0) : PARENT(parent),m_jsObject(0), m_frozenPixmap(0), m_frozenCount(0), m_isActive(0) { }
    virtual ~ControllableViewImpl()
        { delete m_jsObject; delete m_frozenPixmap; }

    //! Return if this view is active or not
    virtual bool isActive() { return m_isActive; }

    //! activate the view's resources. Could be connected by client to view visibility
    virtual void activate() { m_isActive = true; }
    
    //! deactivate the view's resources. Could be connected by client to view visibility
    virtual void deactivate() { m_isActive = false; }

    virtual void displayModeChanged(QString&) {}

    virtual void connectAll() {}

    virtual void show()
    { 
        assert(widget());
        widget()->installEventFilter(this);
        widget()->show(); 
    }

    virtual void hide() 
    { 
        assert(widget());
        widget()->hide(); 
        widget()->removeEventFilter(this);
    }
    
    virtual void triggerAction(const QString & action)
    {
        Q_UNUSED(action)
    }

    virtual QAction * getAction(const QString & action)
    {
      QList<QAction *> viewActions = getContext();
      foreach (QAction * viewAction, viewActions){
	if(viewAction->objectName() == action){
	  return viewAction;
	}
      }
      return 0;
    }

    virtual void initGraphics(QGraphicsScene *scene, qreal zValue)
    {
        if(!scene || !widget()) return;
        if(widget()->scene()) return; // already in scene

        scene->addItem(widget());
        widget()->setZValue(zValue);
    }

    virtual QWebFrame *chromeFrame()
    { 
        return m_jsObject ? m_jsObject->chromeFrame() : 0; 
    }

    virtual void setChromeFrame(QWebFrame *frame)
    { 
        if(m_jsObject)
            m_jsObject->setChromeFrame(frame);
    }

    ControllableViewJSObject* jsObject() const { return m_jsObject; }

    QString objectName () const { return PARENT::objectName(); }
    void setObjectName ( const QString& name ) { PARENT::setObjectName(name); }

    virtual QString title() const { return QString::null; }

    /*!
      \brief "Freezes" the page display until unfreeze is called.

      Uses a ref counter to prevent unfreezing until all callers of freeze() also call
      unfreeze().
      \sa unfreeze()
    */
    void freeze()
    {
        if(!widget()) return;
        if(m_frozenCount == 0) {
            // Take a snapshot.
            m_frozenPixmap = new QPixmap(widget()->size().toSize());
            QPainter painter(m_frozenPixmap);
            QStyleOptionGraphicsItem opt;
            widget()->paint(&painter,&opt);
            painter.end();
            widget()->update();
        }
        m_frozenCount++;
    }

    /*!
      \brief "Unfreezes" the page display if the ref count is 0.
      \sa unfreeze()
    */
    void unfreeze(bool force = false)
    {
        if(!widget() || m_frozenCount == 0) return;
        
        if(force)
            m_frozenCount = 0;
        else
            m_frozenCount--;
        
        if(m_frozenCount == 0){
            assert(m_frozenPixmap);
            delete m_frozenPixmap;
            m_frozenPixmap = 0;
            widget()->update();
        }
    }

   
protected:
    bool eventFilter(QObject *obj, QEvent *event)
    {
        return PARENT::eventFilter(obj, event);
    }

protected:
    JSObject* m_jsObject;
    QPixmap* m_frozenPixmap;
    uint m_frozenCount;
    bool m_isActive;
    
    friend class ViewController;
    friend class ContentViewJSObject;
};

typedef ControllableViewImpl<QObject> ControllableViewBaseParent;
class BWF_EXPORT ControllableViewBase : public ControllableViewBaseParent
{
    Q_OBJECT
public:
    ControllableViewBase(QObject* parent = 0) : ControllableViewBaseParent(parent) {}
/*
    bool signal_connect(const char *signal,const QObject *receiver, const char *member)
        { return connect(this, signal, receiver, member); }

    bool slot_connect(const QObject *sender, const char *signal, const char *member)
        { return connect(sender, signal, this, member); }
*/
    /*! connect objects titleChanged(const QString& title) signal
     * !param receiver connect receiver object 
     * !param method object slot to connect 
     */
    virtual void connectTitleChanged(const QObject* receiver, const char* method)
        { connect(this, SIGNAL(titleChanged(title)), receiver, method); }

    /*! connect objects ContextChanged() signal
     * !param receiver connect receiver object 
     * !param method object slot to connect 
     */
    virtual void connectContextChanged(const QObject* receiver, const char* method)
        { connect(this, SIGNAL(contextChanged()), receiver, method); }

    /*! connect objects freeze() slot
     * !param sender connect sender object
     * !param signal object signal to connect
     */
    void connectFreeze(const QObject* sender, const char* signal)
        { connect(sender, signal, this, SLOT(freeze())); }

    /*! connect objects unfreeze(bool force) slot
     * !param sender connect sender object
     * !param signal object signal to connect
     */
    void connectUnfreeze(const QObject* sender, const char* signal)
        { connect(sender, signal, this, SLOT(unfreeze())); }

public slots:
    void freeze() { ControllableViewBaseParent::freeze(); }
    void unfreeze(bool force = false) { ControllableViewBaseParent::unfreeze(force); }
signals:
    void titleChanged(const QString& title);
    void contextChanged();
    // Sent when the view's widget has been instantiated.
    void instantiated(ControllableViewBase *view);
};

namespace WRT { // TODO: remove when GoAnywhereView converted to QGraphicsWidget
    class GoAnywhereView; 
}

typedef ControllableViewBase ControllableViewQWidgetBaseParent;
class BWF_EXPORT ControllableViewQWidgetBase : public ControllableViewQWidgetBaseParent
{
public:
    ControllableViewQWidgetBase(QObject* parent = 0) : ControllableViewQWidgetBaseParent(parent), m_proxyWidget(0) {}
    virtual ~ControllableViewQWidgetBase() { delete m_proxyWidget; }
    
    virtual QGraphicsWidget* widget() const
    {
        assert(qWidget());
        if(!m_proxyWidget)
        {
            ControllableViewQWidgetBase* that = const_cast<ControllableViewQWidgetBase*>(this);
            that->m_proxyWidget = new QGraphicsProxyWidget();
            m_proxyWidget->setWidget(qWidget());
        }

        return m_proxyWidget;
    }

protected:
    virtual QWidget* qWidget() const = 0;
    QGraphicsProxyWidget* m_proxyWidget;
    friend class WRT::GoAnywhereView; // TODO: remove when GoAnywhereView converted to QGraphicsWidget
};

#endif // __CONTROLLABLEVIEWIMPL_H__

