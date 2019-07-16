#ifndef __otter_app_h__
#define __otter_app_h__
#include "sol/sol.hpp"

class App
{
public:
	App() = default;
	~App() = default;
	void app_main(int argc, char** argv);
private:
	sol::state luaapp_;

	App(const App&) = delete;
	App& operator=(const App&) = delete;
	App(const App&&) = delete;
	App& operator=(const App&&) = delete;
};
#endif //__otter_app_h__