#ifndef UNKNOWN_LOG_H
#define UNKNOWN_LOG_H

// PLACEMENT: provisional (D137): sits between cfaction_database.cpp (0x7E4BE0-0x7E5E50) and distance_calc_client.cpp (0x7E6060-0x7E6400), with no asserts or __FILE__ string in client.exe

void Unknown_7E5F70();											// NAME 1 [Unknown_7E5F70]: neutral (D10); resets enb_client.log
int Unknown_Log(const char *tag, const char *format, ...);		// NAME 1 [Unknown_Log]: 0x7E5F90, the 'HANDOFF:INF' / 'LOGIN:INF' log

#endif
