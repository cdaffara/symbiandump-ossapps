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
* Description:  Declaration of the wizard menu state.
*
*/


#ifndef FTU_STATES_WIZARDMENUSTATE_H
#define FTU_STATES_WIZARDMENUSTATE_H

#include <QAbstractItemModel>
#include <QState>
#include "ftutest_global.h"
#include <ftuwizard.h>

FTU_TEST_CLASS(FtuStateProviderTest)

class HbMainWindow;
class HbView;
class HbListView;
class HbLabel;
class HbDocumentLoader;
class QStandardItemModel;
class FtuContentService;
class QDate;
class FtuWizard;
class HbAction;
class FtuStateCenrepHandler;
class HbMenu;
class HbTranslator;
class HbGroupBox;

/** @ingroup group_ftustateprovider
 * @brief The state where wizard menu is shown
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FtuWizardMenuState : public QState
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    FtuWizardMenuState(QState *aParent = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
	virtual ~FtuWizardMenuState();
	

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
     * Creates table of contents widget.
     * @since S60 ?S60_version.
     */
    void createTocList();
	
	/**
     * Creates title text on a label.
     * @since S60 ?S60_version.
     */
    void createInfoText();
    	
	/**
     * Returns content service object.
     * @since S60 ?S60_version.
     */
    FtuContentService *content() const;
    /**
     * Gets the date as string.
     * @param date The date to convert as string.
     * @since S60 ?S60_version.
     */
    QString updatedAsString(const QDate& date) const;
    
public slots: 

    /**
     * Called when new wizards arrives.
     * @since S60 ?S60_version.
     * @param index Wizard index in content service.
     */
    void addWizardToListModel(int aIndex);

    /**
     * Signalled when user selects an item from the list.
     * @param index The index of the selected item.
     */    
    void activateWizard(const QModelIndex index);

    /**
     * @copydoc FtuWizard::progressUpdated()
     */    
    void updateProgress(FtuWizard *caller, bool show, int progress);
    
	/**
     * Signalled when user changes WizardSettings
     */
    void changeWizardSettings(FtuWizard *caller);
    
    /**
     * changes the streching style of the list
     */
    void changeListStrechingStyle(Qt::Orientation);    
signals:

    /**
     * Signalled when user selects an item from the list and the
     * plugin is activated.
     */    
     void wizardSelected();
	
private:

    HbMainWindow*                           mMainWindow;
    HbView*                                 mTocView;
    HbGroupBox*                             mInfoText;
    HbListView*                             mListView;    
    HbDocumentLoader*                       mDocumentLoader;

    QStandardItemModel*                     mModel;
    HbAction*                               mExitAction;

    FtuStateCenrepHandler*                  mCenrepHandler;
    QList<bool>                             mCompletedWizardList;
    HbTranslator                            *mTranslator;
    FTU_TEST_FRIEND_CLASS(FtuStateProviderTest)
};

#endif //FTU_STATES_WIZARDMENUSTATE_H
