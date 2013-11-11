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
* Description:  
*
*/


#ifndef CIMAGEPRINTSCHEDULER_H
#define CIMAGEPRINTSCHEDULER_H

#include <e32base.h>

class MProtIdleObserver;

class CImagePrintScheduler : public CActiveScheduler
	{
	public: // from CActiveScheduler
		void Error(TInt aError) const;
	public:
	
		/**
         *	Idle observer setter. Asynchronous errors (leaves) will be
         *	propagated to this observer
         *
         *	@param aIdleObserver idle observer
         */
		void SetIdleObserver( MProtIdleObserver& aIdleObserver );
		
		/**
         *	Current protocol id setter
         *
         *	@param aProtocol current protocol id
         */
		void SetCurrentProtocol( TInt aProtocol );
		
	private:
		MProtIdleObserver* iIdleObserver;
		TInt iCurrentProtocol;						
	};


#endif // CIMAGEPRINTSCHEDULER_H

//  End of File
