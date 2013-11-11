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
*              IStfQtUIController is interface of appliction controller.
*              IStfEventListener is interface of Stf view observer.
*
*/
#ifndef ISTFQTUICONTROLLER_H
#define ISTFQTUICONTROLLER_H
#include "cstfmodule.h"
#include "cstfcase.h"
#include <QList>
#include "istfqtuimodel.h"

class IStfEventListener
{
public:
    enum CaseOutputCommand
        {
        ECreate,
        EOutput,
        EClose
        };
public:
    virtual void OnGetMessage(const QString& aMessage) = 0;
    virtual void OnSetListChanged() = 0;
    virtual void OnCaseOutputChanged(const IStfEventListener::CaseOutputCommand& cmd, const QString& index, const QString& msg) = 0;
};

class IStfQtUIController
{
public:
    inline virtual ~IStfQtUIController(){}
    //for cases
    virtual bool OpenEngineIniFile(const QString& fileName) = 0;
    virtual QList<QString> GetModuleList() = 0;
    virtual QList<QString> GetCaseListByModule(const QString& moduleName) = 0;
    virtual CSTFCase GetCase(const QString& moduleName, const int index) = 0;
    virtual void RunCases(const QList<CSTFCase>& caseList, const TSTFCaseRunningType& type) = 0;
    virtual void RepeatRunCases(const QList<CSTFCase>& aCaseList, const bool aIsLoopInfinitely, const int aLoopTimes = 1) = 0;
    virtual bool AddCaseToSet(const QList<CSTFCase>& aCase, const QString& setName) = 0;
    
    //for set
    virtual QList<QString> GetSetList() = 0;
    virtual QList<QString> GetCaseListBySet(const QString& setName) = 0;
    virtual bool CreateSet(QString& setName) = 0;
    virtual bool DeleteSet(const QString& setName) = 0;
    virtual void RunSets(const QString& setName, const TSTFCaseRunningType& type) = 0;
    
    //for Started
    virtual void PauseCase() = 0;
    virtual void ResumeCase() = 0;
    virtual void AbortCase() = 0;
    virtual CSTFCase GetRunningCase(int index) = 0;
    virtual bool ShowOutput() = 0;
    virtual void SetShowOutput(bool isShow) = 0;
    
    //for staticstic
    virtual QList<CSTFCase> GetCasesByStatus(const TSTFCaseStatusType& type) = 0;
    
    //listener
    virtual void AddStfEventListener(IStfEventListener* listener) = 0;
    virtual void RemoveStfEventListener(IStfEventListener* listener) = 0;
    
    //for repeat execution setting
    virtual void InitRepeatSetting(const bool aIsLoopInfinitely, const int aLoopTimes) = 0;
    virtual void ResetRepeatSetting() = 0;

};



#endif // ISTFQTUICONTROLLER_H

// End of File
