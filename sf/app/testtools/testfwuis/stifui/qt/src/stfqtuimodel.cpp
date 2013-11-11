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
 * Description: QT C++ based Class.
 *              application model implementation.
 *
 */
#include "stfqtuimodel.h"
#include <UIEngineContainer.h>

StfQtUIModel::StfQtUIModel()
    {
    //nothing to do.
    }

StfQtUIModel::~StfQtUIModel()
    {
    //nothing to do.
    }

void StfQtUIModel::PauseCase()
    {
    foreach(const CStartedTestCase* startedCase, runningCaseList.keys())
            {
            startedCase->UIEngineContainer().PauseTest();
            }
    }

void StfQtUIModel::ResumeCase()
    {
    foreach(const CStartedTestCase* startedCase, runningCaseList.keys())
            {
            startedCase->UIEngineContainer().ResumeTest();
            }
    }

void StfQtUIModel::AbortCase()
    {
    foreach(const CStartedTestCase* startedCase, runningCaseList.keys())
            {
            startedCase->UIEngineContainer().CancelTest();
            }
    }

void StfQtUIModel::AddRunningCase(const CStartedTestCase* startedCase,
        const CSTFCase& stfCase)
    {
    runningCaseList.insert(startedCase, stfCase);
    FireOnRunningCaseChangedEvent();
    }

void StfQtUIModel::RemoveRunningCase(const CStartedTestCase* startedCase)
    {
    runningCaseList.remove(startedCase);
    FireOnRunningCaseChangedEvent();
    }

CSTFCase StfQtUIModel::GetRunningCase(const CStartedTestCase* startedCase)
    {
    return runningCaseList.value(startedCase);
    }

void StfQtUIModel::AddCaseByStatus(const TSTFCaseStatusType& type, const CSTFCase& aCase)
    {
    switch (type)
        {
        case EStatusRunning:
            break;
        case EStatusExecuted:
            executedCaseList.append(aCase);
            break;
        case EStatusPassed:
            passedCaseList.append(aCase);
            break;
        case EStatusFailed:
            failedCaseList.append(aCase);
            break;
        case EStatusAborted:
            abortCaseList.append(aCase);
            break;
        case EStatusCrashed:
            crashedCaseList.append(aCase);
            break;
        default:
            break;            
        }
    FireOnCaseStatisticChangedEvent();
    }

QList<CSTFCase> StfQtUIModel::GetCasesByStatus(const TSTFCaseStatusType& type)
    {
    switch (type)
        {
        case EStatusRunning:
            return runningCaseList.values();
        case EStatusExecuted:
            return executedCaseList;
        case EStatusPassed:
            return passedCaseList;
        case EStatusFailed:
            return failedCaseList;
        case EStatusAborted:
            return abortCaseList;
        case EStatusCrashed:
            return crashedCaseList;
        default:
            break;
        }
    QList<CSTFCase> list;
    return list;
    }

void StfQtUIModel::AddStifModelEventListener(
        IStifModelEventListener* listener)
    {
    if (!listenerList.contains(listener))
        {
        listenerList.append(listener);
        }
    }

void StfQtUIModel::RemoveStifModelEventListener(
        IStifModelEventListener* listener)
    {
    if (!listenerList.contains(listener))
        {
        listenerList.removeOne(listener);
        }
    }

void StfQtUIModel::FireOnCaseStatisticChangedEvent()
    {
    foreach(IStifModelEventListener* listener, listenerList)
            {
            listener->OnCaseStatisticChanged();
            }
    }

void StfQtUIModel::FireOnRunningCaseChangedEvent()
    {
    foreach(IStifModelEventListener* listener, listenerList)
            {
            listener->OnRunningCaseChanged();
            }
    }

void StfQtUIModel::ClearCasesStatus()
    {
    executedCaseList.clear();
    passedCaseList.clear();
    failedCaseList.clear();
    abortCaseList.clear();
    crashedCaseList.clear();
    FireOnCaseStatisticChangedEvent();
    }

// End of File
