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
* Description:  Content service private implementation.
*
*/


#ifndef FTU_RUNTIMESERVICES_CONTENTSERVICE_P_H
#define FTU_RUNTIMESERVICES_CONTENTSERVICE_P_H

#include <QObject>

class FtuWizard;
class FtuContentService;

/**
 * @ingroup group_ftucontentservice
 * @brief Content service private implementation.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FtuContentServicePrivate: public QObject
{
	Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.
     * @param contentService Content service.
     */
    FtuContentServicePrivate(FtuContentService *ftuContentService);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    virtual ~FtuContentServicePrivate();
    
private:

	Q_DISABLE_COPY(FtuContentServicePrivate) 

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
     * @return wizard index.
     */
    int activeWizard() const;

    /**
     * Returns the wizard at index position index in the list. 
     * @since S60 ?S60_version.
     * @param index Index.
     * @return Wizard.     
     */
    FtuWizard* wizard(int index) const;
    
    /**
     * Returns all wizards as list.  
     * @since S60 ?S60_version.
     * @return Wizards.     
     */
    QList<FtuWizard*> wizards() const;

public:

    /**
     * Public interface.
     */
    FtuContentService *mFtuContentService;

    /**
     * Wizard container.
     */
    QList<FtuWizard*> mWizardList;
    
    /**
     * Active wizard index.
     */
    int mActiveWizardIndex;

};

#endif
