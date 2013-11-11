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

#ifndef GLXPERFORMANCEMACRO_H
#define GLXPERFORMANCEMACRO_H

#include<glxperformancelog.h>

#ifdef GLXPERFORMANCE_LOG

/*
 * create the object of performance log.
 */
    #define CREATE_PERLOG(X) 	GlxPerformanceLog _##X;	

/*
 * measure the time required for execute the group of statements
 * example
 *     START_PERLOG( t ) //t object should be created using CREATE_PERLOG (t)
 *          group of statements
 *     STOP_PERLOG ( t )
 */
    #define START_PERLOG(X)			_##X.start();
    #define STOP_PERLOG(X)			_##X.stop();

/*
 * reset the performance log informanation of object X.
 */
    #define END_PERLOG(X)			_##X.end();

/*
 * update the performance log information into flash.
 */
	#define WRITE_PERLOG(X,Y)		_##X.appendPerformanceLog(#Y);

/*
 * update the log info message of object X
 */
	#define UPDATE_PERLOGINFO(X,Y)	_##X.setInfoMsg(#Y);

/*
 * single macro to calculate the performance of group of statement, no need to create the object
 * example
 * PERFORMANCE( t, execution time ) {
 * statements
 * }
 */
    #define PERFORMANCE(X,Y) \
    GlxPerformanceLog _##X ;\
    _##X.start() ;\
    for ( int _i##X = 1 ; _i##X > 0 ; _##X.done(#Y), --_i##X )

/*
 * single macro to calculate the performance of group of statement, no need to create the object. 
 * It will take a parameter as string
 * example
 * PERFORMANCE_ADV( t, "execution time" ) {
 * statements
 * }
 */
    #define PERFORMANCE_ADV(X,Y) \
    GlxPerformanceLog _##X ;\
    _##X.start() ;\
    for ( int _i##X = 1 ; _i##X > 0 ; _##X.done(Y), --_i##X )


/*
 * single macro to calculate the avg performanc of group of statement, it is mandatory to create the object before using it
 * example
 * PERFORMANCE( t ) {
 * group of statements
 * }
 */
    #define AVG_PERFORMANCE(X) \
    _##X.start();\
    for ( int _i##X = 1; _i##X > 0; _##X.stopMonitor(), --_i##X )

/*
 * single macro to create the object, set the info and start the performance measurments
 * example
 * START_PERFORMANCE( t,  ) 
 * group of statements
 * STOP_PERLOG ( t)
 */
    #define START_PERFORMANCE(X,Y) \
    GlxPerformanceLog _##X ;\
    _##X.setInfoMsg(#Y);\
    _##X.start() ;

/*
 * write the current Time Stamp on the log file
 */
    #define WRITE_TIMESTAMP(X) {\
    GlxPerformanceLog tmp ;\
    tmp.appendTimeStamp(X);\
    }

#else
	#define CREATE_PERLOG(X) 
	#define START_PERLOG(X)
	#define STOP_PERLOG(X)
	#define END_PERLOG(X)
	#define WRITE_PERLOG(X,Y)
	#define UPDATE_PERLOGINFO(X,Y)
    #define PERFORMANCE(X,Y)
    #define PERFORMANCE_ADV(X,Y)
    #define AVG_PERFORMANCE(X)
    #define START_PERFORMANCE(X,Y)
    #define WRITE_TIMESTAMP(X)
#endif



#endif /* GLXPERFORMANCEMACRO_H_ */
