#include <onlmon/OnlMonClient.h>

void CreateHostList(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  char node[10];
  if (!online)
    {
      cl->AddServerHost("localhost");   // check local host first
      for (int i = 1; i <= 75; i++)
        {
          if (i < 33 && i > 59 ) // take out reserved machines
            {
              sprintf(node, "va%03d", i);
              cl->AddServerHost(node);      // put all va machines in search list
            }
        }
    }
  else if (online == 1)
    {
      const char *valist = gSystem->Getenv("ONLMON_RUNDIR");
      char valistname[200];
      if (valist)
        {
          sprintf(valistname, "%s/va.list", valist);
          FILE *f = fopen(valistname, "r");
          while (fscanf(f, "%s", node) != EOF)
            {
              cl->AddServerHost(node);      // put monitoring va machines in search list
            }
        }
    }
  else
    {
      cl->AddServerHost("localhost");   // check only local host
    }
}

void CleanUpClient()
{
  OnlMonClient *cl = OnlMonClient::instance();
  delete cl;
  gSystem->Exit(0);
  return;
}
