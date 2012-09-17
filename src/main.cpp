
#include <limits>
#include <stdio.h>
#include <iostream>


#include "OpenGLViewer.h"


int main(void)
{

	OpenGl_initViewer(600, 600);
		
	while(running) {
			
		//Draw
		OpenGl_drawAndUpdate(running);
	}
	

	TerminateViewer();

	return 0;
}



