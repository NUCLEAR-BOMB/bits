#include <gtest/gtest.h>
#include <vector>
#include <array>
#include <string>

template<class T, class... Args>
constexpr std::array<T, sizeof...(Args)> make_array(Args&&... args) {
	return {{std::forward<Args>(args)...}};
}

int main(int argc, char **argv) {
#ifdef EXTRA_ARGS
	auto append_args = make_array<std::string>(EXTRA_ARGS);

	std::vector<char*> custom_argv(argv, argv + argc);
	for (auto& arg : append_args) {
		custom_argv.push_back(arg.data());
		++argc;
	}

	::testing::InitGoogleTest(&argc, custom_argv.data());
#else
	::testing::InitGoogleTest(&argc, argv);
#endif
	return RUN_ALL_TESTS();
}