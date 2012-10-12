
#include <limits>
#include <stdio.h>
#include <iostream>

#include "OpenGLViewer.h"

#define LOW10 0x3FF
#define MID10 0xFFC00
#define HIGH10 0x3FF00000


int main(void)
{
	
	OpenGl_initViewer(512, 512);
		
	while(running) {
			
		//Draw
	OpenGl_drawAndUpdate(running);
	}

	//do something nice with h_xyzBuffer
	//std::vector<Vec3i> buffer;	
	//buffer.insert(buffer.begin(), h_xyzBuffer.begin(),h_xyzBuffer.end());

	//for(int i = 0; i < h_xyzBuffer.size(); ++i) {
	//	Vec3i p(
	//}
	//buffer.push_back(Vec3i(0,0,0));
	//buffer.push_back(Vec3i(3,0,0));
	//buffer.push_back(Vec3i(0,3,0));
	//buffer.push_back(Vec3i(3,3,0));
	//buffer.push_back(Vec3i(0,0,3));
	//buffer.push_back(Vec3i(3,0,3));
	//buffer.push_back(Vec3i(0,3,3));
	//buffer.push_back(Vec3i(3,3,3));	
	
	

	TerminateViewer();
	
	return 0;
}



