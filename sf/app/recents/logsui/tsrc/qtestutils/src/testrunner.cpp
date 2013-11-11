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

#include "testrunner.h"
#include <QtTest/QtTest>
#include <QDir>
#include <stdio.h>

const char testCaseElement[] = "TestCase";
const char testFunctionElement[] = "TestFunction";
const char incidentElement[] = "Incident";
const char descriptionElement[] = "Description";
const char nameAttr[] = "name";
const char typeAttr[] = "type";
const char fileAttr[] = "file";
const char lineAttr[] = "line";
const char attrValueFail[] = "fail";


TestRunner::TestRunner(const QString& name, const QString& combinedOutputFileName)
: mTestCount(0),
  mParsingIncidentElement(false),
  mParsingDescriptionElement(false),
  mCurrentTestFailed(false),
  mCurrentTestFailureLine(0),
  mCombinedOutputFileName(combinedOutputFileName)
{
    mTestRunParams.append(name);
    mTestRunParams.append("-xml");
    mTestRunParams.append("-o");
    mHomeDir = QDir::homePath();
    mTestRunParams.append(QString()); // Initial result file name

    if (!mHomeDir.endsWith(QString::fromAscii("/")))
        mHomeDir += QString::fromAscii("/");
}

TestRunner::~TestRunner()
{
}

int TestRunner::runTests(QObject& testObject)
{
    QString className(testObject.metaObject()->className());
    printf("Running tests for %s ... ", className.toUtf8().data());
    QString resultFileName = mHomeDir + className + ".xml";
    mTestRunParams.replace(mTestRunParams.count()-1,resultFileName);
    int errorsBefore = mErrors.count();
    int error = QTest::qExec(&testObject, mTestRunParams);
    parse(resultFileName);
    mResultFiles.append(resultFileName);
    printf("Failures: %d\n",mErrors.count()-errorsBefore);
    fflush(stdout);
    return error;
}

void TestRunner::printResults()
{
    printf("\nTests executed: %d\n",mTestCount);
    if (mErrors.count() > 0) {
        printf("Failures (%d):\n", mErrors.count());
        foreach(QString error, mErrors) {
            printf("\n%s", error.toUtf8().data());
        }
        printf("\n");
    } else {
        printf("All passed.\n\n");
    }
    fflush(stdout);
    
    combineResults();
}

void TestRunner::parse(const QString& fileName)
{
    QFile file(fileName);
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.parse(inputSource);
}

bool TestRunner::startElement(
    const QString& /*namespaceURI*/, 
    const QString& /*localName*/, 
    const QString& qName, 
    const QXmlAttributes& atts)
{
    if (qName == QString::fromAscii(testFunctionElement)) {
        mTestCount++;
        mCurrentTestName = atts.value(QString::fromAscii(nameAttr));
        return true;
    }
    if (qName == QString::fromAscii(incidentElement)) {
        mParsingIncidentElement = true;
        if (atts.value(QString::fromAscii(typeAttr)) == QString::fromAscii(attrValueFail)) {
            mCurrentTestFailed = true;
            mCurrentTestFile = atts.value(QString::fromAscii(fileAttr));
            mCurrentTestFailureLine = atts.value(QString::fromAscii(lineAttr)).toInt();
        }
        return true;
    }
    mParsingDescriptionElement =
        (qName == QString::fromAscii(descriptionElement));
    return true;
}

bool TestRunner::endElement(
    const QString& /*namespaceURI*/,
    const QString& /*localName*/,
    const QString& qName)
{
    if (qName == QString::fromAscii(incidentElement)) {
        mParsingIncidentElement = false;
        mCurrentTestFailed = false;
        return true;
    }
    if (qName == QString::fromAscii(descriptionElement)) {
        mParsingDescriptionElement = false;
    }    
    return true;
}

bool TestRunner::characters(const QString& ch)
{
    if (mParsingIncidentElement && 
        mParsingDescriptionElement &&
        mCurrentTestFailed) {
        QByteArray testResult = mCurrentTestName.toAscii() + " failed:\n";
        testResult += "File: ";
        testResult += mCurrentTestFile.toAscii();
        testResult += "\n";
        testResult += "Line: ";
        testResult += QByteArray::number(mCurrentTestFailureLine);
        testResult += "\n";
        testResult += "Reason: ";
        testResult += ch.toAscii();
        testResult += "\n";
        mErrors.append(QString::fromAscii(testResult.data()));
    }
    return true;
}

void TestRunner::combineResults()
{
    if ( mCombinedOutputFileName.isEmpty() ){
        return;
    }
    if ( !mCombinedOutputFileName.contains(QString::fromAscii("/")) ){
        mCombinedOutputFileName.prepend( mHomeDir );
    }
    QFile file(mCombinedOutputFileName);
    if (!file.open(QIODevice::WriteOnly)){
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    QString caseName;
    foreach ( QString resultFile, mResultFiles ){
        QFile readFile( resultFile );
        if ( resultFile != mCombinedOutputFileName && readFile.open(QIODevice::ReadOnly) ){
            QXmlStreamReader reader(&readFile);
            appendToXml(writer, reader, caseName);
            readFile.close();
        }
    }
    writer.writeEndDocument();
    file.close();
}

void TestRunner::appendToXml(QXmlStreamWriter& writer, QXmlStreamReader& reader, QString& caseName)
{
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType type = reader.readNext();
        if ( type == QXmlStreamReader::StartDocument  || 
             type == QXmlStreamReader::EndDocument ){
            // Ignored
        } else if ( reader.name() == testCaseElement ){
            if ( type == QXmlStreamReader::StartElement ){
                QString tempCaseName = reader.attributes().value(nameAttr).toString();
                if ( caseName.isEmpty() && type == QXmlStreamReader::StartElement){
                    writer.writeStartElement(testCaseElement);
                    writer.writeAttribute(nameAttr, mTestRunParams.at(0));
                }
                caseName = tempCaseName;
            }  
        } else if ( type == QXmlStreamReader::StartElement ) {
            if ( reader.name() == testFunctionElement ){
                QString tempFuncName = reader.attributes().value(nameAttr).toString();
                if ( !tempFuncName.isEmpty() ){
                    writer.writeStartElement(testFunctionElement);
                    writer.writeAttribute(nameAttr, caseName + "::" + tempFuncName);
                }
            } else {
                writer.writeCurrentToken( reader );
            }
        } else {
            writer.writeCurrentToken( reader );
        }
    }
}
