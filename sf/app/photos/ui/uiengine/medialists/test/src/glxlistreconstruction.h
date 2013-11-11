/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unit tests
*
*/



#ifndef __C_GLXLISTCONSTRUCTION_H__
#define __C_GLXLISTCONSTRUCTION_H__

//  EXTERNAL INCLUDES
#include <e32std.h>

//  INTERNAL INCLUDES
#include "glxmediaid.h"
#include "mglxnavigablelistobserver.h"
#include "_glxnotification.h"

//  FORWARD DECLARATIONS
class CGlxItemList;
class CGlxStaticItemList;
class CGlxNavigableList;
class TGlxMedia;
class CMPXCollectionPath;

/**
 * Reconstruction of a list
 *
 */
class CGlxListReconstructionBase : public CBase, public MGlxNavigableListObserver
    {
    public:     
        ~CGlxListReconstructionBase();
        
        void ClearNotifications();
        
        TBool NotificationListEquals( const TNotification::TData* aNotificationList );
        
        // From MGlxNavigableListObserver
        void HandleItemsAdded( TInt aAddedAtIndex, TInt aCount );
        void HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount );
        void HandleFocusChanged( NGlxListDefs::TFocusChangeType aType, 
            TInt aNewIndex, TInt aOldIndex );
        void HandleItemSelected( TInt aIndex, TBool aSelected );
        
        virtual const TGlxMediaId& OriginalItem( TInt aIndex ) const = 0;
        virtual TBool ReconstructionEquals() const = 0;
    
    protected:
        void ConstructL( TInt aReservationCount = 400 );
    
    protected:
        // List of notifications received
        RArray< TNotification > iNotifications;
        // list that has been reconstructed from notifications
        RArray< TGlxMediaId > iReconstructedList;
        // focus index reconstructed from notifications
        TInt iFocusIndex;
        // selection reconstructed from notifications
        RArray< TInt> iSelectedItemIndices;
    };

/**
 * Reconstruction of a item list
 */
template < class _ListClass >
class CGlxListReconstruction : public CGlxListReconstructionBase
    {
    public:     
        inline CGlxListReconstruction();
        inline void SetList( _ListClass& aList );
        
        inline TBool ItemsEqual() const;
        
        // From CGlxListReconstructionBase
        inline const TGlxMediaId& OriginalItem( TInt aIndex ) const;
        
    protected: 
        _ListClass* iList;
    };

#include "glxlistreconstruction.inl"

#endif      //  __C_GLXITEMLISTCONSTRUCTION_H__
