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
#ifndef XQACCELERATIONSENSOR_H
#define XQACCELERATIONSENSOR_H

// INCLUDES
#include "xqsensor.h"
#include "sensor_global.h"

// FORWARD DECLARATIONS
class XQAccelerationSensorPrivate;
class XQAbstractAccelerationSensorFilter;

// CLASS DECLARATION
class XQAccelerationSensor: public XQSensor
{
    Q_OBJECT

public:
    XQAccelerationSensor(QObject* parent = 0);
    virtual ~XQAccelerationSensor();
    void addFilter(XQAbstractAccelerationSensorFilter& filter);
    QList<XQAbstractAccelerationSensorFilter*>& filters();

    int xAcceleration() const;
    int yAcceleration() const;
    int zAcceleration() const;

private:
    friend class XQAccelerationSensorPrivate;
};


class XQAbstractAccelerationSensorFilter
{
public:
    virtual bool filter(int& xAcceleration, int& yAcceleration, int &zAcceleration) = 0;
};

class XQAccelerationDataPostFilter : public XQAbstractAccelerationSensorFilter
{
public:
    XQAccelerationDataPostFilter();

protected:  //from XQAbstractAccelerationSensorFilter
    bool filter(int& xAcceleration, int& yAcceleration, int& zAcceleration);
};

#endif /*XQACCELERATIONSENSOR_H*/

// End of file
