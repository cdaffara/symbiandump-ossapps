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
* Description:
*
*/

#include "testrunner.h"

#include "mt_cntactions.h"

#include <QtTest/QtTest>

int main(int argc, char *argv[]) 
{ 
    QApplication app(argc, argv);
    
    TestRunner testRunner("mt_cntactions");
    
    TestCntActions cntActionsTest;
    testRunner.runTests( cntActionsTest );
    
    testRunner.printResults();
    return 0;   
}

