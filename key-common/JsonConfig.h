#pragma once

#include <string>

namespace key {

	class JsonConfig
	{
	private:
		std::string main_configuration_contents;
		bool configuration_is_valid;
	public:
		JsonConfig(std::string main_config_filename);
		virtual ~JsonConfig();

		std::string js_root_dir;
		std::string js_main_file;
	};

}