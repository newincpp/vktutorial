extern "C" {
#include <stdio.h>
}
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define WIDTH 800
#define HEIGHT 900
GLFWwindow* window;

void initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	puts("initWindow");
}


void cleanup() {
	glfwDestroyWindow(window);
	glfwTerminate(); 
	//vkDestroyDevice(logicgpu, nullptr);
	//vkDestroyInstance(instance, nullptr);
	puts("cleanup");
}

void extentionSupport() {
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	VkExtensionProperties extensions[extensionCount];
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
	printf("extention count: %d\n", extensionCount);
	puts("available extentions:");
	for (VkExtensionProperties& extension : extensions) {
		puts(extension.extensionName);
	}
}

void pickGPU(VkInstance& instance, VkDevice& logicgpu) {
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	VkPhysicalDevice devices[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

	VkPhysicalDeviceFeatures deviceFeatures;
	VkPhysicalDeviceProperties deviceProperties;
	int graphicsQueueIndex = 0;
	for (VkPhysicalDevice& device : devices) {
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		if (deviceFeatures.robustBufferAccess && deviceFeatures.tessellationShader) {
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				printf("choosing: %s\n", deviceProperties.deviceName);
				physicalDevice = device;
			} 
			// most likely to be useless if "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU" is true
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
			VkQueueFamilyProperties queueFamilies[queueFamilyCount];
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);
			for (VkQueueFamilyProperties& queueFamily : queueFamilies) {
				if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					// is capable of displaying things (aka not a compute only device...)
					physicalDevice = device;
					break;
				}
				graphicsQueueIndex++;
			}
		}
	}



	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;


	VkDeviceCreateInfo createInfo = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, //VkStructureType                    sType;
		nullptr,                              //const void*                        pNext;
		0,                                    //VkDeviceCreateFlags                flags;
		1,                                    //uint32_t                           queueCreateInfoCount;
		&queueCreateInfo,                     //const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
		0,                                    //uint32_t                           enabledLayerCount;
		nullptr,                              //const char* const*                 ppEnabledLayerNames;
		0,                                    //uint32_t                           enabledExtensionCount;
		nullptr,                              //const char* const*                 ppEnabledExtensionNames;
		&deviceFeatures                       //const VkPhysicalDeviceFeatures*    pEnabledFeatures;
	};
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicgpu) != VK_SUCCESS) {
		puts("this sounds bad =(");
	}
}

void initVK() {
	VkInstance instance;
	VkDevice logicgpu;
	VkApplicationInfo appInfo = { 
		VK_STRUCTURE_TYPE_APPLICATION_INFO, //VkStructureType    sType;
		nullptr,                            //const void*        pNext;
		"Hello Triangle",                   //const char*        pApplicationName
		VK_MAKE_VERSION(1, 0, 0),           //uint32_t           applicationVersion
		"No Engine",                        //const char*        pEngineName;
		VK_MAKE_VERSION(1, 0, 0),           //uint32_t           engineVersion;
		VK_API_VERSION_1_0,                 //uint32_t           engineVersion;
		};
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	#ifdef ENABLE_VALIDATION_LAYERS
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (layerCount == 0) {
		puts("no validation Layer have been found");
	}
	VkLayerProperties availableLayers[layerCount];
	char* enabledLayers[layerCount];
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
	puts("avaliable Validation Layers:");
	unsigned short i = 0;
	for (VkLayerProperties& layer : availableLayers) {
		puts(layer.layerName);
		enabledLayers[i] = layer.layerName;
		++i;
	}
	#else
	uint32_t layerCount = 0;
	const char *const * enabledLayers = nullptr;
	#endif


	VkInstanceCreateInfo createInfo = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, //VkStructureType             sType;
		nullptr,                                //const void*                 pNext;
		0,                                      //VkInstanceCreateFlags       flags;
		&appInfo,                               //const VkApplicationInfo*    pApplicationInfo;
		layerCount,                             //uint32_t                    enabledLayerCount;
		enabledLayers,                          //const char* const*          ppEnabledLayerNames;
		glfwExtensionCount,                     //uint32_t                    enabledExtensionCount;
		glfwExtensions                          //const char* const*          ppEnabledExtensionNames;
	};

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result == VK_SUCCESS) {
		puts("initVK SUCCESS");
	} else {
		puts("initVK FAILED");
	}
	extentionSupport();
	pickGPU(instance, logicgpu);
}

void run() {
	puts("brun");
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	puts("erun");
}

int main() {
	initWindow();
	initVK();
	run();
	cleanup();
}
