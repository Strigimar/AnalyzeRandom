#include "generator.h"


void Generator::gener_rand(unsigned char* string, const int count)
{
	srand(time(NULL));

	int j = 0;
	for (int i = 0; i < count; i++)
	{
		string[i] = rand() % 256;
	}
}
int Generator::quantum_rand(unsigned char* rand, const int count)
{
	QRBG* rndService;
	try {
		rndService = new QRBG;
	}
	catch (QRBG::NetworkSubsystemError) {
		std::cout << "Network error!" << std::endl;
		return 1;
	}
	catch (...) {
		std::cout << "Failed to create QRBG client object!" << std::endl;
		return 1;
	}

	try {
		rndService->defineServer("random.irb.hr", 1227);
		rndService->defineUser("strigimar", "N9n2N.4132");
	}
	catch (QRBG::InvalidArgumentError e) {
		std::cout << "Invalid hostname/port, or username/password specified!" << std::endl;
		delete rndService;
		return 1;
	}
	byte* buffer = new byte[count];
	try {
		std::cout << "QRBG service returned byte: " << rndService->getByte() << std::endl;
		std::cout << "QRBG service returned integer: " << rndService->getInt() << std::endl;
		std::cout << "QRBG service returned long integer: " << rndService->getLongInt() << std::endl;
		std::cout << "QRBG service returned float: " << rndService->getFloat() << std::endl;
		std::cout << "QRBG service returned double: " << rndService->getDouble() << std::endl;
		std::cout << "Block of " << count << " bytes (" << count / 1024.0 / 1024.0 << " MiB) of data requested..." << std::endl;
		rndService->getBytes(buffer, count);
		std::cout << "QRBG service returned block of " << count << " bytes (" << count / 1024.0 / 1024.0 << " MiB) of data." << std::endl;
		delete rndService;

		char* str = new char[count];
		strcpy(str, (const char*)buffer);
		for (int i = 0; i < count; i++)
		{
			if (str[i] != buffer[i])
			{
				bool fa = false;
			}
		}
		//write_file("quantum_string1.txt", str1);

		return 0;
	}
	catch (QRBG::ConnectError e) {
		printf("Connect error! \n");
	}
	catch (QRBG::CommunicationError e) {
		printf("Communication error! \n");
	}
	catch (QRBG::ServiceDenied e) {
		printf("Service denied! \n", e.ServerResponse, e.RefusalReason);
		printf("--> %s! (%s.) \n", e.why(), e.cure());
	}
	delete rndService;
	return 1;
}