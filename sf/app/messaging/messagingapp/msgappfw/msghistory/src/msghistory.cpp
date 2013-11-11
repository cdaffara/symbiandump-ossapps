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

#include <msghistory.h>
#include "msghistoryimpl.h"

// ---------------------------------------------------------------------------
// Default Constructor.
// ---------------------------------------------------------------------------
MsgHistory::MsgHistory(QObject* parent)
:QObject(parent)
    {
    d_ptr = new MsgHistoryImpl(this);
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
MsgHistory::~MsgHistory()
    {
    //do nothing
    }

//---------------------------------------------------------------
// MsgHistory::getMessages
// @see header file
//---------------------------------------------------------------
bool MsgHistory::getMessages(int contactId)
    {
    return (d_ptr->getMessages(contactId));
    }

//---------------------------------------------------------------
// MsgHistory::clearMessages
// @see header file
//---------------------------------------------------------------
bool MsgHistory::clearMessages(int contactId)
    {
    return (d_ptr->clearMessages(contactId));
    }

//---------------------------------------------------------------
// MsgHistory::markRead
// @see header file
//---------------------------------------------------------------
bool MsgHistory::markRead(int contactId)
    {    
    return (d_ptr->markRead(contactId));
    }

//---------------------------------------------------------------
// MsgHistory::subscribe
// @see header file
//---------------------------------------------------------------
bool MsgHistory::subscribe(int contactId)
    {    
    return (d_ptr->subscribe(contactId));
    }

//---------------------------------------------------------------
// MsgHistory::unSubscribe
// @see header file
//---------------------------------------------------------------
bool MsgHistory::unSubscribe(int contactId)
    {   
    return (d_ptr->unSubscribe(contactId));
    }    
