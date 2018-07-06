//#include "window.h"
//#include "vkRenderer.h"
#include <iostream>
#include "systemBalancer.h"
#include "systemFileHandler.h"
#include "fileHandler.h"

int main() {
    //Window w;
    //VkRenderer vk(w);

    //while (!w.ShouldClose()) {
    //        w.dispatchEvents();
    //        vk.update();
    //}

    /*
	FileHandler fs;
	fs.watch("/tmp/lol");

	while (true) {
		fs.pollEvent();
	}*/

System s1("dummy system Example");
s1._jobListTick.push_back([](){ std::cout << "t: 0\n"; });
s1._jobListTick.push_back([](){ std::cout << "t: 1\n"; });
s1._jobListTick.push_back([](){ std::cout << "t: 2\n"; });
s1._jobListTick.push_back([](){ std::cout << "t: 3\n"; });
s1._jobListTick.push_back([](){ std::cout << "t: 4\n"; });
s1._jobListTick.push_back([](){ std::cout << "t: 5\n"; });
s1._jobListTick.push_back([](){ std::cout << "t: 6\n"; });
s1._jobListTick.push_back([](){ std::cout << "t: 7\n"; });

s1._jobListEvent.push_back([](){ std::cout << "e: 0\n";  });
s1._jobListEvent.push_back([](){ std::cout << "e: 1\n";  });
s1._jobListEvent.push_back([](){ std::cout << "e: 2\n";  });
s1._jobListEvent.push_back([](){ std::cout << "e: 3\n";  });
s1._jobListEvent.push_back([](){ std::cout << "e: 4\n";  });
s1._jobListEvent.push_back([](){ std::cout << "e: 5\n";  });
s1._jobListEvent.push_back([](){ std::cout << "e: 6\n";  });
s1._jobListEvent.push_back([](){ std::cout << "e: 7\n";  });
    SystemFileHandler s0;
	SystemBalancer sb;
	sb.registerSystem(&s0);
    sb.registerSystem(&s1);
	sb.run();

    return EXIT_SUCCESS;
}
