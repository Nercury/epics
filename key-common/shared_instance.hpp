#pragma once

#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <key-common/types.h>

namespace key
{
	template<class T>
	class shared_instance : public boost::noncopyable
	{
	private:
		static T* single_instance;
		static uint32_t use_count;
		static boost::recursive_mutex instance_mutex;

		T* local_instance;
	public:
		static T* use_instance();
		static void unuse_instance();

		shared_instance() : local_instance(NULL) {

		}
		~shared_instance() {
			if (local_instance != NULL)
				unuse_instance();
		}
		T* operator->() {
			if (local_instance != NULL)
				return local_instance;
			local_instance = use_instance();
			return local_instance;
		};
	};
}

#include <key-common/shared_instance.inline.hpp>