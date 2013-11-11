/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Calendar widget datetime observer
*
*/

#ifndef DATETIMEOBSERVER_H
#define DATETIMEOBSERVER_H

#include <QObject>

/*!
  \class DateTimeObserver
  \brief Gets notifications about date changes from the Symbian fw and 
  forwards those to the widget.
*/

class DateTimeObserverPrivate;

class DateTimeObserver : public QObject
{
    Q_OBJECT

public:
    /*! 
        \fn DateTimeObserver::DateTimeObserver(QObject *parent = 0)
        
        Constructor of the DateTimeObserver class.
        @param parent is the parent class.
     */
    explicit DateTimeObserver(QObject *parent = 0);
    /*! 
        \fn DateTimeObserver::~DateTimeObserver()
        
        Destructor of the DateTimeObserver class.
     */
    virtual ~DateTimeObserver();

signals:
     /*! 
        \fn void DateTimeObserver::completed()
        
        emitted when a new notification is received from the private part.
     */
    void completed();

protected:
    /*! 
        \fn void DateTimeObserver::handleCompletion()
        
        called by the private part of DateTimeObserver when the date has changed.
     */
    void handleCompletion();

private:
    Q_DISABLE_COPY(DateTimeObserver)
    friend class DateTimeObserverPrivate;

    DateTimeObserverPrivate *d_ptr;
};

#endif // DATETIMEOBSERVER_H
