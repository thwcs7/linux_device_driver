dt: PCDEV0.dts
	dtc -@ -I dts -O dtb -o PCDEV0.dtbo PCDEV0.dts

decompile: PCDEV0.dtbo
	dtc -I dtb -O dts -o PCDEV0_decompiled.dts PCDEV0.dtbo

clean:
	rm -rf PCDEV0.dtbo
	rm -rf PCDEV0_decompiled.dts