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

#ifndef __UNITTEST_IMAGEDECODERWRAPPER_H__
#define __UNITTEST_IMAGEDECODERWRAPPER_H__

#include <QtTest/QtTest>

class GlxImageDecoderWrapper;

class TestGlxImageDecoderWrapper : public QObject
{
    Q_OBJECT
    
signals:

private slots:
    /**
     * called before anything else when test starts
     */
    void initTestCase();

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
     * will be called after testing ends
     *
     */
    void cleanupTestCase();
 
    /**
     * verifies getPixmap
     *
     */
   void testgetPixmap(); 
       
    /**
     * verifies decodeImage
     *
     */
   void testDecodeImage(); 
   
   void testResetDecoder();
    
private:
  
    /**
     * object under test
     */  
    GlxImageDecoderWrapper *mTestObject;
};

#endif //__UNITTEST_IMAGEDECODERWRAPPER_H__
