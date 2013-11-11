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

#ifndef CNTFAVOURITE_H_
#define CNTFAVOURITE_H_

#include <qmobilityglobal.h>
#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
class QContact;
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

#define FavouriteGroupName "Favorites"

class CntFavourite
{
public:
    static QContactLocalId createFavouriteGroup( QContactManager* aManager );
    static QContactLocalId favouriteGroupId( QContactManager* aManager );
    static bool isMemberOfFavouriteGroup( QContactManager* aManager, QContact* aContact );
    static void addContactToFavouriteGroup( QContactManager* aManager, QContactId& aId );
    static void removeContactFromFavouriteGroup( QContactManager* aManager, QContactId& aId );

private:
    static int mCachedSymbianFavouriteId;
};

#endif /* CNTFAVOURITE_H_ */
