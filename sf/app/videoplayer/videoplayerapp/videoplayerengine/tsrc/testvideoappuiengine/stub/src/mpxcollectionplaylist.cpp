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
* Description:  mpx collection playlist class stub for unit testing
*
*/

// Version : %version: 1 %

#include "mpxcollectionplaylist.h"

CMPXCollectionPlaylist::CMPXCollectionPlaylist()
{    
}

CMPXCollectionPlaylist::~CMPXCollectionPlaylist()
{
}	

CMPXCollectionPlaylist* CMPXCollectionPlaylist::NewL(const CMPXCollectionPath& /*aPath*/)
{
		CMPXCollectionPlaylist* p = new CMPXCollectionPlaylist();
    return 	p;
}

void CMPXCollectionPlaylist::SetSingleItemPlaylist()
{
}
        
void CMPXCollectionPlaylist::SetToFirst()
{
}
        
void CMPXCollectionPlaylist::SetAutoPlay(TBool /*aAutoPlay*/)
{
}

void CMPXCollectionPlaylist::SetPreInitPlugin(TBool /*aPreInit*/)
{
}
