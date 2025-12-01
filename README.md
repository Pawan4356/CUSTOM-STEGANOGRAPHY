# CUSTOM-STEGANOGRAPHY
### **Text -> Binary -> Random Pixel Values (GrayScale) -> pgm -> png**

### Text
```text
Steganography is the practice of hiding information inside another message or object so that an unsuspecting observer does not notice anything unusual. In digital contexts, this can involve concealing a file, image, message, or video within another digital medium. Unlike cryptography, which hides the content of a message, steganography hides the existence of the message itself...
```

### Binary
```text
01010011 01110100 01100101 01100111 01100001 01101110 01101111 01100111 01110010 01100001 01110000 01101000 01111001...
```

### Pixel values
```text
112 97 10 255...
```
### PGM Image -> PNG Image
![](image.png)


### Install ImageMagick from [HERE](https://imagemagick.org/archive/binaries/ImageMagick-7.1.2-9-Q16-x64-static.exe) and Add application directory to your system PATH.

### In command line run
```bash
gcc steganography.c -o steganography.exe ; ./steganography.exe ; magick image.pgm image.png
```