
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
* Description:  Helper class to test protected members from the proxy model 
* 
*/

// INCLUDES

#include "filterproxytester.h"

// ---------------------------------------------------------------------------
// FilterProxyTester
// ---------------------------------------------------------------------------
//
FilterProxyTester::FilterProxyTester(QObject *parent) :
VideoProxyModelAllVideos(parent)
{
    // NOP
}

// ---------------------------------------------------------------------------
// ~FilterProxyTester
// ---------------------------------------------------------------------------
//
FilterProxyTester::~FilterProxyTester()
{
    
}

// ---------------------------------------------------------------------------
// callLessThan
// ---------------------------------------------------------------------------
//   
bool FilterProxyTester::callLessThan( const QModelIndex & left, const QModelIndex & right ) const
{
    return VideoProxyModelAllVideos::lessThan(left, right);
}


// ---------------------------------------------------------------------------
// callLessThan
// ---------------------------------------------------------------------------
//   
bool FilterProxyTester::callFilterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    return VideoProxyModelAllVideos::filterAcceptsRow(source_row, source_parent);
}


// End of file
