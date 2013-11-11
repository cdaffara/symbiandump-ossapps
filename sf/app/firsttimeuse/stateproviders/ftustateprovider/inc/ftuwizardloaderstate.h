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


#ifndef FTU_STATES_WIZARDLOADERSTATE_H
#define FTU_STATES_WIZARDLOADERSTATE_H

#include "ftutest_global.h"
#include <QState>

FTU_TEST_CLASS(FtuStateProviderTest)

class FtuWizardProvider;
class FtuContentService;
class FtuWizard;


/** @ingroup group_ftustateprovider
 * @brief Initial state after first boot
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FtuWizardLoaderState : public QState
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    FtuWizardLoaderState(QState *aParent = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
	virtual ~FtuWizardLoaderState();
	
public slots:

    /**
     * Received when wizard is initialized.
     * @param wizard The caller object.
     * @param success True if succesful, false otherwise.
     */
    void addWizard(FtuWizard* wizard, bool success);
	
signals:

protected:

    /**
     * @copydoc QState::onEntry()
     */
    void onEntry(QEvent *event);
    
    /**
     * @copydoc QState::onExit()
     */
    void onExit(QEvent *event);

private:
    /**
     * Convenience method that returns the content service from dynamically
     * added meta object property.
     * @since S60 ?S60_version.
     * @return Content service.
     */
    FtuContentService *content() const;
private:
    /*
    * Wizard provider handle. Owned.    
    */
    FtuWizardProvider* mWizardProvider;

    FTU_TEST_FRIEND_CLASS(FtuStateProviderTest)
};

#endif //FTU_STATES_WIZARDLOADERSTATE_H
