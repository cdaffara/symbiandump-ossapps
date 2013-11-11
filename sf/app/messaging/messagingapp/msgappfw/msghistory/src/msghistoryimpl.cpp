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

//SYSTEM INCLUDE
#include <msghistory.h>
//USER INCLUDE
#include "msghistoryimpl.h"
#include "msghistoryprivate.h"

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
MsgHistoryImpl::MsgHistoryImpl(QObject* parent)
:QObject(parent)
    {
    d_ptr = q_check_ptr (new MsgHistoryPrivate(this));
    
    //connect signals
    connect(this, SIGNAL(messagesReady(QList<MsgItem>&)),
            this->parent(), SIGNAL(messagesReady(QList<MsgItem>&)));
    connect(this, SIGNAL(messageAdded(MsgItem&)),
            this->parent(), SIGNAL(messageAdded(MsgItem&)));        
    connect(this, SIGNAL(messageChanged(MsgItem&)),
            this->parent(), SIGNAL(messageChanged(MsgItem&)));        
    connect(this, SIGNAL(messageDeleted(MsgItem&)),
            this->parent(), SIGNAL(messageDeleted(MsgItem&)));        
    
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
MsgHistoryImpl::~MsgHistoryImpl()
    {
    if ( d_ptr )
        delete d_ptr;
    }

//---------------------------------------------------------------
// MsgHistoryImpl::getMessages
// @see header file
//---------------------------------------------------------------
bool MsgHistoryImpl::getMessages(int contactId)
    {
    return (d_ptr->GetMessagingHistory(contactId));
    }

//---------------------------------------------------------------
// MsgHistoryImpl::clearMessages
// @see header file
//---------------------------------------------------------------
bool MsgHistoryImpl::clearMessages(int contactId)
    {
    return (d_ptr->ClearMessagingHistory(contactId));
    }

//---------------------------------------------------------------
// MsgHistoryImpl::markRead
// @see header file
//---------------------------------------------------------------
bool MsgHistoryImpl::markRead(int contactId)
    {    
    return (d_ptr->MarkReadMessagingHistory(contactId));
    }

//---------------------------------------------------------------
// MsgHistoryImpl::subscribe
// @see header file
//---------------------------------------------------------------
bool MsgHistoryImpl::subscribe(int contactId)
    {    
    return (d_ptr->Subscribe(contactId));
    }

//---------------------------------------------------------------
// MsgHistoryImpl::unSubscribe
// @see header file
//---------------------------------------------------------------
bool MsgHistoryImpl::unSubscribe(int contactId)
    {   
    return (d_ptr->UnSubscribe(contactId));
    }

//---------------------------------------------------------------
// MsgHistoryImpl::messagesReadyEvent
// @see header file
//---------------------------------------------------------------
void MsgHistoryImpl::messagesReadyEvent(QList<MsgItem>& msgs)
    {
    //emit messagesReady
    emit messagesReady(msgs);
    }

//---------------------------------------------------------------
// MsgHistoryImpl::messageAddedEvent
// @see header file
//---------------------------------------------------------------
void MsgHistoryImpl::messageAddedEvent(MsgItem& msg)
    {
    //emit messageAdded
    emit messageAdded(msg);
    }

//---------------------------------------------------------------
// MsgHistoryImpl::messageChangedEvent
// @see header file
//---------------------------------------------------------------
void MsgHistoryImpl::messageChangedEvent(MsgItem& msg)
    {
    //emit messageChanged
    emit messageChanged(msg);
    }

//---------------------------------------------------------------
// MsgHistoryImpl::messageDeletedEvent
// @see header file
//---------------------------------------------------------------
void MsgHistoryImpl::messageDeletedEvent(MsgItem& msg)
    {
    //emit messageDeleted
    emit messageDeleted(msg);
    }

//EOF
