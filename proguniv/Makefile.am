if OS_WINDOWS64
all: ProgUniv.exe
endif

if OS_WINDOWS32
all: ProgUniv.exe
endif

if OS_LINUX
all: ProgUniv
endif

bin_PROGRAMS=ProgUniv
ProgUniv_SOURCES=src/ApolloV.cc src/ATMELGenericSerialIF.cc\
	src/ChipAT90S2313.cc src/ChipAT90S8515.cc src/ChipAT90Usb1286.cc\
	src/ChipATMega168.cc src/ChipATMega328.cc src/ChipATMega32U4.cc src/ChipATMega48.cc\
	src/ChipATMega644.cc src/ChipATMega2560.cc\
	src/ChipATMega88.cc src/ChipATMega8.cc src/ChipATMegaxx8.cc\
	src/ChipATTiny25.cc src/ChipATTiny261.cc src/ChipATTiny45.cc src/ChipATTiny461.cc\
        src/ChipATTiny85.cc src/ChipATTiny861.cc src/ChipATTinyx5.cc src/ChipATTinyx61.cc\
	src/Chip.cc src/Ficheiro.cc src/Intelhex16.cc src/Intelhex.cc src/ChipOptionsFileRWManager.cc\
	src/lista.cc src/LPTInterfaceControl.cc\
	src/MCHIP14Generic.cc src/Memoria.cc src/MemoryInterceptorProxy.cc src/PIC16F874.cc\
	src/Programmer.cc src/ProgrammerFactory.cc src/ProgUniv.cc src/Property.cc src/RealTime.cc\
	src/TimerIF.cc src/TimerStdStrategy.cc\
        src/LPTWinLowLevel.cc src/LPTLowLevelInterface.cc src/LPTLinuxLowLevel.cc\
        src/ATMELGenericSerial.cc src/ATMELGenericSerialPagedMem.cc\
        src/GlobalOptions.cc

if OS_LINUX
ProgUniv_CXXFLAGS=$(AM_CXXFLAGS) -Ilibs/InpOutBinaries -std=c++0x
ProgUniv_LDADD=$(LIBOBJS)
endif

if OS_WINDOWS64
ProgUniv_CXXFLAGS=$(AM_CXXFLAGS) -Ilibs/InpOutBinaries -std=c++0x -linpoutx64
ProgUniv_LDFLAGS=-L"libs/InpOutBinaries/x64" $(AM_LDFLAGS)
ProgUniv_LDADD = $(LIBOBJS) -linpoutx64
endif

if OS_WINDOWS32
ProgUniv_CXXFLAGS=$(AM_CXXFLAGS) -Ilibs/InpOutBinaries -std=c++0x 
ProgUniv_LDFLAGS=-L"libs/InpOutBinaries/Win32" $(AM_LDFLAGS)
ProgUniv_LDADD = $(LIBOBJS) -linpout32
endif
