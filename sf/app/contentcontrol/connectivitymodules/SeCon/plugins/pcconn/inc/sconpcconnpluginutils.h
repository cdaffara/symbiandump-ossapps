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
* Description:  PC Connectivity Plug-in Utils header
*
*/


#ifndef _SCONPCCONNPLUGINUTILS_H
#define _SCONPCCONNPLUGINUTILS_H

#include <e32base.h>
//============================================================
// Class TPCConnpluginUtils declaration
//============================================================
class TPCConnpluginUtils
	{
	public:
		/**
		 * Converts Symbian error code to OBEX error code
		 * @param aError The error code
		 * @return The converted error code
		 */
		static TInt ConvertPCDResponseCode( TInt aError );
		/**
		 * Converts Symbian error code to OBEX error code
		 * @param aError The error code
		 * @return The converted error code
		 */
		static TInt ConvertFTPResponseCode( TInt aError );
		
		/**
		 * Converts Symbian error code to OBEX error code
		 * @param aError The error code
		 * @return The converted error code
		 */
		static TInt ConvertObexMoveResponseCode( TInt aError );
	};
	
#endif //_SCONPCCONNPLUGINUTILS_H

// End of file

