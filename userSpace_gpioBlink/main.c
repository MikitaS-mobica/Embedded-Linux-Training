#include <fcntl.h>	// Works great with writing to the device file
#include <stdbool.h>
#include <stdio.h>	// Works glitchy with writing to the device file, requires constant fclose/fopen to work
#include <string.h>
#include <unistd.h>


static const char _gpioDriverPath[] = "/sys/class/rpi-learning-class/rpi-gpio-module/value";
static const char _gpioValules[2][2] = {"0", "1"};


int main()
{
	int state = 0;
	int i;
	int gpioDriver;

	gpioDriver = open(_gpioDriverPath, O_RDWR);
	if(gpioDriver == -1)
	{
		printf("%s: unable to open file %s\r\n", __func__, _gpioDriverPath);
		return 0;
	}
	else
		printf("file %s opened\r\n", _gpioDriverPath);

	for(i = 1; i <= 20; i++)
	{
		write(gpioDriver, _gpioValules[i & 1], sizeof(_gpioValules[i & 1]));
		state++;
		usleep(100000);
	}

	close(gpioDriver);
	printf("file %s closed\r\n", _gpioDriverPath);
	return 0;
}
