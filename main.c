#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;

// Helper functions
void swap(byte* a, byte* b) {
    byte temp = *a;
    *a = *b;
    *b = temp;
}

// Key schedule
void key_schedule(byte* key, byte* round_keys) {
    int i, j;
    byte S[256];

    // Initialize S-box
    for (i = 0; i < 256; i++) {
        S[i] = i;
    }

    // Permute S-box using key
    j = 0;
    for (i = 0; i < 256; i++) {
        j = (j + key[i % 16] + S[i]) % 256;
        swap(&S[i], &S[j]);
    }

    // Generate round keys
    j = 0;
    for (i = 0; i < 40; i++) {
        round_keys[i] = S[(i + 1) % 256];
        j = (j + round_keys[i]) % 256;
        swap(&S[i], &S[j]);
    }
}

// Encryption
void encrypt(byte* data, byte* key, byte* iv, byte* ciphertext, int data_len) {
    byte round_keys[40];
    byte keystream[16];
    byte block[16];
    int i, j, k;

    // Generate round keys
    key_schedule(key, round_keys);

    // Encrypt each block of data
    for (i = 0; i < data_len; i += 16) {
        // Generate keystream for this block
        memcpy(block, iv, 16);
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 16; k++) {
                keystream[j * 4 + k] = block[k];
            }
            block[15]++;
            if (block[15] == 0) {
                block[14]++;
            }
        }
        for (j = 0; j < 16; j++) {
            keystream[j] ^= round_keys[j + 24];
        }

        // Encrypt the block
        for (j = 0; j < 16; j++) {
            ciphertext[i + j] = data[i + j] ^ keystream[j];
        }

        // Update the IV
        memcpy(iv, ciphertext + i, 16);
    }
}

// Decryption
void decrypt(byte* ciphertext, byte* key, byte* iv, byte* plaintext, int data_len) {
    byte round_keys[40];
    byte keystream[16];
    byte block[16];
    int i, j, k;

    // Generate round keys
    key_schedule(key, round_keys);

    // Decrypt each block of data
    for (i = 0; i < data_len; i += 16) {
        // Generate keystream for this block
        memcpy(block, iv, 16);
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 16; k++) {
                keystream[j * 4 + k] = block[k];
            }
            block[15]++;
            if (block[15] == 0) {
                block[14]++;
            }
        }
        for (j = 0; j < 16; j++) {
            keystream[j] ^= round_keys[j + 24];
        }

        // Decrypt the block
        for (j = 0; j < 16; j++) {
            plaintext[i + j] = ciphertext[i + j] ^ keystream[j];
        }

        // Update the IV
        memcpy(iv, ciphertext + i, 16);
    }
}

int main() {
    byte data[] = "taufiq ee";
    byte key[] = "0123456789ABCDEF";
    byte iv[] = "0000000000000000";
    byte ciphertext[16];
    int data_len = strlen(data);

    // Pad data with zeros
    while (data_len % 16 != 0) {
        data[data_len] = 0;
        data_len++;
    }

    // Encrypt the data
    encrypt(data, key, iv, ciphertext, data_len);

    // Print the ciphertext
    int i;
    printf("Ciphertext: ");
    for (i = 0; i < data_len; i++) {
        printf("%02X", ciphertext[i]);
    }
    printf("\n");

    return 0;
}
