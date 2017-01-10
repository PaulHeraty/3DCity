#ifndef APP_HELPER_H
#define  APP_HELPER_H
#include "Config.h"

class ApplicationHelper : public ConfigHelper
{
public:
	ApplicationHelper() {Clear();};
        virtual void Handler(char *name, char *value) {
                printf("Application Handler: '%s' => '%s'\n", name, value);
		if (strcmp(name, "width")==0) {
                        m_Width = atoi(value);
                } else if (strcmp(name, "height")==0) {
                        m_Height = atoi(value);
                }  else if (strcmp(name, "tesselation_factor")==0) {
                        m_Tess = atoi(value);
                }  else if (strcmp(name, "quadTree_size")==0) {
                        m_quadTreeSize = atoi(value);
                }  else if (strcmp(name, "world_size")==0) {
                        m_World = atoi(value);
                } else if (strcmp(name, "virtual_pos")==0) {
                        m_VP = atoi(value);
                 } else if (strcmp(name, "benchmark_mode")==0) {
                        m_BMM = atoi(value);
                }
	}
	void Clear()
	{
		m_Width = -1;
		m_Height = -1;
		m_World = 0;
		m_Tess = 1;
        m_quadTreeSize = 1;
		m_VP =1;
        m_BMM = 0;
	}
	void Dump()
	{
        printf("DUMP %dx%d T:%d W:%d VP:%d BMM:%d\n", m_Width, m_Height, m_Tess, m_World, m_VP, m_BMM);
	}
	int m_Width;
	int m_Height;
	int m_Tess;
    int m_quadTreeSize;
	int m_World;
	int m_VP;
    int m_BMM;

};
#endif
