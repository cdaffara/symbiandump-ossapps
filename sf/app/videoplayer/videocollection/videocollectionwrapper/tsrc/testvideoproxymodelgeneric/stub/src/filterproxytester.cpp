
/**
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
* Description:  Helper class to test protected members from proxy model 
* 
*/

// INCLUDES

#define private public
#include "videoproxymodelgeneric.h"
#undef private

#include "filterproxytester.h"
#include "videolistdatamodel.h"

// ---------------------------------------------------------------------------
// FilterProxyTester
// ---------------------------------------------------------------------------
//
FilterProxyTester::FilterProxyTester(QObject *parent) :
VideoProxyModelGeneric(parent),
mTmpModel(0),
mTmpClient(0)
{
    // NOP
}

// ---------------------------------------------------------------------------
// ~FilterProxyTester
// ---------------------------------------------------------------------------
//
FilterProxyTester::~FilterProxyTester()
{
    if(!mModel)
    {
        toggleNullModel();
    }

    if(!mCollectionClient)
    {
        toggleNullClient();
    }
}

// ---------------------------------------------------------------------------
// callLessThan
// ---------------------------------------------------------------------------
//   
bool FilterProxyTester::callLessThan( const QModelIndex & left, const QModelIndex & right ) const
{
    return VideoProxyModelGeneric::lessThan(left, right);
}

// ---------------------------------------------------------------------------
// callLessThan
// ---------------------------------------------------------------------------
//   
bool FilterProxyTester::callFilterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    return VideoProxyModelGeneric::filterAcceptsRow(source_row, source_parent);
}

// ---------------------------------------------------------------------------
// setClient
// ---------------------------------------------------------------------------
//
int FilterProxyTester::setClient(VideoCollectionClient *collectionClient)
{
	VideoProxyModelGeneric::mCollectionClient = collectionClient;
	return 0;
}

// ---------------------------------------------------------------------------
// getClient
// ---------------------------------------------------------------------------
//
VideoCollectionClient* FilterProxyTester::getClient()
{
    return VideoProxyModelGeneric::mCollectionClient;
}

/**
 * For getting VideoListDataModel pointer
 *
 */
VideoListDataModel *FilterProxyTester::getModel()
{
    return mModel;
}

/**
 * Toggles null for internal model pointer.
 * 
 */
void FilterProxyTester::toggleNullModel()
{
    if(mModel)
    {
        mTmpModel = mModel;
        mModel = 0;
    }
    else
    {
        mModel = mTmpModel;
        mTmpModel = 0;
    }
}

/**
 * Sets null for internal collection client pointer.
 * 
 */
void FilterProxyTester::toggleNullClient()
{
    if(mCollectionClient)
    {
        mTmpClient = mCollectionClient;
        mCollectionClient = 0;
    }
    else
    {
        mCollectionClient = mTmpClient;
        mTmpClient = 0;
    }
}


/**
 * Sets wanted sort order.
 * 
 */
void FilterProxyTester::setWantedSortOrder(Qt::SortOrder order)
{
    mWantedSortOrder = order;
}

/**
 * Helper method to call protected disconnectSignals.
 */
void FilterProxyTester::callDisconnectSignals()
{
    disconnectSignals();
}

// End of file
