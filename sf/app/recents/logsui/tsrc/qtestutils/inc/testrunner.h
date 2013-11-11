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

#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <QXmlDefaultHandler>

class QXmlStreamWriter;
class QXmlStreamReader;

class TestRunner : public QXmlDefaultHandler
{
public: // Constructors and destructor
    TestRunner(const QString& name, const QString& combinedOutputFileName = QString() );
    ~TestRunner();
        
public: // New functions
    
    int runTests(QObject& testObject);
    void printResults();
    
protected: // From QXmlContentHandler 
    bool startElement(
        const QString& namespaceURI,
        const QString& localName,
        const QString& qName,
        const QXmlAttributes& atts);
    
    bool endElement(
        const QString& namespaceURI,
        const QString& localName,
        const QString& qName);
    
    bool characters(const QString& ch);

private: // New functions

    void parse(const QString& fileName);
    void combineResults();
    void appendToXml(QXmlStreamWriter& writer, QXmlStreamReader& reader, QString& caseName);

private: // Data
    QStringList mTestRunParams;
    QString mHomeDir;
    int mTestCount;
    QStringList mErrors;
    bool mParsingIncidentElement;
    bool mParsingDescriptionElement;
    bool mCurrentTestFailed;
    QString mCurrentTestName;
    QString mCurrentTestFile;
    int mCurrentTestFailureLine;
    QString mCombinedOutputFileName;
    QStringList mResultFiles;
};


#endif // TESTRUNNER_H
