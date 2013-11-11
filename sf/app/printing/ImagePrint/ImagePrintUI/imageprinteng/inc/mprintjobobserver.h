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


#ifndef MPRINTJOBOBSERVER_H
#define MPRINTJOBOBSERVER_H

#include <e32def.h>

/**
 *
 * Interface used to notify UI about the print progress
 *
 */
class MPrintJobObserver
    {
	public:		// Abstract methods

        /**
         *  Notificiation of the printed page
         *
         *  @param aPercentageComplete  The percentage completed
         */
        virtual void PrintProgress( TUint aPercentageComplete ) = 0;

        /**
         *  Printing finished
         */
        virtual void JobFinished() = 0;

        /**
         *  Error during printing - the current job is cancelled
         *
         *  @param aErrorCode   The error code
         *  @param aErrorStringCode   The more detailed error string
         */
        virtual void JobError( TInt aErrorCode, TInt aErrorStringCode ) = 0;

        /**
         *  Minor error during printing, print process is NOT cancelled
         *
         *  @param aErrorCode   The error code
		 *  @param aErrorStringCode   The more detailed error string         
         */
        virtual void JobStatusEvent( TInt aErrorCode, TInt aErrorStringCode ) = 0;
    };

#endif // MPRINTJOBOBSERVER_H

//  End of File
