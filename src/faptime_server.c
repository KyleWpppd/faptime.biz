#include <fcgiapp.h>
#include <fcgi_stdio.h>
#include <time.h>
#include "faptime_server.h"

int faptime_server_serve(void) {
	if (FCGX_Init() != 0) {
		return -1;
	}

	FCGX_Request request;

	FCGX_InitRequest(&request, 0, 0);

	while(0 == FCGX_Accept_r(&request)) {
		FCGX_FPrintF(request.out,
					 "Content-type: text/plain\r\n"
					 "\r\n"
					 "Hello, world. The time is now %d.", time(NULL));
		FCGX_Finish_r(&request);
	}
	return 0;
}
