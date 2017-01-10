#include "Common.h"

#include "Config.h"


class ButtonHelper: public ConfigHelper
{
	virtual void Handler(char *name, char *value) {
		printf("Button Handler: '%s' => '%s'\n", name, value);
	}
};

#if 1
#include "app_helper.h"
#else
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
                }
	}
	void Clear()
	{
		m_Width = -1;
		m_Height = -1;
		m_Tess = 1;
	}
	void Dump()
	{
		printf("DUMP %dx%d T:%d\n", m_Width, m_Height, m_Tess);
	}
	int m_Width;
	int m_Height;
	int m_Tess;

};
#endif

#if 1
#include "button_helper.h"
#else
class ButtonHelper2: public ConfigHelper
{
public:
	ButtonHelper2()
	{
		Clear();
	}
	~ButtonHelper2() {};
        virtual void Handler(char *name, char *value) {
                printf("Button Handler: '%s' => '%s'\n", name, value);
		if (strcmp(name, "action")==0) {
			m_Action = value;
		} else if (strcmp(name, "image")==0) {
                        m_FileName = value;
                } else if (strcmp(name, "posx")==0) {
                        m_PosX = atoi(value);
                } else if (strcmp(name, "posy")==0) {
                        m_PosY = atoi(value);
                }

        }
	void Clear()
	{
		m_Action.clear();
		m_FileName.clear();
		m_PosX = -1;
		m_PosY = -1;
	}

	void Dump()
	{
		printf("DUMP: %d,%d %s %s\n", m_PosX, m_PosY, m_Action.c_str(), m_FileName.c_str());
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
		
};
#endif


int main( int argc, char *argv[] ) {
  Config *t_Config = Config::GetInstance();

  if (t_Config->LoadXml("grfxEmul.xml")) {
    return -1;
  }
  int Count = t_Config->HelperCount("/grfxEmul/home_hud/button");
  printf("Count=%d\n", Count);
  Count = t_Config->HelperCount("/grfxEmul/ibad_test/button");
  printf("Count=%d\n", Count);


  ButtonHelper m_Bt1;
  t_Config->Helper("/grfxEmul/home_hud/button[1]", &m_Bt1);
  ButtonHelper2 m_Bt2;
  t_Config->Helper("/grfxEmul/home_hud/button[1]", &m_Bt2);
  m_Bt2.Dump();
  m_Bt2.Clear();
  t_Config->Helper("/badgrfxEmul/home_hud/button[1]", &m_Bt2);
  m_Bt2.Dump();

   ApplicationHelper m_App;
  t_Config->Helper("/grfxEmul/application", &m_App);
  m_App.Dump();
  return 0;
}
