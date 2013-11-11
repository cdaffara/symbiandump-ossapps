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

#ifndef _MRADIOENGINEHANDLEROBSERVER_H_
#define _MRADIOENGINEHANDLEROBSERVER_H_

// User includes
#include "mradioengineobserver.h"
#include "mradioaudioroutingobserver.h"
#include "mradiosystemeventobserver.h"

// Class declaration
NONSHARABLE_CLASS( MRadioEngineHandlerObserver ) : public MRadioEngineObserver
                                                 , public MRadioSystemEventObserver
{
public:

};

#endif // _MRADIOENGINEHANDLEROBSERVER_H_
