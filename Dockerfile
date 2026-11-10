# syntax=docker/dockerfile:1

# ---------------------------------------------------------------------------
# Etapa 1: builder
# Compila el binario `chuspapulse` con CMake en modo Release.
# git y ca-certificates son necesarios porque CMake descarga las dependencias
# (httplib, nlohmann_json, tomlplusplus, SQLiteCpp) con FetchContent.
# ---------------------------------------------------------------------------
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        cmake \
        g++ \
        git \
        libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel

# ---------------------------------------------------------------------------
# Etapa 2: imagen final
# Solo contiene el binario y la librería de SQLite necesaria en ejecución.
# ---------------------------------------------------------------------------
FROM ubuntu:22.04

RUN apt-get update \
    && apt-get install -y --no-install-recommends libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --no-create-home --shell /usr/sbin/nologin chuspapulse \
    && mkdir -p /data \
    && chown chuspapulse:chuspapulse /data

COPY --from=builder /src/build/bin/chuspapulse /usr/local/bin/chuspapulse

# /data es donde el DaemonSet monta el volumen; chuspapulse.db se crea en el
# directorio de trabajo, por lo que queda en ese volumen.
WORKDIR /data

EXPOSE 8080

USER chuspapulse

ENTRYPOINT ["/usr/local/bin/chuspapulse"]
