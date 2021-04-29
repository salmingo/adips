bin_PROGRAMS=adips
adips_SOURCES=GLog.cpp ADIProcess.cpp ADIReduce.cpp AAstrometry.cpp \
              APhotometry.cpp AFindPV.cpp ADIWorkFlow.cpp adips.cpp

if DEBUG
  AM_CFLAGS = -g3 -O0 -Wall -DNDEBUG
  AM_CXXFLAGS = -g3 -O0 -Wall -DNDEBUG
else
  AM_CFLAGS = -O3 -Wall
  AM_CXXFLAGS = -O3 -Wall
endif

adips_LDFLAGS = -L/usr/local/lib
adips_LDADD = -lm -lcfitsio 
if LINUX
adips_LDADD += -lboost_system-mt-x64 -lboost_thread-mt-x64 -lboost_date_time-mt-x64 \
               -lboost_chrono-mt-x64 -lboost_regex-mt-x64 -lboost_filesystem-mt-x64 -lrt
endif
if OSX
adips_LDADD += -lboost_system-mt -lboost_thread-mt -lboost_date_time-mt -lboost_chrono \
               -lboost_regex-mt -lboost_filesystem-mt
endif
