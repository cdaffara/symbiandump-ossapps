/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for reporting application shutdown readiness.
*
*/


#ifndef M_MLCVTSHUTTEROBSERVER_H
#define M_MLCVTSHUTTEROBSERVER_H


/**
* Observer interface for reporting shutdown readiness.
*
* @since S60 v3.2
*/
class MLcVtShutterObserver
    {
public:

    /**
    * Called when application can exit.
    */
    virtual void HandleShutdownReady() = 0;
    };
    
#endif // M_MLCVTSHUTTEROBSERVER_H
