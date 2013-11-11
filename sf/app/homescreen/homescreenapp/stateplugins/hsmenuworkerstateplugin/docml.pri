# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

symbian {
docmlFiles+= \
    resource/hsappchecklist.docml \
    resource/hsarrangedialog.docml \
    resource/hscollectionslistdialog.docml \
    resource/hsdetailsdialog.docml \
    resource/hsinstallationlogdialog.docml \
    resource/hsuninstalldialog.docml \
    resource/hswidgetpreviewdialog.docml
    
    qrcFile = hsmenuworkerstateplugin.qrc
    }
else {
    qrcFile = hsmenuworkerstateplugin_non_symbian.qrc
}
