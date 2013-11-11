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
* Description:  Test class for homescreen::hsutils
*
*/

#include <QtTest/QtTest>
#include <QWidget>
#include <QGraphicsView>
#include <QObject>
#include <QList>
#include <QGraphicsWidget>
#include "hswidgetpositioningonwidgetmove.h"

/**
* @test Test class for homescreen::hsutils modules.
*/
class t_hsUtils : public QObject
{
    Q_OBJECT

private slots:

    /**
     *
     */
    void initTestCase();

#ifndef ONLY_MENU_TESTCASES
    /**
     * t_hswidgetpositioningonorientationchange.cpp
     */
    void testWidgetPositioningOnOrientationChangeInstance();
    void testAdvancedWidgetPositioningOnOrientationChange_data();
    void testAdvancedWidgetPositioningOnOrientationChange();

    /**
     * t_hswidgetpositioningonwidgetmove.cpp
     */
    void testWidgetPositioningOnWidgetMoveInstance();
    void testHsSnapToLinesConstruct();
    void testHsSnapToLines_data();
    void testHsSnapToLines();
    void testHsSnapToLinesSnapDisabled_data();
    void testHsSnapToLinesSnapDisabled();


    /**
     * t_hspageindicator.cpp
     */
    void testPageIndicatorConstruction();
    void testPageIndicatorActiveItem();
    void testPageIndicatorAddRemoveItem();
    void testPageIndicatorAnimations();

    /**
     * t_trashbinwidget.cpp
     */
    void testTrashbinWidgetConstruct();
    void testTrashbinWidgetActivation();

    /**
     * t_hssnapline.cpp
     */
    void testSnapLineConstruction();
    void testSnapLineSetConfiguration();
    void testShowAndHideHorizontalLine();
    void testShowAndHideVerticalLine();
    void testHideHorizontalLineWhileFadeInAnimationRunning();
    void testHideVerticalLineWhileFadeInAnimationRunning();
    void testShowHorizontalLineWhileFadeOutAnimationRunning();
    void testShowVerticalLineWhileFadeOutAnimationRunning();
    void testShowHorizontalLineAtNewPositionWhileFadeOutAnimationRunning();
    void testShowVerticalLineAtNewPositionWhileFadeOutAnimationRunning();

    /**
    * t_titleresolver.cpp
    */
    void testTitleResolver();
    void testTitleResolverOnStatusChanged();
    void testTitleResolverOnNetworkNameChanged();

    /**
    * t_hsdeviceinfolistener.cpp
    */
    void testDeviceInfoListener();
    void testDeviceInfoListenerNetworkMode_data();
    void testDeviceInfoListenerNetworkMode();
    void testDeviceInfoListenerOfflineProfile();
    void testDeviceInfoListenerNetworkStatusChanged();
    void testDeviceInfoListenerNetworkNameChanged();
    void testDeviceInfoListenerNetworkCurrentProfileChanged();

    /**
    * t_hsspinnderdialog.cpp
    */
    void testSpinnerDialog();
    /**
    * t_hsmessageboxwrapper.cpp
    */
    void testMessageBoxWrapper();
    /**
    * t_hspropertyanimationwrapper.cpp
    */
    void testPropertyAnimationWrapper();

#else // ONLY_MENU_TESTCASES
    /**
     * t_hsutils_menuevent.cpp
     */
    void testMenuEventConstruction();
    void testMenuEventFactoryCreateAddToHomeScreenEvent();
    void testMenuEventFactoryCreatePreviewHSWidgetEvent();
    void testMenuEventFactoryCreateAppSettingsViewEvent();
    void testMenuEventFactoryCreateAppDetailsViewEvent();
    void testMenuEventFactoryCreateOpenCollectionEvent();
    void testMenuEventFactoryCreateOpenCollectionFromAppLibraryEvent();
    void testMenuEventFactoryCreateRenameCollectionEvent();
    void testMenuEventFactoryCreateNewCollectionEvent();
    void testMenuEventFactoryCreateDeleteCollectionEvent();
    void testMenuEventFactoryCreateCollectionDeletedEvent();
    void testMenuEventFactoryCreateBackFromInstalledViewEvent();
    void testMenuEventFactoryCreateOpenInstalledViewEvent();
    void testMenuEventFactorycCreateOpenAppLibraryEvent();
    void testMenuEventFactoryCreateAddAppsFromApplicationsViewEvent();
    void testMenuEventFactoryCreateAddAppsFromCallectionViewEvent();
    void testMenuEventFactoryCreateRemoveAppFromCollectionEvent();
    void testMenuEventFactoryCreateUninstallApplicationEvent();
    void testMenuEventFactoryCreateInstLogEvent();
    void testMenuEventFactoryCreateUnknownEvent();
    void testMenuEventFactoryOpenHSEvent();
    void testMenuEventTransitionConstruction();
    void testMenuEventTransitionTestCondition();
    void testMenuEventFactoryCreateArrangeCollectionEvent();
    void testMenuEventFactorycreateArrangeAllCollectionsEvent();
    void testMenuEventFactoryCreateUninstallFailedEvent();
    void testMenuDialogFactoryCreate();
#endif // ONLY_MENU_TESTCASES

    void cleanupTestCase();

private:
    bool hsLineCompare(QLineF l1, QLineF l2);
    bool hsResultCompare(const HsWidgetPositioningOnWidgetMove::Result& sr1, const HsWidgetPositioningOnWidgetMove::Result& sr2);

};
