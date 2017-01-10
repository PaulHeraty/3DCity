#include "PerfQueries.h"

// #define DEBUG_PERFQUERY

#define LOADFUNC(X) eglGetProcAddress((const char *)X)

typedef GLuint64 UINT64 ;

PerfQuery *PerfQuery::s_Instance_p = (PerfQuery*)NULL;


PerfQuery::~PerfQuery()
{
    if (s_Instance_p) {
        if (m_DataBlock) free(m_DataBlock);
        delete s_Instance_p;
    }
    s_Instance_p = (PerfQuery*)NULL;
}


PerfQuery *PerfQuery::GetInstance()
{
    if (!s_Instance_p)
    {
        s_Instance_p = (PerfQuery*)new PerfQuery();
    }
    return s_Instance_p;
}

unsigned long long PerfQuery::GetMicroTime()
{
  unsigned long long t_Milli;
  struct timeval t_SystemTime;
  gettimeofday (&t_SystemTime, NULL);
  t_Milli = (((unsigned long long) t_SystemTime.tv_sec) * 1000000LL)  + t_SystemTime.tv_usec;
  return  t_Milli;
}


char *PerfQuery::GetValue(unsigned char *data, int pos, int type)
{
  static char buffer[256];
  buffer[0]=0;
  data += pos;
        switch (type) {
        case GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL: 
        {
           GLuint *t = (GLuint*)data;
           sprintf(buffer,"%u", *t); 
           break;
        }
        case GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL:
	{
            UINT64 *t = (UINT64*)data;
	    sprintf(buffer,"%llu", *t);
	}
	break;
        case GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL : 
	{
            float *t = (float*)data;
	    sprintf(buffer,"%f", *t);
	}
	break;
        case GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL:
	{
            double *t = (double*)data;
	    sprintf(buffer,"%f", *t);
	}
	break;
        case GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL:
          sprintf(buffer, "N/A");
        }
        return buffer;
  
}


char *PerfQuery::GetCounterData(int a)
{
        switch (a) {
        case GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL: return "GLuint";
        case GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL: return "UINT64";
        case GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL : return "float";
        case GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL: return "double";
        case GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL: return "BOOL";
        }
        return "???";
}

int PerfQuery::GetCounterDataSize(int a)
{
        switch (a) {
        case GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL: return sizeof(GLuint);
        case GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL: return sizeof(UINT64);
        case GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL : return sizeof(float);
        case GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL: return sizeof(double);
        case GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL: return sizeof(int);
        }
        return 0;
}

char *PerfQuery::GetCounterDataF(int a)
{
        switch (a) {
        case GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL: return "%u";
        case GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL: return "%llu";
        case GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL : return "%f";
        case GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL: return "%f";
        case GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL: return "%d";
        }
        return "???";
}

char *PerfQuery::GetCounterInfo(int a)
{
        switch (a) {
        case GL_PERFQUERY_COUNTER_EVENT_INTEL         : return "EVENT";
        case GL_PERFQUERY_COUNTER_DURATION_NORM_INTEL : return "DURATION_NORM";
        case GL_PERFQUERY_COUNTER_DURATION_RAW_INTEL  : return "DURATION_RAW";
        case GL_PERFQUERY_COUNTER_THROUGHPUT_INTEL    : return "THROUGHPUT";
        case GL_PERFQUERY_COUNTER_RAW_INTEL           : return "RAW";
        }
        return "???";
}

int PerfQuery::Begin()
{
  if (!m_Ok) return -1;
  for (int i=0; i< m_Handles.size(); i++) {
    beginperfqueryintel(m_Handles[i].handle);
  }
  m_RefTime = GetMicroTime();
  return 0;
}

int PerfQuery::End()
{
  if (!m_Ok) return -1;
  m_LastTime = GetMicroTime()-m_RefTime;
  m_EndCount++;
  for (int i=0; i< m_Handles.size(); i++) {
    endperfqueryintel(m_Handles[i].handle);
  }
  return 0;
}

int PerfQuery::Get()
{
  if (!m_Ok) return -1;
  unsigned char *data = m_DataBlock;
  uint written = 0;
  for (int i=0; i< m_Handles.size(); i++) {
    getperfquerydataintel(m_Handles[i].handle, GL_PERFQUERY_WAIT_INTEL, m_Handles[i].size, data, &written);
    data += m_Handles[i].size;
//  printf("%d %d %d\n", i, m_Handles[i].size, written);
  }
  return 0;
}

int PerfQuery::Dump()
{
  if (!m_Ok) return -1;
#ifdef DEBUG_PERFQUERY
  for (int i=0; i< m_Counters.size(); i++) {
    printf("%3d %-40s %8d %8d %8d (%d) %s\n", i, m_Counters[i].name.c_str(), m_Counters[i].hd, m_Counters[i].type, m_Counters[i].pos,GetCounterDataSize(m_Counters[i].type), GetValue(m_DataBlock, m_Counters[i].pos, m_Counters[i].type));
  }
#else

  if (m_Cvs) {
    printf("%llu,%llu,", m_EndCount,  m_LastTime);
    for (int i=0; i< m_Counters.size(); i++) {
      printf("%s,", GetValue(m_DataBlock, m_Counters[i].pos, m_Counters[i].type));
    }
    printf("\n");
  } else {
    printf("S:%llu T:%llu ", m_EndCount,  m_LastTime);
    for (int i=0; i< m_Counters.size(); i++) {
      printf("%s:%s ", m_Counters[i].name.c_str(), GetValue(m_DataBlock, m_Counters[i].pos, m_Counters[i].type));
    }
    printf("\n");
  }
#endif

#if 0
// track abnormal issue - last 4 corrupted
{
 for (int i=160; i<168;i++) {
  printf("%02x ", m_DataBlock[i]);
 }
printf("\n");
}
#endif
  return 0;
}


int PerfQuery::Init()
{

  const GLubyte *s_Ext = glGetString(GL_EXTENSIONS);
  if (strstr((char *)s_Ext, GL_PERFQUERY_INTEL_EXTENSION_NAME)) {
    printf("** PerfQuery Extension found !\n");
    getperfcounterinfointel  = (PFNGETPERFCOUNTERINFOINTEL )LOADFUNC("glGetPerfCounterInfoINTEL");
    getperfqueryinfointel    = (PFNGETPERFQUERYINFOINTEL   )LOADFUNC("glGetPerfQueryInfoINTEL");
    createperfqueryintel     = (PFNCREATEPERFQUERYINTEL    )LOADFUNC("glCreatePerfQueryINTEL");
    deleteperfqueryintel     = (PFNDELETEPERFQUERYINTEL    )LOADFUNC("glDeletePerfQueryINTEL");
    beginperfqueryintel      = (PFNBEGINPERFQUERYINTEL     )LOADFUNC("glBeginPerfQueryINTEL");
    endperfqueryintel        = (PFNENDPERFQUERYINTEL       )LOADFUNC("glEndPerfQueryINTEL");
    getperfquerydataintel    = (PFNGETPERFQUERYDATAINTEL   )LOADFUNC("glGetPerfQueryDataINTEL");
    getfirstperfqueryidintel = (PFNGETFIRSTPERFQUERYIDINTEL)LOADFUNC("glGetFirstPerfQueryIdINTEL");
    getnextperfqueryidintel  = (PFNGETNEXTPERFQUERYIDINTEL )LOADFUNC("glGetNextPerfQueryIdINTEL");
    int query_name_len, counter_name_len, counter_desc_len;
    glGetIntegerv(GL_PERFQUERY_QUERY_NAME_MAX_LENGTH_INTEL, &query_name_len);
    glGetIntegerv(GL_PERFQUERY_COUNTER_NAME_MAX_LENGTH_INTEL, &counter_name_len);
    glGetIntegerv(GL_PERFQUERY_COUNTER_DESC_MAX_LENGTH_INTEL, &counter_desc_len);
#ifdef DEBUG_PERFQUERY
    printf(" GL_PERFQUERY_QUERY_NAME_MAX_LENGTH_INTEL %d\n", query_name_len);
    printf(" GL_PERFQUERY_COUNTER_NAME_MAX_LENGTH_INTEL %d\n", counter_name_len);
    printf(" GL_PERFQUERY_COUNTER_DESC_MAX_LENGTH_INTEL %d\n", counter_desc_len);
#endif
     uint queryId;
     uint nextQueryId;
     getfirstperfqueryidintel(&queryId);
     nextQueryId = queryId;
     char *queryName = (char*)malloc(query_name_len);
     char *counterName = (char*)malloc(counter_name_len);
     char *counterDesc = (char*)malloc(counter_desc_len);
     uint dataSize;
     uint noCounters;
     uint noInstances;
     uint capsMask;
     uint counterOffset;
     uint counterDataSize;
     uint counterTypeEnum;
     uint counterDataTypeEnum;
     GLuint64 rawCounterMaxValue;
     uint queryHandle;
     int CurrentPos = 0;
     handle_data hd;
     counter_data cd;
     while( nextQueryId ) {
        getperfqueryinfointel(nextQueryId,query_name_len,queryName,&dataSize,&noCounters,&noInstances,&capsMask );
        printf(" %s dataSize:%d noCounters:%d noInstances:%d capsMask:%d\n", queryName, dataSize, noCounters, noInstances, capsMask);
        for( int counterId = 1; counterId <= noCounters; counterId++ ) {
          getperfcounterinfointel(nextQueryId, counterId, counter_name_len, counterName, counter_desc_len, counterDesc,&counterOffset, &counterDataSize,&counterTypeEnum, &counterDataTypeEnum,&rawCounterMaxValue );
#if 0
          printf("%s %s;// [%s]\n", GetCounterData(counterDataTypeEnum), counterName,  counterDesc);
#else
          printf("  %s %s;\n", GetCounterData(counterDataTypeEnum), counterName);
#endif
          cd.hd = m_Handles.size();
          cd.name = counterName;
          cd.type = counterDataTypeEnum;
          cd.pos = CurrentPos;
          m_Counters.push_back(cd);
          CurrentPos += counterDataSize;
        }
        createperfqueryintel(nextQueryId, &queryHandle );
        hd.handle = queryHandle;
        hd.name = queryName;
        hd.size = dataSize;
        m_Handles.push_back(hd);
        getnextperfqueryidintel(nextQueryId, &queryId);
        nextQueryId = queryId;
     }
     free(queryName);
     free(counterName);
     free(counterDesc);
     m_DataBlock = (unsigned char*) malloc(CurrentPos+4); // +4 by Security
#ifdef DEBUG_PERFQUERY
     int sz = 0;
     for (int i=0; i< m_Handles.size();i++) {
       printf("%d %-40s %d\n", i, m_Handles[i].name.c_str(), m_Handles[i].size);
       sz +=  m_Handles[i].size;
     }
     printf("%d %d\n", sz, CurrentPos);
     for (int i=0; i< m_Counters.size(); i++) {
       printf("%3d %-40s %8d %8d %8d (%d)\n", i, m_Counters[i].name.c_str(), m_Counters[i].hd, m_Counters[i].type, m_Counters[i].pos,GetCounterDataSize(m_Counters[i].type));
     }
#endif
     if (m_Cvs) {
       printf("S,T,");
       for (int i=0; i< m_Counters.size(); i++) {
         printf("%s,", m_Counters[i].name.c_str());
       }
       printf("\n");
     }
     m_Ok = 1;
  }
  return 0;
}


