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

#include <msgitem.h>

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
MsgItem::MsgItem()
    {
    mAttributes = MsgAttributeNone;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
MsgItem::~MsgItem()
    {
    }

// ---------------------------------------------------------------------------
// MsgItem::id
//@ see header
// ---------------------------------------------------------------------------
int MsgItem::id() const
    {
    return mId;
    }

// ---------------------------------------------------------------------------
// MsgItem::phoneNumber
//@ see header
// ---------------------------------------------------------------------------
QString MsgItem::phoneNumber() const
    {
    return mPhoneNumber;
    }


// ---------------------------------------------------------------------------
// MsgItem::body
//@ see header
// ---------------------------------------------------------------------------
QString MsgItem::body() const 
    {
    return mBody;
    }

// ---------------------------------------------------------------------------
// MsgItem::timeStamp
//@ see header
// ---------------------------------------------------------------------------
QDateTime MsgItem::timeStamp() const
    {
    return mTime;
    }

// ---------------------------------------------------------------------------
// MsgItem::type
//@ see header
// ---------------------------------------------------------------------------
MsgItem::MsgType MsgItem::type() const
    {
    return mType;
    }

// ---------------------------------------------------------------------------
// MsgItem::direction
//@ see header
// ---------------------------------------------------------------------------
MsgItem::MsgDirection MsgItem::direction() const
    {
    return mDirection;
    }

// ---------------------------------------------------------------------------
// MsgItem::state
//@ see header
// ---------------------------------------------------------------------------
MsgItem::MsgState MsgItem::state() const
    {
    return mState;
    }

// ---------------------------------------------------------------------------
// MsgItem::isAttributeSet
//@ see header
// ---------------------------------------------------------------------------
bool MsgItem::isAttributeSet
(MsgItem::MsgAttribute attribute) const
    {
    return (mAttributes & attribute );
    }

// ---------------------------------------------------------------------------
// MsgItem::setId
//@ see header
// ---------------------------------------------------------------------------
void MsgItem::setId(int id)
    {
    mId = id;
    }

// ---------------------------------------------------------------------------
// MsgItem::setPhoneNumber
//@ see header
// ---------------------------------------------------------------------------
void MsgItem::setPhoneNumber(const QString& number)
    {
    mPhoneNumber = number;
    }

// ---------------------------------------------------------------------------
// MsgItem::setBody
//@ see header
// ---------------------------------------------------------------------------
void MsgItem::setBody(const QString& body)
    {   
    mBody = body;
    }

// ---------------------------------------------------------------------------
// MsgItem::setTimeStamp
//@ see header
// ---------------------------------------------------------------------------
void MsgItem::setTimeStamp(uint sec)
    {
    mTime.setTime_t(sec);      
    }

// ---------------------------------------------------------------------------
// MsgItem::setDirection
//@ see header
// ---------------------------------------------------------------------------
void MsgItem::setDirection(MsgItem::MsgDirection direction)
    {
    mDirection = direction;
    }

// ---------------------------------------------------------------------------
// MsgItem::setType
//@ see header
// ---------------------------------------------------------------------------
void MsgItem::setType(MsgItem::MsgType type)
    {
    mType = type;
    }

// ---------------------------------------------------------------------------
// MsgItem::setState
//@ see header
// ---------------------------------------------------------------------------
void MsgItem::setState(MsgItem::MsgState state)
    {
    mState = state;
    }

// ---------------------------------------------------------------------------
// MsgItem::changeAttributes
//@ see header
// ---------------------------------------------------------------------------
void MsgItem::changeAttributes(quint16 setAttributes, 
        quint16 clearAttributes)
    {
    quint16 attribute = mAttributes;
    attribute |= setAttributes;
    attribute &= ~clearAttributes;
    mAttributes = (MsgItem::MsgAttribute)attribute;
    }
