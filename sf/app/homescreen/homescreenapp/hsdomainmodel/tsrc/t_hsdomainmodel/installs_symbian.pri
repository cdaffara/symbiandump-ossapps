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

exporttestdb.sources = hsdomainmodeltest.db
exporttestdb.path = .

exporttestwallpapers.sources =  ./resource/default_portrait.png \
                              ./resource/default_landscape.png
exporttestwallpapers.path = ./wallpapers/scene/

exporttestwallpapers2.sources =  ./resource/default_portrait.png \
                               ./resource/default_landscape.png
exporttestwallpapers2.path = ./wallpapers/page/

exportmockwidgetplugin.sources = ./../mockwidgetplugin/resource/mockwidgetplugin.xml \
                                 ./../mockwidgetplugincheater/resource/mockwidgetplugincheater.xml \
                                 ./../mockwidgetplugintoobig/resource/mockwidgetplugintoobig.xml \
                                 ./../mockwidgetplugintoosmall/resource/mockwidgetplugintoosmall.xml
exportmockwidgetplugin.path = ./hsresources/plugins/widgetplugins

mockwidgetpluginqmfiles.sources = ./../mockwidgetplugin/resource/mockwidgetplugin.qm \
                                  ./../mockwidgetplugincheater/resource/mockwidgetplugincheater.qm
mockwidgetpluginqmfiles.path = /resource/qt/translations

pluginstub.sources = ./../mockwidgetplugin/qmakepluginstubs/mockwidgetplugin.dll \
                     ./../mockwidgetplugincheater/qmakepluginstubs/mockwidgetplugincheater.dll \
                     ./../mockwidgetplugintoobig/qmakepluginstubs/mockwidgetplugintoobig.dll \
                     ./../mockwidgetplugintoosmall/qmakepluginstubs/mockwidgetplugintoosmall.dll
pluginstub.path = ./hsresources/plugins/widgetplugins

DEPLOYMENT += exporttestdb exporttestwallpapers exporttestwallpapers2 exportmockwidgetplugin mockwidgetpluginqmfiles pluginstub
