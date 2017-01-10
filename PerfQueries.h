#ifndef PERFQUERY_H
#define PERFQUERY_H


// Custom part - start - Need GL* headers
#include "X11Window.h"
// Custom part - end 

#include "PerfQueriesINTEL.h"

class PerfQuery
{
public:
     typedef struct {
     std::string name;
     GLuint      handle;
     int         size;
     } handle_data;
     typedef struct {
     int         hd;
     std::string name;
     int         type;
     int         pos;
     } counter_data;

protected:
     PerfQuery():
	glgetqueryidbynameintel(NULL),
	getperfcounterinfointel(NULL),
	getperfqueryinfointel(NULL),
	createperfqueryintel(NULL),
	deleteperfqueryintel(NULL),
	beginperfqueryintel(NULL),
	endperfqueryintel(NULL),
	getperfquerydataintel(NULL),
	getfirstperfqueryidintel(NULL),
	getnextperfqueryidintel(NULL),
	m_DataBlock(NULL),
	m_EndCount(0),
	m_LastTime(0),
	m_RefTime(0),
	m_Ok(0),
	m_Cvs(0)
  {};
public:
	virtual ~PerfQuery();
	int Init();
	char *GetCounterData(int a);
	char *GetCounterDataF(int a);
	char *GetCounterInfo(int a);
        int   GetCounterDataSize(int a);
	int Begin();
	int End();
	int Get();
	int Dump();
        unsigned long long GetMicroTime();
	int EndGetDump() {
         if (!m_Ok) return -1;
	 End();
         Get();
         Dump();
        }
public:
	static PerfQuery *GetInstance();
protected:
	static PerfQuery *s_Instance_p;
        char *GetValue(unsigned char*, int pos, int type);

PFNGLGETQUERYIDBYNAMEINTEL    glgetqueryidbynameintel;
PFNGETPERFCOUNTERINFOINTEL    getperfcounterinfointel;
PFNGETPERFQUERYINFOINTEL      getperfqueryinfointel;
PFNCREATEPERFQUERYINTEL       createperfqueryintel;
PFNDELETEPERFQUERYINTEL       deleteperfqueryintel;
PFNBEGINPERFQUERYINTEL        beginperfqueryintel;
PFNENDPERFQUERYINTEL          endperfqueryintel;
PFNGETPERFQUERYDATAINTEL      getperfquerydataintel;
PFNGETFIRSTPERFQUERYIDINTEL   getfirstperfqueryidintel;
PFNGETNEXTPERFQUERYIDINTEL    getnextperfqueryidintel;
	int m_Ok;
        int m_Cvs;
	unsigned long long m_EndCount;
	unsigned long long m_LastTime;
	unsigned long long m_RefTime;

	std::vector<handle_data> m_Handles;
	std::vector<counter_data> m_Counters;
        unsigned char *m_DataBlock;
};


#endif
