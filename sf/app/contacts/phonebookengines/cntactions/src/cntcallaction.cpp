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

#include "cntcallaction.h"

#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>

//Action class
CntCallAction::CntCallAction() : 
    CntAction("call")
{
}

CntCallAction::~CntCallAction()
{
}

CntCallAction* CntCallAction::clone() const
{
	 return new CntCallAction();
}

void CntCallAction::performAction()
{
    QString service("com.nokia.symbian.ICallDial");
    QString type("dial(QString,int)");
    
    performNumberAction(service, type);
}



