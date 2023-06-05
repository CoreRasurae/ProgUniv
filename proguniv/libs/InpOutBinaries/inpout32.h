#ifndef __INPOUT32__
#define __INPOUT32__
//Functions exported from DLL.
//For easy inclusion is user projects.

extern "C"  {
 void	__stdcall  Out32(short PortAddress, short data);
 short 	__stdcall Inp32(short PortAddress);
}

#endif
