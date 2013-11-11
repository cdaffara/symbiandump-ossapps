######################################################################
# Phone unit test qmake project include 
######################################################################

runtest_target.target = runtest
runtest_target.commands = call "$${EPOCROOT}epoc32/release/winscw/udeb/$${TARGET}.exe"
QMAKE_EXTRA_TARGETS += runtest_target

# for coverage measurement
instrument_target.target = instrument
instrument_target.depends = clean-debug-winscw
instrument_target.commands = ctcwrap -2comp -i d -C "EXCLUDE=*" -C "NO_EXCLUDE=*/src/*.cpp" sbs -c winscw_udeb
QMAKE_EXTRA_TARGETS += instrument_target

runcovtest_target.target = runcovtest
runcovtest_target.depends = instrument
runcovtest_target.commands = call "$${EPOCROOT}epoc32/release/winscw/udeb/$${TARGET}.exe"
QMAKE_EXTRA_TARGETS += runcovtest_target

ctcpost_target.target = ctcpost
ctcpost_target.depends = runcovtest
ctcpost_target.commands = ctcpost mon.sym mon.dat -p profile.txt
QMAKE_EXTRA_TARGETS += ctcpost_target

ctc2html_target.target = ctc2html
ctc2html_target.depends = ctcpost
ctc2html_target.commands = ctc2html -i profile.txt
QMAKE_EXTRA_TARGETS += ctc2html_target  

coverage_target.target = coverage
coverage_target.depends = ctc2html
coverage_target.commands = del MON.sym del MON.dat del ctc*.txt del profile.txt
QMAKE_EXTRA_TARGETS += coverage_target
