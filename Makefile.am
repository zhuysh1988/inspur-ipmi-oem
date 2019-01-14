AM_DEFAULT_SOURCE_EXT = .cpp

if HAVE_SYSTEMD
systemdsystemunit_DATA = \
  gbmc-psudddd-hardreset.target
endif

libinspurcmdsdir = ${libdir}/ipmid-providers
libinspurcmds_LTLIBRARIES = libinspurcmds.la
libinspurcmds_la_SOURCES = main.cpp psucommands.cpp i2c.cpp 24cXX.c

libinspurcmds_la_LDFLAGS = \
	$(SDBUSPLUS_LIBS) \
	$(PHOSPHOR_LOGGING_LIBS) \
	-lstdc++fs \
	-version-info 0:0:0 -shared

libinspurcmds_la_CXXFLAGS = \
	$(SDBUSPLUS_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS) \
	-flto
