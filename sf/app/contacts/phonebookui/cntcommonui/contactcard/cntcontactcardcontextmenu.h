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

#ifndef CNTCONTACTCARDCONTEXTMENU_H_
#define CNTCONTACTCARDCONTEXTMENU_H_

#include <hbmenu.h>

class CntContactCardDetailItem;
class CntContactCardContextMenu : public HbMenu
{
public:
    CntContactCardContextMenu( CntContactCardDetailItem* aItem );
    ~CntContactCardContextMenu();
    
    CntContactCardDetailItem* item();
private:
    CntContactCardDetailItem* mItem;
};
#endif /* CNTCONTACTCARDCONTEXTMENU_H_ */
