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

#include "cntvideocallaction.h"

#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>

//Action class
CntVideoCallAction::CntVideoCallAction() : 
    CntAction("videocall")
{
}

CntVideoCallAction::~CntVideoCallAction()
{
}

CntVideoCallAction* CntVideoCallAction::clone() const
{
	 return new CntVideoCallAction();
}

void CntVideoCallAction::performAction()
{
    QString service("com.nokia.symbian.ICallDial");
    QString type("dialVideo(QString,int)");
    
    performNumberAction(service, type);
}



