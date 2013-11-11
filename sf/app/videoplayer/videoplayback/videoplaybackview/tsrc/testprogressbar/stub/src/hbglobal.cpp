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
* Description:  class for HbGlobal
*
*/

// Version : %version:  1 %



#include "hbglobal.h"
#include <qstring>


QString hbTrId( QString string, int n )
{
    Q_UNUSED( n );

    QString loc = "";


    if ( string == "txt_videos_slidervalue_l1l2l3_2" )
    {
        loc = "%L1:%L2:%L3";
    }
    else if ( string == "txt_videos_slidervalue_l1l2" )
    {
        loc = "%L1:%L2";
    }
    return loc;
}

//End of file
