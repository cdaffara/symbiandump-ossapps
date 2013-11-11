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
 * Description: Private implementaion of sms data model plugin.
 *
 */

#ifndef UNISMSDATAPLUGINPRIVATE_H_
#define UNISMSDATAPLUGINPRIVATE_H_





//Forward Declarations
class CClientMtmRegistry;
class ConvergedMessage;
class UniSMSDataPlugin;
class CSmsClientMtm;
class CMsvSession;

/**
 * Symbian specific implementation of the uni SMS plugin
 */
class UniSMSDataPluginPrivate :public CBase, public MMsvSessionObserver
{

public:
    /**
     * Constructor
     */
    UniSMSDataPluginPrivate(UniSMSDataPlugin* plugin);

    /**
     * Destructor
     */
    ~UniSMSDataPluginPrivate();

    /**
     *  Resets the datamodel
     */
    void reset();

    /**
     * sets the message id for which details needs to be fetched
     * @param message id
     */
    int setMessageId(int mId);

    /**
     * Get the body of the message
     * @param aBodyText, for message body
     */
    void body(QString& aBodyText);

    int messageSize();

    /**
     * List of the message TO recipients
     * @param list of the recipients
     */
    void toRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * Address from which the message was received
     * @param messageAddress
     */
    void fromAddress(QString& messageAddress);

    /**
     * Time stamp of the message
     * @return time stamp of the created/received message
     */
    int timeStamp();

	/**
	 * Session with the Messaging server
	 * @return CMsvSession*
	 */
    CMsvSession* session();
    
private:
    /**
     *  Extracts the name and address from the CMsvRecipientList
     */
    void extractNameAndAddress(const TDesC& aMsvAddress, TPtrC& aName,
                               TPtrC& aAddress);
    void initL();
    
    void setMessageIdL(int mId);
    
    /**
     * @see MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
           TAny* aArg3);

private:
    CMsvSession* iMSession;
    CClientMtmRegistry* iMtmReg;
    CSmsClientMtm* iSmsClient;
    TMsvId iMessageId;
    UniSMSDataPlugin* q_ptr;
    TBool done;   
};
#endif
