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

TEMPLATE = subdirs

SUBDIRS  += t_onlinehandler \
t_baiduplugin \ 
t_bingplugin \
t_googleplugin	   
           
CONFIG  += ordered

symbian: {
    CONFIG += symbian_test
BLD_INF_RULES.prj_exports += "testdb/providerdatabase.sq /epoc32/winscw/c/private/20022F35/providerdatabase.sq" \
								 "testdb/providerdatabase.sq /epoc32/data/z/private/20022F35/providerdatabase.sq"
}
