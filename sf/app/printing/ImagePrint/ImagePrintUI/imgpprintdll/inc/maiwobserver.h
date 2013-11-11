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


#ifndef MAIWOBSERVER_H
#define MAIWOBSERVER_H

#include <e32def.h>
#include <e32std.h>

/**
 *
 * Interface to notify AIW Provider about print progress
 *
 */
class MAiwObserver
    {
	public:		// Abstract methods

       /**
        *  Called when Print job is finished. CImgPrintUtils will be
		*  deleted after this call.
        */
        virtual void JobFinished() = 0;           
	         
    };

#endif //  MAIWOBSERVER_H

//  End of File
