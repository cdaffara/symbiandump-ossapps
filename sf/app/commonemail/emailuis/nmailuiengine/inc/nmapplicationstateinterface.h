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
* Description:
*
*/

#ifndef NMAPPLICATIONSTATEINTERFACE_H_
#define NMAPPLICATIONSTATEINTERFACE_H_


/*!
    \class NmApplicationStateInterface
    \brief provides an interface to indicate application state information to protocol plugins    
*/
class NmApplicationStateInterface
{
public:
    virtual void updateActiveFolder(const NmId &mailboxId, const NmId &folderId) = 0;
};

Q_DECLARE_INTERFACE(NmApplicationStateInterface, "sf.app.commonmail.emailuis.nmailuiengine.NmApplicationStateInterface/1.0")


#endif /* NMAPPLICATIONSTATEINTERFACE_H_ */


