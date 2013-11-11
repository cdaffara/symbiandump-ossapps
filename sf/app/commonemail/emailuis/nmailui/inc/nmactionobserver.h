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

#ifndef NMACTIONROBSERVER_H_
#define NMACTIONROBSERVER_H_

class NmActionResponse;

/*!
    \class NmActionObserver
    \brief Abstract base class for action observer. View needs
    \to derive from this class when requesting action (menu/toolbar) commands
    \from extensions. Action response object ownership is passed to the view
*/
class NmActionObserver
{
public:
    virtual void handleActionCommand(NmActionResponse &actionResponse) = 0;
};

#endif  // NMACTIONROBSERVER_H_

// End of File
