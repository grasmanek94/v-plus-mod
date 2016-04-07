#include <iostream>
#include <thread>

int main()
{
	std::cout << "Plz add some light" << std::endl;
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	return 0;
}
