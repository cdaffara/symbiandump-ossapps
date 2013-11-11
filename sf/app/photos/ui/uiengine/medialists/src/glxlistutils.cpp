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
* Description:    List utilities
*
*/



#include <glxtracer.h>                 //For logging
#include <featmgr.h>
#include <bldvariant.hrh>   // For feature constants

#include "glxlistutils.h"

// -----------------------------------------------------------------------------
// NormalizedIndex
// -----------------------------------------------------------------------------
//
TInt GlxListUtils::NormalizedIndex( TInt aIndex, TInt aListLength ) 
	{
	TRACER("GlxListUtils::NormalizedIndex");
	
	if ( 0 == aListLength ) 
	    {
	    return KErrNotFound;
	    }
	    
	TInt index = aIndex % aListLength;
    if( index < 0 )
        {
        // sign for modulo is the same as left side
        // so need to add aListLength
        index += aListLength;
        }
        
	return index;
	}		

// -----------------------------------------------------------------------------
// VisibleItemsGranularityL
// -----------------------------------------------------------------------------
//
TInt GlxListUtils::VisibleItemsGranularityL()
    {
    TRACER("GlxListUtils::DefaultVisibleItemsGranularityL");
    
    //TBD: In MCL 9.1/Corolla env currently the feature flags 
    //KFeatureIdLayout640_480 and KFeatureIdLayout480_640 are not defined
    //so making the default granularity as 21 instead of 15.
    //Need to replace with KGlxDefaultVisibleItemsGranularity after
    //getting the feature flag.
    TInt ret = KGlxVGAVisibleItemsGranularity;
//  for 10.1 we need it to be 21,
    /*FeatureManager::InitializeLibL();
    
    if (FeatureManager::FeatureSupported( KFeatureIdLayout640_360_Touch ) || 
        FeatureManager::FeatureSupported( KFeatureIdLayout360_640_Touch ))
      {
      ret = KGlxQHDVisibleItemsGranularity;
      }
    else if(FeatureManager::FeatureSupported(KFeatureIdLayout640_480_Touch) || 
          FeatureManager::FeatureSupported(KFeatureIdLayout480_640_Touch) || 
          FeatureManager::FeatureSupported(KFeatureIdLayout640_480) || 
          FeatureManager::FeatureSupported(KFeatureIdLayout480_640))
      {
      ret = KGlxVGAVisibleItemsGranularity;
      }
    FeatureManager::UnInitializeLib(); */
    return ret;
    }
