#ifndef CONFIG_H
#define  CONFIG_H

#include "Common.h"

class ConfigHelper
{
public:
	ConfigHelper() {};
	~ConfigHelper() {};
	virtual void Handler(char *name, char *value) = 0;
};

class Config
{

protected:
	Config():
#ifdef USE_XML
		 m_Doc(NULL),m_Root(NULL),
#endif
#ifdef WIN_XML
		 pReader(NULL),
#endif
		 m_ConfigOk(0)  {};

public:
	virtual ~Config();
	int LoadXml(char *name);

public:
	static Config *GetInstance();
	void Helper(char *paht, ConfigHelper *hlp);
	int  HelperCount(char *paht);
	int IsOk() { return m_ConfigOk; };
protected:
#ifdef USE_XML
	void DumpAttr(xmlAttr *attr, char *path);
#endif
	void Report(char *path, char*name, char *cont);
#ifdef USE_XML
	void ProcessNode(xmlNodePtr node);
	void ProcessNodeHelper(xmlNodePtr node, ConfigHelper *hlp);
	void WalkTree(xmlNodePtr node);
#endif
	static Config *s_Instance_p;
#ifdef USE_XML
    xmlDoc  *m_Doc;
    xmlNode *m_Root;
#endif
#ifdef WIN_XML
	IStream*		pFileStream;
	IXmlReader*		pReader;
	XmlNodeType		nodeType;
#endif
    int      m_ConfigOk;

};


#endif
