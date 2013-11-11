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
*
*/

#ifndef USSDEDITORQUERY_H
#define USSDEDITORQUERY_H

// INCLUDES
#include <hbinputdialog.h>

class UssdComms;

class UssdEditorQuery: public HbInputDialog
{
    Q_OBJECT

public:
    UssdEditorQuery( UssdComms &ussd, QGraphicsItem *parent = 0);
    ~UssdEditorQuery();

private slots:
    void sendUssdString();
    void cancelUssdString();
    void updateButtonVisible( const QString &text );
private:
    // From main.cpp
    UssdComms &mComms;
};

#endif // USSDEDITORQUERY_H

// End of file
