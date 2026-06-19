# descript.ion-shellex

## Building, installing and testing

### Debug build
	make clean
	make

Default build is debug build, remember that it may add some additional dependencies,
especially libwinpthread-1.dll which in my case didn't produce any errors just 
silently prevented dll from being loaded.

By default logs are printed into c:\Logs\dbg#.log

You can debug explorer with gdb, kill it in the task manager and  run
	gdb explorer
then set a breakpoint somewhere in the code, run it and trigger breakpoint containing action,
remember that desktop will hang up.
In order to replace you dll in the system you will have to kill explorer in the task manager 
and start it again or just restart it, you can also use other means like holding down 
**Ctrl+Alt+Shift** and click **Cancel** in the Shutdown menu. 

#### Build (only debug one for now) on MSVC600 can be done using:
	nmake -f Makefile.msvc clean
	nmake -f Makefile.msvc

Logs are now printed into debug console (in a bit broken form), you can debug using windbg
pdb file should be placed in %windir%\symbols\dll\
also some extra dll are required for debug build

### Release build
	make clean
	make BUILD_TYPE=release

### Testing
Some tests may be found in  :file_folder:*tests* folder.

# Draft version 3 is ready!

Everything more or less works.

## Goals for now are:

1. Add more tests, and make them more automatic.
2. Try and compile in MS Visual c++ for 64 bit.
3. Back port to mingw.
3. Refactor code.
4. Make it run on win 98, win 95 if possible.

## Milestones are:

1. Add column with comments in the explorer.
2. Settings and registration in registry.
2. Add error messages.
3. Multi language support
4. Release with installer and stuff. 


## Known issues:

- New file creation is not really robust.
- There is a chance for bugs in new line detection in some codepages.
- Right click menu seem to disappear withing several seconds when my extension is on and you select several files.
(It is caused by updating selected file, but still keep an eye for it)
