/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Panic codes for Provisioning accesspoint adapter
*
*/





#ifndef WPAPADAPTER_PAN
#define WPAPADAPTER_PAN

enum TWPAPAdapterPanicCodes
    {
	EDebugCode = 1000,
	EIllegalCharacteristic,
	EIllegalState,
	EIndexOutOfBounds,
	ENullPointer,
	EPreConditionFail,
	EPostConditionFail, 
	ESaveOperationAlreadyActive,
	ENoUniqueNameResolved
    };

GLDEF_C void Panic( TInt aPanic );

#endif