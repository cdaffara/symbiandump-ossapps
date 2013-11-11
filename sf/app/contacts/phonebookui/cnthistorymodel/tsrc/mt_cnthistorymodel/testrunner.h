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
#include <QObject>
#include <HbApplication>

class Starter : public QObject
{
    Q_OBJECT
public:
    Starter( HbApplication& aApp );
    ~Starter();
    
    bool eventFilter( QObject* aObject, QEvent* aEvent );
        
private:
    HbApplication& mApp;
    bool mTestStarted;
};

class TestRunner : public QXmlDefaultHandler
{
    
public: // Constructors and destructor
    TestRunner(const QString& name);
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
    
    QList<QObject> mTests;
};


#endif // TESTRUNNER_H
