# pinetime

#### Introduction
This project try to port Zephyr to Pebble

#### Hardware
PineTime(pinetime_devkit0)

Pebble Duo 2(p2d)

Pebble Time 2(pt2)

Pebble Round 2(pr2)

#### Initialization
Setup Zephyr environment
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

### History
6 years ago, I started this project, and now it's alive!