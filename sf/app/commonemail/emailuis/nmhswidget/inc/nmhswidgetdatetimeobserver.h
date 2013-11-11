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
* Description:
*
*/

#ifndef NMHSWIDGETDATETIMEOBSERVER_H
#define NMHSWIDGETDATETIMEOBSERVER_H

#include <QObject>

class NmHsWidgetDateTimeObserverPrivate;

class NmHsWidgetDateTimeObserver : public QObject
{
    Q_OBJECT

public:
    explicit NmHsWidgetDateTimeObserver(QObject *parent = 0);
    virtual ~NmHsWidgetDateTimeObserver();

signals:
    void dateTimeChanged();
    
protected:
    void handleCompletion();

private:
    Q_DISABLE_COPY(NmHsWidgetDateTimeObserver)
    friend class NmHsWidgetDateTimeObserverPrivate;

    NmHsWidgetDateTimeObserverPrivate *d_ptr;
};

#endif // NMHSWIDGETDATETIMEOBSERVER_H
