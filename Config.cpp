#include <stdlib.h>
#include <stdio.h>

#include "Config.h"

Config *Config::s_Instance_p = (Config*)NULL;

typedef struct {
	std::string name;
	std::string value;
} _pair;
typedef struct {
	std::vector<_pair> args;
	std::string path;
} _item;

std::vector<_item*> item;


Config::~Config()
{
#ifdef WIN_XML
	if (pReader != NULL)
		pReader->Release();
#endif
    s_Instance_p = (Config*)NULL;
}


Config *Config::GetInstance()
{
    if (!s_Instance_p)
    {
        s_Instance_p = (Config*)new Config();
    }
    return s_Instance_p;
}


void Config::Report(char *path, char*name, char *cont)
{
    if (name) {
        //printf( "%s/%s => %s\n", path, name , cont);
    } else {
        //printf( "%s => %s\n", path , cont);
    }
}
#ifdef USE_XML
void Config::DumpAttr(xmlAttr *attr, char *path)
{
    while (attr)
    {
        Report(path, (char*)attr->name , (char*)attr->children->content);
        attr = attr->next;
    }
}

void Config::ProcessNode(xmlNodePtr node)
{
    if (node->type == XML_ELEMENT_NODE) {
        xmlChar *path = xmlGetNodePath(node);
        //printf("%s [%s]\n", path, node->name);
        if (node->children != NULL && node->children->type == XML_TEXT_NODE) {
            xmlChar *contenu = xmlNodeGetContent(node);
            Report((char*)path, NULL , (char*)contenu);
            xmlFree(contenu);
        }
        DumpAttr(node->properties,(char*)path);
        xmlFree(path);
    }

}

void Config::WalkTree(xmlNodePtr node) {
    xmlNodePtr n;

    for (n = node; n != NULL; n = n->next) {
        ProcessNode(n);
        if ((n->type == XML_ELEMENT_NODE) && (n->children != NULL)) {
            WalkTree(n->children);
        }
    }
}

#endif

int Config::LoadXml(char *name)
{
#ifdef USE_XML
    xmlXPathInit();
    xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
    m_Doc = xmlParseFile(name);
    if (m_Doc == NULL) {
        fprintf(stderr, "Document XML invalide\n");
        return -1;
    }
    m_Root = xmlDocGetRootElement(m_Doc);
    if (m_Root == NULL) {
        fprintf(stderr, "Document XML vierge\n");
        xmlFreeDoc(m_Doc);
        return -1;
    }

    if (xmlStrcasecmp(m_Root->name, BAD_CAST "grfxEmul")) {
        printf("Bad Config ! (%s)\n", m_Root->name);
        return -1;
    }

    //printf("Walk...\n");
    WalkTree(m_Root);
    //printf("Done...\n");
#endif
#ifdef WIN_XML

	HRESULT			hr;
	PCTSTR			pszElementlName;
	PCTSTR			pszAttributelName;
	PCTSTR			pszValue;
	std::wstring    wstr = L"/";
	//_strlist        wlist;
	std::wstring    wlist2[10];
	std::wstring    current = L"";
	wchar_t         wc[1000];
	char temp[4096];
	char temp2[4096];
    mbstowcs (wc, name, 1000);
	if (FAILED(hr = SHCreateStreamOnFile(wc, STGM_READ, &pFileStream)) ||
		FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<PVOID*>(&pReader), NULL)) ||
		FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)) ||
		FAILED(hr = pReader->SetInput(pFileStream)))
	{
		printf("Setup xml failed\n");
		return -1;
	}
	printf("XML loaded ok\n");
	while (S_OK == (hr = pReader->Read(&nodeType)))
	{
		switch (nodeType)
		{
		case XmlNodeType_Element:
			{
//			printf("XmlNodeType_Element\n");
			// test for a start snippet element
			hr = pReader->GetLocalName(&pszElementlName, NULL);
			if (!SUCCEEDED(hr))
				break;
			wstr += pszElementlName;
			wstr += L"/";
			UINT d;
			pReader->GetDepth(&d);
//			printf("Depth %d\n", d);
			wlist2[d] = pszElementlName;

			current = L"";
			for (int i=0;i<=d;i++) {
				//printf("%S/", i, wlist2[i].c_str());
				current += L"/";
				current += wlist2[i];

			}
			//printf("%S\n", current.c_str());
			sprintf(temp, "%S", current.c_str());
			printf("%s\n", temp);
			_item *t_Item = new _item();
			t_Item->path = temp;
			item.push_back(t_Item);
#if 1
			if (SUCCEEDED(hr)) {
				UINT a;
				pReader->GetAttributeCount(&a);
				for (HRESULT result = pReader->MoveToFirstAttribute(); S_OK == result; result = pReader->MoveToNextAttribute()) {
					if (SUCCEEDED(hr = pReader->GetLocalName(&pszAttributelName, NULL))) {
						if (SUCCEEDED(hr = pReader->GetValue(&pszValue, NULL))) {
							printf("  %S => %S\n", pszAttributelName, pszValue);
							sprintf(temp, "%S", pszAttributelName);
							sprintf(temp2, "%S", pszValue);
							_pair a;
							a.name = temp;
							if (strcmp(temp, "image")==0) {
								char *p = temp2;
								while  (p=strchr(p,'/')) {
									*p = '\\';
										p++;
								}
							}
							a.value = temp2;
							t_Item->args.push_back(a);
						} else {
							printf("  %S -> N/A\n", pszAttributelName);
						}
					}
				}
			}
#endif
			if (FAILED(hr = pReader->MoveToElement())) {
			}
		}
			break;
		case XmlNodeType_XmlDeclaration:
		case XmlNodeType_EndElement:
		case XmlNodeType_Whitespace:
		case XmlNodeType_Text:
		case XmlNodeType_CDATA:
		case XmlNodeType_ProcessingInstruction:
		case XmlNodeType_Comment:
		case XmlNodeType_DocumentType:
			break;
		}
	}

#endif
    m_ConfigOk = 1;
    return 0;
}

#ifdef USE_XML
void Config::ProcessNodeHelper(xmlNodePtr node, ConfigHelper *hlp)
{
    if (node->type == XML_ELEMENT_NODE) {

        xmlAttr *attr = node->properties;
        while (attr)
        {
            hlp->Handler((char*)attr->name , (char*)attr->children->content);
            attr = attr->next;
        }

    }

}
#endif

int  Config::HelperCount(char *path)
{
    int ret = -1;
#ifdef USE_XML
    xmlXPathContextPtr ctxt;
    ctxt = xmlXPathNewContext(m_Doc);
    if (ctxt) {
        xmlXPathObjectPtr xpathRes;
        char buffer[4096];
        sprintf(buffer, "count(%s)", path);
        xpathRes = xmlXPathEvalExpression(BAD_CAST buffer, ctxt);
        if (xpathRes) {
            if (xpathRes->type == XPATH_NUMBER) {
                ret =  xmlXPathCastToNumber(xpathRes);
            }
            xmlXPathFreeObject(xpathRes);
        }
        xmlXPathFreeContext(ctxt);
    }
#endif
#ifdef WIN_XML
	ret = 0;
	for (int i=0; i<item.size();i++) {
		// printf("%s\n", item[i]->path.c_str());
		if (strcmp( item[i]->path.c_str(), path)==0) {
			ret++;
		}
	}
	printf("%s found: %d\n", path, ret);
#endif
    return ret;
}

void Config::Helper(char *path, ConfigHelper *hlp)
{
#ifdef USE_XML
    xmlXPathContextPtr ctxt;
    ctxt = xmlXPathNewContext(m_Doc);
    if (ctxt) {
        xmlXPathObjectPtr xpathRes;
        xpathRes = xmlXPathEvalExpression(BAD_CAST path, ctxt);
        if (xpathRes == NULL) {
            xmlXPathFreeContext(ctxt);
            return;
        }

        if(xmlXPathNodeSetIsEmpty(xpathRes->nodesetval)){
            printf("No result\n");
            xmlXPathFreeObject(xpathRes);
            xmlXPathFreeContext(ctxt);
            return;
        }
        if (xpathRes->type == XPATH_NODESET) {
            //printf("Node\n");
            for (int i = 0; i < xpathRes->nodesetval->nodeNr; i++) {
                xmlNodePtr n = xpathRes->nodesetval->nodeTab[i];
#if 1
                xmlChar * path = xmlGetNodePath(n);
                printf("----  %s\n", path);
                xmlFree(path);
#endif
                ProcessNodeHelper(n, hlp);

                for (xmlNodePtr child = n->children;  child != NULL; child = child->next) {
                    if ((child->type == XML_ELEMENT_NODE) && child->children != NULL && child->children->type == XML_TEXT_NODE) {
                        xmlChar *content = xmlNodeGetContent(child);
                        hlp->Handler((char*)child->name , (char*)content);
                        xmlFree(content);
                    }
                }


            }
        }
        xmlXPathFreeObject(xpathRes);
        xmlXPathFreeContext(ctxt);

    }
#endif

#ifdef WIN_XML
	int num = -1;
	int current = 1;
	char local[4096];
	strcpy(local, path);
	char *p = strchr(local, '[');
	if (p) {
		if (sscanf(p, "[%d]", &num) !=1) {
			printf("error !!, can't read num\n");
		}
		*p = 0;
	}
	printf("Helper: %s (%d)\n", local, num);
	for (int i=0; i<item.size();i++) {
		// printf("%s\n", item[i]->path.c_str());
		if (strcmp( item[i]->path.c_str(), local)==0) {
			int ok=1;
			if (num!=-1) {
				if (num != current) ok = 0;
			}
			if (ok) {
				for(int j=0;j<item[i]->args.size();j++) {
					hlp->Handler((char*)item[i]->args[j].name.c_str() , (char*)item[i]->args[j].value.c_str());
				}
				return;
			}
			current++;
		}
	}
#endif


}
