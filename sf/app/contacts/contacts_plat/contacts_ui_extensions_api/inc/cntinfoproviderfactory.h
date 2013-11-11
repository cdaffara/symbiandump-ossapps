/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Factory for info provider plugins to class CntListModel.
*
*/

#ifndef CNTINFOPROVIDERFACTORY_H
#define CNTINFOPROVIDERFACTORY_H

#include <QtPlugin>

class CntInfoProvider;

/*
   Factory for info provider plugins.
   
   Plugins must be exported to /resource/qt/plugins/contacts/infoproviders
 */
class CntInfoProviderFactory
{
public:
    virtual ~CntInfoProviderFactory() {}

    /* 
       CntInfoProvider implementation provided by this factory.

       /return the info provider implementation, ownership is given to the caller
     */
    virtual CntInfoProvider* infoProvider() = 0;

    virtual void* infoProviderFactoryExt( int /*aExtensionId*/ )
    {
        return NULL;
    }
};

Q_DECLARE_INTERFACE(CntInfoProviderFactory, "com.nokia.contacts.infoproviderfactory/1.0")

#endif
