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
* Description:   declaration of Calendar regional info 
 *
*/

#ifndef CALENREGIONALINFODATA_H_
#define CALENREGIONALINFODATA_H_

//system includ//
#include <QObject>


class CCalenLunarChinesePlugin;
/**
 * CCalenRegionalInfo is used for
 * slot implementation for showing regional data .
 */
class  CalenRegionalInfo : public QObject
    {
    Q_OBJECT
    
    public:
        CalenRegionalInfo(CCalenLunarChinesePlugin &plugin);
        ~CalenRegionalInfo();

    public slots: 
       //slot to show reginal info
        void showRegionalDetails();
    
    private :
        CCalenLunarChinesePlugin &mPlugin ;
    };

#endif /* CALENREGIONALINFODATA_H_ */
