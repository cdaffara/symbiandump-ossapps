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

kqtihswallpapers.CONFIG += no_build
kqtihswallpapers.path = $$DESTDIR/resource/kqtihswallpapers
kqtihswallpapers.files = ./resource/wallpapers/page/originals/car.png \
                         ./resource/wallpapers/page/originals/shapes.png \
                         ./resource/wallpapers/page/originals/shoes.png

homescreendb.CONFIG += no_build
homescreendb.path = $$DESTDIR/private/20022f35
homescreendb.files = ./resource/resource_win/homescreen.db

scenewallpapers.CONFIG += no_build
scenewallpapers.path = $$DESTDIR/private/20022f35/wallpapers/scene
scenewallpapers.files = ./resource/wallpapers/scene/default_landscape.png \
                        ./resource/wallpapers/scene/default_portrait.png

pagewallpapers.CONFIG += no_build
pagewallpapers.path = $$DESTDIR/private/20022f35/wallpapers/page
pagewallpapers.files = ./resource/wallpapers/page/default_landscape.png \
                       ./resource/wallpapers/page/default_portrait.png \
                       ./resource/wallpapers/page/1_portrait.png \
                       ./resource/wallpapers/page/1_landscape.png \
                       ./resource/wallpapers/page/2_portrait.png \
                       ./resource/wallpapers/page/2_landscape.png \
                       ./resource/wallpapers/page/3_portrait.png \
                       ./resource/wallpapers/page/3_landscape.png

INSTALLS += kqtihswallpapers homescreendb scenewallpapers pagewallpapers
PRE_TARGETDEPS += install_kqtihswallpapers install_homescreendb install_scenewallpapers install_pagewallpapers
