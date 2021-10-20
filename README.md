# pinetime

#### Introduction
zephyr based PineTime

#### Initialization
```
west init -m https://gitee.com/fwatch/pinetime.git --mr develop

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
