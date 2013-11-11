/*
 * Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
 *     An active object class which takes care of reading the possible new
 *     service centres from SIM and adds them to Sms Settings.
 *
 */

#ifndef __MSGSIMNUMBERDETECTOR_H__
#define __MSGSIMNUMBERDETECTOR_H__

#include <qobject.h>
#include "simscnumberdetector.h"


#ifdef  MSGINIT_DLL
#define MSGINIT_DLL_EXPORT Q_DECL_EXPORT
#else
#define MSGINIT_DLL_EXPORT Q_DECL_IMPORT
#endif

class MsgSimNumDetectorPrivate;

/**
 *  MsgSimOperation
 *  
 */
class MSGINIT_DLL_EXPORT MsgSimNumDetector :public QObject
{
public:
    
    /**
     * Constructor.
     */
    MsgSimNumDetector();

    /**
     * Destructor
     */
    ~MsgSimNumDetector();

private:
   
    MsgSimNumDetectorPrivate* d_ptr;

};


class MsgSimNumDetectorPrivate : public CBase, public MSimOperationObserver
    {

public:
    /**
     * Constructor
     */
    MsgSimNumDetectorPrivate();

    /**
     * Destructor
     */
    ~MsgSimNumDetectorPrivate();
    
public:
    
    void CompleteOperation();


private:
    /**
     * Does all the initialisations. 
     */
    void initL();


private:

    /**
     * Conversation server client 
     */
    CMsgSimOperation* mSimOperation;

    };
    
#endif  // not defined __MSGSIMNUMBERDETECTOR_H__
// End of file
