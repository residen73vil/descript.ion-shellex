//Test if dll loads correctly
//If no errors are printed dll is loade
//TODO: test ole objects 
//g++ loaddll.test.cpp -std=c++11
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
	// Just for testing, don't call the function!!!
    MyFunctionType MyFunction = (MyFunctionType)GetProcAddress(hModule, "DllGetClassObject");
    if (MyFunction == NULL) {
        std::cerr << "Could not locate the function!" << std::endl;
        FreeLibrary(hModule);
        return 1;
    }else{
		std::cout << "The function is located." << std::endl;
	}

    // Call the function
    //MyFunction();

    // Free the DLL
    FreeLibrary(hModule);
    return 0;
}