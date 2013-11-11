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
#include <QString>
#include "cxeerrormappingsymbian.h"
#include "cxutils.h"
#include "cxeharvestercontrolsymbian.h"



/*!
* CxeHarvesterControlSymbian::CxeHarvesterControlSymbian
*/
CxeHarvesterControlSymbian::CxeHarvesterControlSymbian()
: mHarvesterClientConnected(false),
  mRegisteredForHarvesterEvents(false)
{
    CX_DEBUG_IN_FUNCTION();
}


/*!
* CxeHarvesterControlSymbian::~CxeHarvesterControlSymbian
*/
CxeHarvesterControlSymbian::~CxeHarvesterControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    deRegisterHarverterClientEvents();

    if (mHarvesterClientConnected) {
        mHarvesterClient.Close();
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Calls MDS (Meta data system) harvester client to "harvest" files to
* specific album and also to default "captured" album in photos.
*/
CxeError::Id CxeHarvesterControlSymbian::harvestFile(const QString& filename,
                                                     bool addLocation,
                                                     quint32 albumId)
{
    CX_DEBUG_ENTER_FUNCTION();

    TInt err = KErrNone;
    RArray<TUint32> harvestAlbumIds;

    if (albumId > 0) {
       err = harvestAlbumIds.Append(albumId);
    }

    if (!mHarvesterClientConnected) {
        err = mHarvesterClient.Connect();
        if (!err) {
            mHarvesterClientConnected = true;
        }

    }

    if (KErrNone == err) {
        registerForHarvesterEvents();
        TPtrC16 harvestFilename(reinterpret_cast<const TUint16*>(filename.utf16()));
        mHarvesterClient.HarvestFile(harvestFilename,
                                     harvestAlbumIds,
                                     addLocation);
    }

    harvestAlbumIds.Close();

    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(err);
}



/*!
* Call back method, which informs client about harvest complete events from MDS
* harvester client
*/
void CxeHarvesterControlSymbian::HarvestingComplete(TDesC& aURI ,TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();

    QString uri = QString::fromUtf16(aURI.Ptr(),aURI.Length());
    CX_DEBUG(("Harvesting completed! error = %d, filename = %s", aError,
                                                                 uri.toAscii().constData()));
    // update the clients about the harvest complete event
    emit fileHarvested(CxeErrorHandlingSymbian::map(aError), uri);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Helper method to de-register for harvester events
*/
void CxeHarvesterControlSymbian::deRegisterHarverterClientEvents()
{
    if (mRegisteredForHarvesterEvents && mHarvesterClientConnected) {
        mHarvesterClient.RemoveObserver(this);
        mRegisteredForHarvesterEvents = false;
    }
}


/*!
* Helper method to register for harvester events
*/
void CxeHarvesterControlSymbian::registerForHarvesterEvents()
{
    if ( !mRegisteredForHarvesterEvents && mHarvesterClientConnected) {
        mHarvesterClient.SetObserver(this);
        mRegisteredForHarvesterEvents = true;
    }
}
