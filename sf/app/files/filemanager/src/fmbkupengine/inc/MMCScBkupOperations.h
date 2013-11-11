/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration for TMMCScBkupOperationType
*     
*
*/

#ifndef __MMCSCBKUPOPERATIONS_H__
#define __MMCSCBKUPOPERATIONS_H__

// System includes
#include <e32base.h>


/**
*
*
* @since 3.0
*/
enum TMMCScBkupOperationType
    {
    EMMCScBkupOperationTypeIdle = 0,
    EMMCScBkupOperationTypeFullBackup,
    EMMCScBkupOperationTypeFullRestore,
    EMMCScBkupOperationTypePartialBackup,
    EMMCScBkupOperationTypePartialRestore
    };




#endif // __MMCSCBKUPOPERATIONS_H__

//End of File
