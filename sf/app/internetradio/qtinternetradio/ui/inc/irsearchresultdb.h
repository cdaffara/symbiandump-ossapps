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

#ifndef IRSEARCHRESULTDB_H
#define IRSEARCHRESULTDB_H
 
#include "irqenums.h"

class IRQChannelItem;
class searchRltWrapper;

class IRSearchResultDB 
{	
public:
		
    IRSearchResultDB();     
	~IRSearchResultDB();
     
	//not take ownership
	IRQError cacheChannelList(QList<IRQChannelItem*> *aChannelList);
	//caller needs to release the memory
	QList<IRQChannelItem*> * getCahcedChannelList();	
	IRQError clearCache();

private:
    searchRltWrapper         *iSearchRltWrapper;
};
#endif//IRSEARCHRESULTDB_H
