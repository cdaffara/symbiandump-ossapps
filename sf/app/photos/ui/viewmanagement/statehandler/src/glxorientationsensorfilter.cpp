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
* Description:   ?Description
*
*/

#include "glxorientationsensorfilter.h"

GlxOrientationSensorFilter::GlxOrientationSensorFilter()
{
    qDebug("GlxOrientationSensorFilter::GlxOrientationSensor() enter" );
    m_orientation = new QOrientationSensor();
    m_orientation->addFilter( this );
    m_orientation->start();
    mOrient = QOrientationReading::Undefined ;
    qDebug("GlxOrientationSensorFilter::GlxOrientationSensor() exit" );
}

GlxOrientationSensorFilter::~GlxOrientationSensorFilter()
{
    m_orientation->stop();
    delete m_orientation;
}

bool GlxOrientationSensorFilter::filter( QOrientationReading *reading )
{
    qDebug("GlxOrientationSensorFilter::GlxOrientationSensor() filter " );
    mOrient = reading->orientation() ;
    qDebug("GlxOrientationSensorFilter::filter() orientation = %d", mOrient );
    return false;
}
