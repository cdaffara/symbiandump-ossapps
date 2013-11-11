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
* Description:   Tag collection plugin definition
*
*/




#ifndef C_GLXTAGCOLLECTIONPLUGIN_H
#define C_GLXTAGCOLLECTIONPLUGIN_H

// INCLUDES
#include <glxcollectionpluginbase.h>			//Plugin Base Class
/**
 * @internal reviewed 14/06/2007 by Alex Birkett
 */

const TInt KGlxCollectionTagLevel = KGlxCollectionRootLevel + 1;        //Tag Collection Level
const TInt KGlxCollectionTagContentsLevel = KGlxCollectionRootLevel + 2;        //Tag contents Level
const TInt KGlxCollectionTagFSContentsLevel = KGlxCollectionRootLevel + 3;        //Tag FS contents Level



// CLASS DECLARATION
/**
* Tag collection plugin 
*/
NONSHARABLE_CLASS(CGlxCollectionPluginTags) : public CGlxCollectionPluginBase
    {
public: // Constructors and destructor
    /**
    * Two-phased constructor
    *
    * @param aObs observer
    * @return object of constructed
    */
    static CGlxCollectionPluginTags* NewL(TAny* aObs);

    /**
    * Destructor
    */
    ~CGlxCollectionPluginTags();

    private :    
    /**
    * Constructor
    * @param aObs MMPXCollectionPluginObserver instance
    */    
    CGlxCollectionPluginTags(MMPXCollectionPluginObserver* aObs);

    /**
    * ConstructL
    */  
    void ConstructL();

private:
    //From CGlxCollectionPluginBase

    void CpiAttributeAdditionalAttributes(const TMPXAttribute& aCpiAttribute
    , RArray<TMPXAttribute>& aAttributeArray);     	


    void HandleCpiAttributeResponseL(CMPXMedia* aResponse,
    TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> aMediaIds);
    TBool IsUpdateMessageIgnored(CMPXMessage& aMessage);

    void HandleCpiAttributeResponseL(CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, 
    TGlxMediaId aMediaId);

    TGlxFilterProperties DefaultFilter(TInt aLevel);

};

#endif  // C_GLXTAGCOLLECTIONPLUGIN_H

//End Of File
