

Create the cacert for HTTPS
--------------------------------------------------------------------------------
```sh
openssl req -newkey rsa:2048 -nodes -keyout cacert_prvtkey.pem -x509 -days 3650 -out cacert.pem -subj "/CN=Little Free Library"
```