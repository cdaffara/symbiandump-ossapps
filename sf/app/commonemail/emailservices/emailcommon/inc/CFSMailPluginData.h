/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  plugin data container
*
*/


#ifndef CFSMAILPLUGINDATA_H
#define CFSMAILPLUGINDATA_H

//  INCLUDES
//<cmail>
#include "CFSMailPlugin.h"
//</cmail>

/**
 *  plugin data container, contains plugin instance pointer
 *  and corresponding plugin id
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailPluginData ) : public CBase
    {

public:

    /**
     * constructor.
     */
     CFSMailPluginData();
	
    /**
     * destructor.
     */  
     ~CFSMailPluginData();
	
  	/**
     * plugin uid
     */
     TUid              iPluginId;

  	/**
     * plugin instance pointer
     */
     CFSMailPlugin*    iPlugin;

    };
    
#endif  

// End of File
