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
* Description:   This class browses file system
*
*/




#ifndef C_GLXCOLLECTIONPLUGINALL_H
#define C_GLXCOLLECTIONPLUGINALL_H

// INCLUDES
#include <e32cmn.h>
#include "glxcollectionpluginbase.h"

/**
 * @internal reviewed 13/06/2007 by Alex Birkett
 */

// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxCollectionPluginAll class 
 *
 *  Plug-in basically provides browsing.
 *	@ingroup collection_component_design
 */
NONSHARABLE_CLASS(CGlxCollectionPluginAll) : public CGlxCollectionPluginBase
    {
public: // Constructors and destructor
	/**
    * Two-phased constructor
    *
    * @param aObs observer
    * @return object of constructed
    */
    static CGlxCollectionPluginAll* NewL(TAny* aObs);

    /**
    * Destructor
    */
    ~CGlxCollectionPluginAll();
    
private:
	/**
	 * Checks to see if any additional attributes are required to construct required
	 * CPI specific attribute
	 *
	 * @param aCpiAttribute CPI specific attribute to be constructed later
	 * @param aAttributeArray modifiable list of attributes to be retrieved from data source
	 */	 
	void CpiAttributeAdditionalAttributes(const TMPXAttribute& aCpiAttribute, RArray<TMPXAttribute>& aAttributeArray);     	

	/**
	 * Modifies the response to include all requested CPI specific attributes or Leaves.
	 *
	 * @param aResponse Data Source constructed response to which CPI specific attributes should be added (also contains any required data)
	 * @param aCpiAttributes list of CPI specific attributes to be constructed
	 */	 
	void HandleCpiAttributeResponseL(CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> aMediaIds);

	TBool IsUpdateMessageIgnoredL(CMPXMessage& aMessage);
	
private: 
    /**
    * Constructor
    * @param aObs MMPXCollectionPluginObserver instance
    */
    CGlxCollectionPluginAll(MMPXCollectionPluginObserver* aObs);
    /**
    * 2nd stage Constructor
    */
    void ConstructL();
    };

#endif      // C_GLXCOLLECTIONPLUGINALL_H

