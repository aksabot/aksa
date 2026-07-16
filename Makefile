CC = cc
LOCALE = id
CFLAGS = -std=c99 -Wall -Wextra -Werror -O2 -DAKSA_DEFAULT_LOCALE=\"$(LOCALE)\"
CORE = core/error.c core/locale.c core/lexer.c core/ast.c core/parser.c core/checker.c core/vm.c core/emitter.c

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

test: tests/test_locale tests/test_lexer tests/test_parser tests/test_checker tests/test_vm aksa
	./tests/test_locale
	./tests/test_lexer
	./tests/test_parser
	./tests/test_checker
	./tests/test_vm
	./tests/golden.sh
	./tests/diff.sh
	./tests/mem.sh

diff: aksa
	./tests/diff.sh

test-server: aksa
	bun server/test.ts

typecheck:
	cd web && bun install && bun x tsc --noEmit

wasm: wasm/aksa.js

web: wasm/aksa.js web/dist/main.js

web/dist/main.js: web/main.ts web/robot.ts web/board.ts web/flash.ts web/lessons.ts web/lessons.en.ts web/lessons.id.ts web/package.json
	cd web && bun install && bun build main.ts --outdir dist --minify

wasm/aksa.js: wasm/glue.c $(CORE)
	emcc -O2 -std=c99 $^ -o $@ \
	  -sASYNCIFY \
	  -sEXPORTED_FUNCTIONS=_aksa_wasm_tokens,_aksa_wasm_run,_aksa_wasm_check,_malloc,_free \
	  -sEXPORTED_RUNTIME_METHODS=ccall,UTF8ToString,stringToUTF8 \
	  -sMODULARIZE=1 -sEXPORT_NAME=AksaModule

clean:
	rm -f aksa tests/test_lexer tests/test_locale tests/test_parser tests/test_checker tests/test_vm wasm/aksa.js wasm/aksa.wasm
	rm -rf web/dist

.PHONY: test diff test-server typecheck wasm web clean
