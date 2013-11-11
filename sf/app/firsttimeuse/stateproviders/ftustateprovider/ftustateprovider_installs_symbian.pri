#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

export.sources = resource/*.manifest
export.path = /fturesources/plugins/stateproviders

pluginstub.sources = ftustateprovider.dll
pluginstub.path = /fturesources/plugins/stateproviders

DEPLOYMENT += export pluginstub

#deploy.path = z:
#qtplugins.path = /fturesources/plugins/stateproviders
#qtplugins.sources += qmakepluginstubs/ftustateprovider.qtplugin
#qtplugins.sources += resource/ftustateprovider.manifest

#for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"  
  