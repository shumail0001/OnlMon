#ifndef __PMONITORINTERFACE_H__
#define __PMONITORINTERFACE_H__

#include <string>

int setup_server();
void handleconnection(void *arg);
void handletest(void *arg);
int send_message(const int severity, const std::string &msg);

#endif /* __PMONITORINTERFACE_H__ */
