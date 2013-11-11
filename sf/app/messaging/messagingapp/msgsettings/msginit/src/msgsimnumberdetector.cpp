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
 *     A class which takes care of reading the possible new
 *     service centres from SIM and adds them to pda-side Sms Settings.
 *
 */

// INCLUDE FILES

#include "msgsimnumberdetector.h"
#include "debugtraces.h"

// ================= MEMBER FUNCTIONS =======================



MsgSimNumDetector::MsgSimNumDetector()
{
	QDEBUG_WRITE("MsgSimNumDetector::MsgSimNumDetector : Enter")
    d_ptr = q_check_ptr(new MsgSimNumDetectorPrivate());
	QDEBUG_WRITE("MsgSimNumDetector::MsgSimNumDetector : Exit")
}


MsgSimNumDetector::~MsgSimNumDetector()
	{
	QDEBUG_WRITE("MsgSimNumDetector::~MsgSimNumDetector : Enter")
	delete d_ptr;	
	QDEBUG_WRITE("~MsgSimNumDetector::MsgSimNumDetector : Exit")
	}


// ----------------------------------------------------------------------------
// MsgSimNumDetectorPrivate::MsgSimNumDetectorPrivate
// @see MsgSimOperationPrivate_p.h
// ----------------------------------------------------------------------------
MsgSimNumDetectorPrivate::MsgSimNumDetectorPrivate():
mSimOperation(NULL)
{
  QDEBUG_WRITE("MsgSimNumDetectorPrivate::MsgSimNumDetectorPrivate : Enter")
  
  initL();
  
  QDEBUG_WRITE("MsgSimNumDetectorPrivate::MsgSimNumDetectorPrivate : Exit")

}

// ----------------------------------------------------------------------------
// MsgSimNumDetectorPrivate::~MsgSimNumDetectorPrivate
// 
// ----------------------------------------------------------------------------
MsgSimNumDetectorPrivate::~MsgSimNumDetectorPrivate()
{
	QDEBUG_WRITE("MsgSimNumDetectorPrivate::~MsgSimNumDetectorPrivate : Enter")
	
	if(mSimOperation)
        {
        delete mSimOperation;
        mSimOperation = NULL;
        }
	
    QDEBUG_WRITE("MsgSimNumDetectorPrivate::~MsgSimNumDetectorPrivate : Exit")
}

// ----------------------------------------------------------------------------
// MsgSimNumDetectorPrivate::initL
// 
// ----------------------------------------------------------------------------
void MsgSimNumDetectorPrivate::initL()
    {
    QDEBUG_WRITE("MsgSimNumDetectorPrivate::initL : Enter")
    
    mSimOperation = CMsgSimOperation::NewL(*this);
    
    QDEBUG_WRITE("MsgSimNumDetectorPrivate::initL : Exit")
    }

// ----------------------------------------------------------------------------
// MsgSimNumDetectorPrivate::CompleteOperation()
// 
// ----------------------------------------------------------------------------
void MsgSimNumDetectorPrivate::CompleteOperation()
    {
    QDEBUG_WRITE("MsgSimNumDetectorPrivate::CompleteOperation : Enter")
    
    
    QDEBUG_WRITE("MsgSimNumDetectorPrivate::CompleteOperation : Exit")
    }
//  End of File  
