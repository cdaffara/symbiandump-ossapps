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
*              IStfQtUIModel is interface of appliction data model.
*              IStifModelEventListener is used to listen event from data model.
*
*/
#ifndef ISTFQTUIMODEL_H
#define ISTFQTUIMODEL_H

#include "cstfcase.h"
#include "cstfmodule.h"
#include <UIStoreIf.h>
#include <UIStore.h>
#include <UIEngine.h>

class IStifModelEventListener
    {    
public:
    virtual void OnCaseStatisticChanged() = 0; 
    virtual void OnRunningCaseChanged() = 0;
    };

class IStfQtUIModel
{  
public:
    inline virtual ~IStfQtUIModel(){}
    virtual void ClearCasesStatus() = 0;
    virtual QList<CSTFCase> GetCasesByStatus(const TSTFCaseStatusType& type) = 0;
    virtual void AddRunningCase(const CStartedTestCase* startedCase, const CSTFCase& stfCase) = 0;
    virtual void RemoveRunningCase(const CStartedTestCase* startedCase) = 0;
    virtual void AddCaseByStatus(const TSTFCaseStatusType& type, const CSTFCase& aCase) = 0;
    virtual CSTFCase GetRunningCase(const CStartedTestCase* startedCase) = 0;
    virtual void PauseCase() = 0;
    virtual void ResumeCase() = 0;
    virtual void AbortCase() = 0;
    
    virtual void AddStifModelEventListener(IStifModelEventListener* listener) = 0;
    virtual void RemoveStifModelEventListener(IStifModelEventListener* listener) = 0;
    

};

#endif // ISTFQTUIMODEL_H

// End of File
