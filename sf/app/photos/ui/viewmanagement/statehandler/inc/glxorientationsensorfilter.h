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

#ifndef GLXORIENTATIONSENSORFILTER_H
#define GLXORIENTATIONSENSORFILTER_H

#include <QOrientationSensor>
#include <QOrientationFilter>
QTM_USE_NAMESPACE

/**
 * Class Description
 * To read the current orientaion from the sensor
 */
class GlxOrientationSensorFilter : public QOrientationFilter
{
public :
    /**
     * Constructor
     */	
     GlxOrientationSensorFilter();

     /**
      * Destructor
      */
     ~GlxOrientationSensorFilter();
     
     /**
      * filter() - callback function to get the current Orientation information
      * @param QOrientationReading
      * @return please look /// @see QOrientationFilter
      */
     bool filter( QOrientationReading *reading );
     
     /**
      * orientation() - return the current device orientation
      */
     int orientation() { return mOrient; }
	
private :
    int mOrient;
    QOrientationSensor *m_orientation ;
};

/// for detail please @see QOrientationFilter

#endif /* GLXORIENTATIONSENSORFILTER_H */