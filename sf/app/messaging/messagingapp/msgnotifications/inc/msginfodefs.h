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
 * Description: Interface between msgindicatorplugin and msgnotifier.
 *
 */


#ifndef MSGINFODEFS_H
#define MSGINFODEFS_H

#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QDataStream>


/**
 * Indicator types
 */
enum IndicatorTypes
{
    Unknown = 0, 
    NewIndicatorPlugin = 1, 
    FailedIndicatorPlugin = 2, 
    PendingIndicatorPlugin = 3
};


/**
 * return the indicator name
 * @return QString the name of the indicator
 */
inline QString indicatorName(IndicatorTypes type)
{
    switch(type){
   	case NewIndicatorPlugin:
          return QString("com.nokia.messaging.newindicatorplugin");
    case FailedIndicatorPlugin:
    	    return QString("com.nokia.messaging.failedindicatorplugin");
    case PendingIndicatorPlugin:
          return QString("com.nokia.messaging.pendingindicatorplugin");
    default:
    	    return QString();
    }
}

/**
 * Interface class used by indications between msgnotifier 
 * and msgindications.
 * @class MsgInfo
 * 
 */
class MsgInfo
{
public:
    MsgInfo() {
       mMsgCount = 0; 
       mIndicatorType = Unknown; 
       mConversationId = -1;
       mFromSingle = false;
       mDescription = QString();  
       mMessageType = 0;
    }
    virtual ~MsgInfo(){}
    
    /**
     * the count of messages
     */
    int mMsgCount;
    
    /**
     * The type of indicator
     */
    IndicatorTypes mIndicatorType;
    
    /**
     * The indicator type in int.
     */
    int mIndicatorTypeInt;
    
    /**
     * The conversation id.
     */
    int mConversationId;
    
    /**
     * The message type
     */
    int mMessageType;
    
    /**
     * Indication of number of senders/recipients
     * True if one, false if many
     */
    bool mFromSingle;
    
    /**
     * List of Names of the contacts, or phone numbers 
     */
    QStringList mDisplayName;
    
    /**
     * Any body text if present
     */
    QString mDescription;
    // TODO: check if the serialize/deserialize is okay
    /**
     * serialize
     * @param stream QDataStream serialize into this data stream.
     */
    inline void serialize(QDataStream& stream) const
       {
        stream << mMsgCount;
        stream << mIndicatorType;
        stream << mDisplayName;
        stream << mConversationId;
        stream << mFromSingle;
        stream << mDisplayName;
        stream << mDescription;
       }

    /**
     * deserialize
     * @param stream QDataStream deserialize from this data stream.
     */
    inline void deserialize(QDataStream& stream)
       {
        int indicatorType;
        stream >> mMsgCount;
        stream >> indicatorType;
        stream >> mDisplayName;
        stream >> mConversationId;
        stream >> mFromSingle;
        stream >> mDisplayName;
        stream >> mDescription;
        
        mIndicatorType = static_cast<IndicatorTypes>(indicatorType);
       }
};


Q_DECLARE_METATYPE(MsgInfo)

#endif 
