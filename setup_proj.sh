#!/bin/bash

mkdir -p linux-epoll-chat

for stage in \
    "stage01_blocking_io" \
    "stage02_blocking_server" \
    "stage03_select_server" \
    "stage04_poll_server" \
    "stage05_epoll_server"
do
    mkdir -p linux-epoll-chat/$stage
    touch linux-epoll-chat/$stage/README.md
done

touch linux-epoll-chat/README.md

echo "Project directory structure created in ./linux-epoll-chat"

