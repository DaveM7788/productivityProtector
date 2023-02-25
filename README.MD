# Productivity Protector

Productivity Protector is a system tray application that helps you stay productive.

You first create a list of folders that contain the files you should be working on.

Productivity Protector will then periodically poll the filesystem to determine the last 
modified time of the files within your folder list. If no files have been modified 
within the last 15 minutes, you will receive a notification.

# Building

The application has only been built and tested on Windows but it should work on other
platforms as well since it is using Qt and C++.

First, Open the Qt MinGW terminal. It specifically has to be the Qt MinGW terminal or 
else you will most likely get .dll errors. Then run the commands like so. The .exe argument
for windeployqt should be where Qt Creator stores the release mode executables.

```
cd C:\Qt\6.4.2\mingw_64\bin
.\windeployqt.exe "C:\Users\david\cppProj\build-productivityProtector-Desktop_Qt_6_4_2_MinGW_64_bit-Release\productivity_protector.exe"
```
