#ifndef DATAIO_H_INCLUDED
#define DATAIO_H_INCLUDED

#include "data_and_interpolation.h"

int imgread(const char*, const char*);              // odczyt obrazow wejsciowych
int imgwrite(const char*, const char*);             // wypisywanie obrazow wyjsciowych
int add_cell(const char*);                          // dodanie nowego obrazu wejsciowego do listy

int check_tmplt(const char*, const char*);          // sprawdzenie poprawnosci szablonu nazw obrazow do odzczytu/wypisywania
void extract_tmplt(char*, const char*, Uint32);     // przeksztalcenie szablonu i numeru obrazu w nazwu jego pliku
Uint32 convert_to_int(const char*);                 // konwertacja napisu w liczbe

#endif // DATAIO_H_INCLUDED
