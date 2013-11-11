/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: tester for methods in VideoCollectionUiLoader
*
*/

#ifndef TESTVIDEOCOLLECTIONUILOADER_H
#define TESTVIDEOCOLLECTIONUILOADER_H

// INCLUDES
#include <qttest/qttest>

// FORWARD DECLARATIONS
class VideoCollectionUiLoader;

class TestVideoCollectionUiLoader : public QObject
{
    Q_OBJECT

public:
    TestVideoCollectionUiLoader();
    virtual ~TestVideoCollectionUiLoader();
    
private slots: // test functions for the test framework
    void init();
    void cleanup();
    
    /** addData testcase */
    void testAddData();
    
    /** findWidget testcase */
    void testFindWidget();
    
    /** findObject testcase */
    void testFindObject();
        
    /** setIsServce testcase */
    void testIsService();
        
    /** loadDocml testcase */
    void testLoadDocml();
        
    /** loadSection testcase */
    void testLoadSection();
        
    /** loadPhase testcase */
    void testLoadPhase();
    
    /** loadPhase testcase */
    void testLoadPhase_2();
    
    /** delete loaded items testcase*/
    void testDelete();
    
    /** removeOrphanFromList testcase */
    void testRemoveOrphanFromList();    
    
    /** prepareDocmlAndSection testcase */
    void testPrepareDocmlAndSection();
    
private slots:
    /** slot which is called when object is ready */
    void handleObjectReady(QObject *object, const QString &name);

signals:

    void testSignal(QObject *object);
    
private:
    /** class under test */
    VideoCollectionUiLoader *mTestObject;
    
    /** objects initialized */
    QHash<QString, QObject*> mObjects;
};

#endif//TESTVIDEOCOLLECTIONUILOADER_H
