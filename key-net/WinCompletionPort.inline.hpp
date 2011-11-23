#pragma once

#include "WinCompletionPort.hpp"

template<class T>
void key::WinCompletionPort<T>::CompleteThread(void (*completion_callback)(DWORD, T*))
{
	//cout << "In complete thread" << endl;

	ULONG_PTR       *PerHandleKey;
	OVERLAPPED      *Overlap;
	DWORD           dwBytesXfered;
	BOOL ret;
	T *data;

	while (1)
	{
		ret = GetQueuedCompletionStatus(
			this->handle,
			&dwBytesXfered,
			(PULONG_PTR)&PerHandleKey,
			&Overlap,
			INFINITE);

		if (Overlap == NULL && ret == 0)
			break;
		else if (ret == 0)
			continue;
		else 
		{
			data = CONTAINING_RECORD(Overlap, T, ol);
			completion_callback(dwBytesXfered, data);
			this->DeleteData(data);
		}
	}
}

template<class T>
key::WinCompletionPort<T>::WinCompletionPort() :
	handle(NULL),
	number_of_threads(0)
{
}

template<class T>
key::WinCompletionPort<T>::WinCompletionPort(int32_t number_of_threads, void (*completion_callback)(DWORD, T*)) :
	handle(NULL),	
	number_of_threads(number_of_threads)
{
	this->Open(number_of_threads, completion_callback);
}

template<class T>
key::WinCompletionPort<T>::~WinCompletionPort()
{
	this->Close();
}

template<class T>
bool key::WinCompletionPort<T>::Open(int32_t number_of_threads, void (*completion_callback)(DWORD, T*))
{
	if (this->handle != NULL)
		this->Close();

	// if creation fails, return value is NULL
	this->handle = CreateIoCompletionPort(
			INVALID_HANDLE_VALUE,
			NULL,
			(ULONG_PTR)0,
			number_of_threads);

	if (this->handle != NULL) {
		for (int i = 0; i < number_of_threads; i++)
			this->threads.push_back(
				boost::thread(
					std::bind(&key::WinCompletionPort<T>::CompleteThread, this, completion_callback)
				)
			);

		return true;
	}

	return false;
}

template<class T>
bool key::WinCompletionPort<T>::AssignSocket(KEY_SOCKET socket)
{
	if (this->handle == NULL)
		return false;

	if (CreateIoCompletionPort((HANDLE)socket,
		this->handle,
		(ULONG_PTR)0,
		0) == NULL)
	{
		return false;
	}
	return true;
}

template<class T>
void key::WinCompletionPort<T>::Close()
{
	if (this->handle != NULL) {
		// close port
		CloseHandle(this->handle);
		this->handle = NULL;
	}

	// wait for threads to exit normally
	for (auto it = this->threads.begin(); it != this->threads.end(); it++)
		it->join();

	// remove all threads
	this->threads.clear();

	// delete all data
	for (auto it = this->recycled.begin(); it != this->recycled.end(); it++)
		delete (*it);

	// clear circular buffer
	this->recycled.clear();
}

template<class T>
T* key::WinCompletionPort<T>::NewData()
{
	boost::mutex::scoped_lock lock(this->recycled_mutex);
	if (this->recycled.size() > 0) {
		auto data = this->recycled[0];
		this->recycled.pop_front();
		return data;
	}
	return new T();
}

template<class T>
void key::WinCompletionPort<T>::DeleteData(T* data)
{
	boost::mutex::scoped_lock lock(this->recycled_mutex);
	if (this->recycled.size() >= this->number_of_threads * 4) {
		delete data;
		return;
	}
	if (this->recycled.reserve() == 0) {
		if (this->recycled.capacity() == 0) {
			boost::circular_buffer_space_optimized<int>::capacity_type ct(this->number_of_threads * 2, this->number_of_threads * 2);
			this->recycled.set_capacity(ct);
		} else {
			boost::circular_buffer_space_optimized<int>::capacity_type ct(this->recycled.capacity() * 2, this->number_of_threads * 2);
			this->recycled.set_capacity(ct);
		}
	}
	this->recycled.push_back(data);
}