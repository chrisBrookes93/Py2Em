FROM debian:buster

RUN apt update && \
	apt install -y \
		python2.7-dev \
		python3 \
		python3-pip && \
	pip3 install pytest && \
	rm -rf /var/lib/apt/lists/*