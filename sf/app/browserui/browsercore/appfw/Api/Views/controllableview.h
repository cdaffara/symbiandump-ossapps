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


#ifndef __CONTROLLABLEVIEW_H__
#define __CONTROLLABLEVIEW_H__

#include <QAction>
#include <QObject>
#include "BWFGlobal.h"


class QGraphicsScene;
class QWebFrame;
class ControllableViewJSObject;

/*!
 * This is the base class from which views are derived
 */
class BWF_EXPORT ControllableView
{
public:
    //! Virtual destructor to make sure that all views accessed through
    // this interface are destroyed properly
    virtual ~ControllableView() {}

    //! Return the widget handle of this view
    virtual QGraphicsWidget* widget() const = 0;

    //! Return the title of this view for display
    virtual QString title() const = 0;

    //! Return if this view is active or not
    virtual bool isActive() = 0;

    //! activate the view's resources. Could be connected by client to view visibility
    virtual void activate() = 0;
    
    //! deactivate the view's resources. Could be connected by client to view visibility
    virtual void deactivate() = 0;

    /*! 
      Return the list of public QActions most relevant to the view's current context
      (most approptiate for contextual menus, etc.
    */
    virtual QList<QAction*> getContext() = 0;

    virtual void connectAll() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual QString type() const = 0;
    virtual void triggerAction(const QString & action) = 0;
    virtual QAction* getAction(const QString & action) = 0;

    virtual void initGraphics(QGraphicsScene *scene, qreal zValue) = 0;

    /*!
      \brief "Freezes" the page display until unfreeze is called.

      Uses a ref counter to prevent unfreezing until all callers of freeze() also call
      unfreeze().
      \sa unfreeze()
    */
    virtual void freeze() = 0;

    /*!
      \brief "Unfreezes" the page display if the ref count is 0.
      \sa unfreeze()
    */
    virtual void unfreeze(bool force = false) = 0;

    virtual QWebFrame *chromeFrame() = 0;
    virtual void setChromeFrame(QWebFrame *frame) = 0;

    virtual QString objectName () const = 0;
    virtual void setObjectName ( const QString & name ) = 0;

    virtual ControllableViewJSObject* jsObject() const = 0;

    /*! connect objects titleChanged(const QString& title) signal
     * !param receiver connect receiver object 
     * !param method object slot to connect 
     */
    virtual void connectTitleChanged(const QObject* receiver, const char* method) = 0;

    /*! connect objects ContextChanged() signal
     * !param receiver connect receiver object 
     * !param method object slot to connect 
     */
    virtual void connectContextChanged(const QObject* receiver, const char* method) = 0;

    /*! connect objects freeze() slot
     * !param sender connect sender object
     * !param signal object signal to connect
     */
    virtual void connectFreeze(const QObject* sender, const char* signal) = 0;

    /*! connect objects unfreeze(bool force) slot
     * !param sender connect sender object
     * !param signal object signal to connect
     */
    virtual void connectUnfreeze(const QObject* sender, const char* signal) = 0;

//    virtual bool signal_connect(const char *signal,const QObject *receiver, const char *member) = 0;
//    virtual bool slot_connect(const QObject *sender, const char *signal, const char *member) = 0;
    
    virtual void displayModeChanged(QString&) = 0;

};

/*
// connect ControllableView signal to other object slot
inline bool connect(ControllableView* sender, const char* signal,const QObject* receiver, const char* member)
{ return sender->signal_connect(signal, receiver, member); }

// connect ControllableView slot to other object signal
inline bool connect(const QObject* sender, const char* signal,ControllableView* receiver, const char* member)
{ return receiver->slot_connect(sender, signal, member); }
*/
#endif // __CONTROLLABLEVIEW_H__

