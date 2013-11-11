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

TEMPLATE = lib
TARGET = cntcommonui

CONFIG += dll
CONFIG += hb
HB += hbfeedback
QT += sql

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../phonebookengines/cntlistmodel/inc
INCLUDEPATH += ../../phonebookengines/cntimageutility/inc
INCLUDEPATH += ../../phonebookengines/cntsimutility/inc
INCLUDEPATH += ../phonebookapp/inc
INCLUDEPATH += ../cnthistorymodel/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

MOC_DIR = moc

DEFINES += QT_DLL BUILD_QTPBK

symbian: {
        # Skip the UID2/3 thing
        TARGET.UID3 = 0x20026792
        TARGET.EPOCSTACKSIZE = 0x5000
        TARGET.EPOCHEAPSIZE = "0x20000 0x2000000"

        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_services_api/inc/cntservicescontact.h APP_LAYER_PLATFORM_EXPORT_PATH(cntservicescontact.h)"

        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntviewparams.h APP_LAYER_PLATFORM_EXPORT_PATH(cntviewparams.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntabstractview.h APP_LAYER_PLATFORM_EXPORT_PATH(cntabstractview.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntabstractviewmanager.h APP_LAYER_PLATFORM_EXPORT_PATH(cntabstractviewmanager.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntabstractengine.h APP_LAYER_PLATFORM_EXPORT_PATH(cntabstractengine.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntuids.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuids.h)"

        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuiextensionfactory.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuiextensionfactory.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuigroupsupplier.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuigroupsupplier.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuisocialextension.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuisocialextension.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuiextensiongroup.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuiextensiongroup.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntextensiongroupcallback.h APP_LAYER_PLATFORM_EXPORT_PATH(cntextensiongroupcallback.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntviewsupplier.h APP_LAYER_PLATFORM_EXPORT_PATH(cntviewsupplier.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cnteditviewitemsupplier.h APP_LAYER_PLATFORM_EXPORT_PATH(cnteditviewitemsupplier.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cnteditviewitem.h APP_LAYER_PLATFORM_EXPORT_PATH(cnteditviewitem.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuiactionextension.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuiactionextension.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cnteditviewitemcallback.h APP_LAYER_PLATFORM_EXPORT_PATH(cnteditviewitemcallback.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuicontactcardextension.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuicontactcardextension.h)"
}

# Input
HEADERS += ../../inc/cntdebug.h \
    core/cntabstractviewfactory.h \
    core/cntdefaultengine.h \
    core/cntdefaultviewmanager.h \ 
    core/cntextensionmanager.h \
    core/cntmainwindow.h \
    core/cntthumbnailmanager.h \
    core/cntviewnavigator.h \
    common/cntactionlauncher.h \
    common/cntdetailorderinghelper.h \
    common/cntdocumentloader.h \
    common/cntfavourite.h \
    common/cntkeygrabber.h \
    common/cntpresencelistener.h \
    common/cntsavemanager.h \
    common/cntsimengine.h \
    common/cntstringmapper.h \ 
    contactcard/cntcontactcardcontextmenu.h \
    contactcard/cntcontactcarddatacontainer.h \
    contactcard/cntcontactcarddataitem.h \
    contactcard/cntcontactcarddetailitem.h \
    contactcard/cntcontactcardheadingitem.h \ 
    contactcard/cntcontactcardview.h \
    contactcard/cntcontactcardview_p.h \
    contacteditors/cntaddressmodel.h \
    contacteditors/cntaddressviewitem.h \
    contacteditors/cntcommondetailviewitem.h \
    contacteditors/cntcompanyeditormodel.h \
    contacteditors/cntdateeditormodel.h \
    contacteditors/cntdateeditorviewitem.h \
    contacteditors/cntdetailconst.h \
    contacteditors/cntdetaileditor.h \
    contacteditors/cntdetaileditormodel.h \
    contacteditors/cntdetailmodelitem.h \
    contacteditors/cntdetailpopup.h \
    contacteditors/cntdetailviewitem.h \
    contacteditors/cnteditorfactory.h \
    contacteditors/cnteditviewdetailitem.h \
    contacteditors/cnteditviewheadingitem.h \
    contacteditors/cnteditviewlistmodel.h \
    contacteditors/cnteditview.h \
    contacteditors/cnteditview_p.h \
    contacteditors/cntemaileditormodel.h \
    contacteditors/cntemaileditorviewitem.h \
    contacteditors/cntfamilyeditormodel.h \
    contacteditors/cntnameeditormodel.h \
    contacteditors/cntnoteeditormodel.h \
    contacteditors/cntnoteeditorviewitem.h \
    contacteditors/cntphonenumberviewitem.h \
    contacteditors/cntphonenumbermodel.h \
    contacteditors/cnturleditormodel.h \
    contacteditors/cnturleditorviewitem.h \   
    collections/cntcollectionlistmodel.h \
    collections/cntcollectionlistmodel_p.h \
    collections/cntcollectionlistmodelworker.h \
    collections/cntcollectionview.h \
    collections/cntfavoritesmemberview.h \
    collections/cntfavoritesview.h \
    collections/cntgroupactionsview.h \
    collections/cntgroupdeletepopup.h \
    collections/cntgroupdeletepopupmodel.h \
    collections/cntgroupeditormodel.h \
    collections/cntgroupmemberview.h \
    collections/cntgroupmemberview_p.h \  
    views/cntactionmenubuilder.h \
    views/cntbaseselectionview.h \
    views/cnthistoryview.h \
    views/cnthistoryviewitem.h \
    views/cntimageeditorview.h \
    views/cntimportsview.h \
    views/cntimportviewcallback.h \ 
    views/cntmycardview.h \
    views/cntnamesview.h \
    views/cntnamesview_p.h \
    views/cntsettingsmodel.h \
    views/cntsettingsview.h \
    widgets/cntactionpopup.h \
    widgets/cntactionpopup_p.h \   
    widgets/cntfetchcontactpopup.h \
    widgets/cntfetchmarkall.h \
    widgets/cntimagelabel.h      

SOURCES += core/cntdefaultengine.cpp \
    core/cntdefaultviewfactory.cpp \
    core/cntdefaultviewmanager.cpp \ 
    core/cntextensionmanager.cpp \
    core/cntmainwindow.cpp \
    core/cntthumbnailmanager.cpp \
    core/cntviewnavigator.cpp \
    common/cntactionlauncher.cpp \
    common/cntdetailorderinghelper.cpp \
    common/cntdocumentloader.cpp \
    common/cntfavourite.cpp \
    common/cntkeygrabber.cpp \
    common/cntpresencelistener.cpp \
    common/cntsavemanager.cpp \
    common/cntsimengine.cpp \
    contactcard/cntcontactcardcontextmenu.cpp \
    contactcard/cntcontactcarddatacontainer.cpp \
    contactcard/cntcontactcarddataitem.cpp \
    contactcard/cntcontactcarddetailitem.cpp \
    contactcard/cntcontactcardheadingitem.cpp \
    contactcard/cntcontactcardview.cpp \
    contactcard/cntcontactcardview_p.cpp \
    contacteditors/cntaddressmodel.cpp \
    contacteditors/cntaddressviewitem.cpp \
    contacteditors/cntcommondetailviewitem.cpp \
    contacteditors/cntcompanyeditormodel.cpp \
    contacteditors/cntdetaileditor.cpp \
    contacteditors/cntdetailmodelitem.cpp \
    contacteditors/cntdateeditormodel.cpp \
    contacteditors/cntdateeditorviewitem.cpp \
    contacteditors/cntdetaileditormodel.cpp \
    contacteditors/cntdetailpopup.cpp \
    contacteditors/cnteditorfactory.cpp \
    contacteditors/cnteditview.cpp \
    contacteditors/cnteditview_p.cpp \
    contacteditors/cnteditviewdetailitem.cpp \
    contacteditors/cnteditviewheadingitem.cpp \
    contacteditors/cnteditviewitembuilder.cpp \
    contacteditors/cnteditviewlistmodel.cpp \
    contacteditors/cnteditviewseparator.cpp \
    contacteditors/cntemaileditormodel.cpp \
    contacteditors/cntemaileditorviewitem.cpp \
    contacteditors/cntfamilyeditormodel.cpp \
    contacteditors/cntnameeditormodel.cpp \
    contacteditors/cntnoteeditormodel.cpp \
    contacteditors/cntnoteeditorviewitem.cpp \
    contacteditors/cntphonenumbermodel.cpp \
    contacteditors/cntphonenumberviewitem.cpp \
    contacteditors/cnturleditormodel.cpp \
    contacteditors/cnturleditorviewitem.cpp \  
    collections/cntcollectionlistmodel.cpp \
    collections/cntcollectionlistmodelworker.cpp \
    collections/cntcollectionview.cpp \
    collections/cntgroupactionsview.cpp \
    collections/cntgroupdeletepopup.cpp \
    collections/cntgroupdeletepopupmodel.cpp \
    collections/cntgroupeditormodel.cpp \
    collections/cntgroupmemberview.cpp \
    collections/cntgroupmemberview_p.cpp \
    collections/cntfavoritesmemberview.cpp \
    collections/cntfavoritesview.cpp \ 
    views/cntactionmenubuilder.cpp \
    views/cntbaseselectionview.cpp \
    views/cnthistoryview.cpp \
    views/cnthistoryviewitem.cpp \
    views/cntimageeditorview.cpp \
    views/cntimportsview.cpp \
    views/cntmycardview.cpp \
    views/cntnamesview.cpp \
    views/cntnamesview_p.cpp \
    views/cntsettingsmodel.cpp \
    views/cntsettingsview.cpp \   
    widgets/cntactionpopup_p.cpp \
    widgets/cntactionpopup.cpp \
    widgets/cntfetchcontactpopup.cpp \
    widgets/cntfetchmarkall.cpp \
    widgets/cntimagelabel.cpp 
   
DOCML += resources/contacts_contactcard.docml \
    resources/contacts_ev.docml \
    resources/contacts_mc.docml \
    resources/contacts_if.docml \
    resources/contacts_list.docml \
    resources/contacts_namelist.docml \
    resources/contacts_groupactions.docml \
    resources/contacts_history.docml \
    resources/contacts_editor.docml \
    resources/contacts_sim.docml \
    resources/contacts_favorite.docml \
    resources/contacts_favmember.docml \
    resources/contacts_collections.docml \
    resources/contacts_groupmembers.docml \
    resources/contacts_detail_editor.docml \
    resources/contacts_fetchdialog.docml \
    resources/contacts_settings.docml
    
RESOURCES += resources/cntcommonui.qrc

LIBS += -lhbcore \
    -lxqservice \
    -lqtcontacts \
    -lcntlistmodel \
    -lthumbnailmanagerqt \
    -lcnthistorymodel \
    -lmaptileservice \
    -lqtversit \
    -lcntimageutility \
    -lcntsimutility \
    -lshareui \
    -lpresencecacheqt \
    -lxqsettingsmanager \
    -lxqkeycapture \
    -lafservice

# capability
TARGET.CAPABILITY = CAP_GENERAL_DLL
defBlock = "$${LITERAL_HASH}if defined(EABI)" \
    "DEFFILE ../eabi/cntcommonui.def" \
    "$${LITERAL_HASH}else" \
    "DEFFILE ../bwins/cntcommonui.def" \
    "$${LITERAL_HASH}endif"
MMP_RULES += defBlock