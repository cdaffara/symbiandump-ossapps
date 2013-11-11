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
* Description:  Phone application 
*
*/

#ifndef PHONEAPPLICATION_H_
#define PHONEAPPLICATION_H_

#include <HbApplication>

class QSymbianEvent;

class PhoneApplication : public HbApplication
{
    Q_OBJECT
public:
    PhoneApplication(
            int &argc, 
            char *argv[], 
            Hb::ApplicationFlags flags = Hb::DefaultApplicationFlags);
   

signals:
    void focusLost();    
    void focusGained();
    
protected:
#if defined(Q_OS_SYMBIAN)
    // This is required because focusInEvent(),
    // focusOutEvent(), or hideEvent() are not called
    // on the Symbian platform when the foreground application changes. 
    bool symbianEventFilter(const QSymbianEvent *event);
#endif

};
#endif /* PHONEAPPLICATION_H_ */
