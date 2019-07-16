#ifndef __otter_socket_h__
#define __otter_socket_h__
#include "network/socket.h"

class OtterSocket : public Socket<OtterSocket>
{
public:
	using Socket<OtterSocket>::Socket;
	OtterSocket(const OtterSocket&) = delete;
	OtterSocket& operator=(const OtterSocket&) = delete;
	OtterSocket(OtterSocket&&) = delete;
	OtterSocket& operator=(OtterSocket&&) = delete;

	void start() override;
	bool update() override;
	void on_close() override;
	void read_handler() override;

private:
	MessageBuffer head_buffer_;
	MessageBuffer packet_buffer_;
};
#endif //__otter_socket_h__
