#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

symbian*: { 
    include(email_services_api/email_services_api.pri)
    include(nmail_settings_api/nmail_settings_api.pri)
    include(nmail_client_api/nmail_client_api.pri)
    include(email_client_api/email_client_api.pri)
    include(contact_history_model_api/contact_history_model_api.pri)    
}
