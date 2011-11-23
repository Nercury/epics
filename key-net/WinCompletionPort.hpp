#pragma once

#include <key-common/types.h>
#include <key-common/sockets.hpp>

#include <boost/thread.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/circular_buffer/space_optimized.hpp>
#include <boost/thread.hpp>

#include <functional>
#include <memory>

#include <vector>

namespace key
{
	template<class T>
	class WinCompletionPort
	{
	private:
		int32_t number_of_threads;

		boost::mutex recycled_mutex;
		boost::circular_buffer_space_optimized<T*> recycled;
		
		std::vector<boost::thread> threads;
		HANDLE handle;

		void CompleteThread(void (*completion_callback)(DWORD, T*));
	public:
		/**
		Creates zombie completion port with NULL handle.
		*/
		WinCompletionPort();
		/**
		Creates a completion port and opens it.
		*/
		WinCompletionPort(int32_t number_of_threads, void (*completion_callback)(DWORD, T*));
		/**
		Closes completion port and destroys the data
		*/
		~WinCompletionPort();

		/**
		Assign a socket to completion port
		*/
		bool AssignSocket(KEY_SOCKET socket);
		/**
		Close completion port and stop all threads
		*/
		void Close();
		/**
		Open completion port and run all threads
		*/
		bool Open(int32_t number_of_threads, void (*completion_callback)(DWORD, T*));

		/**
		Get new data (or get recycled data in case it exists)
		*/
		T* NewData();
		/**
		Put unused data back (and recycle it if possible)
		*/
		void DeleteData(T* data);
	};
}

#include "WinCompletionPort.inline.hpp"