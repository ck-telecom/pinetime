# pinetime

#### Introduction
Hypnos do not update any more, this is a new home for zephyr based PineTime

Most impotant feature:
- [ ] Apple Media Service client
- [ ] Apple Notification Service client

#### Initialization
```
west init -m https://github.com/ck-telecom/pinetime.git --mr develop pinetime

cd pinetime && west update
```

#### Build
```
cd pinetime/app

west build -p auto -b pinetime_devkit0 .
```

#### Flash
```
west flash
```

#### Debug
```
west debug
```
