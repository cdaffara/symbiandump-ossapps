/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File Transfer Profile Plug-in Utils header
*
*/


#ifndef _SCONFTPPLUGINUTILS_H
#define _SCONFTPPLUGINUTILS_H

#include <e32base.h>
//============================================================
// Class TFTPpluginUtils declaration
//============================================================
class TFTPpluginUtils
	{
	public:
		/**
		 * Converts Symbian error code to OBEX error code
		 * @param aError The error code
		 * @return The converted error code
		 */
		static TInt ConvertFTPResponseCode( TInt aError );
	};

#endif // _SCONFTPPLUGINUTILS_H