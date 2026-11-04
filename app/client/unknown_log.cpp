// Earth & Beyond client (reconstruction): app\client\unknown_log.cpp
// PLACEMENT: provisional (D137): sits between cfaction_database.cpp (0x7E4BE0-0x7E5E50) and distance_calc_client.cpp (0x7E6060-0x7E6400), with no asserts or __FILE__ string in client.exe

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "unknown_log.h"

// 0x7E5F70
void Unknown_7E5F70()											// NAME 1 [Unknown_7E5F70]: neutral (D10)
{
	remove("enb_client.log");
}

// 0x7E5F90
int Unknown_Log(const char *tag, const char *format, ...)		// NAME 1 [Unknown_Log]: neutral (D10)
{
	FILE *f = fopen("enb_client.log", "a");
	if (!f) {
		return 0;
	}
	time_t t = time(0);
	char buf[60];
	strftime(buf, 80, "%m/%d/%y %H:%M:%S", gmtime(&t));
	fprintf(f, "%s: %s:", tag, buf);
	va_list args;
	va_start(args, format);
	int ret = vfprintf(f, format, args);
	va_end(args);
	fprintf(f, "\n");
	fclose(f);
	return ret;
}
