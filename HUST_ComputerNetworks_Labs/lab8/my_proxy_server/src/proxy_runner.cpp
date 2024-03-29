/* FileName:	proxy_runner.cpp
 * Author:		Hover
 * E-Mail:		hover@hust.edu.cn
 * GitHub:		HoverWings
 * Description:	the main fun of proxy 
 */

#include "proxy_server.hpp"

int main(int argc, char** argv) 
{
	try 
	{
		int thread_num = 2, port = 10001;
		std::string interface_address;
		// std::cout<<argv[1];
		if(argc > 1)
			thread_num = boost::lexical_cast<int>(argv[1]);
		// read port number from command line, if provided
		if(argc > 2)
			port = boost::lexical_cast<int>(argv[2]);
		// read local interface address from command line, if provided
		if(argc > 3)
			interface_address = argv[3];
		ios_deque io_services;
		std::deque<ba::io_service::work> io_service_work;
		
		boost::thread_group thr_grp;
		
		for (int i = 0; i < thread_num; ++i) 
		{
			io_service_ptr ios(new ba::io_service);
			io_services.push_back(ios);
			io_service_work.push_back(ba::io_service::work(*ios));
			thr_grp.create_thread(boost::bind(&ba::io_service::run, ios));
		}
		server server(io_services, port, interface_address);
		thr_grp.join_all();
	} 
	catch (std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

