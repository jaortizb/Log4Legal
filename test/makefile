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
 
TARGET=test.exe

install test.exe: test.c
	gcc test.c -o test.exe -I../include -L../lib -ll4l -lcrypto  -lpthread -lrt -lcurl -std=gnu99

configure: test.exe

clean: 
	rm -fr test.exe
	
