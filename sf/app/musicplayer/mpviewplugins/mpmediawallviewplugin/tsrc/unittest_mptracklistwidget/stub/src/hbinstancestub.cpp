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
* Description: stub for hbinstance
*
*/

#include "stub/inc/hbinstancestub.h"

/*!
 Constructor
 */
HbInstanceStub::HbInstanceStub()
{    
    HbMainWindowStub *mainwindowStub = new HbMainWindowStub();
    mList.append( mainwindowStub );
}

/*!
 Destructor
 */
HbInstanceStub::~HbInstanceStub()
{
    mList.clear();
}

HbInstanceStub *HbInstanceStub::instance() 
{
    static HbInstanceStub hbInstanceStub;
    return &hbInstanceStub;    
}

QList<HbMainWindowStub *> HbInstanceStub::allMainWindows() const 
{
    return mList;    
}
