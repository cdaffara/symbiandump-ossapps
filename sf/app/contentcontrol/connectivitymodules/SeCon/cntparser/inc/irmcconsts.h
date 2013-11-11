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
* Description:  Contact Parser constants
*
*/


#ifndef _IRMCCONSTS_H_
#define _IRMCCONSTS_H_

_LIT(KPhoneBookFilename,"c:\\IrMC\\pb.vcf");
const TInt KMaximumNumberOfContacts=-1; //-1 == all contacts are transferred
const TInt KPBNotUpdated=1;	//Must be grater than 0
const TInt KPerformComplete=2; //Must be grater than 0 and different than KPBNotUpdated

#endif
