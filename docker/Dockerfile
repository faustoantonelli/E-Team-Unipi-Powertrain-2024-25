# --- STAGE 1: Compilazione dell'Engine ---
FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y --no-install-recommends g++ && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY tester.cpp .
RUN g++ -O3 tester.cpp -o /engine

# --- STAGE 2: Ambiente di Runtime ---
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# 1. Aggiungiamo i pacchetti raccomandati per LaTeX e le Locales per gli accenti
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    valgrind \
    python3 \
    octave \
    chktex \
    aspell \
    aspell-it \
    # Fondamentali per far girare pdflatex senza errori di pacchetti mancanti
    texlive-latex-base \
    texlive-latex-recommended \
    texlive-fonts-recommended \
    texlive-lang-italian \
    # Per gestire correttamente i caratteri italiani (accenti) nel report
    locales \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# 2. Configurazione della lingua per evitare errori con Aspell e caratteri speciali
RUN locale-gen it_IT.UTF-8
ENV LANG it_IT.UTF-8
ENV LANGUAGE it_IT:it
ENV LC_ALL it_IT.UTF-8

# 3. Copiamo il binario e impostiamo i permessi
COPY --from=builder /engine /usr/local/bin/engine
RUN chmod +x /usr/local/bin/engine

# 4. Pulizia delle ridondanze (ne basta uno)
WORKDIR /github/workspace
ENTRYPOINT ["engine"]