/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CPrintJob class definition and the PJS enum.
*
*/


#include <e32std.h>

#include "cprintjob.h"

//**********************************************************************
//		CPrintJob
//**********************************************************************
CPrintJob::CPrintJob() : iState( PJSNone )
	{
	}

CPrintJob::~CPrintJob()
	{
	iImages.ResetAndDestroy();
	iImages.Close();
	iSettings.Close();
	}

//  End of File
