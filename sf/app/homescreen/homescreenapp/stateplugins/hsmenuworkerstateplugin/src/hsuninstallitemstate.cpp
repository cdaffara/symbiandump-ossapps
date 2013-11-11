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
 * Description: Menu delete collection item state
 *
 */


#include <QScopedPointer>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStateMachine>

#include <HbMessageBox>
#include <HbAction>
#include <HbDocumentLoader>
#include <HbLabel>
#include <HbListView>
#include <HbParameterLengthLimiter>

#include "hsmenuservice.h"
#include "hsshortcutservice.h"
#include "hsuninstallitemstate.h"
#include "hsmenuevent.h"
#include "hsmenudialogfactory.h"
#include "hsdialogcontroller.h"
#include "hsmenueventfactory.h"

#include <casoftwareregistry.h>
#include <caquery.h>
#include <canotifier.h>

const char definition02 [] = "uninstallDialogDefinition02";
const char definition03 [] = "uninstallDialogDefinition03";
const char definition04 [] = "uninstallDialogDefinition04";


/*!
 \class HsUninstallItemState
 \ingroup group_hsworkerstateplugin
 \brief Uninstall Item State.
 Deletes app from collection.
 */

/*!
 Constructor.
 \param parent Parent state.
 */
HsUninstallItemState::HsUninstallItemState(QState *parent) :
    QState(parent),
    mEntry(NULL)
{
    construct();
}

/*!
 Destructor.
 */
HsUninstallItemState::~HsUninstallItemState()
{
    QT_TRY {
        emit exit();
    } QT_CATCH (...) {
        qDebug("Exception thrown");
    }
}

/*!
 Constructs contained objects.
 */
void HsUninstallItemState::construct()
{
    setObjectName("/UninstallItemState");
    if (this->parent()) {
        setObjectName(this->parent()->objectName() + objectName());
    }
    connect(this, SIGNAL(exited()), SLOT(cleanUp()));
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsUninstallItemState::onEntry(QEvent *event)
{
    HSMENUTEST_FUNC_ENTRY("HsUninstallItemState::onEntry");
    qDebug("HsUninstallItemState::onEntry()");

    QState::onEntry(event);

    HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
    int itemId = menuEvent->data().value(Hs::itemIdKey).toInt();
    mEntry = CaService::instance()->getEntry(itemId);

    if (mEntry) {
        QString appType = mEntry->attribute(Hs::swTypeKey);

        if (!appType.compare(Hs::javaSwType)) {
            // java
            createUninstallJavaMessage();
        } else {
            // other
            createSimpleUninstallMessage();
        }
    }
    HSMENUTEST_FUNC_EXIT("HsUninstallItemState::onEntry");
}

/*!
 Slots. Uninstalls item controlled by the state.
 \retval void
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsUninstallItemState::uninstallItem()
{
    HsMenuService::executeAction(*mEntry.data(), Hs::removeActionIdentifier,
            this, SLOT(uninstallFailed(int)));
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Return information about a component: component name, a list of names
 of applications in this component and a delete message.
 \param[out] componentName component name.
 \param[out] applicationsNames a list of names of applications.
 \param[out] confirmationMessage delete message.
 \retval true if there is no error.
 */
bool HsUninstallItemState::getApplicationsNames(QString &componentName,
    QStringList &applicationsNames,
    QString &confirmationMessage)
{
    componentName.clear();
    applicationsNames.clear();
    confirmationMessage.clear();

    QSharedPointer<CaService> service = CaService::instance();
    const int componentId = mEntry->attribute(
            Hs::componentIdAttributeName).toInt();

    QStringList appUids;
    bool retval = CaSoftwareRegistry::create()->getUninstallDetails(
            componentId, componentName, appUids, confirmationMessage);
    if (retval) {
        CaQuery query;
        foreach (QString uid, appUids) {
            query.setAttribute(Hs::applicationUidEntryKey, uid);
            QList< QSharedPointer<CaEntry> > entries =
                    service->getEntries(query);
            if (entries.length() > 0) {
                applicationsNames << entries[0]->text();
            }
        }
        if (applicationsNames.length() == 1
                && applicationsNames[0] == componentName) {
            applicationsNames.clear();
        }
    }
    return retval;
}


/*!
 Slot launched after state has exited and in destructor.
 \retval void
 */
void HsUninstallItemState::cleanUp()
{
    mEntry.clear();
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT (only returns HbMainWindow)
/*!
 Slot invoked when unninstallation failed.
 \param error Uninstallation error.
 \retval void
 */
void HsUninstallItemState::uninstallFailed(int error)
{
    machine()->postEvent(
            HsMenuEventFactory::createUninstallFailedEvent(error));
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Method create uninstall confirmation message.
 \retval void
 */
void HsUninstallItemState::createSimpleUninstallMessage(bool isJava)
{
    CaQuery parentsQuery;
    parentsQuery.setChildId(mEntry->id());
    parentsQuery.setEntryTypeNames(QStringList(Hs::collectionTypeName));
    QList<int> parentsIds = CaService::instance()->getEntryIds(parentsQuery);
    QString message;
    if (HsShortcutService::instance()->isItemShortcutWidget(mEntry->id())
            || (parentsIds.count() > 0)) {
        message.append(HbParameterLengthLimiter(
                "txt_applib_dialog_uninstalls_1_and_deletes_all_sh").arg(
                        mEntry->text()));
    } else {
        message.append(HbParameterLengthLimiter(
                "txt_applib_dialog_1_will_be_removed_from_phone_c").arg(
                        mEntry->text()));
    }

    // create and show message box

    QScopedPointer<HbMessageBox> dialog(
            HsMenuDialogFactory().create(message));
    if (isJava){
        dialog->setIcon(mEntry->makeIcon());
    }

    QScopedPointer<HsDialogController> dialogController(
            new HsDialogController(dialog.take(),
                    HsMenuDialogFactory::acceptActionIndex(),
                    HsMenuDialogFactory::rejectActionIndex()));

    connect(dialogController.data(), SIGNAL(acceptActionTriggered(QAction*)),
            this, SLOT(uninstallItem()));

    connect(dialogController.data(), SIGNAL(dialogCompleted()),
            this, SIGNAL(exit()));

    // ensure dialog is dismissed on app key pressed
    connect(this, SIGNAL(exited()),
            dialogController.data(), SLOT(dismissDialog()));

    dialogController.take()->openDialog(mEntry->id());
}

/*!
 Method create uninstall confirmation dialog for java.
 \retval void
 */
void HsUninstallItemState::createUninstallJavaMessage()
{
    QString componentName;
    QStringList applicationsNames;
    QString detailsMessage;
    getApplicationsNames(componentName, applicationsNames, detailsMessage);

    if (applicationsNames.isEmpty() && detailsMessage.isEmpty()) {
        createSimpleUninstallMessage(true);
    } else {
        HbDocumentLoader loader;

        QString section = loadProperSection(applicationsNames,
                detailsMessage, loader);
        if (!section.isNull()) {
            HbDialog *const dialog = qobject_cast<HbDialog*>(
                    loader.findWidget(HS_UNINSTALL_DIALOG_NAME));

            if (dialog) {
                prepareDialog(dialog);

                HbLabel* iconLabel = qobject_cast<HbLabel*>(loader.findWidget(
                        HS_UNINSTALL_DIALOG_CONFIRMATION_DIALOG_ICON));
                iconLabel->setIcon(mEntry->makeIcon());

                prepareDialogLabel(componentName, loader);

                if ((section == definition02) || (section == definition04)) {
                    qobject_cast<HbLabel*>( loader.findWidget(
                            HS_UNINSTALL_DIALOG_DELETE_LABEL))->setPlainText(
                                    detailsMessage);
                }

                if ((section == definition02) || (section == definition03)) {
                    prepareApplicationListLabel(applicationsNames, loader);
                }

                QScopedPointer<HsDialogController> dialogController(
                        new HsDialogController( dialog,
                                HsMenuDialogFactory::acceptActionIndex(),
                                HsMenuDialogFactory::rejectActionIndex()));
                connect(dialogController.data(), SIGNAL(acceptActionTriggered(QAction*)),
                        this, SLOT(uninstallItem()));
                connect(dialogController.data(), SIGNAL(dialogCompleted()),
                        this, SIGNAL(exit()));
                connect(this, SIGNAL(exited()),
                        dialogController.data(), SLOT(dismissDialog()));

                dialogController.take()->openDialog(mEntry->id());
            }
        }
    }
}

void HsUninstallItemState::prepareDialogLabel(
    QString &componentName, HbDocumentLoader &loader)
{
    CaQuery parentsQuery;
    parentsQuery.setChildId(mEntry->id());
    parentsQuery.setEntryTypeNames(QStringList(Hs::collectionTypeName));
    QList<int> parentsIds = CaService::instance()->getEntryIds(parentsQuery);

    HbLabel* textLabel = qobject_cast<HbLabel*>(loader.findWidget(
            HS_UNINSTALL_DIALOG_CONFIRMATION_DIALOG_LABEL));

    QString plainText;
    if (HsShortcutService::instance()->isItemShortcutWidget(mEntry->id())
            || parentsIds.count()) {
        plainText = HbParameterLengthLimiter(
                "txt_applib_info_delete_1_all_its_shortcuts_and").arg(
                        componentName);
    } else {
        plainText = HbParameterLengthLimiter(
                "txt_applib_info_delete_1_and_associated_data").arg(
                        componentName);
    }
    textLabel->setPlainText(plainText);
}

void HsUninstallItemState::prepareApplicationListLabel(
        QStringList & applicationsNames, HbDocumentLoader & loader)
{
    QString applicationsList;
    QString newLine("\n");
    for(int i = 0;i < applicationsNames.count();i++){
        applicationsList += applicationsNames[i] + newLine;
    }

    HbLabel *listView;
    listView = qobject_cast<HbLabel*>(loader.findWidget(
            HS_UNINSTALL_DIALOG_LIST_VIEW_LABEL));
    listView->setPlainText(applicationsList);
}

QString HsUninstallItemState::loadProperSection(
        QStringList &applicationsNames, QString &detailsMessage,
        HbDocumentLoader &loader)
{
    QString section;
    if (!applicationsNames.isEmpty() && !detailsMessage.isEmpty()) {
        section = QString(definition02);
    } else if (!applicationsNames.isEmpty() && detailsMessage.isEmpty()) {
        section = QString(definition03);
    }else if (applicationsNames.isEmpty() && !detailsMessage.isEmpty()) {
        section = QString(definition04);
    }
    if(!section.isNull()){
        bool loadStatusOk = false;
        loader.load(HS_UNINSTALL_DIALOG_LAYOUT, &loadStatusOk);

        Q_ASSERT_X(loadStatusOk,
                HS_UNINSTALL_DIALOG_LAYOUT,
               "Error while loading docml file.");

        loader.load(
                HS_UNINSTALL_DIALOG_LAYOUT, section, &loadStatusOk);
        Q_ASSERT_X(loadStatusOk,
                HS_UNINSTALL_DIALOG_LAYOUT,
               "Error while loading docml file section.");
    }
    return section;
}

HsDialogController *HsUninstallItemState::prepareDialog(
        HbDialog *dialog)
{
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setTimeout(HbPopup::NoTimeout);
    dialog->actions()[0]->setParent(dialog);
    dialog->actions()[1]->setParent(dialog);
    return NULL;
}
