mkdir -p /guests/nginx && \
chmod 0777 /guests/nginx && \
debootstrap --include=sudo,nano,wget,gnupg,dirmngr,openssl,bridge-utils --arch amd64 bullseye /guests/nginx https://deb.debian.org/debian/ && \
mkdir -p /guests/php && \
chmod 0777 /guests/php && \
debootstrap --include=sudo,nano,wget,gnupg,dirmngr,openssl,bridge-utils --arch amd64 bullseye /guests/php http://deb.debian.org/debian/
