#ifndef ONLMONCLIENT_ONLMONDRAW_H
#define ONLMONCLIENT_ONLMONDRAW_H

#include <set>
#include <string>

class TPad;

class OnlMonDraw
{
 public:
  OnlMonDraw(const std::string &name = "NONE");
  virtual ~OnlMonDraw() {}

  virtual int Init() { return 0; }
  virtual int Draw(const std::string &what = "ALL");
  virtual int SavePlot(const std::string &what = "ALL", const std::string &type = "png");
  virtual int MakePS(const std::string &what = "ALL");
  virtual int MakeHtml(const std::string &what = "ALL");
  const std::string Name() const { return ThisName; }
  void Verbosity(const int i) { verbosity = i; }
  int Verbosity() const { return verbosity; }
  bool isHtml() const {return make_html;}
  void isHtml(const bool b) {make_html = b;}
  virtual void AddServer(const std::string &server) {m_ServerSet.insert(server);}
  std::set<std::string>::const_iterator ServerBegin() {return m_ServerSet.begin();}
  std::set<std::string>::const_iterator ServerEnd() {return m_ServerSet.end();}
  
protected:
  virtual int DrawDeadServer(TPad *transparent);
  int verbosity {0};
  bool make_html {false};
  std::string ThisName;
  std::set<std::string> m_ServerSet;
};

#endif /* ONLMONCLIENT_ONLMONDRAW_H */
