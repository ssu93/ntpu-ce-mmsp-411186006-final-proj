all: build

build:
	gcc encoder_411186006.c -o encoder_411186006.exe
	gcc decoder_411186006.c -o decoder_411186006.exe
demo0:
	./encoder_411186006.exe 0 KimberlyNCat.bmp R.txt G.txt B.txt dim_0.txt
	./decoder_411186006.exe 0 ResKimberlyNCat.bmp R.txt G.txt B.txt dim_0.txt
	diff KimberlyNCat.bmp ResKimberlyNCat.bmp
demo0_txtdiff:
	hexdump -C KimberlyNCat.bmp > KimberlyNCat.txt
	hexdump -C ResKimberlyNCat.bmp > ResKimberlyNCat.txt
	diff KimberlyNCat.txt ResKimberlyNCat.txt
clean:
	rm -rf *.txt *.exe  ResKimberlyNCat.bmp

.PHONY: clean

