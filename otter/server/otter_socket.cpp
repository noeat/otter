#include "otter_socket.h"
#include "log.h"

void OtterSocket::start()
{
	async_read();
}