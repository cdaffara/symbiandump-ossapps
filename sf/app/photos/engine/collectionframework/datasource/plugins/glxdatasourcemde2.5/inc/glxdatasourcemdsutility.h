/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Standard proxy of the ECOM plugin
*
*/


#ifndef GLXDATASOURCEMDSUTILITY_H_
#define GLXDATASOURCEMDSUTILITY_H_

#include <mdccommon.h>
#include <glxmediaid.h>
#include <mdelogiccondition.h>

namespace NGlxDataSourceMdsUtility
	{
	/**
	 * Copies all the elements in an array of TGlxMediaIds to an array of TItemIds
	 * @param aDestArray destination array. It is the callers responsibility to
	 * ensure that aDestArray is on the cleanupstack prior to making the call
	 * to this method.
	 * @param aSrcArray source array.
	 */
	void CopyMediaIdArrayL(RArray<TItemId>& aDestArray, 
		                                  const RArray<TGlxMediaId>& aSrcArray);
	/**
	 * Copies all the elements in an array of TItemIds to an array of TGlxMediaIds
	 * @param aDestArray destination array. It is the callers responsibility to
	 * ensure that aDestArray is on the cleanupstack prior to making the call
	 * to this method.
	 * @param aSrcArray source array.
	 */
	void CopyItemIdArrayL(RArray<TGlxMediaId>& aDestArray,
			                                   const RArray<TItemId>& aSrcArray);
	
	void AddObjectConditionL(CMdELogicCondition& aLogicCondition,
			                                const RArray<TGlxMediaId>& aMediaIds);
	}

#endif /*GLXDATASOURCEMDSUTILITY_H_*/
