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
* Description:  Callback interface for observer of CAccSrvResetInactivityTimeTimer object.
*
*/




#ifndef M_SCHEDULERSTARTANDSTOPTIMEROBSERVER_H
#define M_SCHEDULERSTARTANDSTOPTIMEROBSERVER_H


#include <e32def.h>


/**
 *  Callback interface for observer of CSchedulerStopAndStartTimer object. 
 *
 *  This callback interface has to be implemented by observer class.
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 10.1
 */
NONSHARABLE_CLASS( MSchedulerStartAndStopTimerObserver )
    {

public:
 
    /**
     * Function is called by timer watcher every time when timer elapses.
     *
     * @since S60 10.1
     * @param none. 
     * @return none
     */
    virtual void Timeout( TUint aTimerId ) = 0;

    virtual void CreateMUT() = 0;
    virtual void DeleteMUT() = 0;

    };


#endif // M_SCHEDULERSTARTANDSTOPTIMEROBSERVER_H
