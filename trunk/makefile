

all: e820.exe


e820.exe: e820.obj e820lib.obj
	LINK /NOLOGO e820.obj e820lib.obj, e820,,,,

e820.obj: e820.c
	cl /c /Zi  /nologo e820.c


e820lib.obj: e820lib.asm
	ml /c /nologo e820lib.asm


clean:
	del *.exe
	del *.obj
	del *.pdb
	del *.map

