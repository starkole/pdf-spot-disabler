# Copyright (C) 2013 Pavlo Oleshkevych <oleshkevych@gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = reconf configure
SUBDIRS = m4 src doc
bin_PROGRAMS = pdf-spot-disabler
pdf_spot_disabler_SOURCES = src/getopt_pp.cpp src/getopt_pp.h src/pdf-spot-disabler.cpp src/libpodofo.a
pdf_spot_disabler_LDADD = -l:$(top_srcdir)/src/libpodofo.a -lfreetype \
                          -lcrypto -lfontconfig -ljpeg -lz -lssl -lidn