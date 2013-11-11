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
* Description:  The implementation for presentation elements.
*
*/


#ifndef M_IRSESSIONOBSERVER_H
#define M_IRSESSIONOBSERVER_H

class MIRSessionObserver

    {
	
public:
    virtual void CollectSessionData() = 0;
    
    virtual void CollectPresetData( TInt aChannelType ) = 0;
    
    virtual void CollectBrowseLog( const TDesC& aBrowseUrl, TInt aStatus ) = 0;
    
    };

#endif //M_IRSESSIONOBSERVER_H
