bin_PROGRAMS=adips
adips_SOURCES=GLog.cpp ADIWorkFlow.cpp ADIReduce.cpp AMatchCatalog.cpp \
              AFluxCalibrate AMotioAssociate.cpp adips.cpp

if DEBUG
  AM_CFLAGS = -g3 -O0 -Wall -DNDEBUG
  AM_CXXFLAGS = -g3 -O0 -Wall -DNDEBUG
else
  AM_CFLAGS = -O3 -Wall
  AM_CXXFLAGS = -O3 -Wall
endif

adips_LDFLAGS = -L/usr/local/lib
BOOST_LIBS = -lboost_system-mt -lboost_date_time-mt -lboost_filesystem-mt -lboost_chrono -lboost_regex-mt
adips_LDADD = ${BOOST_LIBS} -lm -lpthread -lcfitsio 
if LINUX
adips_LDADD += -lboost_thread -lrt
endif
if OSX
adips_LDADD += -lboost_thread-mt
endif
