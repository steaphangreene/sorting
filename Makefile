# *************************************************************************
#  This file is part of the Sorting test project by Steaphan Greene
#
#  Copyright 1995-2018 Steaphan Greene <steaphan@gmail.com>
#
#  Sorting is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  Sorting is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Sorting (see the file named "LICENSE");
#  If not, see <http://www.gnu.org/licenses/>.
#
# *************************************************************************

OBJS:=	main.cpp
LIBS:=	-lpthread

all:	sorting.uint64_t sorting.uint32_t sorting.uint16_t \
	sorting.int64_t sorting.int32_t sorting.int16_t \
	sorting.float sorting.double sorting.long\ double

#Production Settings (dynamic)
CXX=	clang++-13 -std=c++17 -Wall -Werror -O3

#Use sanitizers
asan:
	+make CXX='clang++-8 -std=c++17 -g -fsanitize=address -fsanitize=undefined -fsanitize=leak -Wall -Werror'
tsan:
	+make CXX='clang++-8 -std=c++17 -g -fsanitize=thread -Wall -Werror'

#Use profiling settings
profile:
	+make CXX='g++ -std=c++17 -g -pg -fprofile-arcs -Wall -Werror'

clean:
	rm -f gmon.out deps.mk *.o *.da sorting.*

sorting.%: $(OBJS) *.hpp
	rm -f "$@"
	$(CXX) "-DDATATYPE=$*" -Wall -Werror -o "$@" $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $<

include deps.mk

deps.mk:	*.cpp *.hpp
	$(CXX) -MM *.cpp > deps.mk
