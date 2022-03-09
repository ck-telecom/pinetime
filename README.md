# Pebble firmare for pinetime

#### Introduction
Zephyr based PineTime firmware with Pebble API! Contribution is Welcomed!

#### Hardware
pinetine's flash only has 512KB, we will upgrade the hardware in some feture, not take too much long!

#### Software
Inspired by RebbleOS, this project API will be closed with Pebble API so that make more fun!
- [ ] AppMessage
- [ ] AppWorker
- [ ] Event Service
- [ ] Timer
- [ ] Storage
- [x] Vibes
- [ ] Animation
- [ ] Graphics
- [ ] Layers
- [ ] Light
- [ ] Window
- [ ] Window Stack

Most impotant feature:
- [x] Apple Media Service client
- [x] Apple Notification Service client
- [x] CTS
- [x] ANS

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
