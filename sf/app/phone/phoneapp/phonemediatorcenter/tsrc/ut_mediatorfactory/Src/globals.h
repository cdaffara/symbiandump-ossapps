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
#ifndef GLOBALS_H
#define GLOBALS_H

#include <e32std.h>
#include <MediatorCommandResponder.h>

class Singleton : public CBase
	{
	public:
		static Singleton* Instance();
		~Singleton();

	protected:
		Singleton();
		Singleton(const Singleton&);
		Singleton& operator=(const Singleton&);

	private:
		
	public:
        static Singleton* pinstance;
        TBool iLeave;
        TBool iCreateMessageLeave;
        TInt iMessageObject;
        TInt iSenderObject;
	};
    
#endif      // GLOBALS_H
    
// End of File
