# descript.ion-shellex

## Building, installing and testing

### Debug build
	make clean
	make

Default build is debug build, remember that it may add some additional dependencies,
especially libwinpthread-1.dll which in my case didn't produce any errors just 
silently prevented dll from being loaded.

By default logs are printed into c:\Logs\dbg#.log

Although you can debug explorer with gdb, you probably don't want to, so you'll have 
to relay on debut logs. In order to replace you dll in the system you will have to
kill explorer in the task manager and start it again, or use other means like 
holding down **Ctrl+Alt+Shift** and click **Cancel** in the Shutdown menu. 


### Release build
	make clean
	make BUILD_TYPE=release

### Testing
Some tests may be found in  :file_folder:*tests* folder.