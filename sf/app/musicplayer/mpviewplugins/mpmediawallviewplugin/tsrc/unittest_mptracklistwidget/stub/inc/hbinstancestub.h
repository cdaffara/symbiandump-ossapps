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

#ifndef HBINSTANCESTUB_H_
#define HBINSTANCESTUB_H_

#include "stub/inc/hbmainwindowstub.h"

#define hbInstance (HbInstanceStub::instance())

class HbInstanceStub
{
public:
    static HbInstanceStub *instance();

    QList<HbMainWindowStub *> allMainWindows() const;
  
private:
    HbInstanceStub();
    ~HbInstanceStub();
    
public:
    QList<HbMainWindowStub *> mList;
};


#endif /* HBINSTANCESTUB_H_ */
