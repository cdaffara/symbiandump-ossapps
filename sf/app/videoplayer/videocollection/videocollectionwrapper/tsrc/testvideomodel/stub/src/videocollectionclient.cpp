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
* Description:  stubclass of videocollection for testing VideoListDataModel class methods*
*/



// INCLUDE FILES
#include "videocollectionclient.h"
#include "videodatasignalreceiver.h"

bool VideoCollectionClient::mFailInitialize = false;
    
bool VideoCollectionClient::mFailConnectSignals = false; 

int VideoCollectionClient::mInitializeCount = 0;

int VideoCollectionClient::mRemoveItemsReturn = -1;



// -----------------------------------------------------------------------------
// VideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::VideoCollectionClient()
{
}


// -----------------------------------------------------------------------------
// ~VideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::~VideoCollectionClient()
{

}

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::initialize( VideoDataSignalReceiver* /*signalReceiver*/)
{
    mInitializeCount++;
    if(mFailInitialize)
    {
        return -1;
    }
    return 0;
}


// -----------------------------------------------------------------------------
// removeItemsFromAlbum
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::removeItemsFromAlbum(const TMPXItemId &albumId, const QList<TMPXItemId> &items)
{
    Q_UNUSED(albumId);
    Q_UNUSED(items);
    return mRemoveItemsReturn;
}



