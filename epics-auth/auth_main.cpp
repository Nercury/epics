#include <memory>
#include <iostream>
#include <key-common/app.h>
#include <key-common/JsonConfig.h>
#include <key-net/Socket.h>
#include <boost/shared_array.hpp>

using namespace key;
using namespace std;

/*void AcceptCallback(AsyncAcceptResult res) {
	// do something with accepted connection

	Socket *s = (Socket *)res.obj;
	cout << "OMG." << endl;

	// wait for another connection
	auto result = s->BeginAccept(AcceptCallback, s);
	if (result.not_ok()) {
		cout << result.message() << endl;
		s->Close();
	}
}*/

#include <key-common/shared_instance.hpp>

int main(int argc, char* argv[])
{
	setAppPath(*argv);

	char * send_buffer = new char[50];
	char * receive_buffer = new char[1024];

	Socket s;
	if (s.Bind(90).ok()) {

		std::function<void (char * buffer, int buffer_size, uint64_t bytes_transferred, std::shared_ptr<Socket>)> 
		receive_fun = [&receive_fun, &send_buffer] (char * receive_buffer, int buffer_size, uint64_t receive_count, shared_ptr<Socket> accepted_socket) {
			if (receive_count == 0) {
				std::cout << "Socket was closed on another end" << std::endl;
			} else {
				std::string msg(receive_buffer, receive_count);
				std::cout << msg << std::endl;

				std::string str("I have received your message!");
				memcpy(send_buffer, str.c_str(), str.size());

				accepted_socket->BeginSend(send_buffer, str.size(), [] (shared_ptr<Socket> accepted_socket) {
					std::cout << "Data sent." << endl;
				});

				auto rec_rec = accepted_socket->BeginReceive(receive_buffer, 1024, receive_fun);
				if (rec_rec.not_ok())
					std::cout << rec_rec.message() << std::endl;
			}
		};

		std::function<void (uint32_t bytes_transferred, char * bytes, std::shared_ptr<Socket>)>
		accept_fun = [&accept_fun, &send_buffer, &receive_buffer, &receive_fun, &s] (uint32_t bytes_transferred, char * bytes, shared_ptr<Socket> accepted_socket) {
			cout << "accepted a new socket!" << endl;
			std::string msg(bytes, bytes_transferred);
			std::cout << msg << std::endl;

			std::string str("can you hear me?");
			memcpy(send_buffer, str.c_str(), str.size());

			auto rec_rec = accepted_socket->BeginReceive(receive_buffer, 1024, receive_fun);
			if (rec_rec.not_ok())
				std::cout << rec_rec.message() << std::endl;

			accepted_socket->BeginSend(send_buffer, str.size(), [] (shared_ptr<Socket> accepted_socket) {
				std::cout << "Data sent." << endl;
			});

			s.BeginAcceptAndReceive(1, 5, accept_fun);
		};

		s.BeginAcceptAndReceive(1, 5, accept_fun);

		system("PAUSE");

		s.Close();
	
		system("PAUSE");
	}

	delete [] send_buffer;
	delete [] receive_buffer;

	return 0;
}
