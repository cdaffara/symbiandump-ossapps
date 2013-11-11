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
* Description:  Content service.
*
*/


#ifndef FTU_RUNTIMESERVICES_CONTENTSERVICE_H
#define FTU_RUNTIMESERVICES_CONTENTSERVICE_H

#include "fturuntimeservices_global.h"

#include <QObject>
#include <QVariant>

class FtuContentServicePrivate;
class FtuWizard;


/**
 * @ingroup group_ftucontentmodel
 * @brief Content service.
 *
 * States use content service through an execution
 * context. Content service provides access to wizards.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FTURUNTIMESERVICES_EXPORT FtuContentService : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.
     * @param parent Parent object.
     */
    FtuContentService( QObject *parent = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    virtual ~FtuContentService();

private:

    Q_DISABLE_COPY(FtuContentService)

public:

    /**
     * Inserts wizard at the end of the list.
     * @since S60 ?S60_version.
     * @param wizard Wizard.
     */
    void appendWizard(FtuWizard *wizard);

    /**
     * Inserts wizard at the given index position in the list.
     * @since S60 ?S60_version.
     * @param index Index.
     * @param wizard Wizard.
     */
    void insertWizard(int index, FtuWizard *wizard);

    /**
     * Sets the active wizard.
     * @since S60 ?S60_version.
     * @param index Wizard index.
     */
    void setActiveWizard(int index);

    /**
     * Returns the active wizard.
     * @since S60 ?S60_version.
     * @return Wizard index.
     */
    int activeWizard() const;

    /**
     * Returns the wizard at index position aIndex in the list.
     * @since S60 ?S60_version.
     * @param index Index.
     * @return Wizard.
     */
    FtuWizard *wizard(int index) const;

    /**
     * Returns all wizards as list.
     * @since S60 ?S60_version.
     * @return Wizard.
     */
    QList<FtuWizard*> wizards() const;

signals:

    /**
     * Emitted when a wizard is added to the wizard list.
     * @since S60 ?S60_version.
     * @param index Wizard's index position in the list.
     */
    void wizardAdded(int index);

private:

    /**
     * Private implementation.
     */
    FtuContentServicePrivate* mFtuContentServicePrivate;

    friend class FtuContentServicePrivate;

};

Q_DECLARE_METATYPE(FtuContentService*)

#endif

