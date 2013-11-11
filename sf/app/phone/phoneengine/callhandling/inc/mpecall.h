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
* Description:  This file contains the header file of the MPESingleCall class.
*
*/



#ifndef MPECALL_H
#define MPECALL_H

//INCLUDES

// CLASS DECLARATION

/**
*  Abstract base class for call.
*
*  @lib CallHandling.lib
*/
class MPECall
    {
    public:

        /**
        * Returns call status information
        * @return the status of the current call
        */    
        virtual TPEState GetCallState() const = 0;
        
        /**
        * Returns call id number
        * @return the call id number
        */    
        virtual TInt GetCallId() const = 0;
        
    };

#endif // MPESINGLECALL_H

//End of file
