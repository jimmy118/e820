/*
. TODO:
  . Add detail description

. Purpose:
  . Dump E820 map

. Ref:
  . ACPI 4.0 (14.1,INT 15H, E820H - Query System Address Map)
  . http://docs.huihoo.com/gnu_linux/own_os/booting-memory_check_6.htm
  . http://www.uruk.org/orig-grub/mem64mb.html

. Bug contact:
  . Mike Hou (houzz567@gmail.com)

*/



#include <stdio.h>
#include <stdlib.h>


#pragma pack(1)
typedef unsigned char   UINT8;
typedef unsigned int    UINT16;
typedef unsigned long   UINT32;

//
// Native 64-bit integer type is not available in MSVC 16-bit compiler,
// so we create by ourself.
//
typedef struct {
  UINT32 Low;
  UINT32 High;
} UINT64;


typedef struct {
  UINT64  Address;
  UINT64  Length;
  UINT32  Type;
} E820_ENTRY;
#pragma pack()

int gVerbose = 0;

#define MAJOR_V 0
#define MINOR_V 1
#define REV_V   0

extern UINT16 GetE820Entry (UINT32 *_EBX, E820_ENTRY* Entry);

/*
   3         2         1         0
  10987654321098765432109876543210
                       10000000000  1 KB
             100000000000000000000  1 MB
   1000000000000000000000000000000  1 GB
*/
const char* 
GetHumanSize(UINT64 size)
{
  static char HumanSize[20];

  if((size.High==0) && (size.Low==0)) {
    sprintf(HumanSize, "  0");
  }
  else if(size.High != 0) { // >= 4GB
    sprintf(HumanSize, "~ %3ld GB", size.High*4 + (size.Low>>30L));
  }
  else if(size.Low >> 30L) { // >= 1GB
    sprintf(HumanSize, "~ %3ld GB", size.Low>>30);
  }
  else if(size.Low >> 20L) { // >= 1MB
    sprintf(HumanSize, "~ %3ld MB", size.Low>>20);
  }
  else if(size.Low >> 10) {  // >= 1KB
    sprintf(HumanSize, "~ %3ld KB", size.Low>>10);
  }
  else {
    sprintf(HumanSize, "%ld bytes", size.Low);
  }

  return HumanSize;
}


const char*
GetHumanType(UINT32 Type)
{
  static char HumanType[40];

  switch(Type) {
    case 1:
      sprintf(HumanType, "Memory");
      break;
    case 2:
      sprintf(HumanType, "Reserved");
      break;
    case 3:
      sprintf(HumanType, "ACPI");
      break;
    case 4:
      sprintf(HumanType, "NVS");
      break;
    case 5:
      sprintf(HumanType, "Unusuable");
      break;
    case 6:
      sprintf(HumanType, "Disabled");
      break;
    default:
      sprintf(HumanType, "Unknown");
  }

  return HumanType;
}


void
DumpEntry(UINT16 Index, E820_ENTRY *Entry)
{
  printf("%2d: 0x%08lX-%08lX (0x%08lX-%08lX %s) %ld (%s)\n",
      Index,
      Entry->Address.High, 
      Entry->Address.Low, 
      Entry->Length.High, 
      Entry->Length.Low, 
      GetHumanSize(Entry->Length),
      Entry->Type,
      GetHumanType(Entry->Type)
      );
}



void
DumpTitle()
{
  printf("E820 Dump Tool - Version %d.%d.%d - Copyright(C) Mike Hou 2011\n", MAJOR_V, MINOR_V, REV_V);
  printf("----------------------------------------------------------------\n");
  printf("        Base Address                Length             Type    \n");
  printf("-----------------------+------------------------------+---------\n");
}



void
DumpExplain()
{
  printf("----------------------------------------------------------------\n");
  if(gVerbose) {
    printf(
            ". Memory   :This range is available RAM usable by the operating system.       \n"
            ". Reserved :This range of addresses is in use or reserved by the system and is\n"
            "            not to be included in the allocatable memory pool of the operating\n"
            "            system's memory manager.                                          \n"
            ". ACPI     :ACPI Reclaim Memory. This range is available RAM usable by the OS \n"
            "            after it reads the ACPI tables.                                   \n"
            ". NVS      :ACPI NVS Memory. This range of addresses is in use or reserve by  \n"
            "            the system and must not be used by the operating system. This range\n"
            "            is required to be saved and restored across an NVS sleep.         \n"
            ". Unusuable:This range of addresses contains memory in which errors have been \n"
            "            detected. This range must not be used by OSPM.                    \n"
            ". Disabled :This range of addresses contains memory that is not enabled.  This\n"
            "            range must not be used by OSPM.                                   \n"
            ". Undefined:Reserved for future use. OSPM must treat any range of this type as\n"
            "            if the type returned was Reserved.                                \n"
        );
  }
  else {
    printf("(type 'e820 -v' for more details)\n");
  }
}


int main(int argc, char **argv)
{
  UINT32      _EBX;
  UINT16      Flag;
  E820_ENTRY  Entry;
  UINT16      Index = 0;

  if(argc==2 && strcmp(argv[1], "-v")==0) {
    gVerbose = 1;
  }

  DumpTitle();
  _EBX = 0;
  while( GetE820Entry(&_EBX, &Entry) )  {
    DumpEntry(Index++, &Entry);
    if(_EBX == 0) { // We arrive the last item...
      break;
    }
  }
  DumpExplain();
  return 0;
}





