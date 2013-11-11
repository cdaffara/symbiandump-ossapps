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


#ifndef FTU_STATES_WIZARDACTIVATEDSTATE_H
#define FTU_STATES_WIZARDACTIVATEDSTATE_H

#include <QState>
#include <QAbstractItemModel>
#include "ftutest_global.h"

FTU_TEST_CLASS(FtuStateProviderTest)

class QGraphicsWidget;
class HbMainWindow;
class HbLabel;
class HbStackedWidget;
class HbView;
class HbDocumentLoader;
class HbAction;
class HbGridView;
class QGraphicsLinearLayout;
class FtuContentService;
class FtuWizard;
class HbMenu;
class HbTranslator;
 
/** @ingroup group_ftustateprovider
 * @brief The state handles wizard activation and interaction.
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FtuWizardActivatedState : public QState
{
    Q_OBJECT
    
public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    FtuWizardActivatedState(QState *aParent = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
	virtual ~FtuWizardActivatedState();
	
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
     * Returns content service object.
     * @return The content service pointer.
     * @since S60 ?S60_version.
     */
    FtuContentService *content() const;
    
    /**
     * Returns the main window handle.
     * @since S60 ?S60_version.
     * @return The pointer to the main window.
     */
    HbMainWindow* mainWindow();
    
    /**
     * Connects signals and slots for active wizard.
     * @since S60 ?S60_version.
     */  
    void setActiveWizardConnections();
        
    /**
     * Calculates the wizard's geometry.
     * @since S60 ?S60_version.
     * @return The calculated rect.
     */       
    QRectF calculateWizardGeometry();
    
public slots:
    
    /**
     * Signalled when back action is triggerd from toolbaar.
     * @since S60 ?S60_version.
     */
    void handleBackEvent();
    
    /**
     * Signalled when plugin emits wizardDeactivated signal.
     * @since S60 ?S60_version.
     */
    void handleGotoToC();
    
    /**
     * @copydoc FtuWizard::onViewChanged
     */
    void changeWizardView(FtuWizard *caller, QGraphicsWidget* viewWidget);
    
    /**
     * @copydoc FtuWizard::onInfoTextUpdated
     */
    void updateInfoText(FtuWizard *caller, QString text);
    
    /**
     * Handles the item selection from grid.
     * @param index The activated item.
     * @sice S60 ?S60_version.
     */
    void activateWizard(const QModelIndex index);
    
    /**
     * Called when the wizards wants to show/udpate the view options menu.
     * @since S60 ?S60_version.
     * @param caller the Wizard which wants to update/set the options menu
     * @param optionsmenu pointer to the HbMenu instance describing the menu to be show
     */   
    void updateMainMenu(FtuWizard *, HbMenu * );

signals:
        
    /**
     * Emitted when the menu state is to be activated.
     * @since S60 ?S60_version.
     */
    void backEventTriggered();  
 
private:

    /**
     * HbMainWindow instance.
     */
    HbMainWindow*           mMainWindow;
    
    /**
     * HbDocumentLoader instance.
     */
    HbDocumentLoader*       mDocumentLoader;

    /**
     * Plugin view instance.
     */
    HbView*                 mPluginView;
    
    /**
     * Pointer to the activated wizard. Not owned.
     */
    FtuWizard*              mActiveWizard;    
   
    /**
     * Info text label.
     */
    HbLabel*                mPluginTitleLabel;

    /**
     * Wizard widget area.
     */
    HbStackedWidget*        mWizardStackedWidget;

    /**
     * Back action overriding the app exit functionality.
     */
    HbAction*               mBackAction;
    
    /**
     * Plugin's previously activated view.
     */
    QGraphicsWidget*        mPreviousView;

    /**
     * Plugin's currently activated view.
     */
    QGraphicsWidget*        mCurrentView;

    HbTranslator            *mTranslator;
    
    FTU_TEST_FRIEND_CLASS(FtuStateProviderTest)    
};

#endif //FTU_STATES_WizardActivatedState_H
