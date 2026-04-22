FROM debian:bookworm

RUN apt-get update && apt-get install -y guix git curl build-essential

RUN guix pull
RUN guix install shepherd guile

WORKDIR /workspace
CMD ["/bin/bash"]
