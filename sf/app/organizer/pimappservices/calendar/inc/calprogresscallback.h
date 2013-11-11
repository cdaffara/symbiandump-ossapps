// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CALPROGRESSCALLBACK_H__
#define __CALPROGRESSCALLBACK_H__

#include <e32def.h>

class MCalProgressCallBack
/** A call back class to show the progress of long-running operations. 

When a long-running operation is carried out on the instance view or the entry 
view, this class is used to signal its progress, and when the function is complete.

@publishedAll
@released
*/
	{
public:

	/** Progress callback.
	
	This calls the observing class with the percentage complete of the current operation.
	
	@param aPercentageCompleted The percentage complete. */
	virtual void Progress(TInt aPercentageCompleted) = 0;
	
	/** Progress callback.
	
	This calls the observing class when the current operation is finished.
	
	@param aError The error if the operation failed, or KErrNone if successful. */
	virtual void Completed(TInt aError) = 0;
	
	/** 
    Asks the observing class whether progress callbacks are required. This
    is called once only to check if callbacks are enabled.
	
	@return If the observing class returns EFalse, then the Progress() function will not be called. 
    */
	virtual TBool NotifyProgress() = 0;
	};
	
#endif // __CALPROGRESSCALLBACK_H__
