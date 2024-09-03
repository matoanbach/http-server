#include "http.h"

int main(int argc, char **argv)
{
  int ret = 0;
  HTTP *http_server = HTTP::getInstance();
  ret = http_server->run(argc, argv);
  return ret;
}