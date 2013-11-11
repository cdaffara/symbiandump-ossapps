/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The interface
*
*/


#ifndef __AMINSTALLOPTIONSAPI_H__
#define __AMINSTALLOPTIONSAPI_H__

#include <SWInstDefs.h>
namespace NApplicationManagement
	{
	const int KMaxConRef = 256;
	struct TAMInstallOptions
		{
		TBuf8<KMaxConRef> iConRef;
		TInt iIAP;
		SwiUI::TInstallOptions iOptions;
		TAMInstallOptions() : iConRef( KNullDesC8()),
							  iIAP(-1), iOptions()
			{
			}
		};

	typedef TPckgBuf<TAMInstallOptions> TAMInstallOptionsPckgBuf;
	typedef TPckg<TAMInstallOptions> TAMInstallOptionsPckg;

	}
	

class InstallOptionsParser
	{
public:
	IMPORT_C static void ParseOptionsL( const TDesC8 &aBuffer, SwiUI::TInstallOptions &aResult);
	IMPORT_C static void SerializeOptionsL( const SwiUI::TInstallOptions &aOptions, CBufBase &aResult );
	IMPORT_C static void ParseOptionsL( const TDesC8 &aBuffer, SwiUI::TUninstallOptions &aResult);
	IMPORT_C static void SerializeOptionsL( const SwiUI::TUninstallOptions  &aOptions, CBufBase &aResult );
	
	IMPORT_C static void ParseOptionsL( const TDesC8 &aBuffer, NApplicationManagement::TAMInstallOptions &aResult);
	IMPORT_C static void SerializeOptionsL( const NApplicationManagement::TAMInstallOptions &aOptions, CBufBase &aResult );
	};    
	
    
#endif

//  End of File
