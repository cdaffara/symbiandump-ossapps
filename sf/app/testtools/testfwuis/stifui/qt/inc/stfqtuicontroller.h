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
*              Application's controller.
*
*/
#ifndef STFQTUICONTROLLER_H_
#define STFQTUICONTROLLER_H_

#include "istfqtuicontroller.h"
#include "istfqtuimodel.h"
#include "stifexecutor.h"

class StfQtUIController : public IStfQtUIController, public IStifCaseUpdateListener
    {
public:
    StfQtUIController(IStfQtUIModel* aModel);
    ~StfQtUIController();
public: //Implement IStfQtUIController.
    //for cases

    bool OpenEngineIniFile(const QString& fileName);
    QList<QString> GetModuleList();
    QList<QString> GetCaseListByModule(const QString& moduleName);
    CSTFCase GetCase(const QString& moduleName, const int index);
    void RunCases(const QList<CSTFCase>& caseList, const TSTFCaseRunningType& type);
    void RepeatRunCases(const QList<CSTFCase>& aCaseList, const bool aIsLoopInfinitely, const int aLoopTimes = 1);
    bool AddCaseToSet(const QList<CSTFCase>& aCase, const QString& setName);
    
    //for set
    QList<QString> GetSetList();
    QList<QString> GetCaseListBySet(const QString& setName);
    bool CreateSet(QString& setName);
    bool DeleteSet(const QString& setName);
    void RunSets(const QString& setName, const TSTFCaseRunningType& type);
    
    //for Started
    void PauseCase();
    void ResumeCase();
    void AbortCase();
    bool ShowOutput();
    CSTFCase GetRunningCase(int index);
    void SetShowOutput(bool isShow);
    
    //for staticstic
    QList<CSTFCase> GetCasesByStatus(const TSTFCaseStatusType& type);
    
    //listener
    void AddStfEventListener(IStfEventListener* listener);
    void RemoveStfEventListener(IStfEventListener* listener);
    
    //for repeat execution setting
    void InitRepeatSetting(const bool aIsLoopInfinitely, const int aLoopTimes);
    void ResetRepeatSetting();

public://implement IStifCaseUpdateListener
    void OnGetCaseUpdated(CStartedTestCase* aCase, CSTFCase& stfcase, int flags);
    void OnGetCaseOutput(CStartedTestCase* aCase, QString& msg);
    
private: //Fire event
    void FireOnGetOutput(QString message);
    void FireOnSetListChanged();
    void FireOnCaseOutputChanged(IStfEventListener::CaseOutputCommand cmd, int index, QString msg);
    
private:
    // help methods
    CSTFModule GetModuleByName(const QString& moduleName);
    // Repeat execution cases
    void Execution();
    
private:
    CStifExecutor* executor;
    IStfQtUIModel* model;
    QList<IStfEventListener*> listenerList;
    bool isShowOutput;
    
    // repeat run case related member 
    // repeat execution case list   
    QList<CSTFCase> repeatRunCaseList;
    // current position of the case list
    int iCurrentRunPos;
    // is loop infinitely
    bool isLoopInfinitely;
    // loop times
    int loopTimes;
    
    };




#endif /* STFQTUICONTROLLER_H_ */

// End of File
