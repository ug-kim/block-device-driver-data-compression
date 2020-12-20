# Subject

- Block Device Driver Data Compression

# Compression Algorithm

- [lz4](https://github.com/lz4/lz4)

# How to run

- To load sbull

```bash
sudo modprobe lz4
make
make test1_1 test1_2 test2_1 test2_2 test3_1 test3_2
sudo ./sbull_load
```

- Mount

```bash
sudo mkdir /media/sbull
sudo mkfs -t ext4 /dev/sbulla
sodu mount -t ext4 /dev/sbulla /media/sbull
```

- Change mode to sbull - for drop_caches

```bash
sudo -s
```

- Test - you can see test log using 'dmesg'

```bash
sudo ./test1_1
sudo echo 3 > /proc/sys/vm/drop_caches
shdo ./test1_2
```

- If you want to log, using `dmesg`
- If you wannt to delete logs, `sudo dmesg -c`

