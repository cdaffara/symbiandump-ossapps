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
*              application data model.
*
*/
#include "istfqtuimodel.h"
#include "stifexecutor.h"
#include <QHash>

class StfQtUIModel : public IStfQtUIModel
    {
public:
    StfQtUIModel();
    virtual ~StfQtUIModel();
public://implement IStfQtUIModel
    void ClearCasesStatus();
    QList<CSTFCase> GetCasesByStatus(const TSTFCaseStatusType& type);
    void AddRunningCase(const CStartedTestCase* startedCase, const CSTFCase& stfCase);
    void RemoveRunningCase(const CStartedTestCase* startedCase);
    void AddCaseByStatus(const TSTFCaseStatusType& type, const CSTFCase& aCase);
    void AddStifModelEventListener(IStifModelEventListener* listener);
    void RemoveStifModelEventListener(IStifModelEventListener* listener);
    CSTFCase GetRunningCase(const CStartedTestCase* startedCase);
    void PauseCase();
    void ResumeCase();
    void AbortCase();
    
private:
    void FireOnCaseStatisticChangedEvent();
    void FireOnRunningCaseChangedEvent();
    
private:
    QList<IStifModelEventListener*> listenerList;
    QList<CSTFCase> executedCaseList;
    QList<CSTFCase> passedCaseList;
    QList<CSTFCase> failedCaseList;
    QList<CSTFCase> abortCaseList;
    QList<CSTFCase> crashedCaseList;
    QHash<const CStartedTestCase*, CSTFCase> runningCaseList;
    
    
    
    
    
    
    };

// End of File
