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


#ifndef MPRINTINGOBSERVER_H
#define MPRINTINGOBSERVER_H

#include <e32def.h>
#include <e32std.h>

/**
 *
 * Interface for observing printing status
 *
 */
class MPrintingObserver
    {
	public:		// Abstract methods

       /**
        * Receive info that printing process has been ended
        *
        * @param aPrintingCanceled ETrue if printing was ended by canceling
        *                          the process, otherwise EFalse
        */
        virtual void PrintingCompletedL( TBool aPrintingCanceled = EFalse )  = 0;
    };

#endif //  MPRINTINGOBSERVER_H

//  End of File
