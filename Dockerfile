#build 
FROM sphinxdoc/sphinx AS builder

WORKDIR /study
COPY . /study/
RUN pip install sphinx-theme
RUN make html

FROM nginx
RUN rm /etc/nginx/conf.d/default.conf
COPY --from=builder /study/build/html /usr/share/nginx/html
COPY ngx.conf /etc/nginx/conf.d/
STOPSIGNAL SIGTERM
CMD ["nginx", "-g", "daemon off;"]

