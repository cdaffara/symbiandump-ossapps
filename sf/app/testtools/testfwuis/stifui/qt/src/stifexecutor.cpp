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
 *              STIF UI interface and engine caller implementaion.
 *
 */
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>
#include <f32file.h>
#include <HAL.h>
#include <hal_data.h>
#include "stiflogger.h"
#include <QString>
#include "stifexecutor.h"
#include "StifTFwIf.h"

_LIT( KLogPath, "\\Logs\\STFUI\\" ); 
// Log file
_LIT( KLogFile, "StifUi.log" ); 

CStifExecutor::CStifExecutor() :
    listenerList(NULL)
    {
    iLog = CStifLogger::NewL( KLogPath, 
            KLogFile,
            CStifLogger::ETxt,
            CStifLogger::EFile,
            ETrue,
            ETrue,
            ETrue,
            EFalse,
            ETrue,
            EFalse,
            100 );


    iLog->Log(_L("started"));
    TInt result;
    TRAP(result, CUIStoreIf::ConstructL());
    iLog->Log(_L("CUIStoreIf ConstructL, result=%d"), result);
    if (result != KErrNone)
        {
        return;
        }
    TRAP(result, iBuffer = HBufC::NewL(500));
    iLog->Log(_L("Create Case Execution output buffer, result=%d"), result);

    }

CStifExecutor::~CStifExecutor()
    {
    UIStore().Close();
    delete iBuffer;
    if (listenerList)
        {
        delete listenerList;
        listenerList = NULL;
        }
    if(iLog)
        {
        iLog->Log(_L("finished"));
        }
    delete iLog;
    }

bool CStifExecutor::OpenIniFile(const QString& filename)
    {
    TInt result = UIStore().Open(QString2TPtrC(filename));
    iLog->Log(_L("Open ini file %s.result=%d"),QString2TPtrC(filename).Ptr(),result);
    return (result == KErrNone);
    }

TPtrC CStifExecutor::QString2TPtrC(const QString& aString)
    {
    TPtrC ret(reinterpret_cast<const TText*> (aString.constData()),
            aString.length());
    return ret;
    }
QString CStifExecutor::TDesC2QString(const TDesC& des)
    {
    //#ifdef QT_NO_UNICODE
    //return QString::fromLocal8Bit((char*)des.Ptr(), des.Length());
    //#else
    QString rst = QString::fromUtf16(des.Ptr(), des.Length());
    return rst;
    //#endif
    }

bool CStifExecutor::LogResult(const TInt result,const QString str)
    {
    QString tmp = str + " result=%d";
    iLog->Log(QString2TPtrC(tmp), result);
    bool rst = true;
    if(result != KErrNone)
        {
        rst = false;
        }
    return rst;
    }

void CStifExecutor::AddStifCaseUpdateListener(
        IStifCaseUpdateListener* listener)
    {
    iLog->Log(_L("AddStifCaseUpdateListener"));
    if (!listenerList)
        {
        listenerList = new QList<IStifCaseUpdateListener*> ();
        }
    if (!listenerList->contains(listener))
        {
        listenerList->append(listener);
        }
    }

void CStifExecutor::RemoveStifCaseUpdateListener(
        IStifCaseUpdateListener* listener)
    {
    iLog->Log(_L("RemoveStifCaseUpdateListener"));
    if (!listenerList)
        {
        return;
        }

    if (listenerList->contains(listener))
        {
        listenerList->removeOne(listener);
        }

    }

QList<CSTFModule> CStifExecutor::GetModuleList()
    {
    QList<CSTFModule> list;
    RRefArray<TDesC> modules;
    iLog->Log(_L("GetModuleList"));
    TInt ret = UIStore().Modules(modules);
    iLog->Log(_L("LoadAllModules %d"), ret);
    iLog->Log(_L("Modules number=%d"), modules.Count());
    for (TInt i = 0; i < modules.Count(); i++)
        {
        iLog->Log(_L("Get Module Names %d"), i);
        iLog->Log(_L("Get Module Name = %d .=%s"),i,modules[i].Ptr());
            
        CSTFModule module;
        module.SetName(QString::fromUtf16(modules[i].Ptr(),
                modules[i].Length()));
        //module.SetName(TDesC2QString(modules[i]));
        list.append(module);
        }
    modules.Reset();
    modules.Close();
    return list;
    }

QList<CSTFCase> CStifExecutor::GetCaseList(const QString& moduleName)
    {
    TPtrC name = QString2TPtrC(moduleName);
    QList<CSTFCase> list;
    RRefArray<CTestInfo> testCases;
    TInt ret = UIStore().TestCases(testCases, name, KNullDesC);
    iLog->Log(_L("Get TestCases: %d"), ret);
    for (TInt i = 0; i < testCases.Count(); i++)
        {
        iLog->Log(_L("Case Number: %d"),testCases[i].TestCaseNum());
        iLog->Log(_L("Case Name: %s"),testCases[i].TestCaseTitle().Ptr());
        CSTFCase testcase;
        testcase.SetName(TDesC2QString(testCases[i].TestCaseTitle()));
        testcase.SetIndex(i);
        list.append(testcase);
        }
    testCases.Reset();
    testCases.Close();
    return list;
    }

void CStifExecutor::ExecuteSingleCase(const QString& moduleName, const int caseIndex)
    {
    iLog->Log(_L("ExecuteCase start"));
    TPtrC name = QString2TPtrC(moduleName);
    RRefArray<CTestInfo> testCases;
    TInt ret = UIStore().TestCases(testCases, name, KNullDesC);
    iLog->Log(_L("Get TestCases return code=%d"), ret);
    if (testCases.Count() > caseIndex)
        {
        TInt index;
        UIStore().StartTestCase(testCases[caseIndex], index);
        iLog->Log(_L("start test case index=%d"), index);
        }
    testCases.Reset();
    testCases.Close();
    iLog->Log(_L("ExecuteCase end"));

    }

QList<QString> CStifExecutor::GetSetList()
    {
    QList<QString> list;
    RRefArray<TDesC> aArray;
    TInt ret = UIStore().GetTestSetsList(aArray);
    iLog->Log(_L("Get TestSet list return code=%d"), ret);
    if (ret != KErrNone) //setInfos.Count() != 1
        {
        return list;
        }
    for (int i = 0; i < aArray.Count(); i++)
        {
        list.append(TDesC2QString(aArray[i]));
        }
    aArray.Reset();
    aArray.Close();
    return list;
    }

QList<CSTFCase> CStifExecutor::GetCaseListFromSet(const QString& setName)
    {
    iLog->Log(_L("GetCaseListFromSet start."));
    QList<CSTFCase> list;
    TPtrC name = QString2TPtrC(setName);

    //iLog->Log(name);
    if (name.Length() == 0)
        {
        return list;
        }

    iLog->Log(_L("name.Length()=%d"), name.Length());
    TInt ret = UIStore().LoadTestSet(name);
    iLog->Log(_L("Load Test Set return=%d"),ret);
    const CTestSetInfo* set = NULL;
    TRAP(ret , set = &UIStore().TestSetL(name));
    iLog->Log(_L("GetCaseListFromSet TestSetL."));
    if(ret != KErrNone)
        {
        return list;
        }
    const RRefArray<const CTestInfo>& testCases = set->TestCases();
    iLog->Log(_L("GetCaseListFromSet TestCases."));
    TInt count = testCases.Count();
    for (TInt i = 0; i < count; i++)
        {
        CSTFCase testcase;
        testcase.SetName(TDesC2QString(testCases[i].TestCaseTitle()));
        testcase.SetIndex(testCases[i].TestCaseNum());
        testcase.SetModuleName(TDesC2QString(testCases[i].ModuleName()));
        list.append(testcase);
        }
    iLog->Log(_L("GetCaseListFromSet end."));
    return list;
    }

bool CStifExecutor::CreateSet(const QString& setName)
    {
    TPtrC name = QString2TPtrC(setName);
    TInt ret = UIStore().CreateTestSet(name);
    return LogResult(ret, "CreateSet");
    }

bool CStifExecutor::SaveSet(QString& setName)
    {
    TPtrC name = QString2TPtrC(setName);
    TFileName testSetName;
    testSetName.Copy(name);
    TInt ret = UIStore().SaveTestSet(testSetName);
    setName = TDesC2QString(testSetName);
    return LogResult(ret, "SaveSet");
    }

bool CStifExecutor::RemoveSet(const QString& setName)
    {
    //This method wil not work at this stage.
    TPtrC name = QString2TPtrC(setName);
    TInt ret = UIStore().RemoveTestSet(name);
    return LogResult(ret, "RemoveSet");
    }

bool CStifExecutor::AddtoSet(const QString& setName, CSTFCase& caseInfo)
    {
    iLog->Log(_L("AddToSet Start"));
        
    //IMPORT_C TInt AddToTestSet( const TDesC& aSetName, const CTestInfo& aTestInfo );
    TPtrC modulename = QString2TPtrC(caseInfo.ModuleName());
    iLog->Log(_L("AddToSet dealwith module: %s"), modulename.Ptr());
    iLog->Log(_L("Case name: %s"),QString2TPtrC(caseInfo.Name()).Ptr());
    iLog->Log(_L("Case index: %d"),caseInfo.Index());
    TInt caseIndex = caseInfo.Index();
    if(caseInfo.ModuleName().toLower() == "testscripter"
            ||caseInfo.ModuleName().toLower() == "teftestmodule")
        {
        caseIndex++;
        }
    RRefArray<CTestInfo> testCases;
    TInt ret = UIStore().TestCases(testCases, modulename, KNullDesC);
    if(!LogResult(ret, "AddToSet, GetTestCases"))
        {
        return false;
        }
    
    ret = -1;
    for (TInt i = 0; i < testCases.Count(); i++)
        {
        iLog->Log(_L("Case Number: %d"),testCases[i].TestCaseNum());
        iLog->Log(_L("Case Title: %s"),testCases[i].TestCaseTitle().Ptr());        
        
        if (testCases[i].TestCaseNum() == caseIndex)
            {
            ret = UIStore().AddToTestSet(QString2TPtrC(setName), testCases[i]);
            iLog->Log(_L("AddToTestSet: %d"), ret);
            break;
            }
        }
    testCases.Reset();
    testCases.Close();
    return LogResult(ret, "AddToSet");
    }

void CStifExecutor::ExecuteSet(const QString& SetName, const int startIndex,
        const TSTFCaseRunningType type)
    {
    CStartedTestSet::TSetType setType = CStartedTestSet::ESetSequential;
    if (type == Parallel)
        {
        setType = CStartedTestSet::ESetParallel;
        }
    const CTestSetInfo* set = NULL;
    TInt ret;
    TBuf<30> test;
    test.Append(QString2TPtrC(SetName));
    iLog->Log(_L("StartTestSet GetSetName:"));
    iLog->Log(test);
    TRAP(ret, set = &UIStore().TestSetL(test));
            
    //const CTestSetInfo& set = UIStore().TestSetL(QString2TPtrC(SetName));
    if(ret != KErrNone)
        {
        iLog->Log(_L("StartTestSet GetTestSet Error return=%d"),ret);
        return;
        }
    int a = startIndex;
    ret = UIStore().StartTestSet(*set, a, setType);
    iLog->Log(_L("StartTestSet return=%d"),ret);
    }

void CStifExecutor::Update(CStartedTestCase* aCase, int flags)
    {
    iLog->Log(_L("CStifExecutor::Update return case=%d"),aCase);
    iLog->Log(_L("CStifExecutor::Update return status=%d"),flags);
    
    if(aCase == NULL)
        {
        return;
        }

    if (flags & CUIStoreIf::EPrintUpdate)
        {
        //Cases output information update.
        const RPointerArray<CTestProgress> printArray = aCase->PrintArray();
        TInt rows = aCase->PrintArray().Count();
        TPtr buffer(iBuffer->Des());
        buffer.Zero();
        for (int i = 0; i < rows; i++)
            {
            buffer.Append(_L("\r\n"));
            buffer.Append(printArray[i]->iDescription);
            buffer.Append(_L(" "));
            buffer.Append(printArray[i]->iText);
            buffer.Append(_L("\r\n"));
            }
        QString msg = TDesC2QString(buffer);
        iLog->Log(_L("Get output msg:"));
        iLog->Log(buffer);
        if (listenerList)
            {
            for (int i = 0; i < listenerList->size(); i++)
                {
                listenerList->at(i)->OnGetCaseOutput(aCase, msg);
                }
            }

        }
    else
        {
        //case status changed update.
        CSTFCase testcase;
        testcase.SetName(TDesC2QString(aCase->TestInfo().TestCaseTitle()));
        testcase.SetIndex(aCase->TestInfo().TestCaseNum());
        testcase.SetModuleName(TDesC2QString(aCase->TestInfo().ModuleName()));
        if (listenerList)
            {
            for (int i = 0; i < listenerList->size(); i++)
                {
                listenerList->at(i)->OnGetCaseUpdated(aCase, testcase, flags);
                }
            }

        }

    }
	


// End of File
