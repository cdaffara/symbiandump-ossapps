steps to create sis file:
1. copy rd.cer and rd-key.pem to r:\qt
2. build armv5 udeb for QTIR
3. in this folder, run "createpackage IR1_qt_libs_armv5_udeb.pkg"
4. in this folder, run "createpackage IR2_orbitinput_armv5_udeb.pkg"
5. in folder InternetRadio2.0/install", run "createpackage cenrep.pkg" and "createpackage ir_package.pkg",
   then copy ir_package.sisx to QtInternetRadio/install.
6. in folder PresetServer/sis/presetserver_default, run "createpackage package_Inc6.pkg", then copy         package_inc6.sisx to QtInternetRadio/install.
7. in this folder, run "createpackage ir3_qtinternetradio_armv5_udeb.pkg"

install steps:
1. ir1_qt_libs_armv5_udeb.sisx
2. ir2_orbitinput_armv5_udeb.sisx
3. ir3_QtInternetRadio_armv5_udeb.sisx




