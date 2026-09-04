                        descript.ion-shellex

This is a windows explorer extension that adds ability to read and edit 
comments in descript.ion format similar to Total Commander, Double Commander, 
Far Manager, etc.
It adds corresponding tab in file's properties sheet, and, on pre Vista, 
Windows NT there also exists a column with the comments, which unfortunately 
is not really feasible after changes Vista introduced.

Installation:
  To install just run "install.bat".
  It will copy appropriate version into your windows folder, and register it.

 By hand:
  You can also copy file that fits you processor architecture to any folder
  (windows folder recommended) and run "rundll32 file.dll,install" in order 
  to install it. You can later uninstall with "rundll32 file.dll,uninstall"
  or from Programs And Components.

  You can also register/unregister by standard means of regsrv32
  "regsrv32 file.dll" to register the dll, "regsrv32 /u file.dll" 
  to unregister, but it won't add an uninstaller in Programs And Components. 

If you found a bug, want to say thanks, provide feedback, request a feature
contact me on:
  mailto:residen73vil@gmail.com
  https://github.com/residen73vil



Copyright 2026 ResE




The 2-Clause BSD License:

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  