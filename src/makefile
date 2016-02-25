#
# This file is part of the l4l Library.
#
# Copyright (C) 2015 by Jose Antonio Ortiz Bascuas <jaortizb@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>
#
 
ifndef DIST
	DIST := /usr/lib
endif

libl4l.a: l4l.o timestamp.o
	ar -cvq libl4l.a l4l.o timestamp.o

l4l.o timestamp.o: l4l.c timestamp.c
	gcc -I ../cfg -c l4l.c timestamp.c  

clean:
	rm -fr libl4l.a l4l.o timestamp.o

configure:
	
install: libl4l.a
	cp libl4l.a ../lib
	cp l4l.h ../cfg/l4lcompcfg.h ../include

