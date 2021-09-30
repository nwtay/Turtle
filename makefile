CC = gcc
COMMON = -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -Werror
DEBUG = -g3
SANITIZE = $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
PRODUCTION = $(COMMON) -O3
LDLIBS = -lm
CFLAGSSDL= `sdl2-config --cflags`
LIDLIBSSDL= `sdl2-config --libs` $(LDLIBS)
TTL= Examples/valid/parser_valid/neillsexamples/nestedloops.ttl.txt

parse_slow: ADTs/Stack/stack.c Parser/parser.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h General/general.h
						$(CC) Parser/parser.c ADTs/General/general.c General/general.c -o parse_slow $(SANITIZE) $(LDLIBS)

parse_valgrind: ADTs/Stack/stack.c Parser/parser.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h General/general.h
					$(CC) Parser/parser.c ADTs/General/general.c General/general.c -o parse_valgrind $(PRODUCTION) $(LDLIBS)

parse:	ADTs/Stack/stack.c Parser/parser.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h General/general.h
			$(CC) Parser/parser.c ADTs/General/general.c General/general.c -o parse $(PRODUCTION) $(LDLIBS)

run_parse_all:	parse
				./parse Examples/valid/parser_valid/examp1.ttl.txt
				./parse Examples/valid/parser_valid/examp2.ttl
				./parse Examples/valid/parser_valid/examp3.ttl
				./parse Examples/valid/parser_valid/examp4.ttl
				./parse Examples/valid/parser_valid/examp5.ttl
				./parse Examples/valid/parser_valid/examp6.ttl
				./parse Examples/valid/parser_valid/examp7.ttl
				./parse Examples/valid/parser_valid/examp8.ttl
				./parse Examples/valid/parser_valid/excess_ops.ttl.txt
				./parse Examples/valid/parser_valid/neillsexamples/addingloops.ttl.txt
				./parse Examples/valid/parser_valid/neillsexamples/nestedloops.ttl.txt
				./parse Examples/valid/parser_valid/neillsexamples/octagon.ttl
				./parse Examples/valid/parser_valid/neillsexamples/usingvars.ttl.txt
				./parse Examples/Peer_ttl/parser/valid/adding_loops.ttl
				./parse Examples/Peer_ttl/parser/valid/doloop2.ttl
				./parse Examples/Peer_ttl/parser/valid/more_do.ttl
				./parse Examples/Peer_ttl/parser/valid/nested_loops.ttl
				./parse Examples/Peer_ttl/parser/valid/nested2.ttl
				./parse Examples/Peer_ttl/parser/valid/nested3.ttl
				./parse Examples/Peer_ttl/parser/valid/polish_2.ttl
				./parse Examples/Peer_ttl/parser/valid/polish.ttl
				./parse Examples/Peer_ttl/parser/valid/squares1.ttl
				./parse Examples/Peer_ttl/parser/valid/stupid_valid_do.ttl
				./parse Examples/Peer_ttl/parser/valid/using_variables.ttl

run_parse_slow_all:	parse_slow
				./parse_slow Examples/valid/parser_valid/examp1.ttl.txt
				./parse_slow Examples/valid/parser_valid/examp2.ttl
				./parse_slow Examples/valid/parser_valid/examp3.ttl
				./parse_slow Examples/valid/parser_valid/examp4.ttl
				./parse_slow Examples/valid/parser_valid/examp5.ttl
				./parse_slow Examples/valid/parser_valid/examp6.ttl
				./parse_slow Examples/valid/parser_valid/examp7.ttl
				./parse_slow Examples/valid/parser_valid/examp8.ttl
				./parse_slow Examples/valid/parser_valid/excess_ops.ttl.txt
				./parse_slow Examples/valid/parser_valid/neillsexamples/addingloops.ttl.txt
				./parse_slow Examples/valid/parser_valid/neillsexamples/nestedloops.ttl.txt
				./parse_slow Examples/valid/parser_valid/neillsexamples/octagon.ttl
				./parse_slow Examples/valid/parser_valid/neillsexamples/usingvars.ttl.txt
				./parse_slow Examples/Peer_ttl/parser/valid/adding_loops.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/doloop2.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/more_do.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/nested_loops.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/nested2.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/nested3.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/polish_2.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/polish.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/squares1.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/stupid_valid_do.ttl
				./parse_slow Examples/Peer_ttl/parser/valid/using_variables.ttl

run_parse_valgrind_all:	parse_valgrind
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/examp1.ttl.txt
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/examp2.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/examp3.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/examp4.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/examp5.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/examp6.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/examp7.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/examp8.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/excess_ops.ttl.txt
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/neillsexamples/addingloops.ttl.txt
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/neillsexamples/nestedloops.ttl.txt
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/neillsexamples/octagon.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/valid/parser_valid/neillsexamples/usingvars.ttl.txt
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/doloop2.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/more_do.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/nested_loops.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/nested2.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/nested3.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/polish_2.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/polish.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/squares1.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/stupid_valid_do.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind Examples/Peer_ttl/parser/valid/using_variables.ttl

run_parse: parse
			./parse $(TTL)

run_parse_slow: parse_slow
			./parse_slow $(TTL)

run_parse_valgrind: parse_valgrind
			valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./parse_valgrind $(TTL)



interp_slow: ADTs/Stack/stack.c Interpreter/specific.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h SDL/neillsdl2.c SDL/neillsdl2.h General/general.h Interpreter/General/general.h
					$(CC) Interpreter/specific.c ADTs/Stack/stack.c ADTs/General/general.c General/general.c Interpreter/General/general.c SDL/neillsdl2.c -o interp_slow $(SANITIZE) $(CFLAGSSDL) $(LIDLIBSSDL)

interp_valgrind: ADTs/Stack/stack.c Interpreter/specific.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h SDL/neillsdl2.c SDL/neillsdl2.h General/general.h Interpreter/General/general.h
					$(CC) Interpreter/specific.c ADTs/Stack/stack.c ADTs/General/general.c General/general.c Interpreter/General/general.c SDL/neillsdl2.c -o interp_valgrind $(PRODUCTION) $(CFLAGSSDL) $(LIDLIBSSDL)

interp:	ADTs/Stack/stack.c Interpreter/specific.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h SDL/neillsdl2.c SDL/neillsdl2.h General/general.h Interpreter/General/general.h
					$(CC) Interpreter/specific.c ADTs/Stack/stack.c ADTs/General/general.c General/general.c Interpreter/General/general.c SDL/neillsdl2.c -o interp $(PRODUCTION) $(CFLAGSSDL) $(LIDLIBSSDL)

run_interp_all:	interp
				./interp Examples/valid/interpreter_valid/neillsexamples/addingloops.ttl.txt
		      ./interp Examples/valid/interpreter_valid/neillsexamples/nestedloops.ttl.txt
				./interp Examples/valid/interpreter_valid/neillsexamples/octagon.ttl
				./interp Examples/valid/interpreter_valid/neillsexamples/usingvars.ttl.txt
				./interp Examples/Peer_ttl/interpreter/valid/circle_with_more_circles.ttl
				./interp Examples/Peer_ttl/interpreter/valid/circle.ttl
				./interp Examples/Peer_ttl/interpreter/valid/doloop2.ttl
				./interp Examples/Peer_ttl/interpreter/valid/nath.ttl
				./interp Examples/Peer_ttl/interpreter/valid/nested2.ttl
				./interp Examples/Peer_ttl/interpreter/valid/nested3.ttl
				./interp Examples/Peer_ttl/interpreter/valid/octagon.ttl
				./interp Examples/Peer_ttl/interpreter/valid/slinky.ttl
				./interp Examples/Peer_ttl/interpreter/valid/spiral.ttl
				./interp Examples/Peer_ttl/interpreter/valid/spiral2.ttl
				./interp Examples/Peer_ttl/interpreter/valid/square_circ.ttl
				./interp Examples/Peer_ttl/interpreter/valid/square_thing.ttl
				./interp Examples/Peer_ttl/interpreter/valid/squares1.ttl
				./interp Examples/Peer_ttl/interpreter/valid/sun.ttl
				./interp Examples/Peer_ttl/interpreter/valid/test_dir.ttl
				./interp Examples/Peer_ttl/interpreter/valid/two_oct.ttl
				./interp Examples/Peer_ttl/interpreter/valid/large_num.ttl
				./interp Examples/valid/interpreter_valid/examp1.ttl
				./interp Examples/valid/interpreter_valid/examp2.ttl
				./interp Examples/valid/interpreter_valid/examp3.ttl
				./interp Examples/valid/interpreter_valid/examp4.ttl


run_interp_slow_all: interp_slow
				./interp_slow Examples/valid/interpreter_valid/neillsexamples/addingloops.ttl.txt
				./interp_slow Examples/valid/interpreter_valid/neillsexamples/nestedloops.ttl.txt
				./interp_slow Examples/valid/interpreter_valid/neillsexamples/octagon.ttl
				./interp_slow Examples/valid/interpreter_valid/neillsexamples/usingvars.ttl.txt
				./interp_slow Examples/Peer_ttl/interpreter/valid/circle_with_more_circles.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/circle.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/doloop2.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/nath.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/nested2.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/nested3.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/octagon.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/slinky.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/spiral.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/spiral2.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/square_circ.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/square_thing.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/squares1.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/sun.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/test_dir.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/two_oct.ttl
				./interp_slow Examples/Peer_ttl/interpreter/valid/large_num.ttl
				./interp_slow Examples/valid/interpreter_valid/examp1.ttl
				./interp_slow Examples/valid/interpreter_valid/examp2.ttl
				./interp_slow Examples/valid/interpreter_valid/examp3.ttl
				./interp_slow Examples/valid/interpreter_valid/examp4.ttl

run_interp_valgrind_all:	interp_valgrind
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/valid/parser_valid/neillsexamples/addingloops.ttl.txt
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/valid/parser_valid/neillsexamples/nestedloops.ttl.txt
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/valid/parser_valid/neillsexamples/octagon.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/valid/parser_valid/neillsexamples/usingvars.ttl.txt
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/circle_with_more_circles.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/circle.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/doloop2.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/nath.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/nested2.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/nested3.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/octagon.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/slinky.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/spiral.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/spiral2.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/square_circ.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/square_thing.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/squares1.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/sun.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/test_dir.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/Peer_ttl/interpreter/valid/two_oct.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/valid/interpreter_valid/examp1.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/valid/interpreter_valid/examp2.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/valid/interpreter_valid/examp3.ttl
				valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind Examples/valid/interpreter_valid/examp4.ttl


run_interp: interp
			./interp $(TTL)

run_interp_slow: interp_slow
			./interp_slow $(TTL)

run_interp_valgrind: interp_valgrind
			valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./interp_valgrind $(TTL)



ext: Extension/specific.h ADTs/Stack/stack.c Extension/specific.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h SDL/neillsdl2.c SDL/neillsdl2.h General/general.h Interpreter/General/general.h
					$(CC) Extension/specific.c Interpreter/General/general.c ADTs/Stack/stack.c ADTs/General/general.c General/general.c SDL/neillsdl2.c -o ext $(PRODUCTION) $(CFLAGSSDL) $(LIDLIBSSDL)

ext_slow: Extension/specific.h ADTs/Stack/stack.c Extension/specific.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h SDL/neillsdl2.c SDL/neillsdl2.h General/general.h Interpreter/General/general.h
					$(CC) Extension/specific.c Interpreter/General/general.c ADTs/Stack/stack.c ADTs/General/general.c General/general.c SDL/neillsdl2.c -o ext_slow $(SANITIZE) $(CFLAGSSDL) $(LIDLIBSSDL)

ext_valgrind: Extension/specific.h ADTs/Stack/stack.c Extension/specific.h ADTs/General/general.h ADTs/General/bool.h ADTs/Stack/stack.h SDL/neillsdl2.c SDL/neillsdl2.h General/general.h Interpreter/General/general.h
					$(CC) Extension/specific.c Interpreter/General/general.c ADTs/Stack/stack.c ADTs/General/general.c General/general.c SDL/neillsdl2.c -o ext_valgrind $(PRODUCTION) $(CFLAGSSDL) $(LIDLIBSSDL)


run_ext_all: ext
			./ext Examples/valid/ext_valid/pattern1.ttl
			./ext Examples/valid/ext_valid/neillsexamples/addingloops.ttl.txt
			./ext Examples/valid/ext_valid/neillsexamples/nestedloops.ttl.txt
			./ext Examples/valid/ext_valid/neillsexamples/octagon.ttl
			./ext Examples/valid/ext_valid/neillsexamples/usingvars.ttl.txt
			./ext Examples/Peer_ttl/interpreter/valid/circle_with_more_circles.ttl
			./ext Examples/Peer_ttl/interpreter/valid/circle.ttl
			./ext Examples/Peer_ttl/interpreter/valid/doloop2.ttl
			./ext Examples/Peer_ttl/interpreter/valid/nath.ttl
			./ext Examples/Peer_ttl/interpreter/valid/nested2.ttl
			./ext Examples/Peer_ttl/interpreter/valid/nested3.ttl
			./ext Examples/Peer_ttl/interpreter/valid/octagon.ttl
			./ext Examples/Peer_ttl/interpreter/valid/slinky.ttl
			./ext Examples/Peer_ttl/interpreter/valid/spiral.ttl
			./ext Examples/Peer_ttl/interpreter/valid/spiral2.ttl
			./ext Examples/Peer_ttl/interpreter/valid/square_circ.ttl
			./ext Examples/Peer_ttl/interpreter/valid/square_thing.ttl
			./ext Examples/Peer_ttl/interpreter/valid/squares1.ttl
			./ext Examples/Peer_ttl/interpreter/valid/sun.ttl
			./ext Examples/Peer_ttl/interpreter/valid/test_dir.ttl
			./ext Examples/Peer_ttl/interpreter/valid/two_oct.ttl
			./ext Examples/Peer_ttl/interpreter/valid/large_num.ttl
			./ext Examples/valid/ext_valid/if_statement.ttl
			./ext Examples/valid/ext_valid/if_statement2.ttl
			./ext Examples/valid/ext_valid/examp1.ttl
			./ext Examples/valid/ext_valid/examp2.ttl
			./ext Examples/valid/ext_valid/examp3.ttl
			./ext Examples/valid/ext_valid/examp4.ttl

run_ext_slow_all: ext_slow
			./ext_slow Examples/valid/ext_valid/pattern1.ttl
			./ext_slow Examples/valid/ext_valid/neillsexamples/addingloops.ttl.txt
			./ext_slow Examples/valid/ext_valid/neillsexamples/nestedloops.ttl.txt
			./ext_slow Examples/valid/ext_valid/neillsexamples/octagon.ttl
			./ext_slow Examples/valid/ext_valid/neillsexamples/usingvars.ttl.txt
			./ext_slow Examples/Peer_ttl/interpreter/valid/circle_with_more_circles.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/circle.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/doloop2.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/nath.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/nested2.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/nested3.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/octagon.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/slinky.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/spiral.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/spiral2.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/square_circ.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/square_thing.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/squares1.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/sun.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/test_dir.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/two_oct.ttl
			./ext_slow Examples/Peer_ttl/interpreter/valid/large_num.ttl
			./ext_slow Examples/valid/ext_valid/if_statement.ttl
			./ext_slow Examples/valid/ext_valid/if_statement2.ttl
			./ext_slow Examples/valid/ext_valid/examp1.ttl
			./ext_slow Examples/valid/ext_valid/examp2.ttl
			./ext_slow Examples/valid/ext_valid/examp3.ttl
			./ext_slow Examples/valid/ext_valid/examp4.ttl

run_ext_valgrind_all: ext
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/parser_valid/neillsexamples/addingloops.ttl.txt
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/parser_valid/neillsexamples/nestedloops.ttl.txt
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/parser_valid/neillsexamples/octagon.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/parser_valid/neillsexamples/usingvars.ttl.txt
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/ext_valid/neillsexamples/usingvars.ttl.txt
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/circle_with_more_circles.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/circle.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/doloop2.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/nath.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/nested2.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/nested3.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/octagon.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/slinky.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/spiral.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/spiral2.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/square_circ.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/square_thing.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/squares1.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/sun.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/test_dir.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/Peer_ttl/interpreter/valid/two_oct.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/ext_valid/if_statement.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/ext_valid/if_statement2.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/ext_valid/examp1.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/ext_valid/examp2.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/ext_valid/examp3.ttl
	valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./ext_valgrind Examples/valid/ext_valid/examp4.ttl

run_ext: ext
			./ext $(TTL)

run_ext_slow: ext_slow
			./ext_slow $(TTL)

run_ext_valgrind: ext_valgrind
			./ext_valgrind $(TTL)

clean :
		rm -f parse parse_slow parse_valgrind interp interp_slow interp_valgrind ext ext_slow ext_valgrind
