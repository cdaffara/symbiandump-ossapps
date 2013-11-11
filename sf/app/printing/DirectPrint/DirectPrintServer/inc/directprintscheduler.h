/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


// Protection against nested includes
#ifndef CDIRECTPRINTSCHEDULER_H
#define CDIRECTPRINTSCHEDULER_H

// System include files
#include <e32base.h>

class MProtIdleObserver;

class CDirectPrintScheduler : public CActiveScheduler
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


#endif // CDIRECTPRINTSCHEDULER_H

//  End of File
