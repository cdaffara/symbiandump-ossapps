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
*     Command Design Pattern's Command Interface for sending Mediator Messages.
*
*/


#ifndef MPHONEMEDIATORMESSAGE_H
#define MPHONEMEDIATORMESSAGE_H

/**
*  Command Design Pattern's Command Interface for sending Mediator Messages.
*/
class MPhoneMediatorMessage
    {
    public:
        
        /**
        * Destructor.
        */
        virtual ~MPhoneMediatorMessage() {};

        /**
        * Executes the Command.
        */
        virtual void ExecuteL() = 0;
        
    };
    
#endif // MPHONEMEDIATORMESSAGE_H