cppflags = -std=c++20 -g -Wall -Wextra -Wpedantic -Werror
iflags = -I./src
files = $(shell find ./src -type f -name "*.cpp" )

main:
	clang++ $(cppflags) $(files) -o ./p $(iflags)