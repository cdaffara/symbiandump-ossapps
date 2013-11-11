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
* Description: Declaration for TMMCScBkupOwnerDataType
*     
*
*/

#ifndef __TMMCSCBKUPOWNERDATATYPE_H__
#define __TMMCSCBKUPOWNERDATATYPE_H__

// NB. This file is included inside an HRH file, so do not include
// C++ constructs.

/**
* DO NOT CHANGE THE ORDER OF THIS ENUMERATION
*
* @since 3.0
*/
enum TMMCScBkupOwnerDataType
    {
    // Relates to all data owners
    EMMCScBkupOwnerDataTypeDataOwner = 0,

    // Relates to java data for a particular owner
    EMMCScBkupOwnerDataTypeJavaData,

    // Relates to public data for a particular owner
    EMMCScBkupOwnerDataTypePublicData,

    // Relates to system data for a particular owner
    EMMCScBkupOwnerDataTypeSystemData,

    // Relates to active data for a particular owner
    EMMCScBkupOwnerDataTypeActiveData,

    // Relates to passive data for a particular owner
    EMMCScBkupOwnerDataTypePassiveData,

    // Always leave this last, don't assign it
    // a value. Don't use it either!
    EMMCScBkupOwnerDataTypeCount,

    // A generic 'any data type' value. Used by the drive-specific-request &
    // data sizer. Not a real SBE data type, hence it appears after
    // the last marker.
    EMMCScBkupOwnerDataTypeAny
    };



#endif // __TMMCSCBKUPOWNERDATATYPE_H__

//End of File
