
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
* Description:   tester for methods in VideoCollectionWrapperPrivate
* 
*/

#ifndef __TESTVIDEOCOLLECTIONWRAPPER_H__
#define __TESTVIDEOCOLLECTIONWRAPPER_H__


// INCLUDES
#include <QtTest/QtTest>


class VideoCollectionWrapperPrivate;

class TestVideCollectionWrapper_p : public QObject
{
    Q_OBJECT


signals:

    /**
     * common test signal
     */
    void testSignal();    
    
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
     * initialization of model fails test
     */
    void testGetModelInitFail();
    
    /**
     * succeeded getModel test
     */
    void testGetModelSucceed();

    /**
     * verifies aboutToQuitSlot
     */
    void testAboutToQuitSlot();
    
private:

    /**
     * testobject
     */
    VideoCollectionWrapperPrivate* mTestObject;

};




#endif  // __TESTVIDEOCOLLECTIONWRAPPER_H__

// End of file
    


