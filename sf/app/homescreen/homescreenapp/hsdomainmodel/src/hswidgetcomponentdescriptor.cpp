/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "hswidgetcomponentdescriptor.h"


/*!
    \class HsWidgetComponentDescriptor
    \ingroup group_hsdomainmodel
    \brief Homescreen widget component descriptor class.
    
    Homescreen widget component descriptor class hosts meta data of
    widget component. Meta data is delivered in manifest file.
    \see page_widgetsispackage
*/


bool HsWidgetComponentDescriptor::isValid()
{
    bool ret(false);
    if (mVersion == "1.0") {
        ret = !(mUri.isEmpty() || mServiceXml.isEmpty() || mTitle.isEmpty()); 
    } else if (mVersion.isEmpty()) {
        ret = !(mUri.isEmpty() || mTitle.isEmpty()); 
    }
    return ret;
}

