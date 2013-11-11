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
* Description:  Contains the TMapping class definition.
*
*/


#ifndef TMAPPING_H
#define TMAPPING_H

#include <e32base.h>

class TMapping 
	{ 
		public:
			TMapping(TInt aX, TInt aY)
			{
				iImagesPerPage = aX; 
				iCode = aY; 				
			}
			TInt iCode; 
			TInt iImagesPerPage;			
	};

#endif // TMAPPING_H

//  End of File
