/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Command Design Pattern's Command Interface for sending relevant Phone
*     Engine message related to Video Telephony shutdown.
*
*/


#ifndef MPHONESHUTDOWNCOMMAND_H
#define MPHONESHUTDOWNCOMMAND_H

/**
*  Command Design Pattern's Command Interface for sending Phone Engine message
*  when Video Telephony shutdown operation is done.
*/
class MPhoneShutdownCommand
    {
    public:
        
        /**
        * Executes the Command.
        */
        virtual void ExecuteLD() = 0;
        
    };
    
#endif // MPHONESHUTDOWNCOMMAND_H
