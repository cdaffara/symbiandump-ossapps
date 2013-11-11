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
* Description: QT C++ and Symbian C++ combination Class.
*              Call STIF UI engine and UI interface to perform test operation.
*              All the public methods are QT C++ based object.
*
*/
#ifndef STIFEXECUTOR_H_
#define STIFEXECUTOR_H_

#include "cstfcase.h"
#include "cstfmodule.h"
#include <UIStoreIf.h>
#include <UIStore.h>
#include <UIEngine.h>
#include <QList>
#include <StifLogger.h>

class IStifCaseUpdateListener
    {
public:
    virtual void OnGetCaseUpdated(CStartedTestCase* aCase,CSTFCase& stfcase, int flags) = 0;
    virtual void OnGetCaseOutput(CStartedTestCase* aCase, QString& msg) = 0;
    };


class CStifExecutor : public CUIStoreIf 
    {
public:
    CStifExecutor();
    ~CStifExecutor();
public: 
    void AddStifCaseUpdateListener(IStifCaseUpdateListener* listener);
    void RemoveStifCaseUpdateListener(IStifCaseUpdateListener* listener);
    bool OpenIniFile(const QString& filename);
    QList<CSTFModule> GetModuleList();
    QList<CSTFCase> GetCaseList(const QString& moduleName);
    void ExecuteSingleCase(const QString& moduleName, int caseIndex);
    QList<QString> GetSetList();
    QList<CSTFCase> GetCaseListFromSet(const QString& setName);
    bool CreateSet(const QString& setName);
    bool SaveSet(QString& setName);
    bool RemoveSet(const QString& setName);
    bool AddtoSet(const QString& setName, CSTFCase& caseInfo);
    void ExecuteSet(const QString& SetName, const int startIndex, const TSTFCaseRunningType type);
    
public://implement CUIStoreIf 
     void Update( CStartedTestCase* aTestCase,
                          TInt aFlags);
     
public:
     TPtrC QString2TPtrC(const QString& aString);
     QString TDesC2QString(const TDesC& des);
     //CTestInfo* GetTestInfo(CSTFCase aCase);
     
private:
     bool LogResult(const TInt result,const QString str);
private:
   QList<IStifCaseUpdateListener*> *listenerList;
   HBufC*  iBuffer;
   // Pointer to logger
   CStifLogger * iLog;
    };



#endif /* STIFEXECUTOR_H_ */

// End of File
