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
* Description: 
*
*/

#ifndef NMHSWIDGETDATETIMEOBSERVERPRIVATE_H
#define NMHSWIDGETDATETIMEOBSERVERPRIVATE_H

#include <qglobal.h>

class NmHsWidgetDateTimeObserver;
class CEnvironmentChangeNotifier;

class NmHsWidgetDateTimeObserverPrivate
{
public:
    explicit NmHsWidgetDateTimeObserverPrivate(NmHsWidgetDateTimeObserver *q = 0);
    ~NmHsWidgetDateTimeObserverPrivate();
    
private:
    TInt HandleLocaleChange();
    static TInt LocaleChangeCallback(TAny* aThisPtr);
    
private:
    Q_DISABLE_COPY(NmHsWidgetDateTimeObserverPrivate)

    NmHsWidgetDateTimeObserver *q_ptr;
    
    /*
     * Owned
     */
    CEnvironmentChangeNotifier* iLocaleNotifier;
};

#endif // NMHSWIDGETDATETIMEOBSERVERPRIVATE_H
