/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef XQSENSOR_H
#define XQSENSOR_H

// INCLUDES
#include <QObject>
#include "sensor_global.h"

// FORWARD DECLARATIONS
class XQSensorPrivate;

// CLASS DECLARATION
class XQSensor: public QObject
{
    Q_OBJECT

public:
    enum Error {
        NoError = 0,
        OutOfMemoryError,
        NotFoundError,
        UnknownError = -1
    };

public:
    virtual ~XQSensor();

public Q_SLOTS:
    void open();
    void close();
    void startReceiving();
    void stopReceiving();

public:
    XQSensor::Error error() const;

protected:
    XQSensor(XQSensorPrivate& dd, QObject* parent = 0);

protected:
    XQSensorPrivate* d;

};

#endif /*XQSENSOR_H*/

// End of file
