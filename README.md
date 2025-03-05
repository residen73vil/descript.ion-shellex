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


### Release build
	make clean
	make BUILD_TYPE=release

### Testing
Some tests may be found in  :file_folder:*tests* folder.

# Draft version 2 is ready!

Basic functionality is finally working, experemntal settings and error messages singleton working.

## Goals for now are:

1. Port to 32 bits.
2. Add more tests, and make them more automatic.
3. Try and compile in MS Visual c++ 6.
4. Refactor code.
5. Make it run on win 98, win 95 if possible.

## Milestones are:

1. Add settings, and error messages.
2. Polish user interface (squeeze everything into one property sheet page).
3. Add column with comments in the explorer.
4. Add multi line comments support, in TotalCommander and DoubleCommander style.
5. Multi language support
6. Release with installer and stuff. 


## Known issues:

- New file creation is not really robust.
- There is a chance for bugs in new line detection in some codepages.
- Parts of the app are not thread safe although it is not critical.