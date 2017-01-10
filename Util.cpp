#include "Util.h"

char *GetTimeString()
{
  static char buffer [80];
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  strftime (buffer,80," %H:%M:%S",timeinfo);
  return buffer;
}
