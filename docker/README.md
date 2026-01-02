# Docker

## Prerequisites

- Docker with buildx support

## Build

Build multi-architecture Ubuntu 24.04 development image:

```bash
docker buildx build --platform linux/amd64,linux/arm64 -t ghcr.io/learning-process/ppc-ubuntu:latest -f ./ubuntu.Dockerfile .
```

Build multi-architecture Alpine development image:

```bash
docker buildx build --platform linux/amd64,linux/arm64 -t ghcr.io/learning-process/ppc-alpine:latest -f ./alpine.Dockerfile .
```
