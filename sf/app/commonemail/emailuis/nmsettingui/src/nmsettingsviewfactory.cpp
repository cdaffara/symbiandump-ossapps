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
* Description:
*
*/

#include <QProcess>
#include <QTranslator>
#include <QCoreApplication>
#include <HbInstance>
#include <HbAction>

#include "nmsettingsviewfactory.h"
#include "nmmailboxsettingsmanager.h"
#include "nmmailboxsettingview.h"
#include "nmmailboxselectionview.h"
#include "nmmailbox.h"
#include "nmcommon.h"
#include "nmsettingsviewlauncher.h"
#include "nmsettingscommon.h"


/*!
    \class NmSettingsViewFactory
    \brief Constructs and displays settings views.

*/


// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmSettingsViewFactory.

    \param itemDataHelper
    \param text
    \param description
    \param icon
    \param parent
    \return A newly constructed settings view factory instance.
*/
NmSettingsViewFactory::NmSettingsViewFactory(
    CpItemDataHelper &itemDataHelper,
    const QString &text,
    const QString &description,
    const HbIcon &icon,
    const HbDataFormModelItem *parent)
 : CpSettingFormEntryItemData(itemDataHelper, text, description, icon, parent),
   mSettingsManager(new NmMailboxSettingsManager()),
   mSettingsViewLauncher(NULL),
   mPrevView(NULL)
{
    NM_FUNCTION;
}


/*!
    Constructor of NmSettingsViewFactory.

    \param viewLauncher
    \param itemDataHelper
    \param text
    \param description
    \param icon
    \param parent
    \return A newly constructed settings view factory instance.
*/
NmSettingsViewFactory::NmSettingsViewFactory(
    const NmSettingsViewLauncher *viewLauncher,
    CpItemDataHelper &itemDataHelper,
    const QString &text,
    const QString &description,
    const HbIcon &icon,
    const HbDataFormModelItem *parent)
 : CpSettingFormEntryItemData(itemDataHelper, text, description, icon, parent),
     mSettingsManager(new NmMailboxSettingsManager()),
     mSettingsViewLauncher(viewLauncher),
     mPrevView(NULL)
{
    NM_FUNCTION;
}


/*!
    Destructor of NmSettingsViewFactory.
*/
NmSettingsViewFactory::~NmSettingsViewFactory()
{
    NM_FUNCTION;

    delete mSettingsManager;
}


/*!
    Constructs a setting view. The type of the view depends on the number of
    mailboxes present. If there are no mailboxes defined, displays a query
    asking whether to user wants to define a new mailbox or not.

    \return A newly constructed setting view instance.
*/
CpBaseSettingView *NmSettingsViewFactory::createSettingView() const
{
    NM_FUNCTION;

    CpBaseSettingView *view = NULL;
    QList<NmMailbox *> mailboxList;
    mSettingsManager->listMailboxes(mailboxList);
    const int mailboxCount(mailboxList.count());

    // Log the number of mailboxes.
    NM_COMMENT(QString("NmSettingsViewFactory::createSettingView(): mailbox count is %1").arg(mailboxCount));

    switch(mailboxCount) {
        case 0: {
            // Mailbox doesn't exist, we can't open settings
            break;
        }
        case 1: {
            // Construct setting view for the only available mailbox
            NmMailbox *mailbox = mailboxList.at(0);
            view = new NmMailboxSettingView(mailbox->id(),
                                            mailbox->name(),
                                            *mSettingsManager);
            break;
        }
        default: {
            // Construct mailbox selection view
            view = new NmMailboxSelectionView(*this, *mSettingsManager, mailboxList);
            break;
        }

    }

    if (view) {
        connect(mSettingsManager,
                SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
                view, SLOT(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

        connect(mSettingsManager,
                SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
                view, SLOT(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));
    }

    return view;
}


/*!
    Constructs and launches, i.e. displays a setting view for the given mailbox.

    \param mailboxId The id of the mailbox.
    \param mailboxName The name of the mailbox.
*/
void NmSettingsViewFactory::launchSettingView(const NmId &mailboxId,
                                              const QString &mailboxName) const
{
    NM_FUNCTION;

    // There's always at least one valid main window available.
    HbMainWindow *mainWindow = hbInstance->allMainWindows().takeFirst();

    NmMailboxSettingView *currentSettingsView =
        qobject_cast<NmMailboxSettingView*>(mainWindow->currentView());

    // Let's have only one same settings view at a time open,
    // but if some other mailboxes settings view want's to open while
    // there is a settings view open let's open it also.
    if (!currentSettingsView || (currentSettingsView->mailboxId().id() != mailboxId.id())) {
        CpBaseSettingView *view =
             new NmMailboxSettingView(mailboxId, mailboxName, *mSettingsManager);

         connect(mSettingsManager,
                 SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
                 view, SLOT(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

         connect(mSettingsManager,
                 SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
                 view, SLOT(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));

         // Disconnect mSettingsViewLauncher's previous connections to
         // be sure that signals which are offered out will be sent last.
         mSettingsManager->disconnect(
             SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
             mSettingsViewLauncher,
             SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

         mSettingsManager->disconnect(
             SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
             mSettingsViewLauncher,
             SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));

         // Reconnect mSettingsViewLauncher.
         connect(mSettingsManager,
                 SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
                 mSettingsViewLauncher,
                 SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

         connect(mSettingsManager,
                 SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
                 mSettingsViewLauncher,
                 SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));

         connect(mSettingsManager,
                 SIGNAL(goOnline(const NmId &)),
                 mSettingsViewLauncher,
                 SIGNAL(goOnline(const NmId &)), Qt::UniqueConnection);

         connect(mSettingsManager,
                 SIGNAL(goOffline(const NmId &)),
                 mSettingsViewLauncher,
                 SIGNAL(goOffline(const NmId &)), Qt::UniqueConnection);

         connect(this,
                 SIGNAL(aboutToClose()),
                 mSettingsManager,
                 SIGNAL(aboutToClose()), Qt::UniqueConnection);

         // Create back navigation action for a view.
         HbAction *action = new HbAction(Hb::BackNaviAction, view);
         connect(action, SIGNAL(triggered()), this, SLOT(backPress()));
         view->setNavigationAction(action);
         mPrevView = mainWindow->currentView();
         mainWindow->addView(view);
         mainWindow->setCurrentView(view);
    }
}

/*!
   Handels back button press.
   Removes the current view from the main window and activates the previous view.
   Destroys removed view.
*/
void NmSettingsViewFactory::backPress()
{
    NM_FUNCTION;

    emit aboutToClose();
    HbMainWindow *mainWindow = hbInstance->allMainWindows().takeFirst();
    QList<HbView *> views = mainWindow->views();
    if (views.count() > 1) {
        HbView *currentView = mainWindow->currentView();
        int indexOfView(views.indexOf(mPrevView));
        if (indexOfView > -1) {
            mainWindow->setCurrentView(views.at(indexOfView));
            mainWindow->removeView(currentView);
            currentView->deleteLater();
        }
        mPrevView = NULL;
    }
}


// End of file.
