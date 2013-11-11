/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Object for a message item in history.
 *
 */

#ifndef __MSG_ITEM_H__
#define __MSG_ITEM_H__

#include "msghistorydefines.h"

#include <QDateTime>
#include <QString>

/**
 * Message Item in history.
 */
class MSG_HISTORY_EXPORT MsgItem 
    {
public:
    /**
     * Message direction
     */
    enum MsgDirection {
        MsgDirectionIncoming = 0,
        MsgDirectionOutgoing,
        ECsDirectionMissed,
        ECsDirectionUnknown
        };
    
    /**
     * Message type
     */
    enum MsgType {
        MsgTypeSMS = 0,
        MsgTypeMMS,
        MsgTypeMmsNotification,
        MsgTypeAudio,
        MsgTypeBioMsg,
        MsgTypeBlueTooth,
        MsgTypeRingingTone,
				MsgTypeProvisioning,
        MsgTypeUnknown
        };
    
    /**
     * Message state
     */
    enum MsgState {
        MsgStateUnknown = 0,
        MsgStateUponRequest,
        MsgStateWaiting,
        MsgStateSending,
        MsgStateScheduled,
        MsgStateResend,
        MsgStateSuspended,
        MsgStateFailed,
        MsgStateSent,
        MsgStateNotApplicable
        };
    
    /**
     * Message attribute
     */
    enum MsgAttribute {
        MsgAttributeNone            = 0x0000, // None
        MsgAttributeNew             = 0x0001, // New message
        MsgAttributeDraft           = 0x0002, // Draft message
        MsgAttributeAttachment      = 0x0004, // Attachment present
        MsgAttributeHighPriority    = 0x0008, // Priority high
        MsgAttributeLowPriority     = 0x0010, // Low high
        MsgAttributeUnread          = 0x0020, // Unread
        MsgAttributeSent            = 0x0040  // Sent message
        };

public: // member functions
    /*
     * Constructor
     */
    MsgItem();
    
    /*
     * Destructor
     */
    virtual ~MsgItem();
    
    /*
     * get messgae id
     * @return message id.
     */
    int id() const;
    
    /*
     * phoneNumber
     * @return phone number.
     */
    QString phoneNumber() const;
    
    /*
     * body
     * @return message body text.
     */
    QString body() const;
    
    /*
     * timeStamp
     *@return QDateTime
     */
    QDateTime timeStamp() const;
    
    /*
     * direction
     * @return MsgDirection
     */
    MsgDirection direction() const;
    
    /*
     * type
     * @return MsgType
     */
    MsgType type() const;
    
    /*
     * state
     * @return MsgState
     */
    MsgState state() const;
    
    /*
     * isAttributeSet
     * @return true, if set
     */
    bool isAttributeSet(MsgAttribute attribute) const;
    
    /*
     * setId
     * @param id, message id
     */
    void setId(int id);
    
    /*
     * setPhoneNumber
     * @param number, QString
     */
    void setPhoneNumber(const QString& number);
    
    /*
     * setBody
     * @param body, QString
     */
    void setBody(const QString& body);
    
    /*
     * setTimeStamp
     * @param sec, time in secs
     */
    void setTimeStamp(uint sec);
    
    /*
     * setDirection
     * @param direction, MsgDirection
     */
    void setDirection(MsgDirection direction);
    
    /*
     * setType
     * @param, MsgType
     */
    void setType(MsgType type);
    
    /*
     * setState
     * @param state ,MsgState
     */
    void setState(MsgState state);
    
    /*
     * changeAttributes
     * @param setAttributes, attribute to be set
     * @param clearAttributes, attribute to be reset
     */
    void changeAttributes
    (quint16 setAttributes, quint16 clearAttributes = MsgAttributeNone);
    
private: // data members
	
	  /*
     * message time stamp
     */
    QDateTime mTime;
    
    /*
     * phone number
     */
    QString mPhoneNumber;
    
    /*
     * message body
     */
    QString mBody;
    
    /*
     * Msg direction 
     */
    MsgDirection mDirection;
    
    /*
     * mesage type
     */
    MsgType mType;
    
    /*
     * message state
     */
    MsgState mState;
    
    /*
     * message attributes
     */
    MsgAttribute mAttributes;
    
    /*
     * message id
     */
    int mId;
    };

#endif // __MSG_ITEM_H__
