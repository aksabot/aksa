CC = cc
CFLAGS = -std=c99 -Wall -Wextra -Werror -O2
CORE = core/error.c core/locale.c core/lexer.c core/ast.c core/parser.c core/checker.c core/vm.c

aksa: core/main.c $(CORE)
	$(CC) $(CFLAGS) $^ -o $@

tests/test_lexer: tests/test_lexer.c $(CORE)
	$(CC) $(CFLAGS) $^ -o $@

tests/test_locale: tests/test_locale.c $(CORE)
	$(CC) $(CFLAGS) $^ -o $@

tests/test_parser: tests/test_parser.c $(CORE)
	$(CC) $(CFLAGS) $^ -o $@

tests/test_vm: tests/test_vm.c $(CORE)
	$(CC) $(CFLAGS) $^ -o $@

tests/test_checker: tests/test_checker.c $(CORE)
	$(CC) $(CFLAGS) $^ -o $@

test: tests/test_locale tests/test_lexer tests/test_parser tests/test_checker tests/test_vm
	./tests/test_locale
	./tests/test_lexer
	./tests/test_parser
	./tests/test_checker
	./tests/test_vm

wasm: wasm/aksa.js

wasm/aksa.js: wasm/glue.c $(CORE)
	emcc -O2 -std=c99 $^ -o $@ \
	  -sEXPORTED_FUNCTIONS=_aksa_wasm_tokens,_malloc,_free \
	  -sEXPORTED_RUNTIME_METHODS=ccall,UTF8ToString \
	  -sMODULARIZE=1 -sEXPORT_NAME=AksaModule

clean:
	rm -f aksa tests/test_lexer tests/test_locale tests/test_parser tests/test_checker tests/test_vm wasm/aksa.js wasm/aksa.wasm

.PHONY: test wasm clean
