#ifndef ONLMONSERVER_PMONITORINTERFACE_H
#define ONLMONSERVER_PMONITORINTERFACE_H

#include <string>

int setup_server();
void handleconnection(void *arg);
void handletest(void *arg);
int send_message(const int severity, const std::string &msg);

#endif /* ONLMONSERVER_PMONITORINTERFACE_H */
