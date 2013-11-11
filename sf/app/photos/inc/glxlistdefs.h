/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   List definitions
*
*/



#ifndef __GLXLISTDEFS_H__
#define __GLXLISTDEFS_H__

#include <e32std.h>

/**
 * Namespace for list-related definitions
 */
namespace NGlxListDefs 
    {
    /// Focus change type
    enum TFocusChangeType
        {
        EUnknown,
        EForward,
        EBackward
        };

    /**
     * Indicates absolute or relative change to focus
     */
    enum TFocusSetType
        {
        EAbsolute,
        ERelative
        };

    enum TCountType
        {
        ECountAll,
        ECountNonStatic,
        ECountAllStatic,
        ECountPreStatic,
        ECountPostStatic
        };
    
    enum TInsertionPosition
        {
        EInsertFirst,
        EInsertLast
        };

    /// Indicates components to be populated in a path
    enum TPathType
        {
        EPathAllOrSelection,
        EPathFocusOrSelection,
        EPathParent
        };

    /// Indicates initial focus position when populated
    enum TFocusInitialPosition
        {
        EFocusFirst,
        EFocusLast
        };
    }

#endif // __GLXLISTDEFS_H__

