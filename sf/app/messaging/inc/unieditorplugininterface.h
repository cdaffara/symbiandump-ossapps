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

#ifndef UNIEDITORPLUGININTERFACE_H_
#define UNIEDITORPLUGININTERFACE_H_

#include <QtPlugin>
#include <msvstd.h>
#include <gsmuelem.h>
#include "convergedmessage.h"


class UniEditorPluginInterface
{
public:
    
    /**
     * Enum defining EditorOperation 
     * @attention This enum can have values from 0 to 2 only.
     */
    enum EditorOperation
    {
        Forward = 0,
        Reply,
        ReplyAll, 
        Default 
    };

    /**
     * Destructor
     */
    virtual ~UniEditorPluginInterface()
    {
    }

    /**
	 * Type of the message (SMS/MMS etc..)
	 * @return message type
	 */
	virtual ConvergedMessage::MessageType messageType()=0;

    /**
     * Converts message from message store into ConvergedMessage 
     * based on the operation
     * @param TMsvId id
     * @param aOperation operation type
     * @return ConvergedMessage object is returned in successfull cases and 
     *         NULL is returned in failure cases. The ownership of the object
     *         is transferred to the caller. 
     */
    virtual ConvergedMessage* convertFrom( TMsvId aId,
        UniEditorPluginInterface::EditorOperation aOperation
        =UniEditorPluginInterface::Default)=0;

    /**
     * delete entry from drafts folder
     * @param aId TMsvId of the message entry to be deleted
     */
    virtual void deleteDraftsEntry( TMsvId aId )=0;

    /**
     * convert convergedmessage type to messaging f/w message entry
     * @param aMessage convergedmessage
     * @return message entry id in message store
     */
    virtual TMsvId convertTo( ConvergedMessage *aMessage ) = 0;

    /**
     * send message
     * @param aId message entry id in message store
     * @return true if client-mtm send is successful else returns false
     */
    virtual bool send(TMsvId aId) = 0;

    /**
     * check if service is valid
     * @return true/false
     */
    virtual TBool isServiceValid() = 0;

    /**
     * validate service settings
     * @param aEmailOverSms true if email over sms is supported, else false
     * @return true/false
     */
    virtual TBool validateService( TBool aEmailOverSms = EFalse ) = 0;
    
    /*
     * Turkish SMS(PREQ2265) specific...
     */
    virtual void setEncodingSettings(TBool aUnicodeMode,
        TSmsEncoding aAlternativeEncodingType, TInt charSupportType)=0;

    /**
     * for deciding on reduced or full charset support
     */
    virtual bool getNumPDUs(QString& aBuf, TInt& aNumOfRemainingChars,
        TInt& aNumOfPDUs, TBool& aUnicodeMode,
        TSmsEncoding& aAlternativeEncodingType)=0;

};

Q_DECLARE_INTERFACE(UniEditorPluginInterface,
        "org.nokia.messaging.UniEditorPluginInterface/1.0")


#endif //UNIEDITORPLUGININTERFACE_H_
