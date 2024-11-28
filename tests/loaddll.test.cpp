//g++ loaddll.test.cpp dbg.cpp -D_DEBUG -std=c++11
// Main.cpp
#include <windows.h>
#include <iostream>

typedef void (*MyFunctionType)(); // Define a function pointer type

int main() {
    // Load the DLL
    HMODULE hModule = LoadLibraryA("descript.ion-shellex.dll");
    if (hModule == NULL) {
        std::cerr << "Could not load the DLL!" << std::endl;
        return 1;
    }

    // Get the function address
    MyFunctionType MyFunction = (MyFunctionType)GetProcAddress(hModule, "DllGetClassObject");
    if (MyFunction == NULL) {
        std::cerr << "Could not locate the function!" << std::endl;
        FreeLibrary(hModule);
        return 1;
    }

    // Call the function
    //MyFunction();

    // Free the DLL
    FreeLibrary(hModule);
    return 0;
}