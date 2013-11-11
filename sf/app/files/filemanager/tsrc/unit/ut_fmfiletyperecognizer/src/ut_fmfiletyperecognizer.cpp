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

#include "fmfiletyperecognizer.h"
#include <QtTest/QtTest>
#include <QMap>
#include <QString>
#include <QStringList>

class TestFmFileTypeRcognizer: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    
    void testFmFileTypeRecognizer(); // test the FmFileTypeRecognizer() constructor.   
    
    void testGetTypeDrive(); // test the getType() function
    void testGetTypePath();
    void testGetTypeKnown();
    void testGetTypeUnKnown();
    
    void cleanupTestCase();       // Finalize test data
};

/*!
    \class TestFmFileTypeRcognizer
    \brief \n
      class name:  FmDriveModel \n
      class's description: Test the File Manager internal api functions. \n
      type of test case: unit test\n 
      test cases' number totally: 5 \n
 */

void TestFmFileTypeRcognizer::initTestCase()
{ 
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        FmFileTypeRecognizer(); \n
     2. Case Descrition: test the constructor function. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> no \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> no crash \n 
 */
void TestFmFileTypeRcognizer::testFmFileTypeRecognizer()
{
    FmFileTypeRecognizer *testRcognizer = new FmFileTypeRecognizer();
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        FmFileTypeRecognizer::FileType getType( const QString& path ) const ; \n
     2. Case Descrition: Verify the right file type is returned \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retType = FileTypeDrive \n 
 */
void TestFmFileTypeRcognizer::testGetTypeDrive()
{
    FmFileTypeRecognizer *testRcognizer = new FmFileTypeRecognizer();
    FmFileTypeRecognizer::FileType retType = testRcognizer->getType( "C:\\" );
    qDebug() << retType;
    QVERIFY(retType == FmFileTypeRecognizer::FileTypeDrive);
    delete testRcognizer;    
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        FmFileTypeRecognizer::FileType getType( const QString& path ) const ; \n
     2. Case Descrition: Verify the right file type is returned \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retType = FileTypeFolder \n 
 */
void TestFmFileTypeRcognizer::testGetTypePath()
{
    FmFileTypeRecognizer *testRcognizer = new FmFileTypeRecognizer();
    FmFileTypeRecognizer::FileType retType = testRcognizer->getType( Folder_C_Data);
    qDebug() << retType;
    QVERIFY(retType == FmFileTypeRecognizer::FileTypeFolder);
    delete testRcognizer;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        FmFileTypeRecognizer::FileType getType( const QString& path ) const ; \n
     2. Case Descrition: Verify the right file type is returned \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> return the correct file type \n 
 */
void TestFmFileTypeRcognizer::testGetTypeKnown()
{
    QString path1 = "image.jpg";
    QString path2 = "Video.rm";
    QString path3 = "Tone.wav";
    QString path4 = "Text.doc";
    QString path5 = "Install.sisx";
    QString path6 = "Java.jar";
    QString path7 = "Flash.swf";
#ifdef FEATURE_NOT_IMPLEMENTED_TYPERECOG
    QString pathWidget = "widget.wgz"; 
    QString pathWebLink = "abc.lnk";// Need confirm the suffix
#endif
    FmFileTypeRecognizer *testRcognizer = new FmFileTypeRecognizer();
    
    FmFileTypeRecognizer::FileType retType1 = testRcognizer->getType( path1);
    QVERIFY(retType1 == FmFileTypeRecognizer::FileTypeImage);
    
    FmFileTypeRecognizer::FileType retType2 = testRcognizer->getType( path2);
//    qDebug() << retType1;
    QVERIFY(retType2 == FmFileTypeRecognizer::FileTypeVideo);
    
    FmFileTypeRecognizer::FileType retType3 = testRcognizer->getType( path3);
    QVERIFY(retType3 == FmFileTypeRecognizer::FileTypeTone);
    
    FmFileTypeRecognizer::FileType retType4 = testRcognizer->getType( path4);
    QVERIFY(retType4 == FmFileTypeRecognizer::FileTypeText);
    
    FmFileTypeRecognizer::FileType retType5 = testRcognizer->getType( path5);
    QVERIFY(retType5 == FmFileTypeRecognizer::FileTypeSisx);
    
    FmFileTypeRecognizer::FileType retType6 = testRcognizer->getType( path6);
    QVERIFY(retType6 == FmFileTypeRecognizer::FileTypeJava);
    
    FmFileTypeRecognizer::FileType retType7 = testRcognizer->getType( path7);
    QVERIFY(retType7 == FmFileTypeRecognizer::FileTypeFlash);
    
#ifdef FEATURE_NOT_IMPLEMENTED_TYPERECOG
    FmFileTypeRecognizer::FileType retType8 = testRcognizer->getType( pathWidget); 
//    QVERIFY(retType8 == FmFileTypeRecognizer::FileTypeWidget);
    
    FmFileTypeRecognizer::FileType retType9 = testRcognizer->getType( pathWebLink); 
//    QVERIFY(retType9 == FmFileTypeRecognizer::FileTypeWebLink);
#endif    
    delete testRcognizer;
}

/*!
     Test Case Description:\n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        FmFileTypeRecognizer::FileType getType( const QString& path ) const ; \n
     2. Case Descrition: Verify the right file type is returned \n
     3. Input Parameters:  \n &nbsp;&nbsp;
        <1> path = QString(XX) \n
     4. Expected result: \n &nbsp;&nbsp;
        <1> retType = FileTypeUnKnown \n 
 */
void TestFmFileTypeRcognizer::testGetTypeUnKnown()
{
    FmFileTypeRecognizer *testRcognizer = new FmFileTypeRecognizer();
    
    FmFileTypeRecognizer::FileType retType = testRcognizer->getType( "unknown.unknow");
    QVERIFY(retType == FmFileTypeRecognizer::FileTypeUnKnown);
    delete testRcognizer;
}

void TestFmFileTypeRcognizer::cleanupTestCase()
{

}

QTEST_MAIN(TestFmFileTypeRcognizer)
#include "ut_fmfiletyperecognizer.moc"
