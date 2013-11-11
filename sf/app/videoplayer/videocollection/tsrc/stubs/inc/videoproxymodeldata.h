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
* Description: stub VideoProxyModelData
* 
*/

#ifndef VIDEOPROXYMODELDATA_H
#define VIDEOPROXYMODELDATA_H

#include <qnamespace.h>
#include <qstring.h>
#include <mpxitemid.h>
#include <QModelIndex>

class VideoProxyModelData
{
public: // methods
    static void reset()
    {
        mOpenFails = false;
        mSortRole = -1;
        mSortOrder = Qt::AscendingOrder;
        mSortAsync = false;
        mDeleteItemsFails = true;
        mRemoveAlbumsFails = true;
        mDoSortingCallCount = 0;
        mItemIds.clear();
        mLastIndex = QModelIndex();
        mLastItemId = TMPXItemId::InvalidId();
        mOpenedItemId = TMPXItemId::InvalidId();
        mDetailsReturnValue = 0;
        mAddItemsInAlbumReturnValue = 0;
        mRemoveItemsFromAlbumReturnValue = 0;
        mGenericFilterId = TMPXItemId::InvalidId();
        mGenericFilterValue = false;
        mNewAlbumId = TMPXItemId::InvalidId(); 
        mLastItemNameInRename = "";
        mRenameItemReturnValue= 0;
        mLastAddedAlbumName = "";
    }
    
public: // data
    static bool mOpenFails;
    static int mSortRole;
    static Qt::SortOrder mSortOrder;
    static bool mSortAsync;
    static bool mDeleteItemsFails;
    static bool mRemoveAlbumsFails;
    static int mDoSortingCallCount;
    static QList<TMPXItemId> mItemIds;
    static QModelIndex mLastIndex;
    static TMPXItemId mLastItemId;
    static TMPXItemId mOpenedItemId;
    static int mDetailsReturnValue;
    static int mAddItemsInAlbumReturnValue;
    static int mRemoveItemsFromAlbumReturnValue; 
    static TMPXItemId mGenericFilterId;
    static bool mGenericFilterValue; 
    static TMPXItemId mNewAlbumId;
    static QString mLastItemNameInRename;
    static int mRenameItemReturnValue;
    static QString mLastAddedAlbumName;
};

#endif /* VIDEOPROXYMODELDATA_H */
