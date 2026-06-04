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

## Taks Mapping(PebbleOS - Zephyr)
  1. KernelMain - Main kernel task(main task)
  2. KernelBackground - Background kernel task(Background task)
  3. Worker - Worker task(Worker task)
  4. App - Application task(App task)
  5. BTHost - Bluetooth HostZephyr bluetooth
  6. BTController - Bluetooth Controller(Zephyr bluetooth)
  7. BTHCI - Bluetooth HCI(Zephyr bluetooth)

  8. NewTimers - Timer task(Zephyr workqueue)
  9. PULSE - PULSE task(NA)
