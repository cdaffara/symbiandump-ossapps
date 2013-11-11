/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Packet probe hook
*
*/

#ifndef __PROBE_FAMILY_H
#define __PROBE_FAMILY_H

const TUint16 KAfProbe			= 0x08b6;	//Dummy value

enum TProbePanic
{
	EProbePanic_BadBind,
	EProbePanic_NotSupported,
	EProbePanic_NoData
};

void Panic(TProbePanic aPanic);

#endif
