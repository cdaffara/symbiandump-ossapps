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
* Description: Orbit stubs for Live Comms UI unit tests
*
*/
#ifndef HBLINEEDIT_H
#define HBLINEEDIT_H

#include <QMetaType>


class HbLineEdit
{
public:
    HbLineEdit();
    HbLineEdit(const QString &lineEditText);
    ~HbLineEdit();
    
    void setText(const QString &text);
    void paste();
    void copy();
    void selectAll();
    void setSelection(int start, int length);
    QString text() const;

    
public: // Data added for the unit testing stub
    QString mName;
    QString mText;
    
};

Q_DECLARE_METATYPE(HbLineEdit)

#endif // HBLINEEDIT_H
