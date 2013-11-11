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

#ifndef HBCNTINTERFACE_H
#define HBCNTINTERFACE_H

#include <QtPlugin>

class HbMainWindow;
class HbWidget;
class HbAction;

class CntMenuInterface
{
public:
    virtual ~CntMenuInterface() {}

    //maybe not necessary to past HbCntBaseView, would HbMenu be enough?
    virtual QList<HbAction*> createActions() = 0;
};

class CntViewInterface
{
public:
    virtual ~CntViewInterface() {}

    //would this be enough?
    virtual QList<HbWidget*> createViews() = 0;
};

Q_DECLARE_INTERFACE(CntMenuInterface,
                    "com.nokia.contacts.HbCntMenuInterface/1.0")
Q_DECLARE_INTERFACE(CntViewInterface,
                    "com.nokia.contacts.HbCntViewInterface/1.0")

#endif /* HBCNTINTERFACE_H */
