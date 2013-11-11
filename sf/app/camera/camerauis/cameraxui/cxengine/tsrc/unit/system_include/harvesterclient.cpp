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
* Description:
*
*/

#include "cxutils.h"
#include "harvesterclient.h"




/*!
* RHarvesterClient::RHarvesterClient
*/
RHarvesterClient::RHarvesterClient()
{

}


/*!
* open connection.
*/
int RHarvesterClient::Connect()
{
    return KErrNone;
}



/*!
* close connection.
*/
void RHarvesterClient::Close()
{
    // nothing to do
}



/*!
* register observer for receiving harvestcomplete callbacks.
*/
void RHarvesterClient::SetObserver(MHarvestObserver* aObserver)
{
    CX_DEBUG_ENTER_FUNCTION();
    mObserver = aObserver;
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* remove observer for any harvest complete callbacks.
*/
void RHarvesterClient::RemoveObserver(MHarvestObserver* aObserver)
{
    CX_DEBUG_ENTER_FUNCTION();

    if(aObserver == mObserver) {
        mObserver = NULL;
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* harvest file
*/
void RHarvesterClient::HarvestFile(const TDesC& aURI, RArray<TUint32> &aAlbumIds, TBool aAddLocation)
{
    Q_UNUSED(aAlbumIds);
    Q_UNUSED(aAddLocation);

    int error = KErrNone;

    if (aURI == KNullDesC) {
        error = KErrNotFound;
    }

    if (mObserver) {
        TFileName filename = aURI;
        // file harvested, inform clients
        mObserver->HarvestingComplete(filename, error);
    }

}
