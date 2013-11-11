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
* Description:  Menu Event Factory.
*
*/

#include "hsmenuevent.h"
#include "hsmenueventfactory.h"
#include "hsapp_defs.h"


/*!
    \class HsMenuEventFactory
    \ingroup group_hsutils

    \brief Menu event factory class.

    \sa HsMenuEvent
*/

/*!
    \fn HsMenuEvenFactory::HsMenuEventFactory()
    Not used. Declared private and undefined to prevent creating
    instances of the class.
*/

/*!
    Prepares event triggered on add to home screen user request.
    \param entryId Id of an item to be added to home screen.
    \param entryTypeName Name of the entry type (e.g. application, widget).
    \param uri Widget uri.
    \param library Widget library path and name.
    \param attributes Widget params.
    \return Event for adding the widget to homescreen.
*/
QEvent *HsMenuEventFactory::createAddToHomeScreenEvent(
    int entryId,
	Hs::HsMenuMode menuMode,
    QVariant homescreenData)
{
    // get CaEntry type, and if widget get uri and library stored as properties...
    QVariantMap params;
    params.insert(Hs::itemIdKey, entryId);
    params.insert(Hs::menuModeType, menuMode);
    params.insert(Hs::homescreenData, homescreenData);
    return new HsMenuEvent(HsMenuEvent::AddToHomeScreen, params);
}

/*!
    Creates an HsMenuEvent::OpenCollection event.

    \param itemId Item id of the collection to be opened.
    \param collectionType type of the collection to be opened.
    \return Open collection event.
 */
QEvent *HsMenuEventFactory::createOpenCollectionEvent(int itemId,
        const QString &collectionType)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, itemId);
    params.insert(Hs::entryTypeNameKey, collectionType);
    return new HsMenuEvent(HsMenuEvent::OpenCollection, params);
}

/*!
    Creates an HsMenuEvent::createOpenCollectionFromAppLibraryEvent event.

    \param itemId Item id of the collection to be opened.
    \param collectionType type of the collection to be opened.
    \return Open collection event.
 */
QEvent *HsMenuEventFactory::createOpenCollectionFromAppLibraryEvent(int itemId,
        const QString &collectionType)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, itemId);
    params.insert(Hs::entryTypeNameKey, collectionType);
    return new HsMenuEvent(HsMenuEvent::OpenCollectionFromAppLibrary, params);
}

/*!
    Creates an HsMenuEvent::RenameCollection event.

    \param aItemId Item id of the collection to be renamed.
    \return Rename collection event.
 */
QEvent *HsMenuEventFactory::createRenameCollectionEvent(int aItemId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, aItemId);
    return new HsMenuEvent(HsMenuEvent::RenameCollection, params);
}

/*!
    Creates an HsMenuEvent::RenameCollection event.

    \return Rename collection event.
 */
QEvent *HsMenuEventFactory::createNewCollectionEvent()
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, 0);
    return new HsMenuEvent(HsMenuEvent::CreateCollection, params);
}

/*!
    Creates an HsMenuEvent::DeleteCollection event.

    \param aItemId Item id of the collection to be deleted.
    \return Delete collection event.
 */
QEvent *HsMenuEventFactory::createDeleteCollectionEvent(int aItemId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, aItemId);
    return new HsMenuEvent(HsMenuEvent::DeleteCollection, params);
}


/*!
    Creates an HsMenuEvent::OpenApplicationLibrary event.

    \return Open Applications Library event.
 */
QEvent *HsMenuEventFactory::createOpenAppLibraryEvent(
    Hs::HsMenuMode menuMode,
    QVariant homescreenData)
{
    QVariantMap params;
    params.insert(Hs::menuModeType, menuMode);
    params.insert(Hs::homescreenData, homescreenData);
    return new HsMenuEvent(HsMenuEvent::OpenApplicationLibrary, params);
}

/*!
    Creates an HsMenuEvent::BackFromInstalledView event.

    \return Back from installed view event.
 */
QEvent *HsMenuEventFactory::createBackFromInstalledViewEvent(
    int id, QString collectionType)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, id);
    params.insert(Hs::entryTypeNameKey, collectionType);
    return new HsMenuEvent(HsMenuEvent::BackFromInstalledView, params);
}

/*!
    Creates an HsMenuEvent::OpenInstalledView event.

    \return Open Installed View event.
 */
QEvent *HsMenuEventFactory::createOpenInstalledViewEvent(
    int id, QString collectionType)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, id);
    params.insert(Hs::entryTypeNameKey, collectionType);
	return new HsMenuEvent(HsMenuEvent::OpenInstalledView, params);
}

/*!
    Creates an HsMenuEvent::OpenHomeScreen event.

    \return Open HomeScreen event.
 */
QEvent *HsMenuEventFactory::createOpenHomeScreenEvent()
{
    return new HsMenuEvent(HsMenuEvent::OpenHomeScreen);
}

/*!
    Creates an HsMenuEvent::CollectionDeleted event.

    \return Collection deleted event.
 */
QEvent *HsMenuEventFactory::createCollectionDeletedEvent()
{
    return new HsMenuEvent(HsMenuEvent::CollectionDeleted);
}

/*!
    Creates an HsMenuEvent::AddAppsToCollection event.

    \param aApplicationsSortOder applications sort order.
    \param aItemId item id.
    \return Add applications to collection event.
 */
QEvent *HsMenuEventFactory::createAddAppsFromApplicationsViewEvent(
    Hs::HsSortAttribute aApplicationsSortOder,
    int aItemId)
{
    QVariantMap params;
    params.insert(Hs::appSortOrderKey, aApplicationsSortOder);
    params.insert(Hs::itemIdKey, aItemId);
    return  new HsMenuEvent(HsMenuEvent::AddAppsToCollection, params);
}

/*!
    Creates an HsMenuEvent::AddAppsToCollection event.

    \param aCollectionId collection id.
    \param aApplicationId application id.
    \return Add applications to collection event.
 */
QEvent *HsMenuEventFactory::createAddAppsFromCollectionViewEvent(
    int aCollectionId,
    int aApplicationId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, aApplicationId);
    params.insert(Hs::collectionIdKey, aCollectionId);
    return new HsMenuEvent(HsMenuEvent::AddAppsToCollection, params);
}

/*!
    Creates an HsMenuEvent::RemoveAppFromCollection event.

    \param aItemId Item id of the application to be removed from a collection.
    \param aCollectionId Item id of the collection the application is to be
     removed from.
    \return RemoveAppFromCollection event.
 */
QEvent *HsMenuEventFactory::createRemoveAppFromCollectionEvent(int aItemId,
        int aCollectionId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, aItemId);
    params.insert(Hs::collectionIdKey, aCollectionId);
    return new HsMenuEvent(HsMenuEvent::RemoveAppFromCollection, params);
}

/*!
    Creates an HsMenuEvent::UninstallApplication event.

    \param aItemId Item id of the application to be removed from a collection.
    \param aCollectionId Item id of the collection the application is to be
     removed from.
    \return UninstallApplication event.
 */
QEvent *HsMenuEventFactory::createUninstallApplicationEvent(int aItemId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, aItemId);
    return new HsMenuEvent(HsMenuEvent::UninstallApplication, params);
}

/*!
    Creates an HsMenuEvent::ArrangeCollection event.

    \param aTopItemId Item id to be scrolled.
    \param aCollectionId Collection id.
    \retval ArrangeCollection event.
 */
QEvent *HsMenuEventFactory::createArrangeCollectionEvent(
    int aTopItemId,
    int aCollectionId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, aTopItemId);
    params.insert(Hs::collectionIdKey, aCollectionId);
    return new HsMenuEvent(HsMenuEvent::ArrangeCollection, params);
}

/*!
    Creates an HsMenuEvent::ArrangeAllCollections event.

    \param aTopItemId Item id to be scrolled.
    \retval ArrangeAllCollections event.
 */
QEvent *HsMenuEventFactory::createArrangeAllCollectionsEvent(int aTopItemId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, aTopItemId);
    return new HsMenuEvent(HsMenuEvent::ArrangeAllCollections, params);
}

/*!
    Prepares event triggered on tap on Widget.
    \param entryId Id of an item.
    \param entryTypeName Name of the entry type (e.g. application, widget).
    \param uri Widget uri.
    \param library Widget library path and name.
    \return Event for preview the widget.
*/
QEvent *HsMenuEventFactory::createPreviewHSWidgetEvent(
    int entryId,
    const QString &entryTypeName,
    const QString &uri,
    const QString &library)
{
    // get CaEntry type, and if widget get uri and library stored as properties...
    QVariantMap params;

    params.insert(Hs::itemIdKey, entryId);

    params.insert(
        Hs::widgetUriAttributeName,
        uri);

    params.insert(
        Hs::widgetLibraryAttributeName,
        library);

    params.insert(Hs::entryTypeNameKey, entryTypeName);

    return new HsMenuEvent(HsMenuEvent::PreviewHSWidget, params);
}

/*!
    Creates an HsMenuEvent::ShowAppSettings event.
    \param entryId Id of an item.
    \return Event for view the Application Settings.
*/
QEvent *HsMenuEventFactory::createAppSettingsViewEvent(int entryId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, entryId);

    return new HsMenuEvent(HsMenuEvent::ShowAppSettings, params);
}

/*!
    Creates an HsMenuEvent::ShowAppDetails event.
    \param entryId Id of an item.
    \return Event for view the Application Settings.
*/
QEvent *HsMenuEventFactory::createAppDetailsViewEvent(int entryId)
{
    QVariantMap params;
    params.insert(Hs::itemIdKey, entryId);

    return new HsMenuEvent(HsMenuEvent::ShowAppDetails, params);
}

/*!
    Creates an HsMenuEvent::ShowInstallationLog event.

    \return Event for installation log.
*/
QEvent *HsMenuEventFactory::createInstallationLogEvent()
{
    return new HsMenuEvent(HsMenuEvent::ShowInstallationLog);
}

/*!
    Creates an HsMenuEvent::createUninstallationFailedEvent event.
	\param error Uninstallation error.
    \return Event for uninstall failed.
*/
QEvent *HsMenuEventFactory::createUninstallFailedEvent(int error)
{
    QVariantMap params;
    params.insert(Hs::uninstallError, error);
    return new HsMenuEvent(HsMenuEvent::UninstallationFailed, params);
}

/*!
    Creates an HsMenuEvent::Unknown event.

    \return Unknown event.
 */
QEvent *HsMenuEventFactory::createUnknownEvent()
{
    return new HsMenuEvent(HsMenuEvent::Unknown);
}

