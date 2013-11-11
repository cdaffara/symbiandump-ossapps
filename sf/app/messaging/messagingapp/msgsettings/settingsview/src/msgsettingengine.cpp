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

#include "qstringlist.h"
#include "debugtraces.h"


#include "msgsettingengine.h"
#include "smssettingsprivate.h"
#include "mmssettingprivate.h"
#include <xqconversions.h>

//Constructor
MsgSettingEngine::MsgSettingEngine()
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgSettingEngine::MsgSettingEngine";
#endif


    QT_TRAP_THROWING(dptr_smsSettings = SmsSettingsPrivate::NewL());
    QT_TRAP_THROWING(dptr_mmsSettings = MmsSettingsPrivate::NewL());

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgSettingEngine::MsgSettingEngine";
#endif

}

//Destructor
MsgSettingEngine::~MsgSettingEngine()
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgSettingEngine::~MsgSettingEngine";
#endif


    delete dptr_smsSettings;
    delete dptr_mmsSettings;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgSettingEngine::~MsgSettingEngine";
#endif

}

/**
 * set the character encoding
 * @param encoding specifying encoding type
 */
void MsgSettingEngine::setCharacterEncoding(MsgSettingEngine::CharacterEncoding encoding)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setCharacterEncoding: " << encoding;
#endif


    TBool flag = EFalse;

    if (encoding == MsgSettingEngine::FullSupport)
    {
        flag = ETrue;
    }
    QT_TRAP_THROWING(dptr_smsSettings->setCharacterEncodingL(flag));
    
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setCharacterEncoding";
#endif

}

/**
 * returns settings delivery report status
 * and character encoding
 * @param encoding for char encoding
 */
void MsgSettingEngine::settingsCharEncoding( 
        MsgSettingEngine::CharacterEncoding& encoding )
{
    QDEBUG_WRITE("settingsServiceMessagesAndCharEncoding");
    
    TBool encoding1;
        
    QT_TRAP_THROWING(dptr_smsSettings->settingsCharEncodingL(encoding1));
    
    if (encoding1)
    {
        encoding = MsgSettingEngine::FullSupport;
    }
    else
        encoding = MsgSettingEngine::ReducedSupport;
    
    return;
}

/**
 * set the mms retrieval mode
 * @param retrieval specifying mode
 */
void MsgSettingEngine::setMMSRetrieval(MsgSettingEngine::MmsRetrieval retrieval)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setMMSRetrieval " << retrieval;
#endif


    QT_TRAP_THROWING(dptr_mmsSettings->setMMSRetrievalL(retrieval));

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setMMSRetrieval";
#endif

}

/**
 * set the anonymous message
 * @param status true or false
 */
void MsgSettingEngine::setAnonymousMessages(bool status)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setAnonymousMessages " << status;
#endif


    QT_TRAP_THROWING (dptr_mmsSettings->setAnonymousMessagesL(status));

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setAnonymousMessages";
#endif

}

/**
 * set for receiving MMS Adverts
 * @param status for true or false
 */
void MsgSettingEngine::setReceiveMMSAdverts(bool status)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setReceiveMMSAdverts " << status;
#endif


    QT_TRAP_THROWING(dptr_mmsSettings->setReceiveMMSAdvertsL(status));

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setReceiveMMSAdverts";
#endif

}

/**
 * for view 2 mms advance settings\
* get all the fields of mms advance setting
 * @param retrieval return for mms retrieval
 * @param anonymous return status true or false
 * @param mmsadverts return status true or false
 */
void MsgSettingEngine::advanceMmsSettings(MsgSettingEngine::MmsRetrieval& retrieval,
                                          bool& anonymousStatus,
                                          bool& mmsAdvertsStatus)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setReceiveMMSAdverts";
#endif


    TBool anonymous_Status;
    TBool mmsAdverts_Status;
    QT_TRAP_THROWING(dptr_mmsSettings->advanceMmsSettingsL(retrieval,
                                         anonymous_Status,
                                         mmsAdverts_Status));

    anonymousStatus = (bool) anonymous_Status;
    mmsAdvertsStatus = (bool) mmsAdverts_Status;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setReceiveMMSAdverts " << retrieval << " "
            << anonymousStatus << " " << mmsAdvertsStatus;
#endif

}

/**
 * get all the MMs access points
 * @param returns all the access point names
 * @attention to be displayed as per the index o to total count
 */
void MsgSettingEngine::allMMsAcessPoints(QStringList& nameList,
                                         int& defaultIndex)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::allMMsAcessPoints";
#endif


    RPointerArray<HBufC> accessPoints;
    //= new(ELeave)RPointerArray<HBufC>();

    QT_TRAP_THROWING( dptr_mmsSettings->getAllAccessPointsL(accessPoints, defaultIndex));

    for (int i = 0; i < accessPoints.Count(); i++)
        {
        HBufC* name = static_cast<HBufC *> (accessPoints[i]);
        QString qName = XQConversions::s60DescToQString(name->Des());
        nameList.append(qName);
#ifdef _DEBUG_TRACES_
        qDebug() << "\n " << qName;
#endif

        }
    accessPoints.ResetAndDestroy();

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit allMMsAcessPoints count = " << nameList.count()
            << " Default:" << defaultIndex;
#endif

}

/**
 * set the default access point
 * @param index specifying the index
 */
void MsgSettingEngine::setMMSAccesspoint(int index)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setMMSAccesspoint " << index;
#endif


    QT_TRAP_THROWING(dptr_mmsSettings->setMMSAccesspointL(index));

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setMMSAccesspoint ";
#endif

}

/**
 * get the list of all SMS Message Centres
 * @param list of all names QString
 */
void MsgSettingEngine::allSMSMessageCenter(QStringList& nameList,
                                           int& defaultIndex)
{

#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::allSMSMessageCenter";
#endif


    RPointerArray<HBufC> accessPoints;

    QT_TRAP_THROWING(dptr_smsSettings->getAllSMSMessageCenterL(accessPoints, defaultIndex));

    for (int i = 0; i < accessPoints.Count(); i++)
        {
        HBufC* name = accessPoints[i];
        QString qName = XQConversions::s60DescToQString(name->Des());
        nameList.append(qName);
#ifdef _DEBUG_TRACES_
        qDebug() << "\n qName";
#endif

        }
    accessPoints.ResetAndDestroy();

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit allSMSMessageCenter count = " << nameList.count()
            << " Default:" << defaultIndex;
#endif

}

/**
 * set the default SMS message centre
 * @param index specifying default index
 */
void MsgSettingEngine::setSMSMessageCenter(int index)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setSMSMessageCenter " << index;
#endif


    QT_TRAP_THROWING(dptr_smsSettings->setSMSMessageCenterL(index));

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setSMSMessageCenter ";
#endif

}

/**
 * for editing the SMS messafe Centre
 * @param name specifying center name
 * @param number specifying center number
 * @param index specying index of message list
 */
void MsgSettingEngine::editSmsMessageCenter(QString& centreName,
                                            QString& centreNumber, int index)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::editSmsMessageCenter " << index << " "
            << centreName << " " << centreNumber;
#endif

    HBufC* d_addr = XQConversions::qStringToS60Desc(centreNumber);
    HBufC* d_name = XQConversions::qStringToS60Desc(centreName);

    QT_TRAP_THROWING(dptr_smsSettings->editSMSServiceCentreL(d_addr, d_name, index));
    delete d_addr;
    delete d_name;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit editSmsMessageCenter ";
#endif

}

/**
 * for add new sms message centre
 * @param name specifying center name
 * @param number specifying center number
 */
void MsgSettingEngine::addSmsMessageCenter(QString& centreName,
                                           QString& centreNumber)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::addSmsMessageCenter " << centreName << " "
            << centreNumber;
#endif


    HBufC* d_addr = XQConversions::qStringToS60Desc(centreNumber);
    HBufC* d_name = XQConversions::qStringToS60Desc(centreName);

    QT_TRAP_THROWING(dptr_smsSettings->addSmsMessageCenterL(d_addr, d_name));
    delete d_addr;
    delete d_name;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit addSmsMessageCenter ";
#endif

}

/**
 * for delete sms message centre
 * @refer to header file
 */
void MsgSettingEngine::deleteSmsMessageCenter(int deleteIndex)
{
    QT_TRAP_THROWING( dptr_smsSettings->deleteSmsMessageCenterL(deleteIndex));
}

/**
 * get the sms message centre namd and number
 * @index for which center needs
 * @param return name specifying center name
 * @param return number specifying center number
 */
void MsgSettingEngine::smsCenterNameAndNumber(int index, QString& centreName,
                                              QString& centreNumber)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::smsCenterNameAndNumber " << index;
#endif


    HBufC* d_addr;
    HBufC* d_name;

    QT_TRAP_THROWING(  dptr_smsSettings->smsCenterNameAndNumberL(index, &d_addr, &d_name));

    centreNumber = XQConversions::s60DescToQString(d_addr->Des());
    centreName = XQConversions::s60DescToQString(d_name->Des());

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit smsCenterNameAndNumber " << centreNumber << " "
            << centreName;
#endif

}

//eof

