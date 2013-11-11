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

deploy.path = z:

exports1.path = /private/20022F35
# this database is only for winscw emulator, for HW it is generated during image creation from confml
exports1.sources += resource/resource_emulator/homescreen.db
exports1.sources += data/backup_registration.xml
for(export1, exports1.sources):BLD_INF_RULES.prj_exports += "./$$export1 $$deploy.path$$exports1.path/$$basename(export1)"

exports2.path = /private/20022f35/wallpapers/scene
exports2.sources += resource/wallpapers/scene/default_landscape.png
exports2.sources += resource/wallpapers/scene/default_portrait.png
for(export2, exports2.sources):BLD_INF_RULES.prj_exports += "./$$export2 $$deploy.path$$exports2.path/$$basename(export2)"

exports3.path = /private/20022f35/wallpapers/page
exports3.sources += resource/wallpapers/page/default_landscape.png
exports3.sources += resource/wallpapers/page/default_portrait.png
exports3.sources += resource/wallpapers/page/1_landscape.png
exports3.sources += resource/wallpapers/page/1_portrait.png
exports3.sources += resource/wallpapers/page/2_landscape.png
exports3.sources += resource/wallpapers/page/2_portrait.png
exports3.sources += resource/wallpapers/page/3_landscape.png
exports3.sources += resource/wallpapers/page/3_portrait.png
for(export3, exports3.sources):BLD_INF_RULES.prj_exports += "./$$export3 $$deploy.path$$exports3.path/$$basename(export3)"

exports4.path = /private/20022f35/wallpapers/originals
exports4.sources += resource/wallpapers/page/originals/car.png
exports4.sources += resource/wallpapers/page/originals/shapes.png
exports4.sources += resource/wallpapers/page/originals/shoes.png
for(export4, exports4.sources):BLD_INF_RULES.prj_exports += "./$$export4 $$deploy.path$$exports4.path/$$basename(export4)"
