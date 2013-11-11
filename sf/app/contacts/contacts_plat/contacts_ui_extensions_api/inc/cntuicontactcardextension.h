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

#ifndef CNTUICONTACTCARDEXT_H
#define CNTUICONTACTCARDEXT_H

#include <QObject>
#include <QIcon>
#include <QString>
#include <qcontact.h>
#include <qcontactdetail.h>

QTM_USE_NAMESPACE


/**
 * Represents one extension action.
 */
class CntUiContactCardExtAction
{
public:

    /// First line of text on the button.
    virtual QString firstTextLine() const = 0;

    /// Second line of text on the button.
    virtual QString secondTextLine() const = 0;

    /// Large icon that appears on the left.
    virtual const QIcon icon() const = 0;

    /**
     * The position of the action button is set based on detail type.
     * If the detail is empty (dummy detail) then the button appears at the end.
     */
    virtual const QContactDetail& detailUsedForChoosingPosition() const = 0;

protected:
    virtual ~CntUiContactCardExtAction() {}
};


/**
 * UI extension class for extending Contact Card and MyCard
 */
class CntUiContactCardExtension : public QObject
{
public:
    CntUiContactCardExtension() {}

    /**
     * Prepare the data that will be queried by other methods.
     * \param aContact The contact which is being displayed.
     * \param aMyCard Tells whether the contact is my contact.
     */
    virtual void prepare( QContact& aContact, bool aMyCard ) = 0;

    /**
     * Gives the action count.
     * The prepare() will be called before this.
     */
    virtual int actionCount() const = 0;

    /**
     * Gives an action.
     * The prepare() will be called before this.
     */
    virtual const CntUiContactCardExtAction& actionAt( int aIndex ) const = 0;


    /**
     * Allows the plugin to handle the short pressing.
     * \param aIndex The index of the action in the scope of this extension.
     * Should be implemented synchronously.
     */
    virtual void handleShortPress( int aIndex ) = 0;

// TODO Pending UX comments on this.
//    virtual void handleLongPress( int aIndex ) = 0;

public:
    /// For extending this interface later on without BC break.
    virtual void* ContactCardExtensionExtension(
            int /*aExtensionId*/ ) { return NULL; }

signals:

    /**
     * Plugin should re-implement this signal and emit it
     * once extension data changes.
     * Note that an action should not disappear. It can change, but not disappear.
     */  
    void contactCardExtensionDataChanged( QList<int> aChangedActionIndexes );

protected:
    // prevent deleting by client
    virtual ~CntUiContactCardExtension() {}
};

#endif //CNTUICONTACTCARDEXT_H

