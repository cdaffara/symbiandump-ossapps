/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MPRINTJOBSTATUS_H
#define MPRINTJOBSTATUS_H

#include <e32std.h>

/**
 *
 *  Internal mixin-class for CPrintEventCatcher to notify the status
 *  of the print job
 *
 */
class MPrintJobStatus
    {    
    public:

        /** 
         *  Used to notify the status of the print job
         *  
         *  @param aStatus  Possible TInt codes can mean (active, done)
         *  @param aPercentCompletion  0%-100% of the print job completed
         *  @param aJobStateCode Enumeration code for the Job State String         
         */
        virtual void PrintJobProgress( TInt aStatus, TInt aPercentCompletion, 
            TInt aJobStateCode ) = 0;

        /** 
         *  Used to notify occured error during the job
         *
         *  @param aError   The error code
         *  @param aErrorStringCode   More detailed error string
         */
        virtual void PrintJobError( TInt aError, TInt aErrorStringCode ) = 0;

        /**
         *  Printer status is obtained through this interface, usually the
         *  suspend errors are notified using this method
         *
         *  @param aError   The error code
         *  @param aErrorStringCode Error string code         
         */
        virtual void PrinterStatus( TInt aError, TInt aErrorStringCode ) = 0;
    };

#endif  //  MPRINTJOBSTATUS_H

//  End of File
