/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the MPEActiveStarter class.
*
*/



#ifndef MPEACTIVESTARTER_H
#define MPEACTIVESTARTER_H

//INCLUDES

// CLASS DECLARATION

/**
*  Virtual interface for a protocol specific active starter of Phone Engine.
*
*  @lib PhoneEngineBase.lib
*  @since Series60_2.7
*/
NONSHARABLE_CLASS( MPEActiveStarter )
    {
    public:

        /**
        * Recalls starting of modules after leave occured in the middle of StepL sequence.
        * @since Series60_2.7
        */
        virtual void RecallSteps( ) = 0;

        /**
        * Calls a protocol specific step method, which starts modules of PhoneEngine.
        * @since Series60_2.7
        * @return EFalse when starting sequence completed. 
        */
        virtual TBool StepL( ) = 0;

    };

#endif // MPEACTIVESTARTER_H

//End of file
