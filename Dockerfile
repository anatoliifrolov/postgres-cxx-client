FROM anatoliifrolov/pgcxxenv

WORKDIR /projects/postgres-cxx-client

COPY ./docker/files .

ENTRYPOINT ["./entrypoint.sh"]
