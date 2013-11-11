/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of OOM memory plugin. Registers Photos UID with
*				 the framework to receive OOM notifications.
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <oommonitorplugin.h>

TAny* CreatePhotosOOMPluginL()
	{
	const TUid KGlxGalleryApplicationUid = { 0x20000A14 };
	return CAppOomMonitorPlugin::NewL(KGlxGalleryApplicationUid);
	}

const TImplementationProxy ImplementationTable[] =
	{
	{{0x200104E3}, ::CreatePhotosOOMPluginL}
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
	return ImplementationTable;
	}
