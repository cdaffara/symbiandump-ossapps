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

#ifndef NMUIEXTENSIONINTERFACE_H_
#define NMUIEXTENSIONINTERFACE_H_

#include <QList>
#include <QtPlugin>
#include "nmcommon.h"

class NmActionRequest;
class NmAction;

/*!
    \class NmUiExtensionInterface
    \brief Interface class for ui extension class
 */
class NmUiExtensionInterface
{
public:

    virtual ~NmUiExtensionInterface() {};
    virtual void getActions(
            const NmActionRequest &request,
            QList<NmAction*> &actionList) = 0;

};

Q_DECLARE_INTERFACE(NmUiExtensionInterface, "sf.app.commonmail.emailuis.nmailui.NmUiExtensionInterface/1.0")

#endif /* NMUIEXTENSIONINTERFACE_H_ */
