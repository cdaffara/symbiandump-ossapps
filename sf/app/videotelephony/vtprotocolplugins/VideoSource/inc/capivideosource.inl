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
* Description:  Video Source subsystem.
*
*/


/*
 * CVSController
*/

// -----------------------------------------------------------------------------
// CVSController::Observer
// -----------------------------------------------------------------------------
//
inline MVSControllerObserver& CVSController::Observer() const
    {
    return *iObserver;
    }

/*
 * CVSDataProvider
*/

// -----------------------------------------------------------------------------
// CVSDataProvider::Observer
// -----------------------------------------------------------------------------
//
inline MVSDataProviderObserver& CVSDataProvider::Observer() const
    {
    return *iObserver;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::BufferPool
// -----------------------------------------------------------------------------
//
inline MVSBufferPool& CVSDataProvider::BufferPool() const
    {
    return *iPool;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::FillBufferActive
// -----------------------------------------------------------------------------
//
inline TBool CVSDataProvider::FillBufferActive() const
    {
    return iFillBufferActive;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::Consumer
// -----------------------------------------------------------------------------
//
inline MVTVideoSink* CVSDataProvider::Consumer() const
    {
    return iConsumer;
    }

// -----------------------------------------------------------------------------
// CVSDataProvider::MediaId
// -----------------------------------------------------------------------------
//
inline TMediaId CVSDataProvider::MediaId() const
    {
    return iMediaId;
    }

/*
 * CVSDataSource
*/

// -----------------------------------------------------------------------------
// CVSDataSource::Observer
// -----------------------------------------------------------------------------
//
inline MVSDataSourceObserver& CVSDataSource::Observer() const
    {
    return *iObserver;
    }

// -----------------------------------------------------------------------------
// CVSDataSource::DataProvider
// -----------------------------------------------------------------------------
//
inline CVSDataProvider& CVSDataSource::DataProvider() const
    {
    return *iProvider;
    }

// -----------------------------------------------------------------------------
// CVSDataSource::SwitchProvider
// -----------------------------------------------------------------------------
//
inline CVSDataProvider* CVSDataSource::SwitchProvider(
    CVSDataProvider* aNewProvider )
    {
    CVSDataProvider* oldProvider = iProvider;
    iProvider = aNewProvider;
    return oldProvider;
    }

/*
 * TVSStillImageDataProviderParams
 */

// -----------------------------------------------------------------------------
// TVSStillImageDataProviderParams::TVSStillImageDataProviderParams
// -----------------------------------------------------------------------------
//
TVSStillImageDataProviderParams::TVSStillImageDataProviderParams()
    : iInitialize( EInitializeAsBlankImage ), iFileName( TPtrC() )
    {
    }

// -----------------------------------------------------------------------------
// TVSStillImageDataProviderParams::TVSStillImageDataProviderParams
// -----------------------------------------------------------------------------
//
TVSStillImageDataProviderParams::TVSStillImageDataProviderParams(
    const TInitialize aInitialize )
    : iInitialize( aInitialize ), iFileName( TPtrC() )
    {
    }

// -----------------------------------------------------------------------------
// TVSStillImageDataProviderParams::TVSStillImageDataProviderParams
// -----------------------------------------------------------------------------
//
TVSStillImageDataProviderParams::TVSStillImageDataProviderParams(
    const TFileName& aFileName )
    : iInitialize( EInitializeFromFile ), iFileName( aFileName )
    {
    }
