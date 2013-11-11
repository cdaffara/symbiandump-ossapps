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


#ifndef MPRINTJOB_H
#define MPRINTJOB_H

#include <e32def.h>
#include <e32base.h>

class MPrintJobObserver;

/**
 *
 * Interface to manage the printing
 *
 */
class MPrintJob
    {
	public:		// Abstract methods

        /**
         *  Starts printing images with selected settings, template and printer.
         *  If this method does not leave, the printing has started. Further messages
         *  about printing progress are passed through MPrintJobObserver
         *  Asynchronous
         *
         *  @param aObserver Pointer to the observer class that handles the 
         */
        virtual void PrintL( MPrintJobObserver* aObserver ) = 0;

        /**
         *  Cancels the active print job. The cancellation is finished, when 
         *  MPrintJobObserver receives error event with error code KErrCancel
         *  Asynchronous
         */
        virtual void CancelL() = 0;
        
        /**
         *  Get all the images from current print job.
         *
         *  @param aImages An array, in which the printed images are added.
         */
        virtual void GetPrintJobL( RPointerArray<TDesC>& aImages ) = 0;
        
    };

#endif //  MPRINTJOB_H

//  End of File
