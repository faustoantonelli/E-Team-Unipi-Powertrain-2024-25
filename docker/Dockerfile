# Utilizziamo un'immagine base con gli strumenti di build
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Installazione dipendenze essenziali per CMake e GTest
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    gcc \
    g++ \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Configura l'area di lavoro
WORKDIR /app

# Copia l'intero repository all'interno del container
COPY . .

# Comando di default: configura, builda ed esegue i test
# Se i test falliscono, il container restituisce un codice di errore alla GitHub Action
ENTRYPOINT ["sh", "-c", "mkdir -p build && cd build && cmake .. && make && ./unit_tests"]