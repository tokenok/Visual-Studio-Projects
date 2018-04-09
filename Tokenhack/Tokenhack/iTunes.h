#ifndef itunes_header_guard
#define itunes_header_guard

#include <string>

void init_itunes_com();
void itunes_release();
void Iplaypause();
void Inext();
void Iprev();
void Ivolu();
void Ivold();
std::string Igetcursong();
std::string Igetcurartist();


#endif