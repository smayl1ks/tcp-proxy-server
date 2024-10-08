
# TCP PROXY-SERVER

Реализация TCP прокси-сервера для СУБД PostgreSQL с использованием Berkeley sockets (poll)

### Quick start

Сделать сборку проекта. В каталоге `build` через терминал запустить файлы:

Для прокси-сервера:
```bash
$ ./proxy
```

Для клиента:

```bash
$ ./client
```
### Building

#### Сборка вручную

Для работы с PostgreSQL используется -[libpq](https://postgrespro.ru/docs/postgresql/16/libpq)

О сборке программ с [libpq](https://postgrespro.ru/docs/postgresql/16/libpq-build) 

```bash
$ cd tcp-proxy-server
$ mkdir build
$ cmake ..
$ make
```

В результате создаются два исполняемых файла: `client` и `proxy`