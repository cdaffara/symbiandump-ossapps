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
* Description:  Interface for observing security setting.
*
*/



#ifndef MCSPSECURITYSETTINGOBSERVER_H_
#define MCSPSECURITYSETTINGOBSERVER_H_

/**
*  Virtual interface for monitoring call ownership.
*
*  @since S60 3.2
*/
class MCSPSecuritySettingObserver
    {
    public:  

        /* Security setting event value */  
        enum TCSPSecuritySettingEvent
            {
            ESecureCall,
            ENotSecureCall,    
            ESecureNotSpecified
            };

    public:  

        /**
        * SecuritySettingChanged.
        * @param aValue
        * @since S60 3.2
        */
        virtual void SecuritySettingChanged( TInt aValue ) = 0;

    };

#endif /*MCSPSECURITYSETTINGOBSERVER_H_*/
