FROM alpine:3.10

RUN apk add --no-cache python3-dev\
&& pip3 install --upgrade pip

WORKDIR /myapp
COPY . /myapp

RUN pip3 --no-cache-dir install -r reqs.txt

CMD ["python3", "API1.py"]