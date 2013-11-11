/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration for MMMCScBkupProgressObserver
*     
*
*/

#ifndef __MMMCSCBKUPPROGRESSOBSERVER_H__
#define __MMMCSCBKUPPROGRESSOBSERVER_H__

// System includes
#include <e32std.h>

// User includes


/**
*
*
* @since 3.0
*/
class MMMCScBkupProgressObserver
    {
    public: // From MMMCScBkupProgressObserver

        /**
        *
        */
        virtual void MMCScBkupHandleProgress( TInt aAmountCompleted ) = 0;


        /**
        *
        */
        virtual void MMCScBkupHandleProgressDomainUnderstood( TInt aTotalProgressAmount ) = 0;


        /**
        *
        */
        virtual TInt MMCScBkupHandleFreeSpace( TInt aPercent ) = 0;


        /**
        *
        */
        virtual void MMCScBkupStartBackuping( TBool aProceed ) = 0;
    };




#endif // __MMMCSCBKUPPROGRESSOBSERVER_H__

//End of File
