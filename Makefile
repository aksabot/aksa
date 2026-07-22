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

server: aksa
	bun server/compile.ts

typecheck:
	cd play && bun install && bun x tsc --noEmit

DEVICE_FQBN = esp32:esp32:esp32c6
DEVICE_PORT ?= $(firstword $(wildcard /dev/cu.usbmodem*))

device:
	device/build.sh
	arduino-cli compile --fqbn $(DEVICE_FQBN) device/sketch

# Flash the on-device runtime (hotspot + editor + interpreter). Build +
# compile + upload with a retry on the C6 USB hiccup:
#   make device-flash                         (defaults: BOARD=c6, first port)
#   make device-flash DEVICE_PORT=/dev/cu.usbmodemXXXX
device-flash:
	./device/flash.sh $(BOARD) $(DEVICE_PORT)

# Flash one Aksa program to a plugged-in board, no browser/server:
#   make aksa-flash FILE=examples/kedip.aksa       (defaults: LOCALE=id BOARD=c6)
BOARD ?= c6
aksa-flash: aksa
	./flash.sh $(FILE) $(LOCALE) $(BOARD) $(DEVICE_PORT)

wasm: wasm/aksa.js

play: wasm/aksa.js play/dist/main.js

play/dist/main.js: play/main.ts play/robot.ts play/board.ts play/flash.ts play/lessons.ts play/lessons.en.ts play/lessons.id.ts play/package.json
	cd play && bun install && bun build main.ts --outdir dist --minify

wasm/aksa.js: wasm/glue.c $(CORE)
	emcc -O2 -std=c99 $^ -o $@ \
	  -sASYNCIFY \
	  -sEXPORTED_FUNCTIONS=_aksa_wasm_tokens,_aksa_wasm_run,_aksa_wasm_check,_malloc,_free \
	  -sEXPORTED_RUNTIME_METHODS=ccall,UTF8ToString,stringToUTF8 \
	  -sMODULARIZE=1 -sEXPORT_NAME=AksaModule

clean:
	rm -f aksa tests/test_lexer tests/test_locale tests/test_parser tests/test_checker tests/test_vm wasm/aksa.js wasm/aksa.wasm
	rm -rf play/dist device/sketch

.PHONY: test diff test-server server typecheck wasm play device device-flash aksa-flash clean
