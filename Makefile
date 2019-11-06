TARGET := a.out
TARGET_DIR := build

all: build

init: docker/build docker/up

up: docker/up

down: docker/down

sh: docker/sh

build:
	@docker-compose exec dev make local/build

run:
	@docker-compose exec dev make local/run

local/build:
	mkdir -p build
	cd $(TARGET_DIR) && cmake $(shell pwd) && make --no-print-directory

local/run:
	@./$(TARGET_DIR)/$(TARGET)

docker/build: docker-compose.yml
	docker-compose build

docker/up:
	docker-compose up -d

docker/down:
	docker-compose down

docker/sh:
	docker-compose exec dev /bin/bash

.PHONY: init up down sh build run
.PHONY: local/* docker/*
