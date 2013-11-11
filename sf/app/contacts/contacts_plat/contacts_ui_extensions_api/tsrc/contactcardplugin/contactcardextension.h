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
 * Description: Example implementation of contact card extension.
 *
 */

#ifndef CONTACTCARDEXTENSION_H
#define CONTACTCARDEXTENSION_H

#include <QObject>
#include <QtGlobal>

#include <cntuicontactcardextension.h>
#include <qmobilityglobal.h>
#include <qcontactmanager.h>
//#include <qcontactaction.h>


class QtMobility::QContactManager;


/**
 * Example implementation of contact card extension.
 */
class ContactCardExtension : public CntUiContactCardExtension, public CntUiContactCardExtAction
{
    Q_OBJECT
public:
    ContactCardExtension();
    ~ContactCardExtension();

public: // from CntUiContactCardExtension

    void prepare( QContact& aContact, bool aMyCard );

    int actionCount() const;

    const CntUiContactCardExtAction& actionAt( int aIndex ) const;

    /// synchronous
    void handleShortPress( int aIndex );

public: // from CntUiContactCardExtAction
        // Note that it can be a different class that is made to inherit from this interface.

    QString firstTextLine() const;

    /// Second line of text on the button.
    QString secondTextLine() const;

    const QIcon icon() const;

    /// Currently, the item must map to some detail. TODO Rethink this.
    const QContactDetail& detailUsedForChoosingPosition() const;


signals: // from CntUiContactCardExtension
    void contactCardExtensionDataChanged();

private: // data
    QContactDetail* mDetail;
};

#endif // CONTACTCARDEXTENSION_H
