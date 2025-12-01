#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Convert one char to 8 ASCII '0'/'1' characters placed at out[0..7]
void charToBinary(char c, char *out)
{
    for (int i = 7; i >= 0; i--)
    {
        out[7 - i] = ((c >> i) & 1) ? '1' : '0';
    }
}

// Convert `size` characters from text into `size*8` '0'/'1' bytes in binary
void stringToBinary(int size, const char *text, char *binary)
{
    for (int i = 0; i < size; i++)
    {
        charToBinary(text[i], binary + (i * 8));
    }
}

// Map bits ('0'/'1') to pixel values 1..255 with desired parity. 
// randomPixelValue is generated in 1..254, then possibly incremented to 255 so we stay in [1,255].
void binaryToPixels(int totalBits, const char *binaryText, int *pixelValues)
{
    for (int i = 0; i < totalBits; i++)
    {
        // Use 1..254 so we can safely increment to change parity without exceeding 255
        int randomPixelValue = 1 + (rand() % 254); // yields 1..254

        if (binaryText[i] == '0') // make even
        {
            if (randomPixelValue % 2 != 0)
                randomPixelValue++; // odd -> next even (<=255)
        }
        else // make odd
        {
            if (randomPixelValue % 2 == 0)
                randomPixelValue++; // even -> next odd (<=255)
        }
        pixelValues[i] = randomPixelValue;
    }
}

// Write pixelValues (0..255) as a PGM image file "image.pgm".
// totalBits pixels, image width chosen as ceil(sqrt(totalBits)), height computed accordingly.
int pixelsToImage(const char *filename, int totalBits, const int *pixelValues)
{
    if (totalBits <= 0)
        return -1;

    int width = (int)ceil(sqrt((double)totalBits));
    int height = (int)ceil((double)totalBits / width);

    FILE *fp = fopen(filename, "wb");
    if (!fp)
        return -1;

    // PGM binary header
    if (fprintf(fp, "P5\n%d %d\n255\n", width, height) < 0)
    {
        fclose(fp);
        return -1;
    }

    // write pixels row-major; pad with 0 if needed
    int written = 0;
    for (int i = 0; i < height * width; i++)
    {
        unsigned char byte = 0;
        if (i < totalBits)
        {
            int pv = pixelValues[i];
            if (pv < 0)
                pv = 0;
            if (pv > 255)
                pv = 255;
            byte = (unsigned char)pv;
        }
        else
        {
            byte = 0; // padding
        }
        if (fwrite(&byte, 1, 1, fp) != 1)
        {
            fclose(fp);
            return -1;
        }
        written++;
    }

    fclose(fp);
    return 0;
}

int main(void)
{
    int initialCapacity = 64;
    int currentCapacity = initialCapacity;
    int currentSize = 0;
    char *text = malloc(currentCapacity * sizeof(char));
    if (!text)
    {
        fprintf(stderr, "Memory allocation failed for text buffer\n");
        return 1;
    }

    FILE *in = fopen("input.txt", "rb");
    if (!in)
    {
        fprintf(stderr, "Error opening input.txt for reading\n");
        free(text);
        return 1;
    }

    int ch;
    while ((ch = fgetc(in)) != EOF)
    {
        if (currentSize == currentCapacity)
        {
            currentCapacity *= 2;
            char *temp = realloc(text, currentCapacity * sizeof(char));
            if (!temp)
            {
                fprintf(stderr, "Realloc failed\n");
                free(text);
                fclose(in);
                return 1;
            }
            text = temp;
        }
        text[currentSize++] = (char)ch;
    }
    fclose(in);

    if (currentSize == 0)
    {
        fprintf(stderr, "input.txt is empty\n");
        free(text);
        return 1;
    }

    int totalBits = currentSize * 8;
    char *binaryText = malloc((totalBits + 1) * sizeof(char)); // +1 for safety NUL
    if (!binaryText)
    {
        fprintf(stderr, "Memory allocation failed for binaryText\n");
        free(text);
        return 1;
    }

    int *pixelValues = malloc(totalBits * sizeof(int));
    if (!pixelValues)
    {
        fprintf(stderr, "Memory allocation failed for pixelValues\n");
        free(text);
        free(binaryText);
        return 1;
    }

    // fill binaryText with '0'/'1'
    stringToBinary(currentSize, text, binaryText);
    binaryText[totalBits] = '\0';

    // seed RNG once
    srand((unsigned)time(NULL));
    binaryToPixels(totalBits, binaryText, pixelValues);

    // write image
    if (pixelsToImage("image.pgm", totalBits, pixelValues) != 0)
    {
        fprintf(stderr, "Failed to write image.pgm\n");
    }
    else
    {
        printf("Wrote image.pgm (%d x %d) representing %d bits.\n",
               (int)ceil(sqrt((double)totalBits)),
               (int)ceil((double)totalBits / ceil(sqrt((double)totalBits))),
               totalBits);
    }

    free(text);
    free(binaryText);
    free(pixelValues);
    return 0;
}
