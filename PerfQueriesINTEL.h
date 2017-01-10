/*
INTEL CONFIDENTIAL
Copyright 2013 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents related to the
source code ("Material") are owned by Intel Corporation or its suppliers or
licensors. Title to the Material remains with Intel Corporation or its
suppliers and licensors. The Material contains trade secrets and proprietary
and confidential information of Intel or its suppliers and licensors. The
Material is protected by worldwide copyright and trade secret laws and treaty
provisions. No part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in any way
without Intel’s prior express written permission.

No license under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or delivery
of the Materials, either expressly, by implication, inducement, estoppel or
otherwise. Any license under such intellectual property rights must be express
and approved by Intel in writing.
*/

#define GL_PERFQUERY_INTEL_EXTENSION_NAME        "GL_INTEL_performance_queries"

#define GL_PERFQUERY_GLOBAL_CONTEXT_INTEL        0x0
#define GL_PERFQUERY_SINGLE_CONTEXT_INTEL        0x1

#define GL_PERFQUERY_DONOT_FLUSH_INTEL           0x83F9
#define GL_PERFQUERY_FLUSH_INTEL                 0x83FA
#define GL_PERFQUERY_WAIT_INTEL                  0x83FB

#define GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL   0x9402
#define GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL   0x9403
#define GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL    0x9404
#define GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL   0x9405
#define GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL   0x9406

#define GL_PERFQUERY_COUNTER_EVENT_INTEL         0x9407
#define GL_PERFQUERY_COUNTER_DURATION_NORM_INTEL 0x9408
#define GL_PERFQUERY_COUNTER_DURATION_RAW_INTEL  0x9409
#define GL_PERFQUERY_COUNTER_THROUGHPUT_INTEL    0x940A
#define GL_PERFQUERY_COUNTER_RAW_INTEL           0x940B

#define GL_PERFQUERY_QUERY_NAME_MAX_LENGTH_INTEL   0x9410
#define GL_PERFQUERY_COUNTER_NAME_MAX_LENGTH_INTEL 0x9411
#define GL_PERFQUERY_COUNTER_DESC_MAX_LENGTH_INTEL 0x9412


/*****************************************************************************\
PFNGLGETQUERYIDBYNAMEINTEL - pointer to GetPerfQueryIdByNameINTEL.

Description:
    The function returns the integer identifier of the query specified by its
    string identifier (name).

Input:
    char *queryName - string defining query name (type).

Output:
    GLuint *queryId - ID of query identified by queryName.

\*****************************************************************************/
typedef void (* PFNGLGETQUERYIDBYNAMEINTEL) (GLbyte *queryName,
                                                     GLuint *queryId);


/*****************************************************************************\
PFNGETPERFCOUNTERINFOINTEL - pointer to GetPerfCounterInfoINTEL.

\*****************************************************************************/
typedef void (* PFNGETPERFCOUNTERINFOINTEL)   (GLuint queryId,
                                                       GLuint counterId,
                                                       const GLuint counterNameLength,
                                                       char *counterName,
                                                       const GLuint counterDescLength,
                                                       char *counterDesc,
                                                       GLuint *counterOffset,
                                                       GLuint *counterDataSize,
                                                       GLuint *counterTypeEnum,
                                                       GLuint *counterDataTypeEnum,
                                                       GLuint64 *rawCounterMaxValue);

/*****************************************************************************\
PFNGETPERFCOUNTERINFOINTEL - pointer to GetPerfCounterInfoINTEL.

\*****************************************************************************/
typedef void (* PFNGETPERFQUERYINFOINTEL)   (GLuint queryId,
                                                     const GLuint queryNameLength,
                                                     char *queryName,
                                                     GLuint *dataSize,
                                                     GLuint *noCounters,
                                                     GLuint *maxInstances,
                                                     GLuint * capsMask);

/*****************************************************************************\
PFNCREATEPERFQUERYINTEL- pointer to CreatePerfQueryINTEL.

Description:
    Creates Intel performance queries object of requested type.

Input:
    GLuint queryId  - query type.

    GLuint flags    - determines if query is targeted to single or multiple
                      contexts.

Output:
    queryHandle     - unique query handle (identifier).
\*****************************************************************************/
typedef void (* PFNCREATEPERFQUERYINTEL)    (GLuint queryId,
                                                     GLuint *queryHandle);


/*****************************************************************************\
PFNDELETEPERFQUERYINTEL - pointer to DeletePerfQueryINTEL.

Description:
    Delete Intel performance query.

Input:
    GLuint queryHandle  - query handle (identifier).

Output:
    None
\*****************************************************************************/
typedef void (* PFNDELETEPERFQUERYINTEL)    (GLuint queryHandle);


/*****************************************************************************\
PFNBEGINPERFQUERYINTEL - pointer to BeginPerfQueryINTEL.

Description:
    Start new measurement session.

Input:
    GLuint queryHandle  - query handle (identifier).

Output:
    None
\*****************************************************************************/
typedef void (* PFNBEGINPERFQUERYINTEL)     (GLuint queryHandle);


/*****************************************************************************\
PFNGETPERFQUERYDATAINTEL - pointer to EndPerfQueryINTEL.

Description:
    End the measurement session started.

Input:
    GLuint queryHandle  - query handle (identifier).

Output:
    None
\*****************************************************************************/
typedef void (* PFNENDPERFQUERYINTEL)       (GLuint queryHandle);



/*****************************************************************************\
Function:
    PFNGETPERFQUERYDATAINTEL - pointer to GetPerfQueryDataINTEL.

Description:
    Return the values of counters which have been measured within the query
    session.

Input:
    GLuint queryHandle  - query handle (identifier).

    GLuint flags        - determines approach to data collection. Possible
                          values:
                            PERFQUERY_DONOT_FLUSH_INTEL - non blocking -
                                - returns data if available and quits
                                immediately.
                            GL_PERFQUERY_FLUSH_INTEL - non blocking - returns
                                data if available or submits a flush command
                                and quits immediately.
                            PERFQUERY_WAIT_INTEL - blocking - waits for data
                                to be available and then returns counters
                                values.
    GLsizei dataSize    - input buffer size.
    void   *data        - data buffer.

Output:
    GLuint *charsWritten - amount of data returned in data buffer.
\*****************************************************************************/
typedef void (* PFNGETPERFQUERYDATAINTEL)   (GLuint queryHandle,
                                                     GLuint flags,
                                                     GLsizei dataSize,
                                                     GLvoid *data,
                                                     GLuint *charsWritten);


/*****************************************************************************\

Function:
    PFNGETFIRSTPERFQUERYIDINTEL  - pointer to GetFirstPerfQueryIdINTEL

Description:
    Returns the integer identifier of the first Performance Query
    that is supported on given platform. If hardware doesn't
    support any performance queries, then the value of 0 is returned, that
    means 0 is invalid performance query ID. In that case also INVALID_OPERATION
    error is raised. If queryId  pointer is equal to 0, INVALID_VALUE error
    is raised.

Input:
    none

Output:
    GLuint *queryId - identifier of the first query counter.

\*****************************************************************************/
typedef void (* PFNGETFIRSTPERFQUERYIDINTEL) (GLuint *queryId);

/*****************************************************************************\

Function:
    PFNGETNEXTPERFQUERYIDINTEL - pointer to GetNextPerfQueryIdINTEL

Description:
    Returns the integer identifier of next supported Performance Query.
    If specified performance query identifier is invalid then the value of 0 is 
    returned and INVALID_VALUE error is raised. If nextQueryId  pointer is 
    equal to 0, INVALID_VALUE error is raised. If query identified by queryId 
    is the last query available the value of 0 is returned.

Input:
    GLuint queryId - current query identifier.

Output:
    GLuint *nextQueryId - identifier of the next query.

\*****************************************************************************/
typedef void (* PFNGETNEXTPERFQUERYIDINTEL) (GLuint queryId, GLuint *nextQueryId);


