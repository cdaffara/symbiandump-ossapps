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
* Description: LocationPickerService declaration
*
*/

#ifndef LOCATIONPICKERSERVICE_H
#define LOCATIONPICKERSERVICE_H

#include <xqserviceprovider.h>
#include "qlocationpickeritem.h"

class LocationPickerAppWindow;
/**
 *  LocationPickerService
 * 
 */	
class LocationPickerService : public XQServiceProvider
{
    Q_OBJECT

public:

    // constructor
    LocationPickerService( LocationPickerAppWindow *aParent = 0 );

    // destructor
    ~LocationPickerService();

    // item selection complete
    void complete( QLocationPickerItem aLm);
    
    // is active
    bool isActive();

public slots:
    //for QTHighway to notify provider about request
     void pick();
    
private:

    // handle to the application main window
    LocationPickerAppWindow* mServiceApp;
    int mAsyncReqId;
    QLocationPickerItem mReturn;
};

#endif // LOCATIONPICKERSERVICE_H
