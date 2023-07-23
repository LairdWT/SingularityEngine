#include "SEApp/SEApp.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() 
{

	SE::SEApp app{};

	try 
	{
		app.run();
	} 
	catch (const std::exception& exception) 
	{
		std::cerr << exception.what() << '\n';
		return EXIT_FAILURE;
	}

	return 0;
}