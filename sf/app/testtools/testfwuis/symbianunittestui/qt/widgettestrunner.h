/*
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
 * Description: TabWidget class to select, run tests and show the result.
 *
 */

#ifndef WIDGETTESTRUNNER_H_
#define WIDGETTESTRUNNER_H_

#include <QWidget>

class TestRunnerPrivate;
class Settings;

class WidgetTestRunner : public QWidget
    {
    Q_OBJECT
    
public:
    WidgetTestRunner(QWidget* parent = 0);
    virtual ~WidgetTestRunner();
    
public:
    int listTestCases(const QString&, QStringList&);

signals:
    void infoMsgReceived(QString msg);
    void testFailed(QString, QString, int);
    void testPassed(int);
    void errorOccurred(int);
    void errorOccurred(const QString);
    void testExecuted(int);
    
public slots:
    void addTestDllNames(const QStringList&);
    void addTestCaseNames(const QStringList&);
    void saveSettings(const Settings*);
    void runTests();
    
private:
    TestRunnerPrivate* d_ptr;
    
private:
    friend class TestRunnerPrivate;
    };

#endif /* WIDGETTESTRUNNER_H_ */
