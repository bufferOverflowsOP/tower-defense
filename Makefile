.PHONY: build run clean help build-run

build:
	cmake -B build && cmake --build build

run:
	./build/bin/main

build-run: build run

clean:
	rm -rf build

help:
	@echo "Available commands:"
	@echo "  make build      - Build the project"
	@echo "  make run        - Run the executable"
	@echo "  make build-run  - Build and run"
	@echo "  make clean      - Remove build directory"
	@echo "  make help       - Show this help"
