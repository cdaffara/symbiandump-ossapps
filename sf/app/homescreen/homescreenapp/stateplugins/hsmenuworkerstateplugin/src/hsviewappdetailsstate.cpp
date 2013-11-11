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
* Description:  Menu Application Library state.
 *
*/

#include <QDebug>
#include <QScopedPointer>
#include <HbDialog>
#include <HbDocumentLoader>
#include <HbLabel>
#include <HbAction>

#include "caservice.h"
#include "casoftwareregistry.h"

#include "canotifier.h"
#include "canotifierfilter.h"

#include "hsviewappdetailsstate.h"
#include "hsmenuevent.h"
#include "hsmenuitemmodel.h"
#include "hsdialogcontroller.h"

static const char *const HS_VIEWAPPDETAILS_JAVA_DIALOG_SECTION_NAME=
        "detailsDialogForJavaApplication";

/*!
 Constructor.
 /param parent Parent state.
 */
HsViewAppDetailsState::HsViewAppDetailsState(QState *parent) :
    QState(parent)
{
    construct();
}

/*!
 Destructor
 */
HsViewAppDetailsState::~HsViewAppDetailsState()
{
    QT_TRY {
        emit exit();
    } QT_CATCH (...) {
    }
}

/*!
 Construction
 */
void HsViewAppDetailsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppDetailsState::construct");

    setObjectName("/ViewAppDetailsState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    HSMENUTEST_FUNC_EXIT("HsViewAppDetailsState::construct");
}

/*!
 Invoked on entering state
 */
void HsViewAppDetailsState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsViewAppDetailsState::onEntry");
    QState::onEntry(event);
    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    QVariantMap data = menuEvent->data();

    const int entryId = data.value(Hs::itemIdKey).toInt();
    QSharedPointer<const CaEntry> entry
        = CaService::instance()->getEntry(entryId);
    if (!entry) {
        emit exit();
        return;
    }
    const int componentId = entry->attribute(
        Hs::componentIdAttributeName).toInt();

    QSharedPointer<CaSoftwareRegistry> scr = CaSoftwareRegistry::create();
    CaSoftwareRegistry::DetailMap detailMap = scr->entryDetails(componentId);

    QString appType = entry->attribute(Hs::swTypeKey);


    //TODO: Should we display something In that case?
    if (detailMap.size() < 1){
        emit exit();
        return;
    }

    HbDocumentLoader loader;
    bool loadStatusOk = false;
    loader.load(HS_DETAILS_DIALOG_LAYOUT, &loadStatusOk);
    Q_ASSERT_X(loadStatusOk, HS_DETAILS_DIALOG_LAYOUT,
           "Error while loading docml file.");

    if (!appType.compare(Hs::javaSwType)) {
        QString section = QString(HS_VIEWAPPDETAILS_JAVA_DIALOG_SECTION_NAME);
        loader.load(HS_DETAILS_DIALOG_LAYOUT,
                                  section, &loadStatusOk);
    }

    QScopedPointer<HbDialog> dialog(qobject_cast<HbDialog*>(
        loader.findWidget(HS_DETAILS_DIALOG_NAME)));

    if (!dialog.isNull()) {
        dialog->setTimeout(HbPopup::NoTimeout);
        dialog->actions()[0]->setParent(dialog.data());

        setFieldPresentation(CaSoftwareRegistry::componentNameKey(),
            detailMap, loader);
        setFieldPresentation(CaSoftwareRegistry::componentVersionKey(),
            detailMap, loader);
        setFieldPresentation(CaSoftwareRegistry::componentVendorKey(),
            detailMap, loader);
        setFieldPresentation(CaSoftwareRegistry::componentDriveInfoKey(),
            detailMap, loader);
        setFieldPresentation(CaSoftwareRegistry::componentSizeKey(),
            detailMap, loader);
        setFieldPresentation(CaSoftwareRegistry::componentTypeKey(),
            detailMap, loader);

        if (!appType.compare(Hs::javaSwType)) {
            setFieldPresentation(CaSoftwareRegistry::componentDescriptionKey(),
                detailMap, loader);
            setFieldPresentation(CaSoftwareRegistry::componentProtectionDomainKey(),
                detailMap, loader);
        }

        QScopedPointer<HsDialogController> dialogController(
            new HsDialogController(dialog.take(),
                HsMenuDialogFactory::acceptActionIndex(),
                HsMenuDialogFactory::rejectActionIndex()));

        connect(dialogController.data(),
                SIGNAL(dialogCompleted()),
                this,
                SIGNAL(exit()));

        // ensure dialog is dismissed on app key pressed
        connect(this, SIGNAL(exited()),
                dialogController.data(),
                SLOT(dismissDialog()));

        dialogController.take()->openDialog(entryId);
    } else {
        emit exit();
    }
    HSMENUTEST_FUNC_EXIT("HsViewAppDetailsState::onEntry");
}

/*!
 Extracting label from loader and set content of data field.
 Hide label if the is no data to display.
 \param key detail key.
 \param detailMap detail map.
 \param loader session to document loader
 \retval void
 */
void HsViewAppDetailsState::setFieldPresentation(QString key,
        CaSoftwareRegistry::DetailMap &detailMap,
        HbDocumentLoader &loader)
{
    HbLabel* dataLabel = qobject_cast<HbLabel*>(
                loader.findWidget(key));
    QString data = detailMap[key];
    if (data.isEmpty()) {
        dataLabel->setVisible(false);
        dataLabel->setMaximumHeight(0);
        HbLabel* titleLabel = qobject_cast<HbLabel*>(
            loader.findWidget(QString("_") + key));
        titleLabel->setVisible(false);
        titleLabel->setMaximumHeight(0);
    } else {
        dataLabel->setPlainText(data);
    }
}



