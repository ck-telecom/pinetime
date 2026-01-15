# pinetime

#### Introduction
This project try to port PebbleOS to Zephyr

#### Hardware
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
make sure in directory pinetime

west build -p -b pt2 app
```

#### Flash
```
west flash or
west flash --port=<your_serial_port>
```

#### Debug
```
west debug
```

### History
6 years ago, I started this project, and now it's alive!