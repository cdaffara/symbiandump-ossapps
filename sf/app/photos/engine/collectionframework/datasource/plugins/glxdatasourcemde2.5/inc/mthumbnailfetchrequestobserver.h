/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef M_THUMBNAILFETCHREQUESTOBSERVER_H
#define M_THUMBNAILFETCHREQUESTOBSERVER_H


class CGlxtnFileInfo;

// CLASS DECLARATION

class MThumbnailFetchRequestObserver
    {
public:
    virtual void ThumbnailFetchComplete(TInt aError, TBool aQuality) = 0;
    virtual void FetchFileInfoL() = 0;
    };

#endif  // M_THUMBNAILFETCHREQUESTOBSERVER_H

// End of File
