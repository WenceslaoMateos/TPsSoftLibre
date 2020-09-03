FROM ubuntu:latest

RUN mkdir usr/local/dic

WORKDIR usr/local/dic

COPY Buscador/arch1.txt /usr/local/dic
COPY Buscador/arch2.txt  /usr/local/dic
COPY Buscador/main.c /usr/local/dic

RUN apt update; \
    apt install build-essential -y; \
    gcc main.c -o dic;

CMD ./dic