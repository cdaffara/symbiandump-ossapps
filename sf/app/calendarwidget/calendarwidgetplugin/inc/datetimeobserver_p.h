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
* Description: Calendar widget datetime observer
*
*/

#ifndef DATETIMEOBSERVERPRIVATE_H
#define DATETIMEOBSERVERPRIVATE_H

#include <e32base.h>
#include <e32property.h>
#include <qglobal.h>
#include <bacntf.h>

class DateTimeObserver;

/*!
  \class DateTimeObserverPrivate
  \brief Gets notifications about date changes from the Symbian fw and 
  forwards those to its public class.
*/

class DateTimeObserverPrivate
{
public:
    /*! 
        \fn DateTimeObserverPrivate::DateTimeObserverPrivate(DateTimeObserver *q = 0)
        
        Constructor of the DateTimeObserverPrivate class.
        @param q is the public class that will be notified of date changes.
     */
    explicit DateTimeObserverPrivate(DateTimeObserver *q = 0);
    /*! 
        \fn DateTimeObserverPrivate::~DateTimeObserverPrivate
        
        Destructor of the DateTimeObserverPrivate class.
     */
    ~DateTimeObserverPrivate();

private:
    /*! 
        \fn TInt DateTimeObserverPrivate::HandleLocaleChange()
        
        Called by LocaleChangeCallBack to handle the locale change.
        @return 1, if the locale change was handled, otherwise 0
     */
    TInt HandleLocaleChange();
    /*! 
        \fn TInt DateTimeObserverPrivate::LocaleChangeCallback(TAny* aThisPtr)
        
        Called by the framework when a locale change occurs.
        @param aThisPtr is the pointer to this object.
        @return 1, if the locale change was handled, otherwise 0
     */
    static TInt LocaleChangeCallback(TAny* aThisPtr);

private:
    Q_DISABLE_COPY(DateTimeObserverPrivate)

            DateTimeObserver *q_ptr;
    /*
     * Owned
     */
    CEnvironmentChangeNotifier* iLocaleNotifier;
};

#endif // DATETIMEOBSERVERPRIVATE_H
