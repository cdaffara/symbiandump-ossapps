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
# Description: Project file for building videocollectionwrapper tests
#
#

TEMPLATE = subdirs

CONFIG += ordered
                           
SUBDIRS += testvideocollectionclient       
SUBDIRS += testvideocollectionlistener       
SUBDIRS += testvideocollectionwrapper_p   
SUBDIRS += testvideodatacontainer                 
SUBDIRS += testvideodeleteworker              
SUBDIRS += testvideomodel                               
SUBDIRS += testvideomodel_p                             
SUBDIRS += testvideoproxymodelallvideos   
SUBDIRS += testvideoproxymodelcollections
SUBDIRS += testvideoproxymodelcontent        
SUBDIRS += testvideoproxymodelgeneric       
SUBDIRS += testvideothumbnaildata               
SUBDIRS += testvideothumbnaildata_p           
SUBDIRS += testvideothumbnailfetcher       