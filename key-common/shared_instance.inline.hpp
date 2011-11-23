#pragma once

#include <key-common/shared_instance.hpp>

template<class T>
T* key::shared_instance<T>::single_instance = NULL;

template<class T>
uint32_t key::shared_instance<T>::use_count = 0;

template<class T>
boost::recursive_mutex key::shared_instance<T>::instance_mutex;

template<class T>
T* key::shared_instance<T>::use_instance() {
	boost::recursive_mutex::scoped_lock lock(instance_mutex);

	if (single_instance == NULL)
		single_instance = new T();

	use_count++;

	return single_instance;
}

template<class T>
void key::shared_instance<T>::unuse_instance() {
	boost::recursive_mutex::scoped_lock lock(instance_mutex);

	use_count--;

	if (use_count == 0) {
		delete single_instance;
		single_instance = NULL;
	}
}