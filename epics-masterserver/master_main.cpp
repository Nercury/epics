#include <memory>
#include <iostream>
#include <key-common/app.h>
#include <key-common/JsonConfig.h>

using namespace key;

int main(int argc, char* argv[])
{
	setAppPath(*argv);

	std::cout << getAppPath() << std::endl;
	


	//Renderer::addRenderer("OpenGL", std::make_shared<key::GLRenderer>());

	JsonConfig config("config.js");
	std::string js_dir(joinPath(getAppPath(), config.js_root_dir));

	system("PAUSE");

	return 0;
}
