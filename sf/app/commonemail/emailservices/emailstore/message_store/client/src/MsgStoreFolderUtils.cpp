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
* Description:  Message store folder utils implementation.
*
*/



#include "MsgStoreFolderUtils.h"
//<cmail>
#include "MsgStoreFolder.h"
//</cmail>


/**
 * Private specific cleanup stack helper.
 *
 * @param aPtr
 */
static void ResetDestroyClose( TAny *aPtr )
    {
    static_cast< RPointerArray<CMsgStoreFolder>* >( aPtr )->ResetAndDestroy();
    static_cast< RPointerArray<CMsgStoreFolder>* >( aPtr )->Close();
    }


/**
 *
 */
EXPORT_C void MsgStoreFolderUtils::InitializeRootFoldersL(
    CMsgStoreMailBox& aMailBox,
    TMsgStoreRootFolders& aRootFolders )

    {
    CMsgStorePropertyContainer* props = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( props );

    TMsgStoreRootFolders rootFolders = { 0 };
    MsgStoreFolderUtils::GetRootFoldersL( aMailBox, rootFolders );

    for ( TUint32 i = EMsgStoreInbox; i <= EMsgStoreDeleted; i++ )
        {
        if ( 0 == rootFolders.iFolders[ i ] )
            {
            TBool isLocal = ( i == EMsgStoreDeleted );
            props->AddOrUpdatePropertyL( KMsgStorePropertyFolderType, i );
            aRootFolders.iFolders[ i ] = aMailBox.CreateFolderL( aMailBox.Id(), *props, isLocal );
            }
        else
            {
            aRootFolders.iFolders[ i ] = rootFolders.iFolders[ i ];
            }
        }

    CleanupStack::PopAndDestroy( props );
    }


/**
 *
 */
EXPORT_C void MsgStoreFolderUtils::GetRootFoldersL(
    CMsgStoreMailBox& aMailBox,
    TMsgStoreRootFolders& aRootFolders )
    
    {
    RPointerArray<CMsgStoreFolder> folders;
    CleanupStack::PushL( TCleanupItem( ResetDestroyClose, &folders ) );
    aMailBox.FoldersL( aMailBox.Id(), folders );

    TInt count = folders.Count();

    /**@ should the method leave or not if the structure is not complete ?*/
    
    for ( TInt i = 0; i < count; i++ )
        {
        CMsgStoreFolder* folder = folders[i];

        TUint idx = 0;
        if ( folder->FindProperty( KMsgStorePropertyFolderType, idx ) )
            {
            TUint32 type = folder->PropertyValueUint32L( idx );
           
            if ( EMsgStoreInbox <= type && EMsgStoreDeleted >= type )
                {
                aRootFolders.iFolders[type] = folder->Id();
                }
            }
        }

    CleanupStack::PopAndDestroy( &folders );   
    }
