#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).

#
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
#


TEMPLATE = subdirs
SUBDIRS = cnthistorymodel cntcommonui phonebookapp phonebookservices
CONFIG += ordered

deploy.path = /epoc32/rom/include/core/app/

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "./rom/phonebookservices.iby                CORE_APP_LAYER_IBY_EXPORT_PATH(phonebookservices.iby)" \
     "./rom/phonebook.iby                        CORE_APP_LAYER_IBY_EXPORT_PATH(phonebook.iby)" \
     "./rom/phonebookresources.iby               LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(phonebookresources.iby)" 

:BLD_INF_RULES.prj_exports += "conf/contacts.confml APP_LAYER_CONFML(contacts.confml)"
:BLD_INF_RULES.prj_exports += "conf/contacts_2002FF54.crml APP_LAYER_CRML(contacts_2002FF54.crml)"
:BLD_INF_RULES.prj_exports += "conf/contacts_200315A8.crml APP_LAYER_CRML(contacts_200315A8.crml)"
exists(confml/phonebook.confml) :BLD_INF_RULES.prj_exports += "confml/phonebook.confml CONFML_EXPORT_PATH(phonebook.confml,uda_content)"
exists(implml/phonebook.implml) :BLD_INF_RULES.prj_exports += "implml/phonebook.implml CRML_EXPORT_PATH(phonebook.implml,uda_content)"
exists(content/SQLite__Contacts.zip) :BLD_INF_RULES.prj_exports += "content/SQLite__Contacts.zip CRML_EXPORT_PATH(../content/zip/,uda_content)"

BLD_INF_RULES.prj_exports += "../sis/contacts_stub.sis  /epoc32/data/z/system/install/contacts_stub.sis"

DEPLOYMENT += romfiles

