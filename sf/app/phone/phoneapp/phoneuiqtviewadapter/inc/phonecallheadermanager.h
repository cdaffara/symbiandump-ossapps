/*!
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
* Description:  Phone UI's symbian adapter for Qt view.
*
*/

#ifndef PHONECALLHEADERMANAGER_H
#define PHONECALLHEADERMANAGER_H


#include <QObject>

class PhoneUIQtViewIF;
class PhoneBubbleWrapper;
class PhoneResourceAdapter;
class PhoneCallHeaderUtil;
class MPEEngineInfo;

class PhoneCallHeaderManager : 
    public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit PhoneCallHeaderManager(
            PhoneBubbleWrapper &m_bubbleWrapper, 
            PhoneUIQtViewIF &view,
            QObject *parent = 0);
    
    /**
     * Destructor
     */
    virtual ~PhoneCallHeaderManager();

    
public:

    void setEngineInfo(MPEEngineInfo* engineInfo);
    
    /*!
        \fn int PhoneUIQtViewAdapter::createCallHeader()
        
        This method creates new call header.
    */
    void createCallHeader(int callId);
    
    /*!
        \fn int PhoneUIQtViewAdapter::createCallHeader()
        
        This method creates new call header.
    */
    void createEmergencyCallHeader(int callId);
    
    /*!
        \fn int PhoneUIQtViewAdapter::removeCallHeader()
        
        This method removes a call header.
    */
    void removeCallHeader(int callId);

    /*!
        \fn int PhoneUIQtViewAdapter::updateCallHeader()
        
        This method updates call header.
    */
    void updateCallHeader(int callId);
    
    /*!
        \fn int PhoneUIQtViewAdapter::handleCipheringInfoChange()
        
        This method updates ciphering indicators.
    */
    void handleCipheringInfoChange(int callId);
    
    /*!
        \fn int PhoneUIQtViewAdapter::createConference()
        
        This method creates conference bubble.
    */
    void createConferenceBubble(int callId);

    /*!
        \fn int PhoneUIQtViewAdapter::removeConferenceBubble()
        
        This method removes conference bubble.
    */
    void removeConferenceBubble();

    /*!
        \fn int PhoneUIQtViewAdapter::removeCallFromConference()
        
        This method removes a call from conference.
    */
    void removeCallFromConference(int callId);
    
    /*!
        \fn void PhoneUIQtViewAdapter::setPrivateFromConference()
        
        This method removes call from converence bubble.
        Used in private command. 
    */   
    void setPrivateFromConference(int callId);
    
    /*!
        \fn void PhoneUIQtViewAdapter::setExpandedConferenceCallHeader()
        
        This method sets conference call header expanded if needed. 
    */
    void setExpandedConferenceCallHeader();
    
    /*!
        \fn void PhoneUIQtViewAdapter::removeAllCallHeaders()
        
        This method removes all call headers.
    */
    void removeAllCallHeaders();
    
    /*!
        \fn void PhoneUIQtViewAdapter::removeAllCallHeaders()
        
        This method checks is the call a voice call.
    */
    bool isVoiceCall(int callId);
    
private:
    
    void setCommonCallHeaderData(int callId, int bubble);
    
    void sendVideoCallData(int callId);
    
    PhoneCallHeaderUtil *callHeaderUtil();
    
private:

    PhoneBubbleWrapper &m_bubbleWrapper;
    PhoneUIQtViewIF &m_view;
    PhoneCallHeaderUtil *m_callHeaderUtil;
    PhoneResourceAdapter *m_resourceAdapter;
    MPEEngineInfo*  m_engineInfo;
};

#endif // PHONECALLHEADERMANAGER_H
