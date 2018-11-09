#include "window.h"
#include "drawCallManager.h"
#include "vkRenderer.h"

//#include <iostream>
//#include "systemBalancer.h"
//#include "systemDummy.h"
//#include "systemFileHandler.h"
//#include "fileHandler.h"

//#ifdef _WIN32
//#include <Windows.h>
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//	_In_opt_ HINSTANCE hPrevInstance,
//	_In_ LPWSTR    lpCmdLine,
//	_In_ int       nCmdShow) {
//#else
int main() {
//#endif
    //DrawCallManager d("DamagedHelmet.gltf");
    DrawCallManager d("SciFiHelmet.gltf");
    //DrawCallManager d("Duck.gltf");

    Window w;
    VkRenderer vk(w, d);
    
    while (!w.ShouldClose()) {
        w.dispatchEvents();
        vk.update();
    }

	/*
	   FileHandler fs;
	   fs.watch("/tmp/lol");

	   while (true) {
	   fs.pollEvent();
	   }*/


	//SystemFileHandler s0;
	//SystemDummy s1;
	//SystemBalancer sb;
	//sb.registerSystem(&s0);
	//sb.registerSystem(&s1);
	//sb.run();

	return EXIT_SUCCESS;
}
