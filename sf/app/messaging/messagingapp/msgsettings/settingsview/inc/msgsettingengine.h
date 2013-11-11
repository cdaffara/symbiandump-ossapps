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
 * Description:This class provides API m/w for msg settings plugin
 *
 */

#ifndef MSGSETTINGENGINE_H_
#define MSGSETTINGENGINE_H_

#include <qobject.h>

//FORWARD DECLARATION
class SmsSettingsPrivate;
class MmsSettingsPrivate;

/**
 * This class defines interfaces wrapper b/w MsgSettings UI
 * and m/w. 
 */
class MsgSettingEngine : public QObject
{
Q_OBJECT

public:
    /*
     * Enum defining Character Encoding
     */
    enum CharacterEncoding
    {
        ReducedSupport, FullSupport
    };

    /**
     * Enum defining MmsRetrieval mode
     */
    enum MmsRetrieval
    {
        AlwaysAutomatic, AutomInHomeNetwork, Manual, Off, No
    };
    
public:

    /**
     * Constructor
     */
    MsgSettingEngine();

    /**
     * Destructor
     */
    ~MsgSettingEngine();

    /**
     * set the character encoding
     * @param encoding specifying encoding type
     */
    void setCharacterEncoding(MsgSettingEngine::CharacterEncoding encoding);

    /**
     * returns settings delivery report status
     * and character encoding
     * @param encoding for char encoding
     */
    void settingsCharEncoding( 
            MsgSettingEngine::CharacterEncoding& encoding);
    
    /**
     * set the mms retrieval mode
     * @param retrieval specifying mode
     */
    void setMMSRetrieval(MsgSettingEngine::MmsRetrieval);

    /**
     * set the anonymous message
     * @param status true or false
     */
    void setAnonymousMessages(bool status);

    /**
     * set for receiving MMS Adverts
     * @param status for true or false
     */
    void setReceiveMMSAdverts(bool status);

    /**
     * for view 2 mms advance settings\
     * get all the fields of mms advance setting
     * @param retrieval return for mms retrieval
     * @param anonymous return status true or false
     * @param mmsadverts return status true or false
     */
    void advanceMmsSettings(MsgSettingEngine::MmsRetrieval& retrieval,
                            bool& anonymousStatus, bool& mmsAdvertsStatus);

    /**
     * get all the MMs access points
     * @param returns all the access point names
     * @attention to be displayed as per the index o to total count
     * @param defaultIndex returns default selected
     */
    void allMMsAcessPoints(QStringList& nameList, int& defaultIndex);

    /**
     * set the default access point
     * @param index specifying the index
     */
    void setMMSAccesspoint(int index);

    /**
     * get the list of all SMS Message Centres
     * @param list of all names QString
     * @param defaultIndex returns default selected
     */
    void allSMSMessageCenter(QStringList& nameList, int& defaultIndex);

    /**
     * set the default SMS message centre
     * @param index specifying default index
     */
    void setSMSMessageCenter(int index);

    /**
     * for editing the SMS messafe Centre
     * @param name specifying center name
     * @param number specifying center number
     * @param index specying index of message list
     */
    void editSmsMessageCenter(QString& centreName, QString& centreNumber,
                              int index);

    /**
     * for add new sms message centre
     * @param name specifying center name
     * @param number specifying center number
     */
    void addSmsMessageCenter(QString& centreName, QString& centreNumber);

    /**
     * for delete new sms message centre
     * @param deleteIndex specifying index to delete
     */
    void deleteSmsMessageCenter(int deleteIndex);
    
    /**
     * get the sms message centre namd and number
     * @index for which center needs
     * @param return name specifying center name
     * @param return number specifying center number
     */
    void smsCenterNameAndNumber(int index, QString& centreName,
                                QString& centreNumber);    
    
private:
    SmsSettingsPrivate* dptr_smsSettings;
    MmsSettingsPrivate* dptr_mmsSettings;
};

#endif /* MSGSETTINGENGINE_H_ */
