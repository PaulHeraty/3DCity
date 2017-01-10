#ifndef BUTTON_HELPER_H
#define  BUTTON_HELPER_H
#include "Config.h"

class ButtonHelper2: public ConfigHelper
{
public:
    ButtonHelper2()
    {
        Clear();
    }
    ~ButtonHelper2() {};
    virtual void Handler(char *name, char *value) {
        //printf("Button Handler: '%s' => '%s'\n", name, value);
        if (strcmp(name, "action")==0) {
            m_Action = value;
        } else if (strcmp(name, "image")==0) {
            m_FileName = value;
        } else if (strcmp(name, "posx")==0) {
            m_PosX = atoi(value);
        } else if (strcmp(name, "posy")==0) {
            m_PosY = atoi(value);
        } else if (strcmp(name, "visible")==0) {
            m_Visible = atoi(value);
        }

    }
    void Clear()
    {
        m_Action.clear();
        m_FileName.clear();
        m_PosX = -1;
        m_PosY = -1;
        m_Visible = false;
    }

    void Dump()
    {
        //printf("DUMP: %d,%d %s %s\n", m_PosX, m_PosY, m_Action.c_str(), m_FileName.c_str());
    }
    int IsNotValid()
    {
#if 0
        if (!m_Action.length()) {
            printf("No Action");
            return -1;
        }
#endif
        if (!m_FileName.length()) {
            printf("No FileName\n");
            return -1;
        }
        if ((m_PosX == -1) || (m_PosY == -1)) {
            printf("Wrong Pos\n");
            return -1;
        }
        return 0;
    }
    std::string m_Action;
    std::string m_FileName;
    int m_PosX;
    int m_PosY;
    bool m_Visible;

};
#endif
