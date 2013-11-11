/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store folder utilities.
*
*/



#ifndef __MSGSTOREFOLDERUTILS_H__
#define __MSGSTOREFOLDERUTILS_H__

//<cmail>
#include "MsgStoreMailBox.h"
#include "MsgStorePropertyKeys.h"
#include "MsgStoreTypes.h"
//</cmail>

/**
 * Container for the root folder ids. Use the TMsgStoreFolderType enum to index
 * into the array.
 */
struct TMsgStoreRootFolders
    {
    TMsgStoreId iFolders[ 1 + EMsgStoreOther ];
    };

/**
 * Collection of folder management methods.
 */
class MsgStoreFolderUtils
    {
    public:

        /**
         * Create folders for the types defined in the TMsgStoreFolderType enum and
         * return the corresponding TMsgStoreIds in aFolderInfo. If a folder of a
         * particular type already exists its id is returned instead.
         *
         * @param aMailBox [in], mail box object.
         * @param aRootFolders [out], ids of the root folders.
         */
        IMPORT_C static void InitializeRootFoldersL(
            CMsgStoreMailBox& aMailBox,
            TMsgStoreRootFolders& aRootFolders );
    
        /**
         * Retrieve the root folder ids for the specified mailbox.
         *
         * @param aMailBox [in], mail box object.
         * @param aRootFolders [out], ids of the root folders.
         */
        IMPORT_C static void GetRootFoldersL(
            CMsgStoreMailBox& aMailBox,
            TMsgStoreRootFolders& aRootFolders );
    };

#endif
