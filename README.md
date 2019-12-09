# lssd

Lists storage devices. Better than `lsblk`. Less information than `fdisk -l`.

## Screenshot

This is a simple command-line tool, so here's a taste of how it looks:

```
# lssd
sdb (R/W) 1000204886016 bytes
	└ sdb1 (R/W) [ext4] 1000203091968 bytes
		├ Label: SecondHDD
		├ Mount Point: /mnt/hda
		└ UUID: e8942d63-b693-4d6f-8df5-59376e319615

sr0 (R/W) 1073741312 bytes
	No partitions available!

sda (R/W) 120034123776 bytes
	├ sda2 (R/W) [ext4] 114890375168 bytes
	│	├ Mount Point: /
	│	└ UUID: f91e89f4-cca4-477d-b6c5-84d7b750f769
	├ sda3 (R/W) [swap] 4874247680 bytes
	│	└ UUID: 706b41fd-3121-4ebb-adb5-a39d329f2b66
	└ sda1 (R/W) [vfat] 268435456 bytes
		└ UUID: FB04-84D7
# 
```

## License

This project is licensed under the **MIT License**.

