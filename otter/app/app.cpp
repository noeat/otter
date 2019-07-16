#include "app.h"
#include <algorithm>

void App::app_main(int argc, char** argv)
{
	luaapp_.open_libraries();
	sol::table argv_table = luaapp_.create_table();
	std::for_each(argv, argv + argc,
		[&argv_table](const char* item)
		{
			argv_table.add(item);
		});
	luaapp_.create_named_table("process", "argv", argv_table);	

	auto script_from_file_result = luaapp_.safe_script_file("app.lua", sol::script_pass_on_error);
	if (!script_from_file_result.valid()) {
		sol::error err = script_from_file_result;
		std::cerr << "run app.lua error: " << 
			err.what() << "\npanicking and exiting..." << std::endl;
	}
}