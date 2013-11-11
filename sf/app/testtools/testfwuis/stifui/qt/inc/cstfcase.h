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
*              CSTFCase used to describe a test case.
*
*/
#ifndef CSTFCASE_H
#define CSTFCASE_H
#include "QString"

enum TSTFCaseRunningType
    {
    Sequentially = 0,
    Parallel    
    };

enum TSTFCaseStatusType
    {
    EStatusRunning  = 0x00000001,
    EStatusExecuted = 0x00000002,
    EStatusPassed   = 0x00000004,
    EStatusFailed   = 0x00000008,
    EStatusAborted  = 0x00000010,
    EStatusCrashed  = 0x00000020,
    EStatusAll      = 0x000000ff,    
    };

class CSTFCase
{
public:
    CSTFCase()    {isActive = true; caseIndex=-1;}
    CSTFCase(QString name, int index)    {caseName = name; caseIndex = index; isActive = true;}

public:
    QString& Name(){return caseName;}
    int Index(){return caseIndex;}
    bool IsActive(){return isActive;}
    QString& ModuleName() {return moduleName;}

public:
    void SetName(const QString name){caseName = name; }
    void SetIndex(const int index){caseIndex = index;}
    void SetActive(const bool active) {isActive = active;}
    void SetModuleName(const QString name){moduleName = name;}

private:
    QString caseName;
    int caseIndex;
    bool isActive;
    QString moduleName;
};
#endif // CSTFCASE_H

// End of File
