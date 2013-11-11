/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Allows a synchronous wait on a operation
 *
 */

#ifndef __MUIUOPERATIONWAIT_H__
#define __MUIUOPERATIONWAIT_H__

//  INCLUDES
#include "muiuutilsdefines.h"
#include <e32base.h>
#include <e32const.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

// CLASS DECLARATION
/**
*
* Deprecated! Use internal CMsgOperationWait instead!
*
* Allows a synchronous wait on a operation.
*/
class MUIU_UTILS_EXPORT CMuiuOperationWait: public CActive
    {
    public:
        static CMuiuOperationWait* NewLC( TInt aPriority = EPriorityStandard );
        ~CMuiuOperationWait();
        void Start();
    protected:
        CMuiuOperationWait( TInt aPriority );
        void RunL();
        void DoCancel();
    protected:
        CActiveSchedulerWait iWait;
    };

QT_END_NAMESPACE

QT_END_HEADER

#endif //__MUIUOPERATIONWAIT_H__

// End of file
