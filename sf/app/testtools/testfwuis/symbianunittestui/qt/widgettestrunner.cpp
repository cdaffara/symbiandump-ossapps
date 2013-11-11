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
 * Description: widget class to run tests.
 *
 */

#include "widgettestrunner.h"
#include "settings.h"
#ifdef Q_OS_SYMBIAN
#include "testrunner_s60.h"
//#else
//#include "testrunner_stub.h"
#endif

WidgetTestRunner::WidgetTestRunner(QWidget* parent) :
    QWidget(parent)
    {
#ifdef Q_OS_SYMBIAN //Symbian specific compilation
    QT_TRAP_THROWING(d_ptr = TestRunnerPrivate::NewL(this));
//#else
//    d_ptr = new TestRunnerPrivate(this);
#endif
    }

WidgetTestRunner::~WidgetTestRunner()
    {
    delete d_ptr;
    }

void WidgetTestRunner::addTestDllNames(const QStringList& dllNames)
    {
    if (d_ptr->SetTestDllNames(dllNames) != 0)
        {
        emit this->errorOccurred(tr("Failed to add test dll files."));
        }
    }

void WidgetTestRunner::addTestCaseNames(const QStringList& caseNames)
    {
    if (d_ptr->SetTestCaseNames(caseNames) != 0)
        {
        emit this->errorOccurred(tr("Failed to set test cases to run."));
        }
    }

int WidgetTestRunner::listTestCases(const QString& dllName, QStringList& caseNames)
    {
    int err = -1;
    if (!dllName.isEmpty())
        {
        err = d_ptr->ListTestCases(dllName, caseNames);
        if (err != 0)
            {
            emit this->errorOccurred(tr("Failed to list cases in \"") + dllName + tr("\""));
            }
        }
    return err;
    }

void WidgetTestRunner::saveSettings(const Settings* settings)
    {
    if (d_ptr->SetOutputFormat(settings->outputFormat) != 0)
        {
        emit this->errorOccurred(tr("Failed to set output format."));
        }
    d_ptr->SetAllocFailureSimulation(settings->memAllocFailureSim);
    d_ptr->SetTimeout(settings->timeout);
    }

void WidgetTestRunner::runTests()
    {
    d_ptr->RunTests();
    }
