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
* Description: This class is test case for file manager internal api class.
*
*/

#include "fmfileiconprovider.h"
#include "fmfiletyperecognizer.h"
#include <QtTest/QtTest>
#include <QMap>
#include <QString>
#include <QStringList>
#include <qdebug.h>


class TestFmFileTypeProvider: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    
    void testFmFileIconProvider(); // test the FmFileIconProvider() constructor funtion.
    
    void testIconKnown(); // test the icon() funtion.
//    void testIconUnKnown();
    
    void cleanupTestCase();       // Finalize test data
};

/*!
    \class TestFmFileTypeProvider
    \brief describe the test case's goal, like: \n
      class name:  FmDriveModel \n
      class's description: Test the File Manager internal api functions. \n
      type of test case: unit test\n 
      test cases' number totally: \n
 */

void TestFmFileTypeProvider::initTestCase()
{    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        FmFileIconProvider(); \n
     2. Case Descrition: test the constructor function. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> no \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash \n 
 */
void TestFmFileTypeProvider::testFmFileIconProvider()
{
    FmFileIconProvider *testProvider = new FmFileIconProvider();
//QMap<QString, FileType>     mFileExtensionMap is a private member, can not verify the result.
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        virtual QIcon icon(const QFileInfo &info) const; \n
     2. Case Descrition: Verify the right file type is returned \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retType = FileTypeDrive \n 
 */
void TestFmFileTypeProvider::testIconKnown()
{
    QFileInfo path1("C:\\image.jpg");
    QFileInfo path2("D:\\Video.rm");
    QFileInfo path3("D:\\Tone.wav");
    QFileInfo path4("Z:\\Text.doc");
    QFileInfo path5("Z:\\Install.sisx");
    QFileInfo path6("D:\\Java.jar");
    QFileInfo path7("C:\\Flash.swf");
    
    FmFileIconProvider *testProvider = new FmFileIconProvider();
    QIcon retIcon1 = testProvider->icon(Drive_C);
//    QVERIFY(retIcon1 != QIcon());
    
    QIcon retIcon2 = testProvider->icon(Drive_D);
//    QVERIFY(retIcon2 != 0);
    
    QIcon retIcon3 = testProvider->icon(Drive_Z);
//    QVERIFY(retIcon3 != 0);
    
    QIcon retIcon4 = testProvider->icon(Folder_C_Data);
//    QVERIFY(retIcon4 != 0);
    
    QIcon retIcon01 = testProvider->icon(path1);
//    QVERIFY(retIcon01 != 0);
    
    QIcon retIcon02 = testProvider->icon(path2);
//    QVERIFY(retIcon02 != 0);
    
    QIcon retIcon03 = testProvider->icon(path3);
//    QVERIFY(retIcon03 != 0);
    
    QIcon retIcon04 = testProvider->icon(path4);
//    QVERIFY(retIcon04 != 0);
    
    QIcon retIcon05 = testProvider->icon(path5);
//    QVERIFY(retIcon05 != 0);
    
    QIcon retIcon06 = testProvider->icon(path6);
//    QVERIFY(retIcon06 != 0);
    
    QIcon retIcon07 = testProvider->icon(path7);
//    QVERIFY(retIcon07 != 0);
    
    delete testProvider;
}

void TestFmFileTypeProvider::cleanupTestCase()
{

}

QTEST_MAIN(TestFmFileTypeProvider)
#include "ut_fmfiletypeprovider.moc"
