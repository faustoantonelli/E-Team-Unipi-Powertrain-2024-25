# --- STAGE 1: Compilazione dell'Engine ---
FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y --no-install-recommends g++ && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY tester.cpp .
RUN g++ -O3 tester.cpp -o /engine

# --- STAGE 2: Ambiente di Runtime ---
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# 1. Installazione strumenti di analisi e compilazione
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    g++ \
    valgrind \
    cppcheck \
    build-essential \
    valgrind \
    locales \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# 2. Configurazione della lingua (Essenziale per i report in italiano)
RUN locale-gen it_IT.UTF-8
ENV LANG it_IT.UTF-8
ENV LANGUAGE it_IT:it
ENV LC_ALL it_IT.UTF-8

# 3. Copiamo il binario dell'engine e impostiamo i permessi
COPY --from=builder /engine /usr/local/bin/engine
RUN chmod +x /usr/local/bin/engine

# 4. Configurazione area di lavoro per GitHub Actions
WORKDIR /github/workspace
ENTRYPOINT ["engine"]