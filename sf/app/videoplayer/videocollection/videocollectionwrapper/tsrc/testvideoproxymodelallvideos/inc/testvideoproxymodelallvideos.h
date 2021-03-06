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
* Description:   tester for methods in VideoProxyModelAllVideos
* 
*/

#ifndef __TESTVIDEOPROXYMODELALLVIDEOS_H__
#define __TESTVIDEOPROXYMODELALLVIDEOS_H__


// INCLUDES
#include <QtTest/QtTest>
#include <mpxitemid.h>

class FilterProxyTester;

class TestVideoProxyModelAllVideos : public QObject
{
    Q_OBJECT

    // test functions for the test framework
    
signals:
    
private slots:

    /**
     * will be called before each testfunction is executed.
     *
     */
    void init(); 
    
    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();

    /**
     * Tests filterAcceptsRow.
     */
    void testFilterAcceptsRow();
    
    /**
     * Tests getOpenItem.
     */
    void testGetOpenItem();

private:

    /**
     * tester object inherited from the actual test object
     * implemented to be able to call protected objects
     */
    FilterProxyTester *mTestObject;
    
};

#endif  // __TESTVIDEOPROXYMODELALLVIDEOS_H__

// End of file
    


