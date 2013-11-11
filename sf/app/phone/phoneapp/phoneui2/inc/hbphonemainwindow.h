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
* Description:  Main window for PhoneUI
*
*/


#ifndef HBPHONEMAINWINDOW_H_
#define HBPHONEMAINWINDOW_H_

#include <QGraphicsItem>
#include <hbmainwindow.h>

class PhoneUiHouseHold;

class HbPhoneMainWindow: public HbMainWindow
    {
    Q_OBJECT
public:

    HbPhoneMainWindow(QWidget* parent = 0);
    ~HbPhoneMainWindow();
    
signals:
    void focusLost();
    void focusGained(); 
    
public slots:
    void onFocusLost();
    void onFocusGained();


private: // data
    PhoneUiHouseHold* mHouseHold;
    

    };



#endif /* HBPHONEMAINVIEW_H_ */
