#include <stdio.h>
#include <stdlib.h>
//#include <spnav.h>
#include "libspnav\spnav.h"
#include "spnav_ctrl.h"

#define DEBUG 0

#if DEBUG
#define DEBUGPRINT(s) printf s
#else
#define DEBUGPRINT(s)
#endif


#define THRESHOLD 200 

class SpNavDrv{
public:
	SpNavDrv(){
		if(spnav_open()==-1){
			fprintf(stderr, "failed to connect to the space navigator daemon\n");
			m_status_ok = false;
		}else
			m_status_ok = true;
			spnav_sensitivity(1.0f);
	}
	bool IsOnline(){
		return m_status_ok;
	}
	~SpNavDrv(){
		spnav_close();
	}
	void Process(int val, bool &currentA, bool &currentB)
	{
		if ((val >= THRESHOLD) && !currentA){
			currentA = 1;
		}else if ((val < THRESHOLD) &&currentA) {
			currentA = 0;
		}else if ((val <= -THRESHOLD) && !currentB){
			currentB = 1;
		}else if ((val > -THRESHOLD) && currentB){
			currentB = 0;
		}
	}
private:
	bool m_status_ok;
};

struct SpNavPos{
	bool x1;
	bool x2;
	bool y1;
	bool y2;
	bool z1;
	bool z2;
	bool rx1;
	bool rx2;
	bool ry1;
	bool ry2;
	bool rz1;
	bool rz2;

};

static struct SpNavPos s = {0};

int SpNavCheckMessage()
{
int key = 0;
	static SpNavDrv SpNav;
	if (!SpNav.IsOnline())
		return -1;
	spnav_event sev;
	int i = spnav_poll_event(&sev);
	switch(i) {
	case SPNAV_EVENT_MOTION:
		DEBUGPRINT(("MOTION  %8d %8d %8d %8d %8d %8d\n", sev.motion.x, sev.motion.y, sev.motion.z, sev.motion.rx, sev.motion.ry, sev.motion.rz));
		break;
			case SPNAV_EVENT_BUTTON:
		DEBUGPRINT(("BUTTON %d %d\n", sev.button.press, sev.button.bnum));
		if (sev.button.press) {
			switch(sev.button.bnum) {
			case 0:
				 DEBUGPRINT(("BUT_0 ON\n"));
			        key = '0';
				break;
			case 1:
				 DEBUGPRINT(("BUT_1 ON\n"));
				 key = 27;
				break;
			}
		}
		return 0;
		break;
	default:
		return 0;
	}
	SpNav.Process(sev.motion.x,s.x1,s.x2);
	SpNav.Process(sev.motion.y,s.y1,s.y2);
	SpNav.Process(sev.motion.z,s.z1,s.z2);
	SpNav.Process(sev.motion.rx,s.rx1,s.rx2);
	SpNav.Process(sev.motion.ry,s.ry1,s.ry2);
	SpNav.Process(sev.motion.rz,s.rz1,s.rz2);
	spnav_remove_events(SPNAV_EVENT_ANY);
	spnav_remove_events(SPNAV_EVENT_MOTION);
if (s.x1 | s.rz1) return 'd';
if (s.x2 | s.rz2) return 'a';
if (s.y1) return 'z';
if (s.y2) return 'c';
if (s.z1 | s.rx2) return 'e';
if (s.z2 | s.rx1) return 'q';
if (s.ry1) return 0;
if (s.ry2) return 0;

	return 0;
}

