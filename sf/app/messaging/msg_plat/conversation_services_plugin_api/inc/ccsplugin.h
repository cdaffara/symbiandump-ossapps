/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECom conversation server interface definition
 *
*/


#ifndef CCS_PLUGIN_H
#define CCS_PLUGIN_H

//  INCLUDES
#include <mcsplugineventobserver.h>
#include <ecom/ecom.h>

/**
 * CCsPlugin
 * Conversation server plug-ins (Ecom) interface definition.
 *
 * @remarks Lib: It will part of different conversation ecom plugin lib
 * @since S60 v5.0
 */

class CCsPlugin: public CBase
    {

public: 

    /**
     * NewL.
     * Ecom interface static factory method implementation.
     * 
     * @param aImpUid Ecom's implementation uid
     * @param aMCsPluginEventObserver MCsPluginEventObserver object 
     *        which listens to plugin events
     * @return A pointer to the created instance of CCsPlugin
     */
    static inline CCsPlugin* NewL( TUid aImpUid,
            MCsPluginEventObserver* aMCsPluginEventObserver );

    /**
     * NewLC.
     * Ecom interface static factory method implementation.
     * 
     * @param aImpUid Ecom's implementation uid
     * @param aMCsPluginEventObserver MCsPluginEventObserver object 
     *        which listens to plugin events
     * @return A pointer to the created instance of CCsPlugin
     */
    static inline CCsPlugin* NewLC( TUid aImpUid,
            MCsPluginEventObserver* aMCsPluginEventObserver );

    /**
     * Destructor
     */
    virtual ~CCsPlugin();

public: 

    /**
     * PluginId
     * Gets the plugin id.
     * 
     * @return Id of the plugin.
     */   
    inline TUid PluginId() const;                  

    /**
     * GetConversations.
     * This function shall get all the conversation from plugin
     * 
     */
    virtual void GetConversationsL()= 0;

private:    

    /**
     * iPluginId
     * UID of plugin.
     */
    TUid iPluginId;

    /**
     * iDtor_ID_Key
     * UID required on cleanup.
     */
    TUid iDtor_ID_Key;

    };

#include "ccsplugin.inl"

#endif		// CCS_PLUGIN_H

// End of File

